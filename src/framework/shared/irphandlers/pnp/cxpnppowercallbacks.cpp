/*++
Copyright (c) Microsoft. All rights reserved.

Module Name:

    CxPnpPowerCallbacks.cpp

Abstract:

    This module implements the Class Extension callbacks.

Environment:
    Both kernel and user mode

Revision History:
--*/

#include "pnppriv.hpp"

extern "C" {
#if defined(EVENT_TRACING)
#include "CxPnpPowerCallbacks.tmh"
#endif
}

BOOLEAN
FxCxPnpPowerCallbackContext::IsPreCallbackPresent(
    VOID
    )
{
    return (NULL != u.Generic.PreCallback);
}

BOOLEAN
FxCxPnpPowerCallbackContext::IsPostCallbackPresent(
    VOID
    )
{
    return (NULL != u.Generic.PostCallback);
}

BOOLEAN
FxCxPnpPowerCallbackContext::IsCleanupCallbackPresent(
    VOID
    )
{
    return (NULL != u.Generic.CleanupCallback);
}

VOID
FxPrePostCallback::_SaveTheFirstError(
    _Inout_ NTSTATUS *FinalResult,
    _In_    NTSTATUS  IntermeidateResult
    )
{
    if (NT_SUCCESS(*FinalResult)) {
        *FinalResult = IntermeidateResult;
    }
}

