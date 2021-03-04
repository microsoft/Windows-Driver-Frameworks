/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxVersionedStructures.h

Abstract:

    This is the main driver framework.








Environment:

    Both kernel and user mode

Revision History:

--*/

#pragma once

typedef struct _WDF_DRIVER_CONFIG_V1_0 {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    //
    // Event callbacks
    //
    PFN_WDF_DRIVER_DEVICE_ADD EvtDriverDeviceAdd;

    PFN_WDF_DRIVER_UNLOAD    EvtDriverUnload;

    //
    // Combination of WDF_DRIVER_INIT_FLAGS values
    //
    ULONG DriverInitFlags;

} WDF_DRIVER_CONFIG_V1_0, *PWDF_DRIVER_CONFIG_V1_0;

typedef struct _WDF_DRIVER_CONFIG_V1_1 {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    //
    // Event callbacks
    //
    PFN_WDF_DRIVER_DEVICE_ADD EvtDriverDeviceAdd;

    PFN_WDF_DRIVER_UNLOAD    EvtDriverUnload;

    //
    // Combination of WDF_DRIVER_INIT_FLAGS values
    //
    ULONG DriverInitFlags;

} WDF_DRIVER_CONFIG_V1_1, *PWDF_DRIVER_CONFIG_V1_1;

typedef struct _WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_V1_9 {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    //
    // Indicates whether the device can wake itself up while the machine is in
    // S0.
    //
    WDF_POWER_POLICY_S0_IDLE_CAPABILITIES IdleCaps;

    //
    // The low power state in which the device will be placed when it is idled
    // out while the machine is in S0.
    //
    DEVICE_POWER_STATE DxState;

    //
    // Amount of time the device must be idle before idling out.  Timeout is in
    // milliseconds.
    //
    ULONG IdleTimeout;

    //
    // Inidcates whether a user can control the idle policy of the device.
    // By default, a user is allowed to change the policy.
    //
    WDF_POWER_POLICY_S0_IDLE_USER_CONTROL UserControlOfIdleSettings;

    //
    // If WdfTrue, idling out while the machine is in S0 will be enabled.
    //
    // If WdfFalse, idling out will be disabled.
    //
    // If WdfUseDefault, the idling out will be enabled.  If
    // UserControlOfIdleSettings is set to IdleAllowUserControl, the user's
    // settings will override the default.
    //
    WDF_TRI_STATE Enabled;

    //
    // This field is applicable only when IdleCaps == IdleCannotWakeFromS0
    // If WdfTrue,device is powered up on System Wake even if device is idle
    // If WdfFalse, device is not powered up on system wake if it is idle
    // If WdfUseDefault, the behavior is same as WdfFalse
    //
    WDF_TRI_STATE PowerUpIdleDeviceOnSystemWake;

} WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_V1_9, *PWDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_V1_9;

typedef struct _WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_V1_7 {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    //
    // Indicates whether the device can wake itself up while the machine is in
    // S0.
    //
    WDF_POWER_POLICY_S0_IDLE_CAPABILITIES IdleCaps;

    //
    // The low power state in which the device will be placed when it is idled
    // out while the machine is in S0.
    //
    DEVICE_POWER_STATE DxState;

    //
    // Amount of time the device must be idle before idling out.  Timeout is in
    // milliseconds.
    //
    ULONG IdleTimeout;

    //
    // Inidcates whether a user can control the idle policy of the device.
    // By default, a user is allowed to change the policy.
    //
    WDF_POWER_POLICY_S0_IDLE_USER_CONTROL UserControlOfIdleSettings;

    //
    // If WdfTrue, idling out while the machine is in S0 will be enabled.
    //
    // If WdfFalse, idling out will be disabled.
    //
    // If WdfUseDefault, the idling out will be enabled.  If
    // UserControlOfIdleSettings is set to IdleAllowUserControl, the user's
    // settings will override the default.
    //
    WDF_TRI_STATE Enabled;

} WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_V1_7, *PWDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_V1_7;

typedef struct _WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_V1_5 {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    //
    // The low power state in which the device will be placed when it is armed
    // for wake from Sx.
    //
    DEVICE_POWER_STATE DxState;

    //
    // Inidcates whether a user can control the idle policy of the device.
    // By default, a user is allowed to change the policy.
    //
    WDF_POWER_POLICY_SX_WAKE_USER_CONTROL UserControlOfWakeSettings;

    //
    // If WdfTrue, arming the device for wake while the machine is in Sx is
    // enabled.
    //
    // If WdfFalse, arming the device for wake while the machine is in Sx is
    // disabled.
    //
    // If WdfUseDefault, arming will be enabled.  If UserControlOfWakeSettings
    // is set to WakeAllowUserControl, the user's settings will override the
    // default.
    //
    WDF_TRI_STATE Enabled;

} WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_V1_5, *PWDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_V1_5;

