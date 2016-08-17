/*++

Copyright (c) Microsoft Corporation

Module Name:

    PnpCallbacks.cpp

Abstract:

    This module implements the PnP/Power callback objects.

Author:



Environment:

    Both kernel and user mode

Revision History:

--*/


#include "pnppriv.hpp"

extern "C" {
#if defined(EVENT_TRACING)
#include "PnpCallbacks.tmh"
#endif
}


_Must_inspect_result_
NTSTATUS
FxPnpDeviceFilterResourceRequirements::Invoke(
    __in WDFDEVICE Device,
    __in WDFIORESREQLIST Collection
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device, Collection);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

__drv_when(!NT_SUCCESS(return), __drv_arg(Progress, _Must_inspect_result_))
_Must_inspect_result_
NTSTATUS
FxPnpDeviceD0Entry::Invoke(
    _In_ WDFDEVICE  Device,
    _In_ WDF_POWER_DEVICE_STATE PreviousState,
    _Out_ FxCxCallbackProgress *Progress
    )
{
    m_Device = Device;
    m_PreviousState = PreviousState;

    return FxPrePostCallback::InvokeStateful(Progress,
                                             FxCxCleanupAfterPreOrClientFailure);
}

VOID
FxPnpDeviceD0Entry::Initialize(
    _In_ FxPkgPnp* PkgPnp,
    _In_ PFN_WDF_DEVICE_D0_ENTRY Method
    )
{
    m_Method = Method;
    m_PkgPnp = PkgPnp;
    m_CallbackType = FxCxCallbackD0Entry;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceD0Entry::InvokeClient(
    VOID
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(m_Device, m_PreviousState);
        CallbackEnd();

        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(
                m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGPNP,
                "EvtDeviceD0Entry WDFDEVICE 0x%p !devobj 0x%p, old state "
                "%!WDF_POWER_DEVICE_STATE! failed, %!STATUS!",
                m_Device, 
                m_PkgPnp->GetDevice()->GetDeviceObject(),
                m_PreviousState, status);
        }

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceD0Entry::InvokeCxCallback(
    _In_ PFxCxPnpPowerCallbackContext Context,
    _In_ FxCxInvokeCallbackSubType PrePost
    )
{
    NTSTATUS status;
    PFN_WDF_DEVICE_D0_ENTRY method;

    if (PrePost == FxCxInvokePreCallback) {
        method = Context->u.D0Entry.PreCallback;
    }
    else {
        method = Context->u.D0Entry.PostCallback;
    }

    ASSERT(method != NULL);                          

    //
    // EvtCxDevicePreD0Entry EvtCxDevicePostD0Entry
    //
    CallbackStart();
    status = method(m_Device, m_PreviousState);
    CallbackEnd();

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGPNP,
            "EvtCxDevice%sD0Entry WDFDEVICE 0x%p !devobj 0x%p, old state "
            "%!WDF_POWER_DEVICE_STATE! failed, %!STATUS!",
            PrePost == FxCxInvokePreCallback ? "Pre" : "Post",
            m_Device, 
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            m_PreviousState, status);
    }

    return status;
}

VOID
FxPnpDeviceD0Entry::InvokeCxCleanupCallback(
    _In_ PFxCxPnpPowerCallbackContext Context
    )
{
    PFN_WDFCX_DEVICE_PRE_D0_ENTRY_FAILED_CLEANUP method;

    method = Context->u.D0Entry.CleanupCallback;
    ASSERT(method != NULL);                          

    CallbackStart();
    method(m_Device, m_PreviousState);
    CallbackEnd();

    return;
}


_Must_inspect_result_
NTSTATUS
FxPnpDeviceD0EntryPostInterruptsEnabled::Invoke(
    __in WDFDEVICE  Device,
    __in WDF_POWER_DEVICE_STATE PreviousState
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device, PreviousState);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceD0Exit::Invoke(
    _In_ WDFDEVICE  Device,
    _In_ WDF_POWER_DEVICE_STATE TargetState
    )
{
    m_Device = Device;
    m_TargetState = TargetState;

    return FxPrePostCallback::InvokeStateless();
}

