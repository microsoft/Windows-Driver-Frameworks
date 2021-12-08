/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxCxDeviceInitApi.cpp

Abstract:

    This module exposes the "C" interface to the FxDevice object
    for the class extensions.

Author:



Environment:

    Both kernel and user mode

Revision History:



--*/

#include "coreprivshared.hpp"

extern "C" {
#include "FxCxDeviceInitApi.tmh"
}

//
// Extern "C" the entire file
//
extern "C" {

__inline
static
NTSTATUS
FxValiateCx(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in PFX_DRIVER_GLOBALS CxDriverGlobals
    )
{
    NTSTATUS status = STATUS_SUCCESS;

    if (FxIsClassExtension(FxDriverGlobals, CxDriverGlobals) == FALSE) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        DoTraceLevelMessage(FxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                            "This function can only be called by a WDF "
                            "extension driver, Driver 0x%p, %!STATUS!",
                            CxDriverGlobals->Public.Driver, status);
        FxVerifierDbgBreakPoint(FxDriverGlobals);
    }

    return status;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
WDFEXPORT(WdfCxDeviceInitAssignWdmIrpPreprocessCallback)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    PWDFCXDEVICE_INIT   CxDeviceInit,
    __in
    PFN_WDFCXDEVICE_WDM_IRP_PREPROCESS EvtCxDeviceWdmIrpPreprocess,
    __in
    UCHAR MajorFunction,
    __drv_when(NumMinorFunctions > 0, __in_bcount(NumMinorFunctions))
    __drv_when(NumMinorFunctions == 0, __in_opt)
    PUCHAR MinorFunctions,
    __in
    ULONG NumMinorFunctions
    )
{
    DDI_ENTRY();

    PFX_DRIVER_GLOBALS fxDriverGlobals;
    PFX_DRIVER_GLOBALS cxDriverGlobals;
    NTSTATUS status;

    cxDriverGlobals = GetFxDriverGlobals(DriverGlobals);
    FxPointerNotNull(cxDriverGlobals, CxDeviceInit);
    fxDriverGlobals = CxDeviceInit->ClientDriverGlobals;

    //
    // Caller must be a class extension driver.
    //
    status = FxValiateCx(fxDriverGlobals, cxDriverGlobals);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    FxPointerNotNull(fxDriverGlobals, EvtCxDeviceWdmIrpPreprocess);

    if (NumMinorFunctions > 0) {
        FxPointerNotNull(fxDriverGlobals, MinorFunctions);
    }

    //
    // ARRAY_SIZE(CxDeviceInit->PreprocessInfo->Dispatch) just returns a
    // constant size, it does not actually deref PreprocessInfo (which could
    // be NULL)
    //
    if (MajorFunction >= ARRAY_SIZE(CxDeviceInit->PreprocessInfo->Dispatch)) {
        status = STATUS_INVALID_PARAMETER;
        DoTraceLevelMessage(fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                            "MajorFunction %x is invalid, %!STATUS!",
                            (ULONG)MajorFunction, status);

        goto Done;
    }

    //
    // CX must call this API multiple times if it wants to register preprocess callbacks for
    // multiple IRP major codes.
    //
    if (CxDeviceInit->PreprocessInfo == NULL) {
        CxDeviceInit->PreprocessInfo = new(fxDriverGlobals) FxIrpPreprocessInfo();
        if (CxDeviceInit->PreprocessInfo == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            DoTraceLevelMessage(fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                                "Couldn't create object PreprocessInfo, "
                                "%!STATUS!", status);

            goto Done;
        }
        CxDeviceInit->PreprocessInfo->ClassExtension = TRUE;
    }

    ASSERT(CxDeviceInit->PreprocessInfo->ClassExtension);

    if (NumMinorFunctions > 0) {
        if (CxDeviceInit->PreprocessInfo->Dispatch[MajorFunction].NumMinorFunctions != 0) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            DoTraceLevelMessage(fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                                "Already assigned Minorfunctions, %!STATUS!",
                                status);
            goto Done;
        }

        CxDeviceInit->PreprocessInfo->Dispatch[MajorFunction].MinorFunctions =
            (PUCHAR) FxPoolAllocate2(fxDriverGlobals,
                                     POOL_FLAG_NON_PAGED,
                                     sizeof(UCHAR) * NumMinorFunctions);

        if (CxDeviceInit->PreprocessInfo->Dispatch[MajorFunction].MinorFunctions == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            DoTraceLevelMessage(fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                                "Couldn't create object MinorFunctions, "
                                "%!STATUS!", status);
            goto Done;
        }

        RtlCopyMemory(
            &CxDeviceInit->PreprocessInfo->Dispatch[MajorFunction].MinorFunctions[0],
            &MinorFunctions[0],
            NumMinorFunctions
            );

        CxDeviceInit->PreprocessInfo->Dispatch[MajorFunction].NumMinorFunctions =
            NumMinorFunctions;
    }

    CxDeviceInit->PreprocessInfo->Dispatch[MajorFunction].EvtCxDevicePreprocess =
        EvtCxDeviceWdmIrpPreprocess;

    status = STATUS_SUCCESS;