typedef struct _WDF_DEVICE_STATE_V1_27 {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    //
    // If set to WdfTrue, the device will be disabled
    //
    WDF_TRI_STATE Disabled;

    //
    // If set to WdfTrue, the device will not be displayed in device manager.
    // Once hidden, the device cannot be unhidden.
    //
    WDF_TRI_STATE DontDisplayInUI;

    //
    // If set to WdfTrue, the device is reporting itself as failed.  If set
    // in conjuction with ResourcesChanged to WdfTrue, the device will receive
    // a PnP stop and then a new PnP start device.
    //
    WDF_TRI_STATE Failed;

    //
    // If set to WdfTrue, the device cannot be subsequently disabled.
    //
    WDF_TRI_STATE NotDisableable;

    //
    // If set to WdfTrue, the device stack will be torn down.
    //
    WDF_TRI_STATE Removed;

    //
    // If set to WdfTrue, the device will be sent another PnP start.  If the
    // Failed field is set to WdfTrue as well, a PnP stop will be sent before
    // the start.
    //
    WDF_TRI_STATE ResourcesChanged;

} WDF_DEVICE_STATE_V1_27, *PWDF_DEVICE_STATE_V1_27;

typedef struct _WDF_IO_QUEUE_CONFIG_V1_7 {
    ULONG                                       Size;

    WDF_IO_QUEUE_DISPATCH_TYPE                  DispatchType;

    WDF_TRI_STATE                               PowerManaged;

    BOOLEAN                                     AllowZeroLengthRequests;

    BOOLEAN                                     DefaultQueue;

    PFN_WDF_IO_QUEUE_IO_DEFAULT                 EvtIoDefault;

    PFN_WDF_IO_QUEUE_IO_READ                    EvtIoRead;

    PFN_WDF_IO_QUEUE_IO_WRITE                   EvtIoWrite;

    PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL          EvtIoDeviceControl;

    PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL EvtIoInternalDeviceControl;

    PFN_WDF_IO_QUEUE_IO_STOP                    EvtIoStop;

    PFN_WDF_IO_QUEUE_IO_RESUME                  EvtIoResume;

    PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE       EvtIoCanceledOnQueue;

} WDF_IO_QUEUE_CONFIG_V1_7, *PWDF_IO_QUEUE_CONFIG_V1_7;

typedef struct _WDF_IO_QUEUE_CONFIG_V1_9 {
    ULONG                                       Size;

    WDF_IO_QUEUE_DISPATCH_TYPE                  DispatchType;

    WDF_TRI_STATE                               PowerManaged;

    BOOLEAN                                     AllowZeroLengthRequests;

    BOOLEAN                                     DefaultQueue;

    PFN_WDF_IO_QUEUE_IO_DEFAULT                 EvtIoDefault;

    PFN_WDF_IO_QUEUE_IO_READ                    EvtIoRead;

    PFN_WDF_IO_QUEUE_IO_WRITE                   EvtIoWrite;

    PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL          EvtIoDeviceControl;

    PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL EvtIoInternalDeviceControl;

    PFN_WDF_IO_QUEUE_IO_STOP                    EvtIoStop;

    PFN_WDF_IO_QUEUE_IO_RESUME                  EvtIoResume;

    PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE       EvtIoCanceledOnQueue;

    union {
        struct {
            ULONG NumberOfPresentedRequests;

        } Parallel;

    } Settings;

} WDF_IO_QUEUE_CONFIG_V1_9, *PWDF_IO_QUEUE_CONFIG_V1_9;

typedef struct _WDF_INTERRUPT_INFO_V1_7 {
    //
    // Size of this structure in bytes
    //
    ULONG                  Size;

    ULONG64                Reserved1;

    KAFFINITY              TargetProcessorSet;

    ULONG                  Reserved2;

    ULONG                  MessageNumber;

    ULONG                  Vector;

    KIRQL                  Irql;

    KINTERRUPT_MODE        Mode;

    WDF_INTERRUPT_POLARITY Polarity;

    BOOLEAN                MessageSignaled;

    // CM_SHARE_DISPOSITION
    UCHAR                  ShareDisposition;

} WDF_INTERRUPT_INFO_V1_7, *PWDF_INTERRUPT_INFO_V1_7;