VOID
FxPnpDeviceD0Exit::Initialize(
    _In_ FxPkgPnp* PkgPnp,
    _In_ PFN_WDF_DEVICE_D0_EXIT Method
    )
{
    m_Method = Method;
    m_PkgPnp = PkgPnp;
    m_CallbackType = FxCxCallbackD0Exit;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceD0Exit::InvokeClient(
    VOID
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(m_Device, m_TargetState);
        CallbackEnd();

        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(
                m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGPNP,
                "EvtDeviceD0Exit WDFDEVICE 0x%p !devobj 0x%p, new state "
                "%!WDF_POWER_DEVICE_STATE! failed, %!STATUS!",
                m_Device, 
                m_PkgPnp->GetDevice()->GetDeviceObject(),
                m_TargetState, status);
        }

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceD0Exit::InvokeCxCallback(
    _In_ PFxCxPnpPowerCallbackContext Context,
    _In_ FxCxInvokeCallbackSubType PrePost
    )
{
    NTSTATUS status;
    PFN_WDF_DEVICE_D0_EXIT method;

    if (PrePost == FxCxInvokePreCallback) {
        method = Context->u.D0Exit.PreCallback;
    }
    else {
        method = Context->u.D0Exit.PostCallback;
    }

    ASSERT(method != NULL);                          

    //
    // EvtCxDevicePreD0Exit EvtCxDevicePostD0Exit
    //
    CallbackStart();
    status = method(m_Device, m_TargetState);
    CallbackEnd();

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGPNP,
            "EvtCxDevice%sD0Exit WDFDEVICE 0x%p !devobj 0x%p, new state "
            "%!WDF_POWER_DEVICE_STATE! failed, %!STATUS!",
            PrePost == FxCxInvokePreCallback ? "Pre" : "Post",
            m_Device, 
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            m_TargetState, status);
    }

    return status;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceD0ExitPreInterruptsDisabled::Invoke(
    __in WDFDEVICE  Device,
    __in WDF_POWER_DEVICE_STATE TargetState
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device, TargetState);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

__drv_when(!NT_SUCCESS(return), __drv_arg(Progress, _Must_inspect_result_))
_Must_inspect_result_
NTSTATUS
FxPnpDevicePrepareHardware::Invoke(
    _In_ WDFDEVICE  Device,
    _In_ WDFCMRESLIST ResourcesRaw,
    _In_ WDFCMRESLIST ResourcesTranslated,
    _Out_ FxCxCallbackProgress *Progress
    )
{
    m_Device = Device;
    m_ResourcesRaw = ResourcesRaw;
    m_ResourcesTranslated = ResourcesTranslated;

    return FxPrePostCallback::InvokeStateful(Progress, 
                                             FxCxCleanupAfterPreFailure);
}


VOID
FxPnpDevicePrepareHardware::Initialize(
    _In_ FxPkgPnp* PkgPnp,
    _In_ PFN_WDF_DEVICE_PREPARE_HARDWARE Method
    )
{
    m_Method = Method;
    m_PkgPnp = PkgPnp;
    m_CallbackType = FxCxCallbackPrepareHardware;
}

_Must_inspect_result_
NTSTATUS
FxPnpDevicePrepareHardware::InvokeClient(
    VOID
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(m_Device, m_ResourcesRaw, m_ResourcesTranslated);
        CallbackEnd();

        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(
                m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGPNP,
                "EvtDevicePrepareHardware WDFDEVICE 0x%p !devobj 0x%p failed, "
                "%!STATUS!",
                m_Device, m_PkgPnp->GetDevice()->GetDeviceObject(), status);
        }

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

_Must_inspect_result_
NTSTATUS
FxPnpDevicePrepareHardware::InvokeCxCallback(
    _In_ PFxCxPnpPowerCallbackContext Context,
    _In_ FxCxInvokeCallbackSubType PrePost
    )
{
    NTSTATUS status;
    PFN_WDF_DEVICE_PREPARE_HARDWARE method;

    if (PrePost == FxCxInvokePreCallback) {
        method = Context->u.PrepareHardware.PreCallback;
    }
    else {
        method = Context->u.PrepareHardware.PostCallback;
    }

    ASSERT(method != NULL);                          

    //
    // EvtCxDevicePrePrepareHardware EvtCxDevicePostPrepareHardware
    //
    CallbackStart();
    status = method(m_Device, m_ResourcesRaw, m_ResourcesTranslated);
    CallbackEnd();

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGPNP,
            "EvtCxDevice%sPrepareHardware WDFDEVICE 0x%p !devobj 0x%p failed, "
            "%!STATUS!", 
            PrePost == FxCxInvokePreCallback ? "Pre" : "Post",
            m_Device,
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            status);
    }

    return status;
}

