/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxCxPnpPowerCallbacks.hpp

Abstract:

    This module implements the Cx callback objects.

Environment:

    Both kernel and user mode

Revision History:

--*/

#pragma once



//
// Callbacks like EvtDevicePrepareHardware have a contract with the client
// that states if the client callback is called then so will the
// EvtDeviceReleaseHardware. So in this case clean up is only needed after a
// failure happens in the PRE CX callbacks. After that we know ReleaseHardware
// will run and its PRE/POST CX callbacks can clean up.
//
// Other API's like EvtDeviceD0Entry have a contract that states if D0Entry
// fails then D0Exit won't get called. In this case we need to clean up after
// a PRE CX failure or if the client callback fails. If we don't the CX won't get a
// chance to clean up.
//
typedef enum : UCHAR {
    FxCxCleanupAfterPreOrClientFailure,
    FxCxCleanupAfterPreFailure
} FxCxCallbackCleanupAction;

//
// FxCxCallbackProgress:
// When !NT_SUCCESS(status), used to report the progress of the Cx state machines
// from the point of view of the clients callback, for which we have a contract
// about what happens before or after the client is called.
//
typedef enum : UCHAR {
    FxCxCallbackProgressInitialized = 0,
    FxCxCallbackProgressFailedInPreCalls = FxCxCallbackProgressInitialized,

    FxCxCallbackProgressClientCalled,
    FxCxCallbackProgressFailedInClientCall = FxCxCallbackProgressClientCalled,

    FxCxCallbackProgressClientSucceeded,
    FxCxCallbackProgressFailedInPostCalls = FxCxCallbackProgressClientSucceeded,

    FxCxCallbackProgressMax,
} FxCxCallbackProgress;

typedef enum  : UCHAR {
    FxCxPreCallback,
    FxCxPostCallback,
    FxCxCleanupCallback,
}FxCxCallbackSubType;

//
// Types of Cx Callbacks
//
enum FxCxCallbackType : UCHAR {

    //
    // Callback types that abort on chain failure for PRE callbacks. We also
    // cleanup up after the PRE calls if the CX won't be called with the paired
    // call; Paired meaning that D0Exit won't be called because D0Entry failed.
    // We refer to these as stateful callbacks.
    //
    FxCxCallbackPrepareHardware = 0,
    FxCxCallbackD0Entry,
    FxCxCallbackSmIoInit,
    FxCxCallbackSmIoRestart,
    FxCxCallbackSmIoRestartEx,
    FxCxCallbackArmWakeFromS0,
    FxCxCallbackArmWakeFromSx,
    FxCxCallbackArmWakeFromSxWithReason,
    FxCxCallbackD0EntryPostHwEnabled,

    //
    // Callback types that call all callbacks (pre, client, & post)
    // and returns the first error encountered. We refer to these as
    // stateless
    //
    FxCxCallbackReleaseHardware,
    FxCxCallbackD0Exit,
    FxCxCallbackSmIoSuspend,
    FxCxCallbackSmIoSuspendEx,
    FxCxCallbackSmIoFlush,
    FxCxCallbackSmIoCleanup,
    FxCxCallbackSurpriseRemoval,
    FxCxCallbackDisarmWakeFromS0,
    FxCxCallbackDisarmWakeFromSx,
    FxCxCallbackWakeFromS0Triggered,
    FxCxCallbackWakeFromSxTriggered,
    FxCxCallbackD0ExitPreHwDisabled,

    FxCxCallbackMax,
};

class FxCxPnpPowerCallbackContext : public FxStump {

public:
    FxCxPnpPowerCallbackContext(
        _In_ FxCxCallbackType Type
        ) : m_CallbackType(Type), m_PreCallbackSuccessful(FALSE)
    {
        RtlZeroMemory(&u, sizeof(u));
    }

    BOOLEAN
    IsPreCallbackPresent(
        VOID
        );

    BOOLEAN
    IsPostCallbackPresent(
        VOID
        );

    BOOLEAN
    IsCleanupCallbackPresent(
        VOID
        );

    FxCxCallbackType m_CallbackType;
    BOOLEAN          m_PreCallbackSuccessful;

    union {

