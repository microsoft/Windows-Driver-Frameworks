//
//    Copyright (C) Microsoft.  All rights reserved.
//
#include "fxmin.hpp"
#include "wudfx.h"

#include <UmToMx.hpp>

extern "C"
{
#include "UmToMx.tmh"
}

#define TraceEvents(a,b,c,d) UNREFERENCED_PARAMETER(d);

#define WUDF_POWER_POLICY_SETTINGS      L"WudfPowerPolicySettings"

_Must_inspect_result_
NTSTATUS
UmToMx::GetStackCapabilities(
    __in  MdDeviceObject DeviceObject,
    __out PSTACK_DEVICE_CAPABILITIES Capabilities
    )
{
    HRESULT hr;
    IWudfDeviceStack *deviceStack;

    deviceStack = DeviceObject->GetDeviceStackInterface();

    hr = deviceStack->GetStackCapabilities(Capabilities);

    if (S_OK == hr)
    {
        return STATUS_SUCCESS;
    }
    else
    {
        PUMDF_VERSION_DATA driverVersion = deviceStack->GetMinDriverVersion();
    
        BOOL preserveCompat = 
             deviceStack->ShouldPreserveIrpCompletionStatusCompatibility();

        return CHostFxUtil::NtStatusFromHr(
                                        hr,
                                        driverVersion->MajorNumber,
                                        driverVersion->MinorNumber,
                                        preserveCompat
                                        );
    }
}

VOID
UmToMx::SetD3ColdSupport(
    __in  MdDeviceObject DeviceObject,
    __in  BOOLEAN UseD3Cold
    )
{
    IWudfDeviceStack *deviceStack;

    deviceStack = DeviceObject->GetDeviceStackInterface();

    deviceStack->SetD3ColdSupport(UseD3Cold);
}

_Must_inspect_result_
NTSTATUS
UmToMx::InitializeUsbSS(
    __in  MdDeviceObject DeviceObject
    )
{
    HRESULT hr;
    IWudfDeviceStack *deviceStack;

    deviceStack = DeviceObject->GetDeviceStackInterface();

    hr = deviceStack->InitializeUsbSS();

    if (S_OK == hr)
    {
        return STATUS_SUCCESS;
    }
    else
    {
        PUMDF_VERSION_DATA driverVersion = deviceStack->GetMinDriverVersion();

        BOOL preserveCompat = 
             deviceStack->ShouldPreserveIrpCompletionStatusCompatibility();
    
        return CHostFxUtil::NtStatusFromHr(
                                        hr,
                                        driverVersion->MajorNumber,
                                        driverVersion->MinorNumber,
                                        preserveCompat
                                        );
    }
}

VOID
UmToMx::SubmitUsbIdleNotification(
    __in  MdDeviceObject DeviceObject,
    __in  PUSB_IDLE_CALLBACK_INFO UsbIdleCallbackInfo,
    __in  MdCompletionRoutine UsbSSCompletionRoutine,
    __in  PVOID UsbSSCompletionContext
    )
{
    IWudfDeviceStack *deviceStack;

    deviceStack = DeviceObject->GetDeviceStackInterface();

    deviceStack->SubmitUsbIdleNotification(UsbIdleCallbackInfo,
                                           UsbSSCompletionRoutine,
                                           UsbSSCompletionContext);
}

VOID
UmToMx::CancelUsbSS(
    __in  MdDeviceObject DeviceObject
    )
{
    IWudfDeviceStack *deviceStack;

    deviceStack = DeviceObject->GetDeviceStackInterface();

    deviceStack->CancelUsbSS();
}

VOID
UmToMx::SignalUsbSSCallbackProcessingComplete(
    __in  MdDeviceObject DeviceObject
    )
{
    IWudfDeviceStack *deviceStack;

    deviceStack = DeviceObject->GetDeviceStackInterface();

    deviceStack->SignalUsbSSCallbackProcessingComplete();

}