VOID
FxPnpDevicePrepareHardware::InvokeCxCleanupCallback(
    _In_ PFxCxPnpPowerCallbackContext Context
    )
{
    PFN_WDFCX_DEVICE_PRE_PREPARE_HARDWARE_FAILED_CLEANUP method;

    method = Context->u.PrepareHardware.CleanupCallback;
    ASSERT(method != NULL);                          

    CallbackStart();
    method(m_Device, m_ResourcesRaw, m_ResourcesTranslated);
    CallbackEnd();

    return;
}


_Must_inspect_result_
NTSTATUS
FxPnpDeviceReleaseHardware::Invoke(
    __in WDFDEVICE  Device,
    __in WDFCMRESLIST ResourcesTranslated
    )
{
    m_Device = Device;
    m_ResourcesTranslated = ResourcesTranslated;

    return FxPrePostCallback::InvokeStateless();
}

VOID
FxPnpDeviceReleaseHardware::Initialize(
    _In_ FxPkgPnp* PkgPnp,
    _In_ PFN_WDF_DEVICE_RELEASE_HARDWARE Method
    )
{
    m_Method = Method;
    m_PkgPnp = PkgPnp;
    m_CallbackType = FxCxCallbackReleaseHardware;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceReleaseHardware::InvokeClient(
    VOID
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(m_Device, m_ResourcesTranslated);
        CallbackEnd();

        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(
                m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGPNP,
                "EvtDeviceReleaseHardware WDFDEVICE 0x%p !devobj 0x%p failed, "
                "%!STATUS!",
                m_Device, m_PkgPnp->GetDevice()->GetDeviceObject(), status);
        }

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}
    
_Must_inspect_result_
NTSTATUS
FxPnpDeviceReleaseHardware::InvokeCxCallback(
    _In_ PFxCxPnpPowerCallbackContext Context,
    _In_ FxCxInvokeCallbackSubType PrePost
    )
{
    NTSTATUS status;
    PFN_WDF_DEVICE_RELEASE_HARDWARE method;

    if (PrePost == FxCxInvokePreCallback) {
        method = Context->u.ReleaseHardware.PreCallback;
    }
    else {
        method = Context->u.ReleaseHardware.PostCallback;
    }

    ASSERT(method != NULL);                          

    //
    // EvtCxDevicePreReleaseHardware EvtCxDevicePostReleaseHardware
    //
    CallbackStart();
    status = method(m_Device, m_ResourcesTranslated);
    CallbackEnd();

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGPNP,
            "EvtCxDevice%sReleaseHardware WDFDEVICE 0x%p !devobj 0x%p failed, "
            "%!STATUS!",
            PrePost == FxCxInvokePreCallback ? "Pre" : "Post",
            m_Device,
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            status);
    }

    return status;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceRemoveAddedResources::Invoke(
    __in WDFDEVICE Device,
    __in WDFCMRESLIST ResourcesRaw,
    __in WDFCMRESLIST ResourcesTranslated
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device, ResourcesRaw, ResourcesTranslated);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

VOID
FxPnpDeviceSelfManagedIoCleanup::Invoke(
    _In_  WDFDEVICE  Device
    )
{
    m_Device = Device;

    (VOID) FxPrePostCallback::InvokeStateless();
    return;
}

