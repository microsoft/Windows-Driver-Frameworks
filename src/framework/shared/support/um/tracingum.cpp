/*++

Copyright (c) Microsoft Corporation

Module Name:

    Tracing.cpp

Abstract:

    This module implements tracing for the driver frameworks

Author:




Revision History:




--*/

#include "FxSupportPch.hpp"
#include "fxldrum.h"
#include <FeatureStaging-WDF.h>

extern "C" {
#if defined(EVENT_TRACING)
#include "TracingUM.tmh"
#endif
}

#include <strsafe.h>
#include <initguid.h>
#include "fxIFR.h"       // shared struct between IFR and debug ext.
#include "fxIFRKm.h"     // kernel mode only IFR definitions

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
    // Initialize the tracing package.
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
    WPP_CLEANUP(NULL);
}

_Must_inspect_result_
NTSTATUS
FxWmiQueryTraceInformation(
    __in TRACE_INFORMATION_CLASS TraceInformationClass,
    __out_bcount(TraceInformationLength) PVOID TraceInformation,
    __in ULONG TraceInformationLength,
    __out_opt PULONG RequiredLength,
    __in_opt PVOID Buffer
    )
{
    UNREFERENCED_PARAMETER(TraceInformationClass);
    UNREFERENCED_PARAMETER(TraceInformation);
    UNREFERENCED_PARAMETER(TraceInformationLength);
    UNREFERENCED_PARAMETER(RequiredLength);
    UNREFERENCED_PARAMETER(Buffer);

    return STATUS_UNSUCCESSFUL;
}

