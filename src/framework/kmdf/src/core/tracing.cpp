/*++

Copyright (c) Microsoft Corporation

Module Name:

    Tracing.cpp

Abstract:

    This module implements tracing for the driver frameworks

Author:



Environment:

    Kernel mode only

Revision History:





--*/

#include "fxcorepch.hpp"

// We use DoTraceMessage
extern "C" {
#include "tracing.tmh"
}

#include <initguid.h>
#include "fxIFR.h"       // shared struct between IFR and debug ext.
#include "fxIFRKm.h"     // kernel mode only IFR definitions


//=============================================================================
//
//=============================================================================

_Must_inspect_result_
NTSTATUS
FxTraceInitialize(
    VOID
    )

/*++

Routine Description:

    This routine initializes the frameworks tracing.

    It must be called early on in the frameworks DriverEntry
    initialization.

Arguments:

    None

Returns:

    NTSTATUS code

--*/
{
    //
    // Initialize the tracing package: Vista or later
    //
    WPP_INIT_TRACING(NULL, NULL);

    return STATUS_SUCCESS;
}

VOID
TraceUninitialize(
    VOID
    )
/*++

Routine Description:
    This routine uninitializes the frameworks tracing.  It must be called just
    before DriverUnload

Arguments:
    None

Returns:
    None

--*/
{
    //
    // Vista and later
    //
    WPP_CLEANUP(NULL);
}

_Must_inspect_result_
NTSTATUS
FxWmiTraceMessage(
    __in TRACEHANDLE  LoggerHandle,
    __in ULONG        MessageFlags,
    __in LPGUID       MessageGuid,
    __in USHORT       MessageNumber,
    __in ...
    )
{
    NTSTATUS status;
    va_list va;

    va_start(va, MessageNumber);

    status = WmiTraceMessageVa(LoggerHandle,
                               MessageFlags,
                               MessageGuid,
                               MessageNumber,
                               va);
    va_end(va);

    return status;
}


//-----------------------------------------------------------------------------
// Subcomponents for the In-Flight Recorder follow.
//-----------------------------------------------------------------------------