VOID
FxPnpDeviceSelfManagedIoCleanup::Initialize(
    _In_ FxPkgPnp* PkgPnp,
    _In_ PFN_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP Method
    )
{
    m_Method = Method;
    m_PkgPnp = PkgPnp;
    m_CallbackType = FxCxCallbackSmIoCleanup;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoCleanup::InvokeClient(
    VOID
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(m_Device);
        CallbackEnd();
    }
    return STATUS_SUCCESS;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoCleanup::InvokeCxCallback(
    _In_ PFxCxPnpPowerCallbackContext Context,
    _In_ FxCxInvokeCallbackSubType PrePost
    )
{
    PFN_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP method;

    if (PrePost == FxCxInvokePreCallback) {
        method = Context->u.SmIoCleanup.PreCallback;
    }
    else {
        method = Context->u.SmIoCleanup.PostCallback;
    }

    ASSERT(method != NULL);                          

    //
    // EvtCxDevicePreSelfManagedIoCleanup EvtCxDevicePostSelfManagedIoCleanup
    //
    CallbackStart();
    method(m_Device);
    CallbackEnd();

    
    return STATUS_SUCCESS;
}
 
VOID
FxPnpDeviceSelfManagedIoFlush::Invoke(
    __in  WDFDEVICE  Device
    )
{
    m_Device = Device;

    (VOID) FxPrePostCallback::InvokeStateless();
    return;
}

VOID
FxPnpDeviceSelfManagedIoFlush::Initialize(
    _In_ FxPkgPnp* PkgPnp,
    _In_ PFN_WDF_DEVICE_SELF_MANAGED_IO_FLUSH Method
    )
{
    m_Method = Method;
    m_PkgPnp = PkgPnp;
    m_CallbackType = FxCxCallbackSmIoFlush;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoFlush::InvokeClient(
    VOID
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(m_Device);
        CallbackEnd();
    }
    return STATUS_SUCCESS;
}
    
_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoFlush::InvokeCxCallback(
    _In_ PFxCxPnpPowerCallbackContext Context,
    _In_ FxCxInvokeCallbackSubType PrePost
    )
{
    PFN_WDF_DEVICE_SELF_MANAGED_IO_FLUSH method;

    if (PrePost == FxCxInvokePreCallback) {
        method = Context->u.SmIoFlush.PreCallback;
    }
    else {
        method = Context->u.SmIoFlush.PostCallback;
    }

    ASSERT(method != NULL);

    //
    // EvtCxDevicePreSelfManagedIoFlush EvtCxDevicePostSelfManagedIoFlush
    //
    CallbackStart();
    method(m_Device);
    CallbackEnd();

    return STATUS_SUCCESS;
}

__drv_when(!NT_SUCCESS(return), __drv_arg(Progress, _Must_inspect_result_))
_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoInit::Invoke(
    _In_  WDFDEVICE  Device,
    _Out_ FxCxCallbackProgress *Progress
    )
{
    m_Device = Device;

    return FxPrePostCallback::InvokeStateful(Progress,
                                             FxCxCleanupAfterPreOrClientFailure);
}

VOID
FxPnpDeviceSelfManagedIoInit::Initialize(
    _In_ FxPkgPnp* PkgPnp,
    _In_ PFN_WDF_DEVICE_SELF_MANAGED_IO_INIT Method
    )
{
    m_Method = Method;
    m_PkgPnp = PkgPnp;
    m_CallbackType = FxCxCallbackSmIoInit;
}  

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoInit::InvokeClient(
    VOID
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(m_Device);
        CallbackEnd();

        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(
                m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_INFORMATION, TRACINGPNP,
                "EvtDeviceSelfManagedIoInit WDFDEVICE 0x%p !devobj 0x%p failed, %!STATUS!",
                m_Device,
                m_PkgPnp->GetDevice()->GetDeviceObject(),
                status);
        }

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoInit::InvokeCxCallback(
    _In_ PFxCxPnpPowerCallbackContext Context,
    _In_ FxCxInvokeCallbackSubType PrePost
    )
{
    NTSTATUS status;
    PFN_WDF_DEVICE_SELF_MANAGED_IO_INIT method;

    if (PrePost == FxCxInvokePreCallback) {
        method = Context->u.SmIoInit.PreCallback;
    }
    else {
        method = Context->u.SmIoInit.PostCallback;
    }

    ASSERT(method != NULL);

    //
    // EvtCxDevicePreSelfManagedIoInit EvtCxDevicePostSelfManagedIoInit
    //
    CallbackStart();
    status = method(m_Device);
    CallbackEnd();

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_INFORMATION, TRACINGPNP,
            "EvtCxDevice%sSelfManagedIoInit WDFDEVICE 0x%p !devobj 0x%p failed, "
            "%!STATUS!",
            PrePost == FxCxInvokePreCallback ? "Pre" : "Post",
            m_Device,
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            status);
    }

    return status;
}