_Must_inspect_result_
NTSTATUS
FxWmiTraceMessage(
    __in TRACEHANDLE  LoggerHandle,
    __in ULONG        MessageFlags,
    __in LPGUID       MessageGuid,
    __in USHORT       MessageNumber,
         ...
    )
{
    NTSTATUS status = STATUS_SUCCESS;

    va_list va;
    va_start(va, MessageNumber);
    //
    // UMDF is supported only on XP and newer OS so no need to support w2k
    // tracing (which requires using a different tracing api, see kmdf impl)
    //
    status = TraceMessageVa(LoggerHandle,
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

VOID
FxIFRGetSettings(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in PCUNICODE_STRING RegistryPath,
    __out ULONG * Size,
    __out BOOLEAN * UseTimeStamp,
    __out BOOLEAN * PreciseTimeStamp
    )
/*++

Routine Description:
    Checks to see if the service has overriden the default number of pages that
    are in the IFR.

Arguments:
    RegistryPath - path to the service
    [out] Size - The size of the IFR to create in bytes (not pages!)
    [out] UseTimeStamp - Whether to store timestamp or not
    [out] PreciseTimeStamp - Use precise timestamp.

  --*/
{
    FxAutoRegKey service, parameters;
    NTSTATUS status;
    ULONG numPages;
    ULONG regValue;
    BOOLEAN useTimeStamp;
    BOOLEAN preciseTimeStamp;

    //
    // This is the value used in case of any error while retrieving 'LogPages'
    // from the registry.
    //
    numPages  = FxIFRMinLogPages;

    useTimeStamp = TRUE;
    preciseTimeStamp = FALSE;

    //
    // External representation of the IFR is the "log", so use tha term when
    // overriding the size via the registry.
    //
    DECLARE_CONST_UNICODE_STRING(parametersPath, L"Parameters\\Wdf");
    DECLARE_CONST_UNICODE_STRING(valueName, L"LogPages");
    DECLARE_CONST_UNICODE_STRING(nameTimeStamp, L"LogUseTimeStamp");
    DECLARE_CONST_UNICODE_STRING(namePreciseTimeStamp, L"LogPreciseTimeStamp");

    //
    // UMDF may not provide this registry path
    //
    if (NULL == RegistryPath) {
        goto defaultValues;
    }

    status = FxRegKey::_OpenKey(NULL,
                                (PCUNICODE_STRING)RegistryPath,
                                &service.m_Key,
                                KEY_READ);
    if (!NT_SUCCESS(status)) {
        goto defaultValues;
    }

    status = FxRegKey::_OpenKey(service.m_Key,
                                (PCUNICODE_STRING)&parametersPath,
                                &parameters.m_Key,
                                KEY_READ);
    if (!NT_SUCCESS(status)) {
        goto defaultValues;
    }

    status = FxRegKey::_QueryULong(parameters.m_Key, &valueName, &numPages);
    if (NT_SUCCESS(status)) {
        if (numPages == 0) {
            numPages = FxIFRMinLogPages;
        }
        //
        // This behavior is different from KMDF. KMDF would allocate Average page count (5)
        // if Verifier is on otherwise 1 page if the request is large.
        // Since for UMDF the memory is coming from WudfRd, which does not know about verifier
        // we will give it max pages here.
        //
        if (numPages > FxIFRMaxLogPages) {
            numPages = FxIFRMaxLogPages;
        }
    }

    status = FxRegKey::_QueryULong(parameters.m_Key, &nameTimeStamp, &regValue);
    if (NT_SUCCESS(status)) {
        useTimeStamp = (regValue != 0);
    }

    if (useTimeStamp) {
        status = FxRegKey::_QueryULong(parameters.m_Key, &namePreciseTimeStamp, &regValue);
        if (NT_SUCCESS(status)) {
            preciseTimeStamp = (regValue != 0);
        }
    }

defaultValues:

    *Size = numPages * PAGE_SIZE;
    *UseTimeStamp = useTimeStamp;
    *PreciseTimeStamp = preciseTimeStamp;
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

Arguments:

Returns:

--*/
{
    PWDF_IFR_HEADER pHeader;
    ULONG pageCount;
    ULONG sizeCb;
    HRESULT hr;
    IWudfDeviceStack2 *pDeviceStack2;
    IWudfCompanion* pHostCompanion;
    PDRIVER_OBJECT_UM umDriverObject;
    PWCHAR serviceName;
    size_t bufferLengthCch;
    ULONG allocatedBufferLengthCb;
    LONG i;
    DECLARE_UNICODE_STRING_SIZE(svcNameW, WDF_DRIVER_GLOBALS_NAME_LEN);
    NTSTATUS ntStatus;
    BOOLEAN useTimeStamp;
    BOOLEAN preciseTimeStamp;

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

    //
    // It is safe to use StringCchLength here as WudfHost makes sure that this
    // RegistryPath is null terminated
    //
    hr = StringCchLength(RegistryPath->Buffer,
                         RegistryPath->MaximumLength/sizeof(WCHAR),
                         &bufferLengthCch);

    if (FAILED(hr)) {
        return;
    }

    //
    // Lets find the last '\' that will mark the begining of the service name
    //
    for (i = (ULONG)bufferLengthCch - 1;
        i >= 0 && RegistryPath->Buffer[i] != '\\';
        i--);

    //
    // We did not find the service name
    //
    if (i < 0) {
        return;
    }

    serviceName = &RegistryPath->Buffer[i+1];

    FxIFRGetSettings(FxDriverGlobals, RegistryPath, &sizeCb,
                     &useTimeStamp, &preciseTimeStamp);

    pageCount = sizeCb / PAGE_SIZE;
    umDriverObject = (PDRIVER_OBJECT_UM)DriverObject;

    if (FxDriverGlobals->IsCompanion()) {
        pHostCompanion = (IWudfCompanion*)umDriverObject->DriverLoadContext->Companion;
        if (pHostCompanion == NULL) {
            return;
        }
        allocatedBufferLengthCb = 0;
        pHeader = NULL;

        ntStatus = pHostCompanion->AllocateIfrMemory(serviceName,
                                              pageCount,
                                              FALSE,  // IsDriverIFR
                                              TRUE,   // Store in MiniDump
                                              (PVOID *)&pHeader,
                                              &allocatedBufferLengthCb);
        if (!NT_SUCCESS(ntStatus) || pHeader == NULL || allocatedBufferLengthCb <= sizeof(WDF_IFR_HEADER)) {
            return;
        }

    }
    else {
        //
        // Get the IWudfDeviceStack interface
        //
        pDeviceStack2 = (IWudfDeviceStack2 *)umDriverObject->DriverLoadContext->DeviceStack;

        if(pDeviceStack2 == NULL) {
            return;
        }

        allocatedBufferLengthCb = 0;
        hr = pDeviceStack2->AllocateIfrMemory(serviceName,
                                              pageCount,
                                              FALSE, // IsDriverIFR
                                              TRUE,  // Store in MiniDump
                                              (PVOID *)&pHeader,
                                              &allocatedBufferLengthCb);

        if (pHeader == NULL || allocatedBufferLengthCb <= sizeof(WDF_IFR_HEADER)) {
            return;
        }
    }
    //
    // Initialize the header.
    // Base will be where the IFR records are placed.
    // WPP_ThisDir_CTLGUID_FrameworksTraceGuid
    //
    RtlCopyMemory(&pHeader->Guid, (PVOID) &WdfTraceGuid, sizeof(GUID));

    pHeader->Base = (PUCHAR) &pHeader[1];
    pHeader->Size = allocatedBufferLengthCb - sizeof(WDF_IFR_HEADER);
    pHeader->UseTimeStamp = useTimeStamp;
    pHeader->PreciseTimeStamp = preciseTimeStamp;

    pHeader->Offset.u.s.Current  = 0;
    pHeader->Offset.u.s.Previous = 0;
    pHeader->SequenceNumberPointer = &(DriverObject->IfrSequenceNumber);

    StringCchCopyA(pHeader->DriverName, WDF_IFR_HEADER_NAME_LEN, FxDriverGlobals->Public.DriverName);

    FxDriverGlobals->WdfLogHeader = pHeader;

    DoTraceLevelMessage(FxDriverGlobals, TRACE_LEVEL_INFORMATION, TRACINGDRIVER,
                        "FxIFR logging started" );

    if (sizeCb > FxIFRMinLogSize) {
        DoTraceLevelMessage(
            FxDriverGlobals, TRACE_LEVEL_INFORMATION, TRACINGDRIVER,
            "FxIFR has been started with a size override:  size 0x%x bytes, "
            "# Pages %d.  An extended IFR size may not be written to a minidump!",
            sizeCb, sizeCb/PAGE_SIZE);
    }

    if (sizeCb != allocatedBufferLengthCb) {
        ASSERTMSG("FxIFR requested buffer size could not be allocated",FALSE);
        DoTraceLevelMessage(
            FxDriverGlobals, TRACE_LEVEL_WARNING, TRACINGDRIVER,
            "FxIFR requested an allocation of size 0x%x bytes, "
            "Allocated memory was of size 0x%x bytes",
            sizeCb, allocatedBufferLengthCb);
    }
}

VOID
FxIFRStop(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals
    )
/*++

Routine Description:

    This routine stops the In-Flight Recorder (IFR).

    For UMDF drivers FxIFRStop is not required as WudfRd manages the buffer's lifetime
    The buffer is kept alive till WudfRd unloads to aid in debugging in cases of
    WudfHost crash or in dumps with WudfHost paged out or not captured

    For UMDF companions FxIFRStop behaves similar to KMDF drivers. It frees the
    IFR buffer allocation, because it was allocated on the heap and not mapped to
    user mode via the reflector.

Arguments:
Returns:

--*/
{
    if (FxDriverGlobals->IsCompanion()) {
         if (FxLibraryGlobals.IfrDisabled) {
             ASSERT(FxDriverGlobals->WdfLogHeader == NULL);
             return;
         }

         delete[] FxDriverGlobals->WdfLogHeader;
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
         ...
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

























    UNREFERENCED_PARAMETER( MessageFlags );


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

    header = (PWDF_IFR_HEADER) FxDriverGlobals->WdfLogHeader;
    ASSERT(header->Size < FxIFRMaxLogSize); // size doesn't include header.
    ASSERT(header->Size >= header->Offset.u.s.Current);
    ASSERT(header->Size >= header->Offset.u.s.Previous);

    //
    // Allocate memory for timestamp only if necessary.
    //
    size_t recordSize = header->UseTimeStamp
                        ? sizeof(WDF_IFR_RECORD)
                        : sizeof(WDF_IFR_RECORD_V1);

    size += recordSize;

    //
    // Allocate log space of the calculated size
    //
    {
        WDF_IFR_OFFSET   offsetRet;
        WDF_IFR_OFFSET   offsetCur;
        WDF_IFR_OFFSET   offsetNew;
        USHORT           usSize = (USHORT) size;  // for a prefast artifact.


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
        record->Length        = (USHORT) size;
        record->PrevOffset    = (USHORT) offsetRet.u.s.Previous;
        record->MessageNumber = MessageNumber;
        record->Sequence      = InterlockedIncrement(header->SequenceNumberPointer);
        record->MessageGuid   = *MessageGuid;

        if (!header->UseTimeStamp) {
            record->Signature = WDF_IFR_RECORD_SIGNATURE_V1;
        } else {
            record->Signature = WDF_IFR_RECORD_SIGNATURE;
            LARGE_INTEGER timestamp;
            if (header->PreciseTimeStamp) {
                Mx::MxQuerySystemTimePrecise(&timestamp);
            }
            else {
                Mx::MxQuerySystemTime(&timestamp);
            }
            record->TimeStamp.LowPart  = timestamp.LowPart;
            record->TimeStamp.HighPart = timestamp.HighPart;
        }
    }

    //
    // Move variable part of data into log.
    //
    {
        va_list  ap;
        size_t   argLen;
        PVOID    source;
        PUCHAR   argsData;

        argsData = ((UCHAR*)record) + recordSize;

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

    return STATUS_SUCCESS;

    {
        //
        // Increment sequence number to indicate dropped message
        //
drop_message:
        PWDF_IFR_HEADER  header;
        header = (PWDF_IFR_HEADER) FxDriverGlobals->WdfLogHeader;
        InterlockedIncrement(header->SequenceNumberPointer);
        return STATUS_UNSUCCESSFUL;
    }
}