Done:
    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
WDFEXPORT(WdfCxDeviceInitSetIoInCallerContextCallback)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    PWDFCXDEVICE_INIT CxDeviceInit,
    __in
    PFN_WDF_IO_IN_CALLER_CONTEXT EvtIoInCallerContext
    )

/*++

Routine Description:

    Registers an I/O pre-processing callback for the class extension device.

    If registered, any I/O for the device is first presented to this
    callback function before being placed in any I/O Queue's.

    The callback is invoked in the thread and/or DPC context of the
    original WDM caller as presented to the I/O package. No framework
    threading, locking, synchronization, or queuing occurs, and
    responsibility for synchronization is up to the device driver.

    This API is intended to support METHOD_NEITHER IRP_MJ_DEVICE_CONTROL's
    which must access the user buffer in the original callers context. The
    driver would probe and lock the buffer pages from within this event
    handler using the functions supplied on the WDFREQUEST object, storing
    any required mapped buffers and/or pointers on the WDFREQUEST context
    whose size is set by the RequestContextSize of the WDF_DRIVER_CONFIG structure.

    It is the responsibility of this routine to either complete the request, or
    pass it on to the I/O package through WdfDeviceEnqueueRequest(Device, Request).

Arguments:

    CxDeviceInit - Class Extension Device initialization structure

    EvtIoInCallerContext - Pointer to driver supplied callback function

Return Value:

    None

--*/
{
    DDI_ENTRY();

    PFX_DRIVER_GLOBALS fxDriverGlobals;
    PFX_DRIVER_GLOBALS cxDriverGlobals;
    NTSTATUS status;

    cxDriverGlobals = GetFxDriverGlobals(DriverGlobals);
    FxPointerNotNull(cxDriverGlobals, CxDeviceInit);
    fxDriverGlobals = CxDeviceInit->ClientDriverGlobals;

    //
    // Caller must be a class extension driver.
    //
    status = FxValiateCx(fxDriverGlobals, cxDriverGlobals);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    FxPointerNotNull(fxDriverGlobals, EvtIoInCallerContext);

    CxDeviceInit->IoInCallerContextCallback = EvtIoInCallerContext;

Done:
    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
WDFEXPORT(WdfCxDeviceInitSetRequestAttributes)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    PWDFCXDEVICE_INIT CxDeviceInit,
    __in
    PWDF_OBJECT_ATTRIBUTES RequestAttributes
    )
{
    DDI_ENTRY();

    PFX_DRIVER_GLOBALS fxDriverGlobals;
    PFX_DRIVER_GLOBALS cxDriverGlobals;
    NTSTATUS status;

    cxDriverGlobals = GetFxDriverGlobals(DriverGlobals);
    FxPointerNotNull(cxDriverGlobals, CxDeviceInit);
    fxDriverGlobals = CxDeviceInit->ClientDriverGlobals;

    //
    // Caller must be a class extension driver.
    //
    status = FxValiateCx(fxDriverGlobals, cxDriverGlobals);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    FxPointerNotNull(fxDriverGlobals, RequestAttributes);

    //
    // Parent of all requests created from WDFDEVICE are parented by the WDFDEVICE.
    //
    status = FxValidateObjectAttributes(fxDriverGlobals,
                                        RequestAttributes,
                                        FX_VALIDATE_OPTION_PARENT_NOT_ALLOWED);

    if (!NT_SUCCESS(status)) {
        FxVerifierDbgBreakPoint(fxDriverGlobals);
        return;
    }

    RtlCopyMemory(&CxDeviceInit->RequestAttributes,
                  RequestAttributes,
                  sizeof(WDF_OBJECT_ATTRIBUTES));

Done:
    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
WDFEXPORT(WdfCxDeviceInitSetFileObjectConfig)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    PWDFCXDEVICE_INIT CxDeviceInit,
    __in
    PWDFCX_FILEOBJECT_CONFIG CxFileObjectConfig,
    __in_opt
    PWDF_OBJECT_ATTRIBUTES FileObjectAttributes
    )