VOID
FxPnpDeviceSelfManagedIoInit::InvokeCxCleanupCallback(
    _In_ PFxCxPnpPowerCallbackContext Context
    )
{
    PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_INIT_FAILED_CLEANUP method;

    method = Context->u.SmIoInit.CleanupCallback;
    ASSERT(method != NULL);                          

    CallbackStart();
    method(m_Device);
    CallbackEnd();

    return;
}


    

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoSuspend::Invoke(
    _In_  WDFDEVICE  Device
    )
{
    m_Device = Device;

    return FxPrePostCallback::InvokeStateless();
}

VOID
FxPnpDeviceSelfManagedIoSuspend::Initialize(
    _In_ FxPkgPnp* PkgPnp,
    _In_ PFN_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND Method
    )
{
    m_Method = Method;
    m_PkgPnp = PkgPnp;
    m_CallbackType = FxCxCallbackSmIoSuspend;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoSuspend::InvokeClient(
    VOID
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(m_Device);
        CallbackEnd();

        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(
                m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_INFORMATION, TRACINGPNP,
                "EvtDeviceSelfManagedIoInit WDFDEVICE 0x%p !devobj 0x%p failed, %!STATUS!",
                m_Device,
                m_PkgPnp->GetDevice()->GetDeviceObject(),
                status);
        }

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}
        
_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoSuspend::InvokeCxCallback(
    _In_ PFxCxPnpPowerCallbackContext Context,
    _In_ FxCxInvokeCallbackSubType PrePost
    )
{
    NTSTATUS status;
    PFN_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND method;

    if (PrePost == FxCxInvokePreCallback) {
        method = Context->u.SmIoSuspend.PreCallback;
    }
    else {
        method = Context->u.SmIoSuspend.PostCallback;
    }

    ASSERT(method != NULL);

    // 
    // EvtCxDevicePreSelfManagedIoSuspend EvtCxDevicePostSelfManagedIoSuspend
    //
    CallbackStart();
    status = method(m_Device);
    CallbackEnd();

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_INFORMATION, TRACINGPNP,
            "EvtCxDevice%sSelfManagedIoSuspend WDFDEVICE 0x%p !devobj "
            "0x%p failed, %!STATUS!",
            PrePost == FxCxInvokePreCallback ? "Pre" : "Post",
            m_Device,
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            status);
    }

    return status;
}

__drv_when(!NT_SUCCESS(return), __drv_arg(Progress, _Must_inspect_result_))
_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoRestart::Invoke(
    _In_  WDFDEVICE  Device,
    _Out_ FxCxCallbackProgress *Progress
    )
{
    m_Device = Device;

    return FxPrePostCallback::InvokeStateful(Progress,
                                             FxCxCleanupAfterPreOrClientFailure);
}

VOID
FxPnpDeviceSelfManagedIoRestart::Initialize(
    _In_ FxPkgPnp* PkgPnp,
    _In_ PFN_WDF_DEVICE_SELF_MANAGED_IO_RESTART Method
    )
{
    m_Method = Method;
    m_PkgPnp = PkgPnp;
    m_CallbackType = FxCxCallbackSmIoRestart;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoRestart::InvokeClient(
    VOID
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(m_Device);
        CallbackEnd();

        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(
                m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_INFORMATION, TRACINGPNP,
                "EvtDeviceSelfManagedIoRestart WDFDEVICE 0x%p !devobj 0x%p failed, %!STATUS!",
                m_Device,
                m_PkgPnp->GetDevice()->GetDeviceObject(),
                status);
        }
        
        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSelfManagedIoRestart::InvokeCxCallback(
    _In_ PFxCxPnpPowerCallbackContext Context,
    _In_ FxCxInvokeCallbackSubType PrePost
    )
{
    NTSTATUS status;
    PFN_WDF_DEVICE_SELF_MANAGED_IO_RESTART method;

    if (PrePost == FxCxInvokePreCallback) {
        method = Context->u.SmIoRestart.PreCallback;
    }
    else {
        method = Context->u.SmIoRestart.PostCallback;
    }

    ASSERT(method != NULL);

    // 
    // EvtCxDevicePreSelfManagedIoRestart EvtCxDevicePostSelfManagedIoRestart
    //
    CallbackStart();
    status = method(m_Device);
    CallbackEnd();

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(), TRACE_LEVEL_INFORMATION, TRACINGPNP,
            "EvtCxDevice%sSelfManagedIoRestart WDFDEVICE 0x%p !devobj "
            "0x%p failed, %!STATUS!",
            PrePost == FxCxInvokePreCallback ? "Pre" : "Post",
            m_Device,
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            status);
    }
    
    return status;
}

