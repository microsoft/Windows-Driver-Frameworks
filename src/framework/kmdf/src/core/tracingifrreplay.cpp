/*++

Copyright (c) Microsoft Corporation

Module Name:

    TracingIfrReplay.cpp

Abstract:

    This module implements IFR replay/capture for kernel driver framework

Author:



Environment:

    Kernel mode only

Revision History:
--*/

#include "fxcorepch.hpp"

extern "C" {
#include "tracingIfrReplay.tmh"
}

#include <initguid.h>
#include "fxIFR.h"       // shared struct between IFR and debug ext.
#include "fxIFRKm.h"     // kernel mode only IFR definitions

NTSTATUS
FxIFRValidateLogHeader(
    __in PWDF_IFR_HEADER Header
    )
/*++
Routine Description:
    Used to validate that a IFR header is valid

Arguments:
    PWDF_IFR_HEADER - IFR header.

Returns:
    NTSTATUS status of the operation.
--*/
{
    NTSTATUS status;
    ULONG totalSize;

    status = RtlULongAdd(Header->Size,
                        sizeof(WDF_IFR_HEADER),
                        &totalSize);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (totalSize > FxIFRMaxLogSize || totalSize < FxIFRMinLogSize) {
        return STATUS_INVALID_BUFFER_SIZE;
    }

    if (FxIsEqualGuid((LPGUID)&(Header->Guid), (LPGUID)&WdfTraceGuid) == FALSE
        ||
        Header->Offset.u.s.Current > Header->Size
        ||
        Header->Offset.u.s.Previous > Header->Size) {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
FxIFRValidateRecord(
    _In_ PWDF_IFR_RECORD IfrRecord,
    _In_ ULONG_PTR       IfrHeaderMaxValidPtr,
    _In_ ULONG_PTR       IfrMaxValidPtr
    )
/*++

Routine Description:
    Used to validate that a IFR record is within the bounds of the IFR buffer

Arguments:
    PWDF_IFR_RECORD - IFR record to validate.
    IfrHeaderMaxValidPtr - Maximum valid value for the ptr to the IFR record
    IfrMaxValidPtr - Maximum valid value for the ptr to the IFR's data payload

Returns:
    NTSTATUS status of the operation.
--*/
{
    ULONG_PTR maxIfrPtr;
    NTSTATUS status;

    if ((ULONG_PTR)IfrRecord > IfrHeaderMaxValidPtr) {
        return STATUS_INVALID_BUFFER_SIZE;
    }

    if (IfrRecord->Length < sizeof(WDF_IFR_RECORD)) {
        return STATUS_INVALID_BUFFER_SIZE;
    }

    status = RtlULongPtrAdd((ULONG_PTR)IfrRecord,
                            (IfrRecord->Length)-1,
                            &maxIfrPtr);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (maxIfrPtr > IfrMaxValidPtr) {
        return STATUS_INVALID_BUFFER_SIZE;
    }

    return STATUS_SUCCESS;
}

_Check_return_
PWDF_IFR_HEADER
FxIFRCreateSnapshot(
    _In_ PCSTR DriverName,
    _In_ PFX_DRIVER_GLOBALS FxGlobalsForTracing)
/*++

Routine Description:
    Given a driver name iterate the library globals and return a copy/snapshot
    of the WDF_IFR_HEADER for the driver.

    NOTE: Caller must free allocation for returned PWDF_IFR_HEADER.

Arguments:
    DriverName 
    FxGlobalsForTracing - To be used for DoTraceMessage

Returns:
    PWDF_IFR_HEADER - Copy of driver's IFR header if found. NULL otherwise.
--*/
{
    BOOLEAN bFound = FALSE;
    KIRQL irql;
    PLIST_ENTRY listEntry;
    PFX_DRIVER_GLOBALS fxGlobals = NULL;
    PWDF_IFR_HEADER pHeaderCopy = NULL;
    ULONG totalIFRsize = 0;
    NTSTATUS status = STATUS_SUCCESS;
    BOOLEAN bDropIfrReference = FALSE;

    ASSERT(DriverName);

    FxLibraryGlobals.FxDriverGlobalsListLock.Acquire(&irql);

    listEntry = FxLibraryGlobals.FxDriverGlobalsList.Flink;

    while (listEntry != &FxLibraryGlobals.FxDriverGlobalsList) {
        fxGlobals = CONTAINING_RECORD(listEntry, FX_DRIVER_GLOBALS, Linkage);
        if (_stricmp(fxGlobals->Public.DriverName, DriverName) == 0) {
            bFound = TRUE;
            break;
        }

        listEntry = listEntry->Flink;
    }

    if (!bFound) {
        DoTraceLevelMessage(FxGlobalsForTracing, TRACE_LEVEL_WARNING, TRACINGIFRCAPTURE,
            "Unable to replay IFR. Service %s was not found loaded.", DriverName);
        DoTraceLevelMessage(FxGlobalsForTracing, TRACE_LEVEL_WARNING, TRACINGIFRCAPTURE,
            "Hint: Service name must be provided, NOT image name.");

        goto DoneReleaseLock;
    }

    if (fxGlobals->WdfLogHeader == NULL) {
        DoTraceLevelMessage(FxGlobalsForTracing, TRACE_LEVEL_WARNING, TRACINGIFRCAPTURE,
                            "Unable to replay. IFR log unavailable for driver %s.", DriverName);
        goto DoneReleaseLock;
    }

    //
    // Attempt to take a reference on the IFR buffer. 
    //
    if (0 == FxInterlockedIncrementGTZero(&(fxGlobals->WdfLogHeaderRefCount))) {
        //
        // FxIFR stop has already driven the count to 0. Do not touch the buffer
        //
        goto DoneReleaseLock;
    }

    bDropIfrReference = TRUE;

    status = RtlULongAdd(((PWDF_IFR_HEADER)fxGlobals->WdfLogHeader)->Size,
                        sizeof(WDF_IFR_HEADER),
                        &totalIFRsize);
    if (!NT_SUCCESS(status)) {
        goto DoneReleaseLock;
    }

    if (totalIFRsize > FxIFRMaxLogSize || totalIFRsize < FxIFRMinLogSize) {
        status = STATUS_INVALID_BUFFER_SIZE;
        goto DoneReleaseLock;
    }

    pHeaderCopy = (PWDF_IFR_HEADER)ExAllocatePoolWithTag(NonPagedPool,
                                                        totalIFRsize,
                                                        WDF_IFR_LOG_TAG);
    if (pHeaderCopy == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto DoneReleaseLock;
    }

    RtlCopyMemory(pHeaderCopy, fxGlobals->WdfLogHeader, totalIFRsize);
    pHeaderCopy->Base = (PUCHAR)&pHeaderCopy[1];

    status = FxIFRValidateLogHeader(pHeaderCopy);
    if (!NT_SUCCESS(status)) {
        ExFreePoolWithTag(pHeaderCopy, WDF_IFR_LOG_TAG);
        pHeaderCopy = NULL;
        goto DoneReleaseLock;
    }

DoneReleaseLock:
    if (bDropIfrReference) {
        //
        // FxIFRStop will undo the reference taken above.
        //
        FxIFRStop(fxGlobals);
    }

    FxLibraryGlobals.FxDriverGlobalsListLock.Release(irql);

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(FxGlobalsForTracing, TRACE_LEVEL_ERROR, TRACINGIFRCAPTURE,
            "FxIFR failed to copy IFR log with %!STATUS! for driver %s",
            status, DriverName);
    }
    return pHeaderCopy;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
FxIFRSendRecordsToWpp(
    _In_ TRACEHANDLE     LoggerHandle,
    _In_ PWDF_IFR_HEADER HeaderCopy
    )
/*++

Routine Description:
    Used to replay IFR records to WPP.

Arguments:
    LoggerHandle    - WPP trace handle.
    PWDF_IFR_HEADER - IFR header to replay.

Returns:
    NTSTATUS status of the operation.
--*/
{
    PUCHAR  ifrMaxValidPtr, ifrHeaderMaxValidPtr;
    BOOLEAN atBase;
    BOOLEAN wrappedBase = FALSE;
    PUCHAR  ifrBase;
    PWDF_IFR_RECORD currentIfrRecord = NULL;
    USHORT  startingOffset;
    USHORT  previousOffset;
    PUSHORT offsetList = NULL;
    ULONG   maxIfrRecords;
    ULONG   count;
    NTSTATUS status = STATUS_SUCCESS;

    maxIfrRecords = (HeaderCopy->Size) / sizeof(WDF_IFR_HEADER);

    ifrBase = HeaderCopy->Base;

    //
    // An IFR record should never be read beyond ifrMaxValidPtr.
    // 
    status = RtlULongPtrAdd((ULONG_PTR)ifrBase,
                            ((HeaderCopy)->Size-1),
                            (ULONG_PTR*)&ifrMaxValidPtr);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    //
    // A ptr to an IFR record's header should never exceed ifrHeaderMaxValidPtr 
    //
    status = RtlULongPtrSub((ULONG_PTR)ifrMaxValidPtr,
                            sizeof(WDF_IFR_RECORD),
                            (ULONG_PTR*)&ifrHeaderMaxValidPtr);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    startingOffset = HeaderCopy->Offset.u.s.Current;
    previousOffset = HeaderCopy->Offset.u.s.Previous;

    //
    // Get to the first IFR record that is to be read.
    //
    status = RtlULongPtrAdd((ULONG_PTR)ifrBase,
                            previousOffset,
                            (ULONG_PTR*)(&currentIfrRecord));
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    status = FxIFRValidateRecord(currentIfrRecord,
                            (ULONG_PTR)ifrHeaderMaxValidPtr,
                            (ULONG_PTR)ifrMaxValidPtr);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    if (currentIfrRecord->Signature == 0) {
        //
        // Empty IFR
        //
        goto Done;
    }

    count = 0;
    atBase = FALSE;

    //
    // Push the offsets onto a stack (array) so they can be redirected to WPP 
    // in the order they were written to IFR.
    //
    offsetList = (PUSHORT)ExAllocatePoolWithTag(PagedPool,
                                            maxIfrRecords*sizeof(USHORT),
                                            WDF_IFR_LOG_TAG);
    if (offsetList == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Done;
    }
    RtlZeroMemory(offsetList, maxIfrRecords*sizeof(USHORT));

    //
    // Check if the IFR records are valid and read them in the reverse order they
    // were written.
    //
    while (count < maxIfrRecords) {

        status = FxIFRValidateRecord(currentIfrRecord, 
                                    (ULONG_PTR)ifrHeaderMaxValidPtr, 
                                    (ULONG_PTR)ifrMaxValidPtr);
        if (!NT_SUCCESS(status)) {
            break;
        }
        
        if (currentIfrRecord->Signature != WDF_IFR_RECORD_SIGNATURE) {
            break;
        }

        //
        // This is a valid IFR entry. Add it to the list.
        //
        offsetList[count] = previousOffset;
        count++;

        if ((PUCHAR)currentIfrRecord == ifrBase) {
            atBase = TRUE;
        }

        //
        // Move to the next entry (in reverse order) that must be parsed.
        //
        previousOffset = currentIfrRecord->PrevOffset;
        status = RtlULongPtrAdd((ULONG_PTR)ifrBase,
                                previousOffset,
                                (ULONG_PTR*)(&currentIfrRecord));
        if (!NT_SUCCESS(status)) {
            break;
        }

        if (atBase && previousOffset == 0) {
            //
            // Already read currentIfrRecord at base. No more records to read.
            //
            break;
        }

        if (wrappedBase && previousOffset <= startingOffset) {
            // 
            // We've wrapped around and read upto the start offset.
            //
            break;
        }

        if (atBase && previousOffset != 0) {
            //
            // Already read the currentIfrRecord at base. There are more to read
            //
            (ASSERT(!wrappedBase));
            wrappedBase = TRUE;
            atBase = FALSE;
        }
    }

    //
    // List has been built up and validated. Now send the messages to WPP in the order they
    // were written.
    //
    if (count == 0) {
        goto Done;
    }

    while (count > 0) {
        currentIfrRecord = WDF_PTR_ADD_OFFSET_TYPE(ifrBase,
                                                offsetList[count - 1],
                                                PWDF_IFR_RECORD);

        //
        // Invoke WMI trace message such that the ETW event built up by it
        // is effectively no different than the one built up from an actual 
        // WPP trace call (via DoTraceLevelMessage).
        //
        if (currentIfrRecord->Length > sizeof(WDF_IFR_RECORD)) {
            FxWmiTraceMessage(LoggerHandle,
                            TRACE_MESSAGE_SEQUENCE | TRACE_MESSAGE_GUID | TRACE_MESSAGE_SYSTEMINFO |
                            TRACE_MESSAGE_TIMESTAMP,
                            &currentIfrRecord->MessageGuid,
                            currentIfrRecord->MessageNumber,
                            (PVOID)&currentIfrRecord[1],
                            currentIfrRecord->Length - sizeof(WDF_IFR_RECORD),
                            0);
        }
        else if (currentIfrRecord->Length == sizeof(WDF_IFR_RECORD)) {
            FxWmiTraceMessage(LoggerHandle,
                            TRACE_MESSAGE_SEQUENCE | TRACE_MESSAGE_GUID | TRACE_MESSAGE_SYSTEMINFO |
                            TRACE_MESSAGE_TIMESTAMP,
                            &currentIfrRecord->MessageGuid,
                            currentIfrRecord->MessageNumber,
                            0);
        }
        else {
            ASSERT(FALSE);
            break;
        }

        count--;
    }

Done:
    if (offsetList != NULL) {
        ExFreePoolWithTag(offsetList, WDF_IFR_LOG_TAG);
    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
FxIFRGetDriverMultiString(
    _In_ ULONG ValueLength,
    _Out_opt_bytecap_(ValueLength) PWCHAR Value,
    _Out_ PULONG ValueLengthQueried
    )
/*++
Routine Description:
    Obtain the multi string from the registry that tells us which drivers
    want their IFR replayed.

Arguments:
    ValueLength - Size in bytes of buffer Value points to.
    Value       - The WCHAR buffer that the multi-sz will be copied into.
    ValueLengthQueried - Size required to copy the buffer.

Returns:
    NTSTATUS - Status of operation. 
--*/
{
    FxAutoRegKey hWdf;
    NTSTATUS status = STATUS_SUCCESS;
    DECLARE_CONST_UNICODE_STRING(path, WDF_REGISTRY_BASE_PATH);
    DECLARE_CONST_UNICODE_STRING(driverListToReplay, WDF_GLOBAL_VALUE_IFR_REPLAY);
    ULONG type;
    ULONG numChars;

    status = FxRegKey::_OpenKey(NULL, &path, &hWdf.m_Key, KEY_READ);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    //
    // Find out how big a buffer we need to allocate if the value is present
    //
    status = FxRegKey::_QueryValue(NULL,
                                hWdf.m_Key,
                                &driverListToReplay,
                                ValueLength,
                                (PVOID)Value,
                                ValueLengthQueried,
                                &type);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    if (type != REG_MULTI_SZ) {
        status = STATUS_OBJECT_TYPE_MISMATCH;
        goto Done;
    }

    numChars = (*ValueLengthQueried) / sizeof(WCHAR);

    if ((*ValueLengthQueried) % 2 != 0 ||
        numChars < 2    ||
        Value[numChars - 1] != UNICODE_NULL ||
        Value[numChars - 2] != UNICODE_NULL) {
        //
        // Invalid multi sz string
        //
        status = STATUS_OBJECT_TYPE_MISMATCH;
        goto Done;
    }

Done:
    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
FxIFRReplay(
    _In_ TRACEHANDLE LoggerHandle
    )
/*++
Routine Description:
    This is the main IFR replay function invoked by the trace control callback
    when TRACINGIFRCAPTURE flag is enabled by the trace listener.

Arguments:
    LoggerHandle - WPP trace handle that will be used to write WPP trace msgs

Returns:
    VOID
--*/
{
    PWDF_IFR_HEADER pHeaderCopy = NULL;
    NTSTATUS status = STATUS_SUCCESS;
    PWCHAR driverMultiString = NULL;
    PWCHAR pdriverWChar = NULL;
    CHAR   driverChar[WDF_DRIVER_GLOBALS_NAME_LEN];
    ULONG  multiStringLength = 0;
    UNICODE_STRING currentDriverUnicode;
    ANSI_STRING currentDriverAnsi;
    FX_DRIVER_GLOBALS fxGlobalsForReplay = { 0 };
    PFX_DRIVER_GLOBALS pFxGlobals;

    ASSERT(KeGetCurrentIrql() <= APC_LEVEL);

    //
    // Note: Following is necessary because we do not want trace msg calls from
    // this module to land in IFR . If they do, they will get replayed and 
    // show up as redundant entries in the replayed trace. This also lets us 
    // generate trace msgs without having located the actual Fx_Driver_Globals 
    // for any driver.
    //
    fxGlobalsForReplay.WdfLogHeader = NULL;
    pFxGlobals = &fxGlobalsForReplay;

    if (LoggerHandle == NULL) {
        status = STATUS_INVALID_HANDLE;
        ASSERT(FALSE);
        goto Done;
    }

    if (FxLibraryGlobals.IfrDisabled) {
        DoTraceLevelMessage(pFxGlobals, TRACE_LEVEL_ERROR, TRACINGIFRCAPTURE,
            "No IFRs to replay. IFR is disabled globally");
    }

    //
    // Obtain the list of drivers from the registry
    // 
    status = FxIFRGetDriverMultiString(multiStringLength,
                                    driverMultiString,
                                    &multiStringLength);
    if (status != STATUS_BUFFER_OVERFLOW && status != STATUS_BUFFER_TOO_SMALL) {
        goto Done;
    }

    ASSERT(multiStringLength != 0);
    driverMultiString = (PWCHAR)ExAllocatePoolWithTag(PagedPool, 
                                                    multiStringLength, 
                                                    WDF_IFR_LOG_TAG);
    if (driverMultiString == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Done;
    }

    status = FxIFRGetDriverMultiString(multiStringLength,
                                    driverMultiString,
                                    &multiStringLength);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    //
    // For each driver in the list, obtain it's IFR snapshot and redirect
    // it to WPP.
    //
    for (pdriverWChar = driverMultiString; 
        *pdriverWChar != UNICODE_NULL; 
        pdriverWChar += wcslen(pdriverWChar) + 1) {

        //
        // Convert the unicode string to ansi
        // 
        RtlInitUnicodeString(&currentDriverUnicode, pdriverWChar);
        RtlZeroMemory(driverChar, sizeof(driverChar));
        RtlInitEmptyAnsiString(&currentDriverAnsi, driverChar, sizeof(driverChar));

        status = RtlUnicodeStringToAnsiString(&currentDriverAnsi, &currentDriverUnicode, FALSE);
        if (!NT_SUCCESS(status)) {
            continue;
        }

        //
        // We have an ANSI string. Capture the IFR snapshot.
        //
        pHeaderCopy = FxIFRCreateSnapshot(currentDriverAnsi.Buffer, pFxGlobals);
        if (pHeaderCopy == NULL) {
            continue;
        }

        DoTraceLevelMessage(pFxGlobals, TRACE_LEVEL_INFORMATION, TRACINGIFRCAPTURE,
                        "-----> Replaying IFR for driver service %s", driverChar);

        status = FxIFRSendRecordsToWpp(LoggerHandle,
                                    pHeaderCopy);
        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(pFxGlobals, TRACE_LEVEL_ERROR, TRACINGIFRCAPTURE,
                "-----> Replaying IFR for %s failed %!STATUS!", driverChar, status);
        }
        else {
            DoTraceLevelMessage(pFxGlobals, TRACE_LEVEL_INFORMATION, TRACINGIFRCAPTURE,
                "-----> Replaying IFR for %s complete", driverChar);
        }

        if (pHeaderCopy != NULL) {
            ExFreePoolWithTag(pHeaderCopy, WDF_IFR_LOG_TAG);
            pHeaderCopy = NULL;
        }
    }
Done:

    if (driverMultiString != NULL) {
        ExFreePoolWithTag(driverMultiString, WDF_IFR_LOG_TAG);
    }

    DoTraceLevelMessage(pFxGlobals, TRACE_LEVEL_INFORMATION, TRACINGIFRCAPTURE,
                "IFR Replay complete with status %!STATUS!", status);
    return;
}