typedef struct _WDF_INTERRUPT_CONFIG_V1_9 {
    ULONG              Size;

    //
    // If this interrupt is to be synchronized with other interrupt(s) assigned
    // to the same WDFDEVICE, create a WDFSPINLOCK and assign it to each of the
    // WDFINTERRUPTs config.
    //
    WDFSPINLOCK        SpinLock;

    WDF_TRI_STATE      ShareVector;

    BOOLEAN            FloatingSave;

    //
    // Automatic Serialization of the DpcForIsr
    //
    BOOLEAN            AutomaticSerialization;

    // Event Callbacks
    PFN_WDF_INTERRUPT_ISR         EvtInterruptIsr;

    PFN_WDF_INTERRUPT_DPC         EvtInterruptDpc;

    PFN_WDF_INTERRUPT_ENABLE      EvtInterruptEnable;

    PFN_WDF_INTERRUPT_DISABLE     EvtInterruptDisable;

} WDF_INTERRUPT_CONFIG_V1_9, *PWDF_INTERRUPT_CONFIG_V1_9;

//
// The interrupt config structure has changed post win8-Beta. This is a
// temporary definition to allow beta drivers to load on post-beta builds.
// Note that size of win8-beta and win8-postbeta structure is different only on
// non-x64 platforms, but the fact that size is same on amd64 is harmless because
// the struture gets zero'out by init macro, and the default value of the new
// field is 0 on amd64.
//
typedef struct _WDF_INTERRUPT_CONFIG_V1_11_BETA {
    ULONG              Size;

    //
    // If this interrupt is to be synchronized with other interrupt(s) assigned
    // to the same WDFDEVICE, create a WDFSPINLOCK and assign it to each of the
    // WDFINTERRUPTs config.
    //
    WDFSPINLOCK                     SpinLock;

    WDF_TRI_STATE                   ShareVector;

    BOOLEAN                         FloatingSave;

    //
    // DIRQL handling: automatic serialization of the DpcForIsr/WaitItemForIsr.
    // Passive-level handling: automatic serialization of all callbacks.
    //
    BOOLEAN                         AutomaticSerialization;

    //
    // Event Callbacks
    //
    PFN_WDF_INTERRUPT_ISR           EvtInterruptIsr;
    PFN_WDF_INTERRUPT_DPC           EvtInterruptDpc;
    PFN_WDF_INTERRUPT_ENABLE        EvtInterruptEnable;
    PFN_WDF_INTERRUPT_DISABLE       EvtInterruptDisable;
    PFN_WDF_INTERRUPT_WORKITEM      EvtInterruptWorkItem;

    //
    // These fields are only used when interrupt is created in
    // EvtDevicePrepareHardware callback.
    //
    PCM_PARTIAL_RESOURCE_DESCRIPTOR InterruptRaw;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR InterruptTranslated;

    //
    // Optional passive lock for handling interrupts at passive-level.
    //
    WDFWAITLOCK                     WaitLock;

    //
    // TRUE: handle interrupt at passive-level.
    // FALSE: handle interrupt at DIRQL level. This is the default.
    //
    BOOLEAN                         PassiveHandling;

} WDF_INTERRUPT_CONFIG_V1_11_BETA, *PWDF_INTERRUPT_CONFIG_V1_11_BETA;

