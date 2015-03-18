//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

typedef enum FxWmiInstanceAction FxWmiInstanceAction; // forward declaration

class UmToMx {
public:
    static
    _Must_inspect_result_
    NTSTATUS
    GetStackCapabilities(
        __in  MdDeviceObject DeviceObject,
        __out PSTACK_DEVICE_CAPABILITIES Capabilities
        );

    static
    VOID
    SetD3ColdSupport(
        __in  MdDeviceObject DeviceObject,
        __in  BOOLEAN UseD3Cold
        );

    static
    _Must_inspect_result_
    NTSTATUS
    InitializeUsbSS(
        __in  MdDeviceObject DeviceObject
        );

    static
    VOID
    SubmitUsbIdleNotification(
        __in  MdDeviceObject DeviceObject,
        __in  PUSB_IDLE_CALLBACK_INFO UsbIdleCallbackInfo,
        __in  MdCompletionRoutine UsbSSCompletionRoutine,
        __in  PVOID UsbSSCompletionContext
        );

    static
    VOID
    CancelUsbSS(
        __in  MdDeviceObject DeviceObject
        );

    static
    VOID
    SignalUsbSSCallbackProcessingComplete(
        __in  MdDeviceObject DeviceObject
        );

    static
    _Must_inspect_result_
    NTSTATUS
    WriteStateToRegistry(
        __in MdDeviceObject DeviceObject,
        __in const UNICODE_STRING *ValueName,
        __in ULONG Value
        );

    static
    _Must_inspect_result_
    NTSTATUS
    ReadStateFromRegistry(
        __in MdDeviceObject DeviceObject,
        __in const UNICODE_STRING *ValueName,
        __out ULONG *Value
        );

    static
    _Must_inspect_result_
    NTSTATUS
    UpdateWmiInstanceForS0Idle(
        __in MdDeviceObject DeviceObject,
        __in FxWmiInstanceAction Action
        );
    
    static
    _Must_inspect_result_
    NTSTATUS
    UpdateWmiInstanceForSxWake(
        __in MdDeviceObject DeviceObject,
        __in FxWmiInstanceAction Action
        );

};