VOID
FxPnpDeviceSelfManagedIoRestart::InvokeCxCleanupCallback(
    _In_ PFxCxPnpPowerCallbackContext Context
    )
{
    PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_RESTART_FAILED_CLEANUP method;

    method = Context->u.SmIoRestart.CleanupCallback;
    ASSERT(method != NULL);                          

    CallbackStart();
    method(m_Device);
    CallbackEnd();

    return;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceQueryStop::Invoke(
    __in  WDFDEVICE  Device
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}


_Must_inspect_result_
NTSTATUS
FxPnpDeviceQueryRemove::Invoke(
    __in  WDFDEVICE  Device
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}


_Must_inspect_result_
NTSTATUS
FxPnpDeviceResourcesQuery::Invoke(
    __in WDFDEVICE  Device,
    __in WDFCMRESLIST Collection
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device, Collection);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}


_Must_inspect_result_
NTSTATUS
FxPnpDeviceResourceRequirementsQuery::Invoke(
    __in WDFDEVICE  Device,
    __in WDFIORESREQLIST Collection
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device, Collection);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceEject::Invoke(
    __in WDFDEVICE  Device
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

VOID
FxPnpDeviceSurpriseRemoval::Invoke(
    _In_ WDFDEVICE  Device
    )
{
    m_Device = Device;

    (VOID) FxPrePostCallback::InvokeStateless();

    return;
}

VOID
FxPnpDeviceSurpriseRemoval::Initialize(
    _In_ FxPkgPnp* PkgPnp,
    _In_ PFN_WDF_DEVICE_SURPRISE_REMOVAL Method
    )
{
    m_Method = Method;
    m_PkgPnp = PkgPnp;
    m_CallbackType = FxCxCallbackSurpriseRemoval;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSurpriseRemoval::InvokeClient(
    VOID
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(m_Device);
        CallbackEnd();
    }
    return STATUS_SUCCESS;
}

_Must_inspect_result_
NTSTATUS
FxPnpDeviceSurpriseRemoval::InvokeCxCallback(
    _In_ PFxCxPnpPowerCallbackContext Context,
    _In_ FxCxInvokeCallbackSubType PrePost
    )
{
    PFN_WDF_DEVICE_SURPRISE_REMOVAL method;
                    
    if (PrePost == FxCxInvokePreCallback) {
        method = Context->u.SurpriseRemoval.PreCallback;
    }
    else {
        method = Context->u.SurpriseRemoval.PostCallback;
    }
    
    ASSERT(method != NULL);

    //
    // EvtCxDevicePreSurpriseRemoval EvtCxDevicePostSurpriseRemoval
    //
    CallbackStart();
    method(m_Device);
    CallbackEnd();

    return STATUS_SUCCESS;
}

VOID
FxPnpDeviceUsageNotification::Invoke(
    __in WDFDEVICE Device,
    __in WDF_SPECIAL_FILE_TYPE NotificationType,
    __in BOOLEAN InPath
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(Device, NotificationType, InPath);
        CallbackEnd();
    }
}


_Must_inspect_result_
NTSTATUS
FxPnpDeviceUsageNotificationEx::Invoke(
    __in WDFDEVICE Device,
    __in WDF_SPECIAL_FILE_TYPE NotificationType,
    __in BOOLEAN InPath
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device, NotificationType, InPath);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}


VOID
FxPnpDeviceRelationsQuery::Invoke(
    __in WDFDEVICE Device,
    __in DEVICE_RELATION_TYPE RelationType
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(Device, RelationType);
        CallbackEnd();
    }
}


_Must_inspect_result_
NTSTATUS
FxPnpDeviceSetLock::Invoke(
    __in WDFDEVICE Device,
    __in BOOLEAN Lock
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device, Lock);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_UNSUCCESSFUL;
    }
}

VOID
FxPnpDeviceReportedMissing::Invoke(
    __in WDFDEVICE Device
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(Device);
        CallbackEnd();
    }
}