_Must_inspect_result_
NTSTATUS
FxPrePostCallback::_InitializeContext(
    _In_  PFX_DRIVER_GLOBALS    Globals,
    _In_  PWDFCXDEVICE_INIT     CxInit,
    _Out_ PFxCxPnpPowerCallbackContext *Context,
    _In_  FxCxCallbackType      CallbackType
    )
{
    PFxCxPnpPowerCallbackContext context;
    PWDFCX_PNPPOWER_EVENT_CALLBACKS pnpPowerCallbacks;
    PWDFCX_POWER_POLICY_EVENT_CALLBACKS powerPolicyCallbacks;
    PVOID preCallback, postCallback, cleanupCallback;

    preCallback = NULL;
    postCallback = NULL;
    cleanupCallback = NULL;

    pnpPowerCallbacks = &CxInit->PnpPowerCallbacks.PnpPowerCallbacks;
    powerPolicyCallbacks = &CxInit->PnpPowerCallbacks.PowerPolicyCallbacks;

    switch(CallbackType) {

    //
    // PnP & Power callbacks
    //
    case FxCxCallbackPrepareHardware:
        preCallback = pnpPowerCallbacks->EvtCxDevicePrePrepareHardware;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostPrepareHardware;
        cleanupCallback = pnpPowerCallbacks->EvtCxDevicePrePrepareHardwareFailedCleanup;
        break;
    case FxCxCallbackReleaseHardware:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreReleaseHardware;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostReleaseHardware;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackD0Entry:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreD0Entry;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostD0Entry;
        cleanupCallback = pnpPowerCallbacks->EvtCxDevicePreD0EntryFailedCleanup;
        break;
    case FxCxCallbackD0Exit:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreD0Exit;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostD0Exit;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackSmIoInit:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoInit;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoInit;
        cleanupCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoInitFailedCleanup;
        break;
    case FxCxCallbackSmIoRestart:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestart;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoRestart;
        cleanupCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestartFailedCleanup;
        break;
    case FxCxCallbackSmIoRestartEx:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestartEx;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoRestartEx;
        cleanupCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestartExFailedCleanup;
        break;
    case FxCxCallbackSmIoSuspend:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoSuspend;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoSuspend;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackSmIoSuspendEx:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoSuspendEx;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoSuspendEx;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackSmIoFlush:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoFlush;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoFlush;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackSmIoCleanup:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoCleanup;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoCleanup;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackSurpriseRemoval:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreSurpriseRemoval;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostSurpriseRemoval;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackD0EntryPostHwEnabled:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreD0EntryPostHardwareEnabled;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostD0EntryPostHardwareEnabled;
        cleanupCallback = pnpPowerCallbacks->EvtCxDevicePreD0EntryPostHardwareEnabledFailedCleanup;;
        break;
    case FxCxCallbackD0ExitPreHwDisabled:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreD0ExitPreHardwareDisabled;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostD0ExitPreHardwareDisabled;
        cleanupCallback = NULL;
        break;

    //
    // Power Policy callbacks
    //
    case FxCxCallbackArmWakeFromS0:
        preCallback = powerPolicyCallbacks->EvtCxDevicePreArmWakeFromS0;
        postCallback = powerPolicyCallbacks->EvtCxDevicePostArmWakeFromS0;
        cleanupCallback = powerPolicyCallbacks->EvtCxDevicePreArmWakeFromS0FailedCleanup;;
        break;
    case FxCxCallbackDisarmWakeFromS0:
        preCallback = powerPolicyCallbacks->EvtCxDevicePreDisarmWakeFromS0;
        postCallback = powerPolicyCallbacks->EvtCxDevicePostDisarmWakeFromS0;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackWakeFromS0Triggered:
        preCallback = powerPolicyCallbacks->EvtCxDevicePreWakeFromS0Triggered;
        postCallback = powerPolicyCallbacks->EvtCxDevicePostWakeFromS0Triggered;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackArmWakeFromSx:
        preCallback = powerPolicyCallbacks->EvtCxDevicePreArmWakeFromSx;
        postCallback = powerPolicyCallbacks->EvtCxDevicePostArmWakeFromSx;
        cleanupCallback = powerPolicyCallbacks->EvtCxDevicePreArmWakeFromSxFailedCleanup;;
        break;
    case FxCxCallbackArmWakeFromSxWithReason:
        preCallback = powerPolicyCallbacks->EvtCxDevicePreArmWakeFromSxWithReason;
        postCallback = powerPolicyCallbacks->EvtCxDevicePostArmWakeFromSxWithReason;
        cleanupCallback = powerPolicyCallbacks->EvtCxDevicePreArmWakeFromSxWithReasonFailedCleanup;;
        break;
    case FxCxCallbackDisarmWakeFromSx:
        preCallback = powerPolicyCallbacks->EvtCxDevicePreDisarmWakeFromSx;
        postCallback = powerPolicyCallbacks->EvtCxDevicePostDisarmWakeFromSx;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackWakeFromSxTriggered:
        preCallback = powerPolicyCallbacks->EvtCxDevicePreWakeFromSxTriggered;
        postCallback = powerPolicyCallbacks->EvtCxDevicePostWakeFromSxTriggered;
        cleanupCallback = NULL;
        break;

    default:
        ASSERT(0);
        break;
    }

    if (preCallback == NULL && postCallback == NULL && cleanupCallback == NULL) {
        //
        // Nothing to do
        //
        *Context = NULL;
        return STATUS_SUCCESS;
    }

    context = new (Globals) FxCxPnpPowerCallbackContext(CallbackType);
    if (context == NULL) {
        DoTraceLevelMessage(Globals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
                            "Could not allocate FxCxPnpPowerCallbackContext");
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    context->u.Generic.PreCallback = preCallback;
    context->u.Generic.PostCallback = postCallback;
    context->u.Generic.CleanupCallback = cleanupCallback;

    *Context = context;
    return STATUS_SUCCESS;
}

_Must_inspect_result_
NTSTATUS
FxPrePostCallback::InvokeStateful(
    _Out_opt_ FxCxCallbackProgress *Progress,
    _In_ FxCxCallbackCleanupAction CleanupAction
    )
/*++

Routine Description:
    Calls all CX PRE API's in a stateful manner. When an error is hit prior
    to presenting the callback to the client driver then we unwind by calling
    the registered CX clean up functions. If an error happens after the client
    API is called then the error is reported to the caller, but all POST
    methods are called.

Arguments:
    OUT Progress, if an error occurs this indicates what stage the error
    happened in

Return Value:
    First Error encountered.

  --*/
{
    CfxDevice* device;
    NTSTATUS status = STATUS_SUCCESS;
    FxCxCallbackProgress progress = FxCxCallbackProgressInitialized;

#if (FX_CORE_MODE==FX_CORE_KERNEL_MODE)

    FxCompanionTarget* companionTarget;
    companionTarget = m_PkgPnp->GetCompanionTarget();

    if (NULL != companionTarget) {
        //
        // If the companion callback fails, then the value of 'Progress'
        // which is FxCxCallbackProgressInitialized informs the caller that
        // the client callback was not called.
        //
        status = InvokeCompanionCallback(companionTarget);
        if (!NT_SUCCESS(status)) {
            goto exit;
        }
    }
#endif

    device = m_PkgPnp->GetDevice();

    if (device->IsACxPresent() == FALSE) {

        status = InvokeClient();
        progress = FxCxCallbackProgressClientCalled;
        if (NT_SUCCESS(status)) {
            progress = FxCxCallbackProgressClientSucceeded;
        }
    }
    else {
        //
        // notify all CX's to call PRE callback function
        //
        status = IssuePreCxCallbacksStateful(device);

        if (!NT_SUCCESS(status)) {
            IssueCleanupCxCallbacks(device);
        }
        else {
            status = InvokeClient();
            progress = FxCxCallbackProgressClientCalled;

            if (NT_SUCCESS(status)) {
                progress = FxCxCallbackProgressClientSucceeded;
                //
                // notify all CX's to call POST callback function
                //
                status = IssuePostCxCallbacks(device);
            }
            else if (CleanupAction == FxCxCleanupAfterPreOrClientFailure) {
                IssueCleanupCxCallbacks(device);
            }
        }
    }

#if (FX_CORE_MODE==FX_CORE_KERNEL_MODE)
exit:
#endif

    if (Progress) {
        *Progress = progress;
    }
    return status;
}

_Must_inspect_result_
NTSTATUS
FxPrePostCallback::IssuePreCxCallbacksStateful(
    _In_ CfxDevice* Device
    )
/*++

Routine Description:
    Calls all CX PRE API's for the given callback type, exits loop is an error
    is encountered

Arguments:
    Device - pointer to the device object to access the CxInfo

Return Value:
    First Error encountered.

  --*/
{
    FxCxDeviceInfo *cxInfo;
    PFxCxPnpPowerCallbackContext context;
    NTSTATUS status = STATUS_SUCCESS;
    BOOLEAN failed = FALSE;

    cxInfo = Device->GetFirstCxDeviceInfo();

    //
    // We should not evaluate this code if there are not at least one CX
    //
    ASSERT(cxInfo != NULL);

    while (cxInfo != NULL) {
        context = cxInfo->GetCxPnpPowerCallbackContexts(m_CallbackType);

        if (context != NULL &&
            context->IsPreCallbackPresent()) {

            //
            // Clear the state in case a previous IssuePreCxCallbacksStateful succeeded
            //
            context->m_PreCallbackSuccessful = FALSE;

            //
            // If a pre-callback fails, stop calling more PreCallbacks, but do
            // continue to mark the rest as m_PreCallbackSuccessful = FALSE.
            //
            if (!failed) {

                status = InvokeCxCallback(context, FxCxInvokePreCallback);
                if (NT_SUCCESS(status)) {

                    context->m_PreCallbackSuccessful = TRUE;
                }
                else {
                    failed = TRUE;
                }
            }
        }

        cxInfo = Device->GetNextCxDeviceInfo(cxInfo);
    }
    return status;
}

_Must_inspect_result_
NTSTATUS
FxPrePostCallback::InvokeStateless(
    VOID
    )
/*++

Routine Description:
    Calls all CX PRE API's, then invokes the client method. Finally it will
    call all CX POST API's. An error will not stop this sequence. The first
    error encountered will be stored temporarily and returned to the
    caller.

Arguments:
    N/A

Return Value:
    First Error encountered.

  --*/
{
    CfxDevice* device;
    NTSTATUS status = STATUS_SUCCESS;
    NTSTATUS tempStatus;

    device = m_PkgPnp->GetDevice();
    if (device->IsACxPresent() == FALSE) {
        status = InvokeClient();
    }
    else {
        //
        // notify all CX's to call PRE callback function
        //
        status = IssuePreCxCallbacksStateless(device);

        tempStatus = InvokeClient();
        _SaveTheFirstError(&status, tempStatus);

        //
        // notify all CX's to call POST callback function
        //
        tempStatus = IssuePostCxCallbacks(device);
        _SaveTheFirstError(&status, tempStatus);
    }

#if (FX_CORE_MODE==FX_CORE_KERNEL_MODE)

    FxCompanionTarget* companionTarget;
    companionTarget = m_PkgPnp->GetCompanionTarget();

    if (NULL != companionTarget) {
        tempStatus = InvokeCompanionCallback(companionTarget);
        _SaveTheFirstError(&status, tempStatus);
    }
#endif

    return status;
}

_Must_inspect_result_
NTSTATUS
FxPrePostCallback::IssuePreCxCallbacksStateless(
    _In_ CfxDevice* Device
    )
/*++

Routine Description:
    Calls all CX PRE API's for the given callback type; errors do not
    stop the callback chain. The first error encountered is returned to the
    caller

Arguments:
    Device - pointer to the device object to access the CxInfo

Return Value:
    First Error encountered.

  --*/
{
    FxCxDeviceInfo *cxInfo;
    PFxCxPnpPowerCallbackContext context;
    NTSTATUS status = STATUS_SUCCESS;
    NTSTATUS tempStatus;

    cxInfo = Device->GetFirstCxDeviceInfo();

    //
    // We should not evaluate this code if there are not at least one CX
    //
    ASSERT(cxInfo != NULL);

    while (cxInfo != NULL) {
        context = cxInfo->GetCxPnpPowerCallbackContexts(m_CallbackType);

        if (context != NULL &&
            context->IsPreCallbackPresent()) {

            tempStatus = InvokeCxCallback(context, FxCxInvokePreCallback);
            _SaveTheFirstError(&status, tempStatus);
        }

        cxInfo = Device->GetNextCxDeviceInfo(cxInfo);
    }
    return status;
}


_Must_inspect_result_
NTSTATUS
FxPrePostCallback::IssuePostCxCallbacks(
    _In_ CfxDevice* Device
    )
{
    FxCxDeviceInfo *cxInfo;
    PFxCxPnpPowerCallbackContext context;
    NTSTATUS status = STATUS_SUCCESS;
    NTSTATUS tempStatus;

    cxInfo = Device->GetFirstCxDeviceInfo();

    //
    // We should not evaluate this code if there are not at least one CX
    //
    ASSERT(cxInfo != NULL);

    while (cxInfo != NULL) {
        context = cxInfo->GetCxPnpPowerCallbackContexts(m_CallbackType);

        if (context != NULL &&
            context->IsPostCallbackPresent()) {

            tempStatus = InvokeCxCallback(context, FxCxInvokePostCallback);
            _SaveTheFirstError(&status, tempStatus);
        }
        cxInfo = Device->GetNextCxDeviceInfo(cxInfo);
    }
    return status;
}

VOID
FxPrePostCallback::IssueCleanupCxCallbacks(
    _In_ CfxDevice* Device
    )
{
    FxCxDeviceInfo *cxInfo;
    PFxCxPnpPowerCallbackContext context;

    cxInfo = Device->GetFirstCxDeviceInfo();

    //
    // We should not evaluate this code if there are not at least one CX
    //
    ASSERT(cxInfo != NULL);

    while (cxInfo != NULL) {
        context = cxInfo->GetCxPnpPowerCallbackContexts(m_CallbackType);

        if ((context != NULL) &&
            (context->IsCleanupCallbackPresent()) &&
            (context->m_PreCallbackSuccessful == TRUE)) {

            InvokeCxCleanupCallback(context);
        }

        cxInfo = Device->GetNextCxDeviceInfo(cxInfo);
    }
    return;
}