/*++

Routine Description:

    Registers file object callbacks for class extensions.

    Defaults to WdfFileObjectNotRequired if no file obj config set.

Arguments:

Returns:

--*/

{
    DDI_ENTRY();

    PFX_DRIVER_GLOBALS fxDriverGlobals;
    PFX_DRIVER_GLOBALS cxDriverGlobals;
    NTSTATUS status;
    WDF_FILEOBJECT_CLASS normalizedFileClass;

    cxDriverGlobals = GetFxDriverGlobals(DriverGlobals);
    FxPointerNotNull(cxDriverGlobals, CxDeviceInit);
    fxDriverGlobals = CxDeviceInit->ClientDriverGlobals;

    //
    // Caller must be a class extension driver.
    //
    status = FxValiateCx(fxDriverGlobals, cxDriverGlobals);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    FxPointerNotNull(fxDriverGlobals, CxFileObjectConfig);

    if (CxFileObjectConfig->Size != sizeof(WDFCX_FILEOBJECT_CONFIG)) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid CxFileObjectConfig Size %d, expected %d",
            CxFileObjectConfig->Size, sizeof(WDFCX_FILEOBJECT_CONFIG));

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    status = FxValidateObjectAttributes(
        fxDriverGlobals,
        FileObjectAttributes,
        FX_VALIDATE_OPTION_PARENT_NOT_ALLOWED |
            FX_VALIDATE_OPTION_EXECUTION_LEVEL_ALLOWED |
            FX_VALIDATE_OPTION_SYNCHRONIZATION_SCOPE_ALLOWED
        );

    if (!NT_SUCCESS(status)) {
        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    //
    // Validate AutoForwardCleanupClose.
    //
    switch (CxFileObjectConfig->AutoForwardCleanupClose) {
    case WdfTrue:
    case WdfFalse:
    case WdfUseDefault:
        break;

    default:
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid CxFileObjectConfig->AutoForwardCleanupClose value 0x%x, "
            "expected WDF_TRI_STATE value",
            CxFileObjectConfig->AutoForwardCleanupClose);

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    CxDeviceInit->FileObject.Set = TRUE;

    CxDeviceInit->FileObject.AutoForwardCleanupClose =
        CxFileObjectConfig->AutoForwardCleanupClose;

    //
    // Remove bit flags and validate file object class value.
    //
    normalizedFileClass = FxFileObjectClassNormalize(
                                CxFileObjectConfig->FileObjectClass);

    if (normalizedFileClass == WdfFileObjectInvalid ||
        normalizedFileClass > WdfFileObjectWdfCannotUseFsContexts)  {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Out of range CxFileObjectConfig->FileObjectClass %d",
            CxFileObjectConfig->FileObjectClass);
        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    //
    // The optional flag can only be combined with a subset of values.
    //
    if (FxIsFileObjectOptional(CxFileObjectConfig->FileObjectClass)) {
        switch(normalizedFileClass) {
        case WdfFileObjectWdfCanUseFsContext:
        case WdfFileObjectWdfCanUseFsContext2:
        case WdfFileObjectWdfCannotUseFsContexts:
            break;

        default:
            DoTraceLevelMessage(
                fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                "Invalid CxFileObjectConfig->FileObjectClass %d",
                CxFileObjectConfig->FileObjectClass);
            FxVerifierDbgBreakPoint(fxDriverGlobals);
            goto Done;
            break; // just in case static verification tools complain.
        }
    }

    CxDeviceInit->FileObject.Class = CxFileObjectConfig->FileObjectClass;

    RtlCopyMemory(&CxDeviceInit->FileObject.Callbacks,
                  CxFileObjectConfig,
                  sizeof(CxDeviceInit->FileObject.Callbacks));

    if (FileObjectAttributes != NULL) {
        RtlCopyMemory(&CxDeviceInit->FileObject.Attributes,
                      FileObjectAttributes,
                      sizeof(CxDeviceInit->FileObject.Attributes));
    }

Done:
    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
WDFEXPORT(WdfCxDeviceInitSetPnpPowerEventCallbacks)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFCXDEVICE_INIT CxDeviceInit,
    _In_
    PWDFCX_PNPPOWER_EVENT_CALLBACKS CxPnpPowerCallbacks
    )