ULONG
FxIFRGetSize(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in PCUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
    Checks to see if the service has overriden the default number of pages that
    are in the IFR.

Arguments:
    RegistryPath - path to the service

Return Value:
    The size of the IFR to create in bytes (not pages!)

  --*/
{
    FxAutoRegKey service, parameters;
    NTSTATUS status;
    ULONG numPages;
    UNREFERENCED_PARAMETER(RegistryPath);

    //
    // This is the value used in case of any error while retrieving 'LogPages'
    // from the registry.
    //
    numPages  = FxIFRMinLogPages;

    //
    // External representation of the IFR is the "LogPages", so use that term when
    // overriding the size via the registry.
    //
    DECLARE_CONST_UNICODE_STRING(parametersPath, L"Wdf");
    DECLARE_CONST_UNICODE_STRING(valueName, L"LogPages");

    status = OpenDriverParamsKeyForRead(FxDriverGlobals, &service.m_Key);
    if (!NT_SUCCESS(status)) {
        goto defaultValues;
    }

    status = FxRegKey::_OpenKey(service.m_Key, &parametersPath, &parameters.m_Key, KEY_READ);
    if (!NT_SUCCESS(status)) {
        goto defaultValues;
    }

    status = FxRegKey::_QueryULong(parameters.m_Key, &valueName, &numPages);
    if (!NT_SUCCESS(status)) {
        goto defaultValues;
    }

    if (numPages == 0) {
        numPages = FxIFRMinLogPages;
    }

defaultValues:
    //
    // Use FxIFRAvgLogPages if user specifies greater than FxIFRMaxLogPages and if
    // Verifier flag is on and so is Verbose flag.
    //
    if (numPages > FxIFRMaxLogPages) {
        if (FxDriverGlobals->FxVerifierOn && FxDriverGlobals->FxVerboseOn) {
            numPages = FxIFRAvgLogPages;
        }
        else {
            numPages = FxIFRMinLogPages;
        }
    }

    return numPages * PAGE_SIZE;
}

VOID
FxIFRStart(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in PCUNICODE_STRING RegistryPath,
    __in MdDriverObject DriverObject
    )
/*++

Routine Description:

    This routine initialize the In-Flight Recorder (IFR).

    The default log size is set by WDF_IFR_LOG_SIZE and currently
    is 4096 (one x86 page).
    This routine should be called very early in driver initialization
    to allow the capture of all significant events.

--*/
{
    PWDF_IFR_HEADER pHeader;
    ULONG size;

    UNREFERENCED_PARAMETER( DriverObject );

    WDFCASSERT(FxIFRRecordSignature == WDF_IFR_RECORD_SIGNATURE);

    //
    // Return early if IFR is disabled.
    //
    if (FxLibraryGlobals.IfrDisabled) {
        ASSERT(FxDriverGlobals->WdfLogHeader == NULL);
        return;
    }

    if (FxDriverGlobals == NULL || FxDriverGlobals->WdfLogHeader != NULL) {
        return;
    }

    size = FxIFRGetSize(FxDriverGlobals, RegistryPath);

    pHeader = (PWDF_IFR_HEADER) ExAllocatePool2(POOL_FLAG_NON_PAGED,
                                                size,
                                                WDF_IFR_LOG_TAG );
    if (pHeader == NULL) {
        return;
    }

    RtlZeroMemory(pHeader, size);

    //
    // Initialize the header.
    // Base will be where the IFR records are placed.
    // WPP_ThisDir_CTLGUID_FrameworksTraceGuid
    //
    RtlCopyMemory(&pHeader->Guid, (PVOID) &WdfTraceGuid, sizeof(GUID));

    pHeader->Base = (PUCHAR) &pHeader[1];
    pHeader->Size = size - sizeof(WDF_IFR_HEADER);

    pHeader->Offset.u.s.Current  = 0;
    pHeader->Offset.u.s.Previous = 0;
    RtlStringCchCopyA(pHeader->DriverName, WDF_IFR_HEADER_NAME_LEN, FxDriverGlobals->Public.DriverName);

    FxDriverGlobals->WdfLogHeader = pHeader;

    InterlockedIncrement(&FxDriverGlobals->WdfLogHeaderRefCount);

    DoTraceLevelMessage(FxDriverGlobals, TRACE_LEVEL_INFORMATION, TRACINGDRIVER,
                        "FxIFR logging started" );

    if (size > FxIFRMinLogSize) {
        DoTraceLevelMessage(
            FxDriverGlobals, TRACE_LEVEL_INFORMATION, TRACINGDRIVER,
            "FxIFR has been started with a size override:  size 0x%x bytes, "
            "# Pages %d.  An extended IFR size may not be written to a minidump!",
            size, size/PAGE_SIZE);
    }
}

VOID
FxIFRStop(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals
    )
/*++

Routine Description:

    This routine stops the In-Flight Recorder (IFR).

    It should be called as late in the driver teardown as possible
    to allow for the capture of all significant events.

--*/
{
    //
    // Return early if IFR is disabled.
    //
    if (FxLibraryGlobals.IfrDisabled) {
        ASSERT(FxDriverGlobals->WdfLogHeader == NULL);
        return;
    }

    if (FxDriverGlobals == NULL || FxDriverGlobals->WdfLogHeader == NULL) {
        return;
    }

    //
    // Under normal operation the ref count should usually drop to zero when
    // FxIfrStop is called by FxLibraryCommonUnregisterClient, unless
    // FxIfrReplay is in the process of making a copy of the IFR buffer.
    // In which case that thread will call FxIfrStop.
    //
    if (0 == InterlockedDecrement(&(FxDriverGlobals->WdfLogHeaderRefCount))) {
        //
        // Free the Log buffer.
        //
        ExFreePoolWithTag(FxDriverGlobals->WdfLogHeader, WDF_IFR_LOG_TAG);
        FxDriverGlobals->WdfLogHeader = NULL;
    }
}

_Must_inspect_result_
NTSTATUS
FxIFR(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in UCHAR              MessageLevel,
    __in ULONG              MessageFlags,
    __in LPGUID             MessageGuid,
    __in USHORT             MessageNumber,
    __in ...
    )
/*++

Routine Description:

    This routine is the main In-Flight Recorder (IFR) routine.

    It captures a WPP message to the IFR log.
    The IFR is always running, e.g. not WPP logger is necessary
    to start logging.

Arguments:

    MessageLevel  - The WPP message level for this event
    MessageFlags  - The WPP message flags for this event (see trace GUID)
    MessageGuid   - The tracewpp generated guid for module emitting this event.
    MessageNumber - The tracewpp generated message number within
                    the emitting module.
    ...           - Variable arguments associates with the emitted message.

Returns:

    NTSTATUS

--*/
{
    size_t            size;
    PWDF_IFR_RECORD   record;
    PWDF_IFR_HEADER  header;

    UNREFERENCED_PARAMETER( MessageLevel );
    UNREFERENCED_PARAMETER( MessageFlags );

    //
    // Return early if IFR is disabled.
    //
    if (FxLibraryGlobals.IfrDisabled) {
        ASSERT(FxDriverGlobals->WdfLogHeader == NULL);
        return STATUS_SUCCESS;
    }

    if ( FxDriverGlobals->WdfLogHeader == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Determine the number bytes to follow header
    //
    size = 0;   // For Count of Bytes

    //
    // Determine how much log space is needed for this
    // trace record's data.
    //
    {
        va_list   ap;
        size_t    argLen;

        va_start(ap, MessageNumber);

        while ((va_arg(ap, PVOID)) != NULL) {

            argLen = va_arg(ap, size_t);

            if (argLen > 0) {

                if (argLen > FxIFRMaxMessageSize) {
                    goto drop_message;
                }
                size += (USHORT) argLen;
            }
        }

        va_end(ap);

        //
        // NOTE: The final size must be 32-bit (ULONG) aligned.
        //       This is necessary for IA64 to prevent Alignment Faults.
        //
        size += (size % sizeof(ULONG)) ? sizeof(ULONG) - (size % sizeof(ULONG)) : 0;

        if (size > FxIFRMaxMessageSize) {
            goto drop_message;
        }
    }

    size += sizeof(WDF_IFR_RECORD);

    //
    // Allocate log space of the calculated size
    //
    {
        WDF_IFR_OFFSET   offsetRet;
        WDF_IFR_OFFSET   offsetCur;
        WDF_IFR_OFFSET   offsetNew;
        USHORT           usSize = (USHORT) size;  // for a prefast artifact.

        header = (PWDF_IFR_HEADER) FxDriverGlobals->WdfLogHeader;

        FxVerifyLogHeader(FxDriverGlobals, header);

        //
        // Allocate space for the log in our circular buffer in a lockless way.
        // The idea is: read the current buffer position, try and reserve space
        // for our log, and then try and write the new buffer position. If another
        // thread has changed the buffer position in this time simply try again.
        //
        offsetRet.u.AsLONG = header->Offset.u.AsLONG;

        do {
            //
            // See if we can reserve based on our expected buffer position, and
            // verify with InterlockedCompareExchange that this is the actual
            // position (that another thread hasn't already beaten us here).
            //
            offsetCur.u.AsLONG = offsetRet.u.AsLONG;

            //
            // See if we need to wrap around or if we can fit in the forward iteration
            //
            if (&header->Base[header->Size] < &header->Base[offsetCur.u.s.Current+size]) {

                //
                // We need to wrap around to the start of the buffer
                //
                offsetNew.u.s.Current  = usSize;
                offsetNew.u.s.Previous = 0;

            } else {

                //
                // We didn't need to wrap around so try claiming room at the
                // end of the buffer
                //
                offsetNew.u.s.Current  = offsetCur.u.s.Current + usSize;
                offsetNew.u.s.Previous = offsetCur.u.s.Current;
            }

            //
            // Check if another thread has preempted us and moved the log global
            // offset pointer. If it has not, then our expected offset matches
            // the log global offset, and we move it ourselves and claim the
            // memory for our thread's use.
            //
            //   Thread 1:                          |  Thread 2:
            //                                      |
            //   offsetCur = header->Offset;        |  offsetCur = header->Offset;
            //                                      |
            //   compare and exchange, i.e.         |
            //   offsetRet = header->Offset;        |
            //   if (offsetCur == header->Offset) { |
            //       header->Offset = offsetNew;    |
            //   }                                  |
            //                                      |  offsetRet = header->Offset; // read changed header
            //                                      |  if (offsetCur == header->Offset) { // false
            //                                      |      // because of false, do not modify header
            //                                      |  }
            //                                      |
            //   break loop as offsetCur==offsetRet |  loop again as offsetCur != offsetRet
            //
            offsetRet.u.AsLONG =
                InterlockedCompareExchange( &header->Offset.u.AsLONG,
                                            offsetNew.u.AsLONG,
                                            offsetCur.u.AsLONG );

        } while (offsetCur.u.AsLONG != offsetRet.u.AsLONG);

        //
        // We had a successful compare+exchange, meaning we successfully reserved
        // space in the buffer for this log message.
        //
        record = (PWDF_IFR_RECORD) &header->Base[offsetNew.u.s.Previous];

        // RtlZeroMemory( record, sizeof(WDF_IFR_RECORD) );

        //
        // Build record (fill all fields!)
        //
        record->Signature     = FxIFRRecordSignature;
        record->Length        = (USHORT) size;
        record->PrevOffset    = (USHORT) offsetRet.u.s.Previous;
        record->MessageNumber = MessageNumber;
        record->Sequence      = InterlockedIncrement( &header->Sequence );
        record->MessageGuid   = *MessageGuid;
    }

    //
    // Move variable part of data into log.
    //
    {
        va_list  ap;
        size_t   argLen;
        PVOID    source;
        PUCHAR   argsData;

        argsData = (UCHAR*) &record[1];

        va_start(ap, MessageNumber);

        while ((source = va_arg(ap, PVOID)) != NULL) {

            argLen = va_arg(ap, size_t);

            if (argLen > 0) {

                RtlCopyMemory( argsData, source, argLen );
                argsData += argLen;
            }
        }

        va_end(ap);
    }

    FxVerifyLogHeader(FxDriverGlobals, header);

    return STATUS_SUCCESS;

    {
        //
        // Increment sequence number to indicate dropped message
        //
drop_message:
        header = (PWDF_IFR_HEADER) FxDriverGlobals->WdfLogHeader;
        InterlockedIncrement( &header->Sequence );
        return STATUS_UNSUCCESSFUL;
    }
}