typedef struct _WDF_INTERRUPT_CONFIG_V1_11 {
    ULONG              Size;

    //
    // If this interrupt is to be synchronized with other interrupt(s) assigned
    // to the same WDFDEVICE, create a WDFSPINLOCK and assign it to each of the
    // WDFINTERRUPTs config.
    //
    WDFSPINLOCK                     SpinLock;

    WDF_TRI_STATE                   ShareVector;

    BOOLEAN                         FloatingSave;

    //
    // DIRQL handling: automatic serialization of the DpcForIsr/WaitItemForIsr.
    // Passive-level handling: automatic serialization of all callbacks.
    //
    BOOLEAN                         AutomaticSerialization;

    //
    // Event Callbacks
    //
    PFN_WDF_INTERRUPT_ISR           EvtInterruptIsr;
    PFN_WDF_INTERRUPT_DPC           EvtInterruptDpc;
    PFN_WDF_INTERRUPT_ENABLE        EvtInterruptEnable;
    PFN_WDF_INTERRUPT_DISABLE       EvtInterruptDisable;
    PFN_WDF_INTERRUPT_WORKITEM      EvtInterruptWorkItem;

    //
    // These fields are only used when interrupt is created in
    // EvtDevicePrepareHardware callback.
    //
    PCM_PARTIAL_RESOURCE_DESCRIPTOR InterruptRaw;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR InterruptTranslated;

    //
    // Optional passive lock for handling interrupts at passive-level.
    //
    WDFWAITLOCK                     WaitLock;

    //
    // TRUE: handle interrupt at passive-level.
    // FALSE: handle interrupt at DIRQL level. This is the default.
    //
    BOOLEAN                         PassiveHandling;

    //
    // TRUE: Interrupt is reported inactive on explicit power down
    //       instead of disconnecting it.
    // FALSE: Interrupt is disconnected instead of reporting inactive
    //        on explicit power down.
    // DEFAULT: Framework decides the right value.
    //
    WDF_TRI_STATE                   ReportInactiveOnPowerDown;

} WDF_INTERRUPT_CONFIG_V1_11, *PWDF_INTERRUPT_CONFIG_V1_11;

typedef struct _WDF_IO_TARGET_OPEN_PARAMS_V1_11 {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    //
    // Indicates which fields of this structure are going to be used in
    // creating the WDFIOTARGET.
    //
    WDF_IO_TARGET_OPEN_TYPE Type;

    //
    // Notification when the target is being queried for removal.
    // If !NT_SUCCESS is returned, the query will fail and the target will
    // remain opened.
    //
    PFN_WDF_IO_TARGET_QUERY_REMOVE EvtIoTargetQueryRemove;

    //
    // The previous query remove has been canceled and the target can now be
    // reopened.
    //
    PFN_WDF_IO_TARGET_REMOVE_CANCELED EvtIoTargetRemoveCanceled;

    //
    // The query remove has succeeded and the target is now removed from the
    // system.
    //
    PFN_WDF_IO_TARGET_REMOVE_COMPLETE EvtIoTargetRemoveComplete;

    // ========== WdfIoTargetOpenUseExistingDevice begin ==========
    //
    // The device object to send requests to
    //
    PDEVICE_OBJECT TargetDeviceObject;

    //
    // File object representing the TargetDeviceObject.  The PFILE_OBJECT will
    // be passed as a parameter in all requests sent to the resulting
    // WDFIOTARGET.
    //
    PFILE_OBJECT TargetFileObject;

    // ========== WdfIoTargetOpenUseExistingDevice end ==========
    //
    // ========== WdfIoTargetOpenByName begin ==========
    //
    // Name of the device to open.
    //
    UNICODE_STRING TargetDeviceName;

    //
    // The access desired on the device being opened up, ie WDM FILE_XXX_ACCESS
    // such as FILE_ANY_ACCESS, FILE_SPECIAL_ACCESS, FILE_READ_ACCESS, or
    // FILE_WRITE_ACCESS or you can use values such as GENERIC_READ,
    // GENERIC_WRITE, or GENERIC_ALL.
    //
    ACCESS_MASK DesiredAccess;

    //
    // Share access desired on the target being opened, ie WDM FILE_SHARE_XXX
    // values such as FILE_SHARE_READ, FILE_SHARE_WRITE, FILE_SHARE_DELETE.
    //
    // A zero value means exclusive access to the target.
    //
    ULONG ShareAccess;

    //
    // File  attributes, see ZwCreateFile in the DDK for a list of valid
    // values and their meaning.
    //
    ULONG FileAttributes;

    //
    // Create disposition, see ZwCreateFile in the DDK for a list of valid
    // values and their meaning.
    //
    ULONG CreateDisposition;

    //
    // Options for opening the device, see CreateOptions for ZwCreateFile in the
    // DDK for a list of valid values and their meaning.
    //
    ULONG CreateOptions;

    PVOID EaBuffer;

    ULONG EaBufferLength;

    PLONGLONG AllocationSize;

    // ========== WdfIoTargetOpenByName end ==========
    //
    //
    // On return for a create by name, this will contain one of the following
    // values:  FILE_CREATED, FILE_OPENED, FILE_OVERWRITTEN, FILE_SUPERSEDED,
    // FILE_EXISTS, FILE_DOES_NOT_EXIST
    //
    ULONG FileInformation;

} WDF_IO_TARGET_OPEN_PARAMS_V1_11, *PWDF_IO_TARGET_OPEN_PARAMS_V1_11;