/*++

Routine Description:

    Registers PNP & Power callbacks for class extensions.


Arguments:

    CxDeviceInit - Class Extension Device initialization structure

    CxPnpPowerCallbacks - Pointer to a Cx supplied structure.

Returns:

    VOID

--*/

{
    DDI_ENTRY();

    PFX_DRIVER_GLOBALS fxDriverGlobals;
    PFX_DRIVER_GLOBALS cxDriverGlobals;
    NTSTATUS status;

    cxDriverGlobals = GetFxDriverGlobals(DriverGlobals);
    FxPointerNotNull(cxDriverGlobals, CxDeviceInit);
    fxDriverGlobals = CxDeviceInit->ClientDriverGlobals;

    //
    // Caller must be a class extension driver.
    //
    status = FxValiateCx(fxDriverGlobals, cxDriverGlobals);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    FxPointerNotNull(fxDriverGlobals, CxPnpPowerCallbacks);

    if (CxPnpPowerCallbacks->Size != sizeof(WDFCX_PNPPOWER_EVENT_CALLBACKS) &&
        CxPnpPowerCallbacks->Size != sizeof(WDFCX_PNPPOWER_EVENT_CALLBACKS_V1_29)) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid Size in WDFCX_PNPPOWER_EVENT_CALLBACKS: %d, "
            "expected v29 size %d or cur ver size %d",
            CxPnpPowerCallbacks->Size,
            sizeof(WDFCX_PNPPOWER_EVENT_CALLBACKS_V1_29),
            sizeof(WDFCX_PNPPOWER_EVENT_CALLBACKS));

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    if (CxPnpPowerCallbacks->EvtCxDevicePrePrepareHardware == NULL &&
        CxPnpPowerCallbacks->EvtCxDevicePrePrepareHardwareFailedCleanup != NULL) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid WDFCX_PNPPOWER_EVENT_CALLBACKS configuration. Can not specify "
            "EvtCxDevicePrePrepareHardwareFailedCleanup if "
            "EvtCxDevicePrePrepareHardware is set to NULL");

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    if (CxPnpPowerCallbacks->EvtCxDevicePreD0Entry == NULL &&
        CxPnpPowerCallbacks->EvtCxDevicePreD0EntryFailedCleanup != NULL) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid WDFCX_PNPPOWER_EVENT_CALLBACKS configuration. Can not specify "
            "EvtCxDevicePreD0EntryFailedCleanup if "
            "EvtCxDevicePreD0Entry is set to NULL");

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    if (CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoInit == NULL &&
        CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoInitFailedCleanup != NULL) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid WDFCX_PNPPOWER_EVENT_CALLBACKS configuration. Can not specify "
            "EvtCxDevicePreSelfManagedIoInitFailedCleanup if "
            "EvtCxDevicePreSelfManagedIoInit is set to NULL");

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    if (CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestart == NULL &&
        CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestartFailedCleanup != NULL) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid WDFCX_PNPPOWER_EVENT_CALLBACKS configuration. Can not specify "
            "EvtCxDevicePreSelfManagedIoRestartFailedCleanup if "
            "EvtCxDevicePreSelfManagedIoRestart is set to NULL");

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    if (CxPnpPowerCallbacks->Size > sizeof(WDFCX_PNPPOWER_EVENT_CALLBACKS_V1_29)) {

        if (CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestartEx == NULL &&
            CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestartExFailedCleanup != NULL) {
            DoTraceLevelMessage(
                fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                "Invalid WDFCX_PNPPOWER_EVENT_CALLBACKS configuration. Can not specify "
                "EvtCxDevicePreSelfManagedIoRestartExFailedCleanup if "
                "EvtCxDevicePreSelfManagedIoRestartEx is set to NULL");

            FxVerifierDbgBreakPoint(fxDriverGlobals);
            goto Done;
        }

        if ((CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestart != NULL ||
            CxPnpPowerCallbacks->EvtCxDevicePostSelfManagedIoRestart != NULL ||
            CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestartFailedCleanup != NULL)
            &&
            (CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestartEx != NULL ||
            CxPnpPowerCallbacks->EvtCxDevicePostSelfManagedIoRestartEx != NULL ||
            CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestartExFailedCleanup != NULL)) {
            DoTraceLevelMessage(
                fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                "Invalid WDFCX_PNPPOWER_EVENT_CALLBACKS configuration. Can not specify "
                "both EvtCxDevicePre/PostSelfManagedIoRestart "
                "and  EvtCxDevicePre/PostSelfManagedIoRestartEx");

            FxVerifierDbgBreakPoint(fxDriverGlobals);
            goto Done;
        }

        if ((CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoSuspend != NULL ||
            CxPnpPowerCallbacks->EvtCxDevicePostSelfManagedIoSuspend != NULL)
            &&
            (CxPnpPowerCallbacks->EvtCxDevicePreSelfManagedIoSuspendEx != NULL ||
            CxPnpPowerCallbacks->EvtCxDevicePostSelfManagedIoSuspendEx != NULL)) {
            DoTraceLevelMessage(
                fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                "Invalid WDFCX_PNPPOWER_EVENT_CALLBACKS configuration. Can not specify "
                "both EvtCxDevicePre/PostSelfManagedIoSuspend "
                "and  EvtCxDevicePre/PostSelfManagedIoSuspendEx");

            FxVerifierDbgBreakPoint(fxDriverGlobals);
            goto Done;
        }

        if (CxPnpPowerCallbacks->EvtCxDevicePreD0EntryPostHardwareEnabled == NULL &&
            CxPnpPowerCallbacks->EvtCxDevicePreD0EntryPostHardwareEnabledFailedCleanup != NULL) {
            DoTraceLevelMessage(
                fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                "Invalid WDFCX_PNPPOWER_EVENT_CALLBACKS configuration. Can not specify "
                "EvtCxDevicePreD0EntryPostHardwareEnabledFailedCleanup if "
                "EvtCxDevicePreD0EntryPostHardwareEnabled is set to NULL");

            FxVerifierDbgBreakPoint(fxDriverGlobals);
            goto Done;
        }
    }

    CxDeviceInit->PnpPowerCallbacks.Set = TRUE;

    RtlZeroMemory(&CxDeviceInit->PnpPowerCallbacks.PnpPowerCallbacks,
                  sizeof(CxDeviceInit->PnpPowerCallbacks.PnpPowerCallbacks));

    RtlCopyMemory(&CxDeviceInit->PnpPowerCallbacks.PnpPowerCallbacks,
                  CxPnpPowerCallbacks,
                  CxPnpPowerCallbacks->Size);

