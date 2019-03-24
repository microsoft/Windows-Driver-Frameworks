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
    return IsCallbackPresent(FxCxPreCallback);
}

BOOLEAN
FxCxPnpPowerCallbackContext::IsPostCallbackPresent(
    VOID
    )
{
    return IsCallbackPresent(FxCxPostCallback);
}

BOOLEAN
FxCxPnpPowerCallbackContext::IsCleanupCallbackPresent(
    VOID
    )
{
    return IsCallbackPresent(FxCxCleanupCallback);
}

BOOLEAN
FxCxPnpPowerCallbackContext::IsCallbackPresent(
    FxCxCallbackSubType SubType
    )
{
    BOOLEAN present = FALSE;
    
    switch(m_CallbackType) {
    case FxCxCallbackPrepareHardware:
    {
        switch(SubType) {
        case FxCxPreCallback:
            present = (NULL != u.PrepareHardware.PreCallback);
            break;
        case FxCxPostCallback:
            present = (NULL != u.PrepareHardware.PostCallback);
            break;
        case FxCxCleanupCallback:
            present = (NULL != u.PrepareHardware.CleanupCallback);
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    }
    case FxCxCallbackReleaseHardware:
    {
        switch(SubType) {
        case FxCxPreCallback:
            present = (NULL != u.ReleaseHardware.PreCallback);
            break;
        case FxCxPostCallback:
            present = (NULL != u.ReleaseHardware.PostCallback);
            break;
        case FxCxCleanupCallback:
            __fallthrough;
        default:
            ASSERT(0);
            break;
        }
        break;
    }
    case FxCxCallbackD0Entry:
    {
        switch(SubType) {
        case FxCxPreCallback:
            present = (NULL != u.D0Entry.PreCallback);
            break;
        case FxCxPostCallback:
            present = (NULL != u.D0Entry.PostCallback);
            break;
        case FxCxCleanupCallback:
            present = (NULL != u.D0Entry.CleanupCallback);
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    }
    case FxCxCallbackD0Exit:
    {
        switch(SubType) {
        case FxCxPreCallback:
            present = (NULL != u.D0Exit.PreCallback);
            break;
        case FxCxPostCallback:
            present = (NULL != u.D0Exit.PostCallback);
            break;
        case FxCxCleanupCallback:
            __fallthrough;
        default:
            ASSERT(0);
            break;
        }
        break;
    }
    case FxCxCallbackSmIoInit:
    {
        switch(SubType) {
        case FxCxPreCallback:
            present = (NULL != u.SmIoInit.PreCallback);
            break;
        case FxCxPostCallback:
            present = (NULL != u.SmIoInit.PostCallback);
            break;
        case FxCxCleanupCallback:
            present = (NULL != u.SmIoInit.CleanupCallback);
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    }
    case FxCxCallbackSmIoRestart:
    {
        switch(SubType) {
        case FxCxPreCallback:
            present = (NULL != u.SmIoRestart.PreCallback);
            break;
        case FxCxPostCallback:
            present = (NULL != u.SmIoRestart.PostCallback);
            break;
        case FxCxCleanupCallback:
            present = (NULL != u.SmIoRestart.CleanupCallback);
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    }
    case FxCxCallbackSmIoSuspend:
    {
        switch(SubType) {
        case FxCxPreCallback:
            present = (NULL != u.SmIoSuspend.PreCallback);
            break;
        case FxCxPostCallback:
            present = (NULL != u.SmIoSuspend.PostCallback);
            break;
        case FxCxCleanupCallback:
            __fallthrough;
        default:
            ASSERT(0);
            break;
        }
        break;
    }
    case FxCxCallbackSmIoFlush:
    {
        switch(SubType) {
        case FxCxPreCallback:
            present = (NULL != u.SmIoFlush.PreCallback);
            break;
        case FxCxPostCallback:
            present = (NULL != u.SmIoFlush.PostCallback);
            break;
        case FxCxCleanupCallback:
            __fallthrough;
        default:
            ASSERT(0);
            break;
        }
        break;
    }
    case FxCxCallbackSmIoCleanup:
    {
        switch(SubType) {
        case FxCxPreCallback:
            present = (NULL != u.SmIoCleanup.PreCallback);
            break;
        case FxCxPostCallback:
            present = (NULL != u.SmIoCleanup.PostCallback);
            break;
        case FxCxCleanupCallback:
            __fallthrough;
        default:
            ASSERT(0);
            break;
        }
        break;
    }
    case FxCxCallbackSurpriseRemoval:
    {
        switch(SubType) {
        case FxCxPreCallback:
            present = (NULL != u.SurpriseRemoval.PreCallback);
            break;
        case FxCxPostCallback:
            present = (NULL != u.SurpriseRemoval.PostCallback);
            break;
        case FxCxCleanupCallback:
            __fallthrough;
        default:
            ASSERT(0);
            break;
        }
        break;
    }
    default:
        ASSERT(0);
        break;
    }

    return present;
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
    PVOID preCallback, postCallback, cleanupCallback;

    preCallback = NULL;
    postCallback = NULL;
    cleanupCallback = NULL;    

    ASSERT(CxInit->PnpPowerCallbacks.Set == TRUE);

    pnpPowerCallbacks = &CxInit->PnpPowerCallbacks.Callbacks;

    switch(CallbackType) {
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
    case FxCxCallbackSmIoSuspend:
        preCallback = pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoSuspend;
        postCallback = pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoSuspend;
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

    //
    // NOTE: We are not using preCallback / postCallback / cleanupCallback
    // from above. It would make the code simpler, but we would
    // loose type safety.
    //
    switch(CallbackType) {
    case FxCxCallbackPrepareHardware:
        context->u.PrepareHardware.PreCallback = 
            pnpPowerCallbacks->EvtCxDevicePrePrepareHardware;
        context->u.PrepareHardware.PostCallback =
            pnpPowerCallbacks->EvtCxDevicePostPrepareHardware;
        context->u.PrepareHardware.CleanupCallback =
            pnpPowerCallbacks->EvtCxDevicePrePrepareHardwareFailedCleanup;
        break;
    case FxCxCallbackReleaseHardware:
        context->u.ReleaseHardware.PreCallback =
            pnpPowerCallbacks->EvtCxDevicePreReleaseHardware;
        context->u.ReleaseHardware.PostCallback =
            pnpPowerCallbacks->EvtCxDevicePostReleaseHardware;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackD0Entry:
        context->u.D0Entry.PreCallback =
            pnpPowerCallbacks->EvtCxDevicePreD0Entry;
        context->u.D0Entry.PostCallback =
            pnpPowerCallbacks->EvtCxDevicePostD0Entry;
        context->u.D0Entry.CleanupCallback =
            pnpPowerCallbacks->EvtCxDevicePreD0EntryFailedCleanup;
        break;
    case FxCxCallbackD0Exit:
        context->u.D0Exit.PreCallback =
            pnpPowerCallbacks->EvtCxDevicePreD0Exit;
        context->u.D0Exit.PostCallback =
            pnpPowerCallbacks->EvtCxDevicePostD0Exit;
        break;
    case FxCxCallbackSmIoInit:
        context->u.SmIoInit.PreCallback =
            pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoInit;
        context->u.SmIoInit.PostCallback =
            pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoInit;
        context->u.SmIoInit.CleanupCallback =
            pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoInitFailedCleanup;
        break;
    case FxCxCallbackSmIoRestart:
        context->u.SmIoRestart.PreCallback =
            pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestart;
        context->u.SmIoRestart.PostCallback =
            pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoRestart;
        context->u.SmIoRestart.CleanupCallback =
            pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoRestartFailedCleanup;
        break;
    case FxCxCallbackSmIoSuspend:
        context->u.SmIoSuspend.PreCallback =
            pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoSuspend;
        context->u.SmIoSuspend.PostCallback =
            pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoSuspend;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackSmIoFlush:
        context->u.SmIoFlush.PreCallback =
            pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoFlush;
        context->u.SmIoFlush.PostCallback =
            pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoFlush;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackSmIoCleanup:
        context->u.SmIoCleanup.PreCallback =
            pnpPowerCallbacks->EvtCxDevicePreSelfManagedIoCleanup;
        context->u.SmIoCleanup.PostCallback =
            pnpPowerCallbacks->EvtCxDevicePostSelfManagedIoCleanup;
        cleanupCallback = NULL;
        break;
    case FxCxCallbackSurpriseRemoval:
        context->u.SurpriseRemoval.PreCallback =
            pnpPowerCallbacks->EvtCxDevicePreSurpriseRemoval;
        context->u.SurpriseRemoval.PostCallback =
            pnpPowerCallbacks->EvtCxDevicePostSurpriseRemoval;
        cleanupCallback = NULL;
        break;
    default:
        ASSERT(0);
        break;
    }

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
    FxCompanionTarget* companionTarget;

    device = m_PkgPnp->GetDevice();
    companionTarget = m_PkgPnp->GetCompanionTarget();

    if (device->IsACxPresent() == FALSE) {
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

        status = InvokeClient();
        progress = FxCxCallbackProgressClientCalled;
        if (NT_SUCCESS(status)) {
            progress = FxCxCallbackProgressClientSucceeded;
        }

    }
    else {
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
            }
            else if (CleanupAction == FxCxCleanupAfterPreOrClientFailure) {
                IssueCleanupCxCallbacks(device);
            }
        }
            
        if (NT_SUCCESS(status)) {
            //
            // notify all CX's to call POST callback function
            //
            status = IssuePostCxCallbacks(device);
        }
    }

exit:
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

    cxInfo = Device->GetFirstCxDeviceInfo();

    //
    // We should not evaluate this code if there are not at least one CX
    //
    ASSERT(cxInfo != NULL);

    while (cxInfo != NULL) {
        context = cxInfo->CxPnpPowerCallbackContexts[m_CallbackType];
        if (context != NULL) {

            //
            // Should be cleared by the cleanup callback if its ever set.
            //
            ASSERT(context->m_PreCallbackSuccessful == FALSE);

            if (context->IsPreCallbackPresent()) {
                status = InvokeCxCallback(context, FxCxInvokePreCallback);
                if (NT_SUCCESS(status)) {

                    context->m_PreCallbackSuccessful = TRUE;
                }
                else {
                    //
                    // Abort the chain in the event of an error
                    //
                    break;
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
    NTSTATUS tempStatus = STATUS_SUCCESS;
    FxCompanionTarget* companionTarget;

    device = m_PkgPnp->GetDevice();
    companionTarget = m_PkgPnp->GetCompanionTarget();

    if (device->IsACxPresent() == FALSE) {
        status = InvokeClient();

        if (NULL != companionTarget) {
#pragma warning(suppress: 28193)
            tempStatus = InvokeCompanionCallback(companionTarget);
            if (NT_SUCCESS(status)) {
                status = tempStatus;
            }
        }
    }
    else {
        //
        // notify all CX's to call PRE callback function
        //
        status = IssuePreCxCallbacksStateless(device);

// We conditionaly inspect the results instead of 100%
#pragma warning(suppress: 28193)
        tempStatus = InvokeClient();
        if (NT_SUCCESS(status)) {
            status = tempStatus;
        }

        //
        // notify all CX's to call POST callback function
        //
// We conditionaly inspect the results instead of 100%
#pragma warning(suppress: 28193)
        tempStatus = IssuePostCxCallbacks(device);
        if (NT_SUCCESS(status)) {
            status = tempStatus;
        }

        if (NULL != companionTarget) {
#pragma warning(suppress: 28193)
            tempStatus = InvokeCompanionCallback(companionTarget);
            if (NT_SUCCESS(status)) {
                status = tempStatus;
            }
        }
    }
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
    NTSTATUS tempStatus = STATUS_SUCCESS;

    cxInfo = Device->GetFirstCxDeviceInfo();

    //
    // We should not evaluate this code if there are not at least one CX
    //
    ASSERT(cxInfo != NULL);

    while (cxInfo != NULL) {
        context = cxInfo->CxPnpPowerCallbackContexts[m_CallbackType];

        if ((context != NULL) &&
            (context->IsPreCallbackPresent())) {
// We conditionaly inspect the results instead of 100%
#pragma warning(suppress: 28193)
            tempStatus = InvokeCxCallback(context, FxCxInvokePreCallback);

            //
            // save the first error encountered
            //
            if (NT_SUCCESS(status)) {

                status = tempStatus;
            }
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
    NTSTATUS tempStatus = STATUS_SUCCESS;

    cxInfo = Device->GetFirstCxDeviceInfo();

    //
    // We should not evaluate this code if there are not at least one CX
    //
    ASSERT(cxInfo != NULL);

    while (cxInfo != NULL) {
        context = cxInfo->CxPnpPowerCallbackContexts[m_CallbackType];
        if (context != NULL) {

            //
            // We no longer need to keep track of this so we pre-clear it prior to 
            // the next PRE callback being invoked
            //
            context->m_PreCallbackSuccessful = FALSE;

            if (context->IsPostCallbackPresent()) {
// We conditionaly inspect the results instead of 100%
#pragma warning(suppress: 28193)
                tempStatus = InvokeCxCallback(context, FxCxInvokePostCallback);

                if (NT_SUCCESS(status)) {
                    status = tempStatus;
                }
            }
        }

        cxInfo = Device->GetNextCxDeviceInfo(cxInfo);
    }
    return status;
}

_Must_inspect_result_
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
        context = cxInfo->CxPnpPowerCallbackContexts[m_CallbackType];
        
        if ((context != NULL) && 
            (context->IsCleanupCallbackPresent()) &&
            (context->m_PreCallbackSuccessful == TRUE)) {

            InvokeCxCleanupCallback(context);
            context->m_PreCallbackSuccessful = FALSE;
        }

        cxInfo = Device->GetNextCxDeviceInfo(cxInfo);
    }
    return;
}