        struct {
            PVOID PreCallback;
            PVOID PostCallback;
            PVOID CleanupCallback;
        } Generic;
        struct {
            PFN_WDFCX_DEVICE_PRE_PREPARE_HARDWARE PreCallback;
            PFN_WDFCX_DEVICE_POST_PREPARE_HARDWARE PostCallback;
            PFN_WDFCX_DEVICE_PRE_PREPARE_HARDWARE_FAILED_CLEANUP CleanupCallback;
        } PrepareHardware;
        struct {
            PFN_WDFCX_DEVICE_PRE_RELEASE_HARDWARE PreCallback;
            PFN_WDFCX_DEVICE_POST_RELEASE_HARDWARE PostCallback;
        } ReleaseHardware;
        struct {
            PFN_WDFCX_DEVICE_PRE_D0_ENTRY PreCallback;
            PFN_WDFCX_DEVICE_POST_D0_ENTRY PostCallback;
            PFN_WDFCX_DEVICE_PRE_D0_ENTRY_FAILED_CLEANUP CleanupCallback;
        } D0Entry;
        struct {
            PFN_WDFCX_DEVICE_PRE_D0_EXIT PreCallback;
            PFN_WDFCX_DEVICE_POST_D0_EXIT PostCallback;
        } D0Exit;
        struct {
            PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_INIT PreCallback;
            PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_INIT PostCallback;
            PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_INIT_FAILED_CLEANUP CleanupCallback;
        } SmIoInit;
        struct {
            PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_RESTART PreCallback;
            PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_RESTART PostCallback;
            PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_RESTART_FAILED_CLEANUP CleanupCallback;
        } SmIoRestart;
        struct {
            PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_RESTART_EX PreCallback;
            PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_RESTART_EX PostCallback;
            PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_RESTART_EX_FAILED_CLEANUP CleanupCallback;
        } SmIoRestartEx;
        struct {
            PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_SUSPEND PreCallback;
            PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_SUSPEND PostCallback;
        } SmIoSuspend;
        struct {
            PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_SUSPEND_EX PreCallback;
            PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_SUSPEND_EX PostCallback;
        } SmIoSuspendEx;
        struct {
            PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_FLUSH PreCallback;
            PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_FLUSH PostCallback;
        } SmIoFlush;
        struct {
            PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_CLEANUP PreCallback;
            PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_CLEANUP PostCallback;
        } SmIoCleanup;
        struct {
            PFN_WDFCX_DEVICE_PRE_SURPRISE_REMOVAL PreCallback;
            PFN_WDFCX_DEVICE_POST_SURPRISE_REMOVAL PostCallback;
        } SurpriseRemoval;
        struct {
            PFN_WDFCX_DEVICE_PRE_ARM_WAKE_FROM_S0 PreCallback;
            PFN_WDFCX_DEVICE_POST_ARM_WAKE_FROM_S0 PostCallback;
            PFN_WDFCX_DEVICE_PRE_ARM_WAKE_FROM_S0_FAILED_CLEANUP CleanupCallback;
        } ArmWakeFromS0;
        struct {
            PFN_WDFCX_DEVICE_PRE_DISARM_WAKE_FROM_S0 PreCallback;
            PFN_WDFCX_DEVICE_POST_DISARM_WAKE_FROM_S0 PostCallback;
        } DisarmWakeFromS0;
        struct {
            PFN_WDFCX_DEVICE_PRE_WAKE_FROM_S0_TRIGGERED PreCallback;
            PFN_WDFCX_DEVICE_POST_WAKE_FROM_S0_TRIGGERED PostCallback;
        } WakeFromS0Triggered;
        struct {
            PFN_WDFCX_DEVICE_PRE_ARM_WAKE_FROM_SX PreCallback;
            PFN_WDFCX_DEVICE_POST_ARM_WAKE_FROM_SX PostCallback;
            PFN_WDFCX_DEVICE_PRE_ARM_WAKE_FROM_SX_FAILED_CLEANUP CleanupCallback;
        } ArmWakeFromSx;
        struct {
            PFN_WDFCX_DEVICE_PRE_ARM_WAKE_FROM_SX_WITH_REASON PreCallback;
            PFN_WDFCX_DEVICE_POST_ARM_WAKE_FROM_SX_WITH_REASON PostCallback;
            PFN_WDFCX_DEVICE_PRE_ARM_WAKE_FROM_SX_WITH_REASON_FAILED_CLEANUP CleanupCallback;
        } ArmWakeFromSxWithReason;
        struct {
            PFN_WDFCX_DEVICE_PRE_DISARM_WAKE_FROM_SX PreCallback;
            PFN_WDFCX_DEVICE_POST_DISARM_WAKE_FROM_SX PostCallback;
        } DisarmWakeFromSx;
        struct {
            PFN_WDFCX_DEVICE_PRE_WAKE_FROM_SX_TRIGGERED PreCallback;
            PFN_WDFCX_DEVICE_POST_WAKE_FROM_SX_TRIGGERED PostCallback;
        } WakeFromSxTriggered;
        struct {
            PFN_WDFCX_DEVICE_PRE_D0_ENTRY_POST_HARDWARE_ENABLED PreCallback;
            PFN_WDFCX_DEVICE_POST_D0_ENTRY_POST_HARDWARE_ENABLED PostCallback;
            PFN_WDFCX_DEVICE_PRE_D0_ENTRY_POST_HARDWARE_ENABLED_FAILED_CLEANUP CleanupCallback;
        } D0EntryPostHardwareEnabled;
        struct {
            PFN_WDFCX_DEVICE_PRE_D0_EXIT_PRE_HARDWARE_DISABLED PreCallback;
            PFN_WDFCX_DEVICE_POST_D0_EXIT_PRE_HARDWARE_DISABLED PostCallback;
        } D0ExitPreHardwareDisabled;
    }u;
};
typedef FxCxPnpPowerCallbackContext *PFxCxPnpPowerCallbackContext;