Done:
    return;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
WDFEXPORT(WdfCxDeviceInitSetPowerPolicyEventCallbacks)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFCXDEVICE_INIT CxDeviceInit,
    _In_
    PWDFCX_POWER_POLICY_EVENT_CALLBACKS CxPowerPolicyCallbacks
    )

/*++

Routine Description:

    Register a class extention's Power Policy event callbacks.

    Only class extensions are allowed to call the API.

Arguments:

    CxDeviceInit - A caller-supplied pointer to a WDFCXDEVICE_INIT structure.

    CxPowerPolicyCallbacks - A pointer to a caller-initialized WDFCX_POWER_POLICY_EVENT_CALLBACKS structure.

Returns:

    VOID

--*/

{
    DDI_ENTRY();

    PFX_DRIVER_GLOBALS fxDriverGlobals;
    PFX_DRIVER_GLOBALS cxDriverGlobals;
    NTSTATUS status;

    cxDriverGlobals = GetFxDriverGlobals(DriverGlobals);
    FxPointerNotNull(cxDriverGlobals, CxDeviceInit);
    fxDriverGlobals = CxDeviceInit->ClientDriverGlobals;

    //
    // Caller must be a class extension driver.
    //
    status = FxValiateCx(fxDriverGlobals, cxDriverGlobals);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    FxPointerNotNull(fxDriverGlobals, CxPowerPolicyCallbacks);

    if (CxPowerPolicyCallbacks->Size != sizeof(WDFCX_POWER_POLICY_EVENT_CALLBACKS)) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid Size in WDFCX_POWER_POLICY_EVENT_CALLBACKS: %d, expected %d",
            CxPowerPolicyCallbacks->Size, sizeof(WDFCX_POWER_POLICY_EVENT_CALLBACKS));

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    if (CxPowerPolicyCallbacks->EvtCxDevicePreArmWakeFromS0 == NULL &&
        CxPowerPolicyCallbacks->EvtCxDevicePreArmWakeFromS0FailedCleanup != NULL) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid WDFCX_POWER_POLICY_EVENT_CALLBACKS configuration. Can not specify "
            "EvtCxDevicePreArmWakeFromS0FailedCleanup if "
            "EvtCxDevicePreArmWakeFromS0 is set to NULL");

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    if (CxPowerPolicyCallbacks->EvtCxDevicePreArmWakeFromSx == NULL &&
        CxPowerPolicyCallbacks->EvtCxDevicePreArmWakeFromSxFailedCleanup != NULL) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid WDFCX_POWER_POLICY_EVENT_CALLBACKS configuration. Can not specify "
            "EvtCxDevicePreArmWakeFromSxFailedCleanup if "
            "EvtCxDevicePreArmWakeFromSx is set to NULL");

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    if (CxPowerPolicyCallbacks->EvtCxDevicePreArmWakeFromSxWithReason == NULL &&
        CxPowerPolicyCallbacks->EvtCxDevicePreArmWakeFromSxWithReasonFailedCleanup != NULL) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid WDFCX_POWER_POLICY_EVENT_CALLBACKS configuration. Can not specify "
            "EvtCxDevicePreArmWakeFromSxWithReasonFailedCleanup if "
            "EvtCxDevicePreArmWakeFromSxWithReason is set to NULL");

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    if ((CxPowerPolicyCallbacks->EvtCxDevicePreArmWakeFromSx != NULL ||
         CxPowerPolicyCallbacks->EvtCxDevicePreArmWakeFromSxFailedCleanup != NULL ||
         CxPowerPolicyCallbacks->EvtCxDevicePostArmWakeFromSx != NULL)
        &&
        (CxPowerPolicyCallbacks->EvtCxDevicePreArmWakeFromSxWithReason != NULL ||
         CxPowerPolicyCallbacks->EvtCxDevicePreArmWakeFromSxWithReasonFailedCleanup != NULL ||
         CxPowerPolicyCallbacks->EvtCxDevicePostArmWakeFromSxWithReason != NULL)
        ) {
        DoTraceLevelMessage(
            fxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Invalid WDFCX_POWER_POLICY_EVENT_CALLBACKS configuration. Can not specify "
            "both ArmWakeFromSx and ArmWakeFromSxWithReason");

        FxVerifierDbgBreakPoint(fxDriverGlobals);
        goto Done;
    }

    CxDeviceInit->PnpPowerCallbacks.Set = TRUE;

    RtlCopyMemory(&CxDeviceInit->PnpPowerCallbacks.PowerPolicyCallbacks,
                  CxPowerPolicyCallbacks,
                  sizeof(CxDeviceInit->PnpPowerCallbacks.PowerPolicyCallbacks));