_Must_inspect_result_
NTSTATUS
UmToMx::WriteStateToRegistry(
    __in MdDeviceObject DeviceObject,
    __in const UNICODE_STRING *ValueName,
    __in ULONG Value
    )
{
    HRESULT                 hr;
    HKEY                    hKey = NULL;
    DWORD                   err;
    WDF_PROPERTY_STORE_ROOT propertyStore;
    IWudfDeviceStack       *deviceStack;

    ASSERT(ValueName != NULL && 
           ValueName->Length != 0 && 
           ValueName->Buffer != NULL);

    deviceStack = DeviceObject->GetDeviceStackInterface();

    propertyStore.LengthCb = sizeof(WDF_PROPERTY_STORE_ROOT);
    propertyStore.RootClass = WdfPropertyStoreRootClassHardwareKey;
    propertyStore.Qualifier.HardwareKey.ServiceName = WUDF_POWER_POLICY_SETTINGS;

    hr = deviceStack->CreateRegistryEntry(&propertyStore,
                             WdfPropertyStoreCreateIfMissing,
                             KEY_QUERY_VALUE | KEY_SET_VALUE,
                             NULL,
                             &hKey,
                             NULL
                             );

    if (FAILED(hr))
    {
        goto clean0;
    }

    err = RegSetValueEx(hKey,
                        ValueName->Buffer,
                        0,
                        REG_DWORD,
                        (BYTE *) &Value,
                        sizeof(Value));

    if (err != ERROR_SUCCESS)
    {
        hr = HRESULT_FROM_WIN32(err);
        TraceEvents(
            TRACE_LEVEL_ERROR,
            FX_TRACE_PNP,
            "%!FUNC!: Failed to set Registry value for S0Idle/SxWake error %d",
            err);
        goto clean0;
    }

    hr = S_OK;

clean0:

    if (NULL != hKey) 
    {
        RegCloseKey(hKey);
    }

    if (S_OK == hr)
    {
        return STATUS_SUCCESS;
    }
    else
    {
        PUMDF_VERSION_DATA driverVersion = deviceStack->GetMinDriverVersion();
    
        BOOL preserveCompat = 
             deviceStack->ShouldPreserveIrpCompletionStatusCompatibility();
        
        return CHostFxUtil::NtStatusFromHr(
                                        hr,
                                        driverVersion->MajorNumber,
                                        driverVersion->MinorNumber,
                                        preserveCompat
                                        );
    }
}

_Must_inspect_result_
NTSTATUS
UmToMx::ReadStateFromRegistry(
    __in MdDeviceObject DeviceObject,
    __in const UNICODE_STRING *ValueName,
    __out ULONG *Value
    )
{
    DWORD Err;
    HKEY pwrPolKey = NULL;
    DWORD Data;
    DWORD DataSize;
    IWudfDeviceStack *deviceStack;
    
    ASSERT(NULL != Value);
    ASSERT(ValueName != NULL && 
           ValueName->Length != 0 && 
           ValueName->Buffer != NULL);

    deviceStack = DeviceObject->GetDeviceStackInterface();
    *Value = 0;

    Err = RegOpenKeyEx(deviceStack->GetDeviceRegistryKey(),
                       WUDF_POWER_POLICY_SETTINGS,
                       0,
                       KEY_READ,
                       &pwrPolKey);
    if (ERROR_SUCCESS != Err)
    {
        TraceEvents(
            TRACE_LEVEL_ERROR, 
            RD_TRACE_PNP,
            "%!FUNC!: RegOpenKeyEx returned error %d\n", 
            Err);
        goto clean0;
    }

    DataSize = sizeof(Data);
    Err = RegQueryValueEx(pwrPolKey,
                          ValueName->Buffer,
                          NULL,
                          NULL,
                          (BYTE*) &Data,
                          &DataSize);
    if (ERROR_SUCCESS != Err)
    {
        TraceEvents(
            TRACE_LEVEL_ERROR, 
            RD_TRACE_PNP,
            "%!FUNC!: failed to read registry, RegQueryValueEx returned error %d\n", 
            Err);
        goto clean0;
    }

    *Value = Data;

    Err = ERROR_SUCCESS;

clean0:
    
    if (NULL != pwrPolKey) {
        RegCloseKey(pwrPolKey);
    }
    
    if (ERROR_SUCCESS == Err)
    {
        return STATUS_SUCCESS;
    }
    else
    {
        PUMDF_VERSION_DATA driverVersion = deviceStack->GetMinDriverVersion();
        BOOL preserveCompat = 
             deviceStack->ShouldPreserveIrpCompletionStatusCompatibility();
        
        return CHostFxUtil::NtStatusFromHr(
                                        HRESULT_FROM_WIN32(Err),
                                        driverVersion->MajorNumber,
                                        driverVersion->MinorNumber,
                                        preserveCompat
                                        );
    }
}