typedef enum : UCHAR {
    FxCxInvokePreCallback,
    FxCxInvokePostCallback,
}FxCxInvokeCallbackSubType;

class FxPrePostCallback : public FxCallback {

public:
    _Must_inspect_result_
    virtual
    NTSTATUS
    InvokeClient(
        VOID
        ) = 0;

    _Must_inspect_result_
    virtual
    NTSTATUS
    InvokeCxCallback(
        _In_ PFxCxPnpPowerCallbackContext Context,
        _In_ FxCxInvokeCallbackSubType PrePost
        ) = 0;

    virtual
    VOID
    InvokeCxCleanupCallback(
        _In_ PFxCxPnpPowerCallbackContext Context
        )
    {
        //
        // Default implementation for stateless callbacks that do not specify
        // a cleanup callback function,
        //
        UNREFERENCED_PARAMETER(Context);
        ASSERT(0);
    };

    static
    VOID
    _SaveTheFirstError(
        _Inout_ NTSTATUS *FinalResult,
        _In_    NTSTATUS  IntermeidateResult
        );

    _Must_inspect_result_
    static
    NTSTATUS
    _InitializeContext(
        _In_  PFX_DRIVER_GLOBALS    Globals,
        _In_  PWDFCXDEVICE_INIT     CxInit,
        _Out_ PFxCxPnpPowerCallbackContext *Context,
        _In_  FxCxCallbackType      CallbackType
        );

    _Must_inspect_result_
    NTSTATUS
    InvokeStateful(
        _Out_opt_ FxCxCallbackProgress *Progress,
        _In_ FxCxCallbackCleanupAction CleanupAction
        );

    _Must_inspect_result_
    NTSTATUS
    InvokeStateless(
        VOID
        );

    _Must_inspect_result_
    NTSTATUS
    IssuePreCxCallbacksStateless(
        _In_ CfxDevice* Device
        );

    _Must_inspect_result_
    NTSTATUS
    IssuePreCxCallbacksStateful(
        _In_ CfxDevice* Device
        );

    _Must_inspect_result_
    NTSTATUS
    IssuePostCxCallbacks(
        _In_ CfxDevice* Device
        );

    VOID
    IssueCleanupCxCallbacks(
        _In_ CfxDevice* Device
        );

#if (FX_CORE_MODE==FX_CORE_KERNEL_MODE)
    virtual
    NTSTATUS
    InvokeCompanionCallback(
        _In_ FxCompanionTarget* CompanionTarget
        )
    {
        UNREFERENCED_PARAMETER(CompanionTarget);
        //
        // By default the base class returns STATUS_SUCCESS. Specific
        // callbacks such as prepare hw and d0entry will override this
        // definition
        //
        return STATUS_SUCCESS;
    }
#endif

protected:
    FxCxCallbackType m_CallbackType;
    FxPkgPnp*        m_PkgPnp;
};