Done:
    return;
}

_Must_inspect_result_
__drv_maxIRQL(PASSIVE_LEVEL)
PWDFCXDEVICE_INIT
WDFEXPORT(WdfCxDeviceInitAllocate)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    PWDFDEVICE_INIT DeviceInit
    )
{
    DDI_ENTRY();

    PFX_DRIVER_GLOBALS cxDriverGlobals;
    PFX_DRIVER_GLOBALS fxDriverGlobals;
    PWDFCXDEVICE_INIT  cxDeviceInit;
    NTSTATUS status;

    cxDriverGlobals = GetFxDriverGlobals(DriverGlobals);
    FxPointerNotNull(cxDriverGlobals, DeviceInit);
    fxDriverGlobals = DeviceInit->DriverGlobals;
    cxDeviceInit = NULL;

    //
    // Caller must be a class extension driver.
    //
    status = FxValiateCx(fxDriverGlobals, cxDriverGlobals);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    status = FxVerifierCheckIrqlLevel(fxDriverGlobals, PASSIVE_LEVEL);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    cxDeviceInit = WDFCXDEVICE_INIT::_AllocateCxDeviceInit(DeviceInit);
    if (NULL == cxDeviceInit) {
        goto Done;
    }

    cxDeviceInit->ClientDriverGlobals = fxDriverGlobals;
    cxDeviceInit->CxDriverGlobals = cxDriverGlobals;

Done:
    return cxDeviceInit;
}

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfCxDeviceInitAllocateContext)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_OBJECT_ATTRIBUTES ContextAttributes,
    _Outptr_opt_
    PVOID* Context
    )