_Must_inspect_result_
NTSTATUS
UmToMx::UpdateWmiInstanceForS0Idle(
    __in MdDeviceObject DeviceObject,
    __in FxWmiInstanceAction Action
    )
{
    IWudfDeviceStack *deviceStack;
    HRESULT           hr;
    NTSTATUS          status;
    WmiIdleWakeInstanceUpdate updateType;

    deviceStack = DeviceObject->GetDeviceStackInterface();

    ASSERT(Action != InstanceActionInvalid);

    if (Action == AddInstance) {
        updateType = AddS0IdleInstance;
    } else {
        updateType = RemoveS0IdleInstance;
    }

    hr = deviceStack->UpdateIdleWakeWmiInstance(updateType);

    if (S_OK == hr)
    {
        status = STATUS_SUCCESS;
    }
    else
    {
        PUMDF_VERSION_DATA driverVersion = deviceStack->GetMinDriverVersion();
    
        BOOL preserveCompat = 
             deviceStack->ShouldPreserveIrpCompletionStatusCompatibility();
        
        status = CHostFxUtil::NtStatusFromHr(
                                        hr,
                                        driverVersion->MajorNumber,
                                        driverVersion->MinorNumber,
                                        preserveCompat
                                        );
    }
    
    if (!NT_SUCCESS(status)) {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            FX_TRACE_PNP,
            "%!FUNC!: failed to send ioctl to update S0Idle wmi instance "
            "%!STATUS!", status);
    }

    return status;
}

_Must_inspect_result_
NTSTATUS
UmToMx::UpdateWmiInstanceForSxWake(
    __in MdDeviceObject DeviceObject,
    __in FxWmiInstanceAction Action
    )
{
    IWudfDeviceStack *deviceStack;
    HRESULT           hr;
    NTSTATUS          status;
    WmiIdleWakeInstanceUpdate updateType;

    deviceStack = DeviceObject->GetDeviceStackInterface();

    ASSERT(Action != InstanceActionInvalid);

    if (Action == AddInstance) {
        updateType = AddSxWakeInstance;
    } else {
        updateType = RemoveSxWakeInstance;
    }

    hr = deviceStack->UpdateIdleWakeWmiInstance(updateType);

    if (S_OK == hr)
    {
        status = STATUS_SUCCESS;
    }
    else
    {
        PUMDF_VERSION_DATA driverVersion = deviceStack->GetMinDriverVersion();
    
        BOOL preserveCompat = 
             deviceStack->ShouldPreserveIrpCompletionStatusCompatibility();
        
        status = CHostFxUtil::NtStatusFromHr(
                                        hr,
                                        driverVersion->MajorNumber,
                                        driverVersion->MinorNumber,
                                        preserveCompat
                                        );
    }
    
    if (!NT_SUCCESS(status)) {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            FX_TRACE_PNP,
            "%!FUNC!: failed to send ioctl to update SxWake wmi instance "
            "%!STATUS!", status);
    }

    return status;
}