typedef struct _WDF_PDO_EVENT_CALLBACKS_V1_9 {
    //
    // The size of this structure in bytes
    //
    ULONG Size;

    //
    // Called in response to IRP_MN_QUERY_RESOURCES
    //
    PFN_WDF_DEVICE_RESOURCES_QUERY EvtDeviceResourcesQuery;

    //
    // Called in response to IRP_MN_QUERY_RESOURCE_REQUIREMENTS
    //
    PFN_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY EvtDeviceResourceRequirementsQuery;

    //
    // Called in response to IRP_MN_EJECT
    //
    PFN_WDF_DEVICE_EJECT EvtDeviceEject;

    //
    // Called in response to IRP_MN_SET_LOCK
    //
    PFN_WDF_DEVICE_SET_LOCK EvtDeviceSetLock;

    //
    // Called in response to the power policy owner sending a wait wake to the
    // PDO.  Bus generic arming shoulding occur here.
    //
    PFN_WDF_DEVICE_ENABLE_WAKE_AT_BUS       EvtDeviceEnableWakeAtBus;

    //
    // Called in response to the power policy owner sending a wait wake to the
    // PDO.  Bus generic disarming shoulding occur here.
    //
    PFN_WDF_DEVICE_DISABLE_WAKE_AT_BUS      EvtDeviceDisableWakeAtBus;

} WDF_PDO_EVENT_CALLBACKS_V1_9, *PWDF_PDO_EVENT_CALLBACKS_V1_9;

typedef struct _WDF_PNPPOWER_EVENT_CALLBACKS_V1_9 {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    PFN_WDF_DEVICE_D0_ENTRY                 EvtDeviceD0Entry;

    PFN_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED EvtDeviceD0EntryPostInterruptsEnabled;

    PFN_WDF_DEVICE_D0_EXIT                  EvtDeviceD0Exit;

    PFN_WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED EvtDeviceD0ExitPreInterruptsDisabled;

    PFN_WDF_DEVICE_PREPARE_HARDWARE         EvtDevicePrepareHardware;

    PFN_WDF_DEVICE_RELEASE_HARDWARE         EvtDeviceReleaseHardware;

    PFN_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP  EvtDeviceSelfManagedIoCleanup;

    PFN_WDF_DEVICE_SELF_MANAGED_IO_FLUSH    EvtDeviceSelfManagedIoFlush;

    PFN_WDF_DEVICE_SELF_MANAGED_IO_INIT     EvtDeviceSelfManagedIoInit;

    PFN_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND  EvtDeviceSelfManagedIoSuspend;

    PFN_WDF_DEVICE_SELF_MANAGED_IO_RESTART  EvtDeviceSelfManagedIoRestart;

    PFN_WDF_DEVICE_SURPRISE_REMOVAL         EvtDeviceSurpriseRemoval;

    PFN_WDF_DEVICE_QUERY_REMOVE             EvtDeviceQueryRemove;

    PFN_WDF_DEVICE_QUERY_STOP               EvtDeviceQueryStop;

    PFN_WDF_DEVICE_USAGE_NOTIFICATION       EvtDeviceUsageNotification;

    PFN_WDF_DEVICE_RELATIONS_QUERY          EvtDeviceRelationsQuery;

} WDF_PNPPOWER_EVENT_CALLBACKS_V1_9, *PWDF_PNPPOWER_EVENT_CALLBACKS_V1_9;

typedef struct _WDF_POWER_POLICY_EVENT_CALLBACKS_V1_5 {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    PFN_WDF_DEVICE_ARM_WAKE_FROM_S0         EvtDeviceArmWakeFromS0;

    PFN_WDF_DEVICE_DISARM_WAKE_FROM_S0      EvtDeviceDisarmWakeFromS0;

    PFN_WDF_DEVICE_WAKE_FROM_S0_TRIGGERED   EvtDeviceWakeFromS0Triggered;

    PFN_WDF_DEVICE_ARM_WAKE_FROM_SX         EvtDeviceArmWakeFromSx;

    PFN_WDF_DEVICE_DISARM_WAKE_FROM_SX      EvtDeviceDisarmWakeFromSx;

    PFN_WDF_DEVICE_WAKE_FROM_SX_TRIGGERED   EvtDeviceWakeFromSxTriggered;

} WDF_POWER_POLICY_EVENT_CALLBACKS_V1_5, *PWDF_POWER_POLICY_EVENT_CALLBACKS_V1_5;