/*++

Routine Description:

    WDF extension driver calls this routine to allocate a typed context and attach
    it to DeviceInit. The typed context can be retrieved back using either
     - WdfCxDeviceInitGetTypedContext(DeviceInit, TYPE) during initialization,
     - or later after a WDFDEVICE has been successfully created, either
       * WdfObjectGetTypedContext(WDFDEVICE, TYPE), or
       * type-casting-function(WDFDEVICE) if such a function is defined with
         WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(TYPE, type-casting-function).

Arguments:

    DeviceInit - A pointer to an opaque WDFDEVICE_INIT structure. The client
                driver typically gets such a pointer in its EvtDriverDeviceAdd
                callback, and then passes it to the extension driver when calling
                extension-specific APIs.

    ContextAttributes - A pointer to a caller-supplied WDF_OBJECT_ATTRIBUTES
                structure that describes the context type.

    Context - A pointer to a location that receives a pointer to the allocated,
                zero-initialized typed context. If the call fails, the returned
                pointer will be NULL.

Return Value:

    STATUS_SUCCESS upon success.

    STATUS_OBJECT_NAME_EXISTS (0x40000000) - The extension driver has already
                allocated a context of the same type. In this case, the call
                returns a pointer to the existing typed context and does not
                allocate a duplicated one.

    !NT_SUCCESS on failure. The returned pointer in Context will be NULL.

Remarks:

    The API is similar to WdfObjectAllocateContext.

    The API should only be called by WDF extension drivers.

    If succeeded, a typed context whose type is specified in the ContextAttributes
    will be allocated and zero initialized. The typed context is initially owned
    by an internal WDF object; later if a WDFDEVICE is successfully created using
    the said DeviceInit, the typed context will be owned by the WDFDEVICE. The typed
    context will be deleted when its owner is deleted.

    EvtCleanupCallback and EvtDestroyCallback can be specified in ContextAttributes.
    In the two callbacks always use WdfObjectGetTypedContext(WDFOBJECT, TYPE) to
    retrieve the typed context. Do not assume the WDFOBJECT is a valid WDFDEVICE,
    because it could be an internal WDF object as mentioned above if device creation
    fails. If the two callbacks need a valid WDFDEVICE handle, the class extension
    must store the handle inside the typed context by itself.

Example:

    struct NET_INIT_CONTXT { ... };
    WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(NET_INIT_CONTXT, NetDeviceGetInitContext);

    NetDeviceInit(PWDFDEVICE_INIT DeviceInit)
    {
      WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attr, NET_INIT_CONTXT);
      attr.EvtCleanupCallback = EvtNetDeviceInitCleanup;
      WdfCxDeviceInitAllocateContext(DeviceInit, &attr, &NetInitContext);
    }

    NetDeviceInitSetResetConfig(
      PWDFDEVICE_INIT          DeviceInit,
      NET_DEVICE_RESET_CONFIG  ResetConfig
      )
    {
      NetInitContext = WdfCxDeviceInitGetTypedContext(DeviceInit, NET_INIT_CONTXT);
      NetInitContext->ResetConfig = ResetConfig;
    }

    NetDevicePrepareHardware(WDFDEVICE Device, ..)
    {
      NetInitContext = NetDeviceGetInitContext(Device);
      ... Use NetInitContext->ResetConfig ...
    }

    EvtNetDeviceInitCleanup(WDFOBJECT Object)
    {
      NetInitContext = (NET_INIT_CONTXT*)WdfObjectGetTypedContext(Object, NET_INIT_CONTXT);
      ... Cleanup NetInitContext ...
    }

--*/
{
    DDI_ENTRY();

    PFX_DRIVER_GLOBALS cxDriverGlobals;
    PFX_DRIVER_GLOBALS fxDriverGlobals;
    NTSTATUS status;

    cxDriverGlobals = GetFxDriverGlobals(DriverGlobals);
    FxPointerNotNull(cxDriverGlobals, DeviceInit);
    fxDriverGlobals = DeviceInit->DriverGlobals;

    //
    // Caller must be a class extension driver.
    //
    status = FxValiateCx(fxDriverGlobals, cxDriverGlobals);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    status = DeviceInit->AllocateCxContext(cxDriverGlobals,
                                           ContextAttributes,
                                           Context);

Done:
    return status;
}