_Must_inspect_result_
NTSTATUS
FxPowerDeviceEnableWakeAtBus::Invoke(
    __in WDFDEVICE Device,
    __in SYSTEM_POWER_STATE PowerState
    )
{
    NTSTATUS status;

    if (m_Method != NULL) {
        CallbackStart();
        status = m_Method(Device, PowerState);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

VOID
FxPowerDeviceDisableWakeAtBus::Invoke(
    __in WDFDEVICE Device
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(Device);
        CallbackEnd();
    }
}


_Must_inspect_result_
NTSTATUS
FxPowerDeviceArmWakeFromS0::Invoke(
    __in WDFDEVICE Device
    )
{
    if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

_Must_inspect_result_
NTSTATUS
FxPowerDeviceArmWakeFromSx::Invoke(
    __in WDFDEVICE Device,
    __in BOOLEAN DeviceWakeEnabled,
    __in BOOLEAN ChildrenArmedForWake
    )
{
    if (m_MethodWithReason != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_MethodWithReason(Device,
                                    DeviceWakeEnabled,
                                    ChildrenArmedForWake);
        CallbackEnd();

        return status;
    }
    else if (m_Method != NULL) {
        NTSTATUS status;

        CallbackStart();
        status = m_Method(Device);
        CallbackEnd();

        return status;
    }
    else {
        return STATUS_SUCCESS;
    }
}

VOID
FxPowerDeviceDisarmWakeFromS0::Invoke(
    __in WDFDEVICE Device
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(Device);
        CallbackEnd();
    }
}
 
VOID
FxPowerDeviceDisarmWakeFromSx::Invoke(
    __in WDFDEVICE Device
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(Device);
        CallbackEnd();
    }
}

VOID
FxPowerDeviceWakeFromSxTriggered::Invoke(
    __in WDFDEVICE Device
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(Device);
        CallbackEnd();
    }
}
 
VOID
FxPowerDeviceWakeFromS0Triggered::Invoke(
    __in WDFDEVICE Device
    )
{
    if (m_Method != NULL) {
        CallbackStart();
        m_Method(Device);
        CallbackEnd();
    }
}


VOID
FxPnpStateCallback::Invoke(
    __in WDF_DEVICE_PNP_STATE State,
    __in WDF_STATE_NOTIFICATION_TYPE Type,
    __in WDFDEVICE Device,
    __in PCWDF_DEVICE_PNP_NOTIFICATION_DATA NotificationData
    )
{
    FxPnpStateCallbackInfo* pInfo;

    pInfo = &m_Methods[WdfDevStateNormalize(State)-WdfDevStatePnpObjectCreated];

    if (pInfo->Callback != NULL && (pInfo->Types & Type)) {
        CallbackStart();
        pInfo->Callback(Device, NotificationData);
        CallbackEnd();
    }
}


VOID
FxPowerStateCallback::Invoke(
    __in WDF_DEVICE_POWER_STATE State,
    __in WDF_STATE_NOTIFICATION_TYPE Type,
    __in WDFDEVICE Device,
    __in PCWDF_DEVICE_POWER_NOTIFICATION_DATA NotificationData
    )
{
    FxPowerStateCallbackInfo *pInfo;

    pInfo = &m_Methods[WdfDevStateNormalize(State)-WdfDevStatePowerObjectCreated];

    if (pInfo->Callback != NULL && (pInfo->Types & Type)) {
        CallbackStart();
        pInfo->Callback(Device, NotificationData);
        CallbackEnd();
    }
}


VOID
FxPowerPolicyStateCallback::Invoke(
    __in WDF_DEVICE_POWER_POLICY_STATE State,
    __in WDF_STATE_NOTIFICATION_TYPE Type,
    __in WDFDEVICE Device,
    __in PCWDF_DEVICE_POWER_POLICY_NOTIFICATION_DATA NotificationData
    )
{
    FxPowerPolicyStateCallbackInfo *pInfo;

    pInfo = &m_Methods[WdfDevStateNormalize(State)-WdfDevStatePwrPolObjectCreated];

    if (pInfo->Callback != NULL && (pInfo->Types & Type)) {
        CallbackStart();
        pInfo->Callback(Device, NotificationData);
        CallbackEnd();
    }
}