typedef struct _WDF_TIMER_CONFIG_V1_7 {
    ULONG         Size;

    PFN_WDF_TIMER EvtTimerFunc;

    LONG          Period;

    //
    // If this is TRUE, the Timer will automatically serialize
    // with the event callback handlers of its Parent Object.
    //
    // Parent Object's callback constraints should be compatible
    // with the Timer DPC (DISPATCH_LEVEL), or the request will fail.
    //
    BOOLEAN       AutomaticSerialization;

} WDF_TIMER_CONFIG_V1_7, *PWDF_TIMER_CONFIG_V1_7;

typedef struct _WDF_TIMER_CONFIG_V1_11 {
    ULONG Size;

    PFN_WDF_TIMER EvtTimerFunc;

    ULONG Period;

    //
    // If this is TRUE, the Timer will automatically serialize
    // with the event callback handlers of its Parent Object.
    //
    // Parent Object's callback constraints should be compatible
    // with the Timer DPC (DISPATCH_LEVEL), or the request will fail.
    //
    BOOLEAN AutomaticSerialization;

    //
    // Optional tolerance for the timer in milliseconds.
    //
    ULONG TolerableDelay;

} WDF_TIMER_CONFIG_V1_11, *PWDF_TIMER_CONFIG_V1_11;

typedef struct _WDFCX_PNPPOWER_EVENT_CALLBACKS_V1_29 {

    //
    // Size of this structure in bytes
    //
    ULONG Size;

    PFN_WDFCX_DEVICE_PRE_PREPARE_HARDWARE                EvtCxDevicePrePrepareHardware;
    PFN_WDFCX_DEVICE_PRE_PREPARE_HARDWARE_FAILED_CLEANUP EvtCxDevicePrePrepareHardwareFailedCleanup;
    PFN_WDFCX_DEVICE_POST_PREPARE_HARDWARE               EvtCxDevicePostPrepareHardware;
    PFN_WDFCX_DEVICE_PRE_RELEASE_HARDWARE                EvtCxDevicePreReleaseHardware;
    PFN_WDFCX_DEVICE_POST_RELEASE_HARDWARE               EvtCxDevicePostReleaseHardware;
    PFN_WDFCX_DEVICE_PRE_D0_ENTRY                        EvtCxDevicePreD0Entry;
    PFN_WDFCX_DEVICE_PRE_D0_ENTRY_FAILED_CLEANUP         EvtCxDevicePreD0EntryFailedCleanup;
    PFN_WDFCX_DEVICE_POST_D0_ENTRY                       EvtCxDevicePostD0Entry;
    PFN_WDFCX_DEVICE_PRE_D0_EXIT                         EvtCxDevicePreD0Exit;
    PFN_WDFCX_DEVICE_POST_D0_EXIT                        EvtCxDevicePostD0Exit;

    PFN_WDFCX_DEVICE_PRE_SURPRISE_REMOVAL                EvtCxDevicePreSurpriseRemoval;
    PFN_WDFCX_DEVICE_POST_SURPRISE_REMOVAL               EvtCxDevicePostSurpriseRemoval;

    PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_INIT            EvtCxDevicePreSelfManagedIoInit;
    PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_INIT_FAILED_CLEANUP    EvtCxDevicePreSelfManagedIoInitFailedCleanup;
    PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_INIT           EvtCxDevicePostSelfManagedIoInit;
    PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_RESTART         EvtCxDevicePreSelfManagedIoRestart;
    PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_RESTART_FAILED_CLEANUP EvtCxDevicePreSelfManagedIoRestartFailedCleanup;
    PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_RESTART        EvtCxDevicePostSelfManagedIoRestart;
    PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_SUSPEND         EvtCxDevicePreSelfManagedIoSuspend;
    PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_SUSPEND        EvtCxDevicePostSelfManagedIoSuspend;
    PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_FLUSH           EvtCxDevicePreSelfManagedIoFlush;
    PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_FLUSH          EvtCxDevicePostSelfManagedIoFlush;
    PFN_WDFCX_DEVICE_PRE_SELF_MANAGED_IO_CLEANUP         EvtCxDevicePreSelfManagedIoCleanup;
    PFN_WDFCX_DEVICE_POST_SELF_MANAGED_IO_CLEANUP        EvtCxDevicePostSelfManagedIoCleanup;

} WDFCX_PNPPOWER_EVENT_CALLBACKS_V1_29, *PWDFCX_PNPPOWER_EVENT_CALLBACKS_V1_29;