_IRQL_requires_max_(DISPATCH_LEVEL+1)
WDFAPI
PVOID
NTAPI
WDFEXPORT(WdfCxDeviceInitGetTypedContextWorker)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PCWDF_OBJECT_CONTEXT_TYPE_INFO TypeInfo
    )
/*++

Routine Description:

    This is the helper routine for WDF macro:

        (TYPE*)
        WdfCxDeviceInitGetTypedContext(
            _In_ PWDFDEVICE_INIT DeviceInit,
            _In_ TYPE
            );

    WDF extension driver calls this routine to get back the typed context which
    was previously allocated using WdfCxDeviceInitAllocateContext.

Arguments:

    DeviceInit - A pointer to an opaque WDFDEVICE_INIT structure. The client
                driver typically gets such a pointer in its EvtDriverDeviceAdd
                callback, and then passes it to the extension driver when calling
                extension-specific APIs.

    Type - The symbol name of a structure defined by the extension driver that
                describes the context type. Use either WDF_DECLARE_CONTEXT_TYPE
                or WDF_DECLARE_CONTEXT_TYPE_WITH_NAME to declare the type.

Return Value:

    Return a pointer to the allocated typed context, or NULL if the extension
    driver did not call WdfCxDeviceInitAllocateContext with the said type.

Remarks:

    The API is similar to WdfObjectGetTypedContext.

    The API should only be called by WDF extension drivers.

    The API returns the typed context given a DeivceInit. On the other hand, if
    you have a valid WDFDEVICE handle, call either WdfObjectGetTypedContext or the
    casting function as defined in WDF_DECLARE_CONTEXT_TYPE_WITH_NAME instead.
    The third case is that in EvtCleanupCallback and EvtDestroyCallback, you need
    to always call WdfObjectGetTypedContext(WDFOBJECT).

    Refer to WdfCxDeviceInitAllocateContext for examples.

--*/
{
    DDI_ENTRY();

    PFX_DRIVER_GLOBALS cxDriverGlobals;
    PFX_DRIVER_GLOBALS fxDriverGlobals;
    NTSTATUS status;
    PVOID context;

    cxDriverGlobals = GetFxDriverGlobals(DriverGlobals);
    FxPointerNotNull(cxDriverGlobals, DeviceInit);
    fxDriverGlobals = DeviceInit->DriverGlobals;
    context = NULL;

    //
    // Caller must be a class extension driver.
    //
    status = FxValiateCx(fxDriverGlobals, cxDriverGlobals);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    context = DeviceInit->GetCxTypedContext(TypeInfo);

Done:
    return context;
}

} // extern "C"

