/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDriverApiKm.cpp

Abstract:

    This module contains the "C" interface for the FxDriver object.

Author:



Environment:

    Kernel mode only

Revision History:

--*/

#include "coreprivshared.hpp"

// Tracing support
extern "C" {
#include <ntverp.h>
#include "FxDriverApiKm.tmh"
}

//
// extern the whole file
//
extern "C" {

_Must_inspect_result_
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
WDFEXPORT(WdfDriverOpenParametersRegistryKey)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    WDFDRIVER Driver,
    __in
    ACCESS_MASK DesiredAccess,
    __in_opt
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    __out
    WDFKEY* Key
    )
{
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    NTSTATUS status;
    FxDriver* pDriver;
    FxRegKey* pKey;
    HANDLE hKey;
    WDFKEY keyHandle;

    pFxDriverGlobals = GetFxDriverGlobals(DriverGlobals);

    FxPointerNotNull(pFxDriverGlobals, Key);

    *Key = NULL;

    status = FxVerifierCheckIrqlLevel(pFxDriverGlobals, PASSIVE_LEVEL);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = FxValidateObjectAttributes(pFxDriverGlobals, KeyAttributes);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    FxObjectHandleGetPtr(pFxDriverGlobals,
                         Driver,
                         FX_TYPE_DRIVER,
                         (PVOID*) &pDriver);

    pKey = new(pFxDriverGlobals, KeyAttributes) FxRegKey(pFxDriverGlobals);

    if (pKey == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    status = pKey->Commit(KeyAttributes, (WDFOBJECT*)&keyHandle);

    if (NT_SUCCESS(status)) {

        if ((DesiredAccess & ~(KEY_READ | GENERIC_READ | STANDARD_RIGHTS_READ)) != 0) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_WARNING, TRACINGAPIERROR,
                "WdfDriverOpenParametersRegistryKey should not be used for write. "
                "Consider WdfDriverOpenPersistentStateRegistryKey instead.");

            //
            // TODO: Re-enable the breakpoint after fixing known violators
            //
            // FxVerifierDbgBreakPoint(pFxDriverGlobals);

            //
            // Static worker function (no object assignment for opened handled)
            //
            status = FxRegKey::_OpenKey(
                NULL, pDriver->GetRegistryPathUnicodeString(), &hKey);

            if (NT_SUCCESS(status)) {
                DECLARE_CONST_UNICODE_STRING(parameters, L"Parameters");

                //
                // This will store the resulting handle in pKey
                //
                status = pKey->Create(hKey, &parameters, DesiredAccess);
                if (NT_SUCCESS(status)) {
                    *Key = keyHandle;
                }

                FxRegKey::_Close(hKey);
            }
        }
        else {
            status = IoOpenDriverRegistryKey(pDriver->GetDriverObject(),
                                             DriverRegKeyParameters,
                                             DesiredAccess,
                                             0, // Flags - Must be 0
                                             &hKey);
            if (!NT_SUCCESS(status)) {
                DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGAPIERROR,
                    "WdfDriverOpenParametersRegistryKey failed with %!STATUS!",
                    status);
            }
            else {
                pKey->SetHandle(hKey);
                *Key = keyHandle;
            }
        }
    }

    if (!NT_SUCCESS(status)) {
        pKey->DeleteFromFailedCreate();
    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
PDRIVER_OBJECT
WDFEXPORT(WdfDriverWdmGetDriverObject)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    WDFDRIVER Driver
    )
{
    FxDriver *pDriver;

    FxObjectHandleGetPtr(GetFxDriverGlobals(DriverGlobals),
                         Driver,
                         FX_TYPE_DRIVER,
                         (PVOID*) &pDriver);

    return pDriver->GetDriverObject();
}

__drv_maxIRQL(DISPATCH_LEVEL)
WDFDRIVER
WDFEXPORT(WdfWdmDriverGetWdfDriverHandle)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    PDRIVER_OBJECT DriverObject
    )
{
    FxPointerNotNull(GetFxDriverGlobals(DriverGlobals), DriverObject);

    return FxDriver::GetFxDriver(DriverObject)->GetHandle();
}

VOID
WDFEXPORT(WdfDriverMiniportUnload)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    WDFDRIVER Driver
    )
{
    FxDriver *pDriver;

    FxObjectHandleGetPtr(GetFxDriverGlobals(DriverGlobals),
                         Driver,
                         FX_TYPE_DRIVER,
                         (PVOID *)&pDriver);

    FxDriver::Unload(pDriver->GetDriverObject());
}

_Must_inspect_result_
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
WDFEXPORT(WdfDeviceMiniportCreate)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    WDFDRIVER Driver,
    __in_opt
    PWDF_OBJECT_ATTRIBUTES Attributes,
    __in
    PDEVICE_OBJECT DeviceObject,
    __in_opt
    PDEVICE_OBJECT AttachedDeviceObject,
    __in_opt
    PDEVICE_OBJECT Pdo,
    __out
    WDFDEVICE* Device
    )
{
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    FxDriver* pDriver;
    FxMpDevice* pMpDevice;
    NTSTATUS status;

    FxObjectHandleGetPtrAndGlobals(GetFxDriverGlobals(DriverGlobals),
                                   Driver,
                                   FX_TYPE_DRIVER,
                                   (PVOID *)&pDriver,
                                   &pFxDriverGlobals);

    FxPointerNotNull(pFxDriverGlobals, DeviceObject);

    if (AttachedDeviceObject == NULL && Pdo != NULL) {
        FxPointerNotNull(pFxDriverGlobals, AttachedDeviceObject);
    }
    else if (AttachedDeviceObject != NULL && Pdo == NULL) {
       FxPointerNotNull(pFxDriverGlobals, Pdo);
    }

    status = FxVerifierCheckIrqlLevel(pFxDriverGlobals, PASSIVE_LEVEL);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = FxValidateObjectAttributes(pFxDriverGlobals, Attributes);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    pMpDevice = new(pFxDriverGlobals, Attributes)
        FxMpDevice(pFxDriverGlobals,
                   pDriver,
                   DeviceObject,
                   AttachedDeviceObject,
                   Pdo);

    if (pMpDevice == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    if (AttachedDeviceObject != NULL) {
        status = pMpDevice->AllocateTarget(&pMpDevice->m_DefaultTarget,
                                           FALSE /*SelfTarget=FALSE*/);
        if (!NT_SUCCESS(status)) {
            goto Done;
        }
    }

    status = FxDisposeList::_Create(pFxDriverGlobals,
                                   pMpDevice->GetDeviceObject(),
                                   &pMpDevice->m_DisposeList);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    status = pMpDevice->ConfigureConstraints(Attributes);
    if (!NT_SUCCESS(status)) {
        goto Done;
    }

    status = pMpDevice->Commit(Attributes, (PVOID*)Device);
    if (!NT_SUCCESS(status)) {  // follow the same error pattern as above.
        goto Done;
    }

Done:
    if (!NT_SUCCESS(status)) {
        pMpDevice->DeleteFromFailedCreate();
    }

    return status;
}

_Must_inspect_result_
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
WDFEXPORT(WdfDriverOpenPersistentStateRegistryKey)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDRIVER Driver,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    )
{
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    NTSTATUS status;
    FxDriver* pDriver;
    FxRegKey* pKey;
    HANDLE hKey;
    WDFKEY keyHandle;

    pFxDriverGlobals = GetFxDriverGlobals(DriverGlobals);

    FxPointerNotNull(pFxDriverGlobals, Key);

    *Key = NULL;

    status = FxVerifierCheckIrqlLevel(pFxDriverGlobals, PASSIVE_LEVEL);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = FxValidateObjectAttributes(pFxDriverGlobals, KeyAttributes);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    FxObjectHandleGetPtr(pFxDriverGlobals,
                         Driver,
                         FX_TYPE_DRIVER,
                         (PVOID*) &pDriver);

    pKey = new(pFxDriverGlobals, KeyAttributes) FxRegKey(pFxDriverGlobals);
    if (pKey == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    status = pKey->Commit(KeyAttributes, (WDFOBJECT*)&keyHandle);
    if (NT_SUCCESS(status)) {
        status = IoOpenDriverRegistryKey(pDriver->GetDriverObject(),
                                         DriverRegKeyPersistentState,
                                         DesiredAccess,
                                         0, // Flags - Must be 0
                                         &hKey);
        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGAPIERROR,
                "WdfDriverOpenPersistentStateRegistryKey failed because "
                "IoOpenDriverRegistryKey returned  %!STATUS!",
                status);
        }
        else {
            pKey->SetHandle(hKey);
            *Key = keyHandle;
        }
    }

    if (!NT_SUCCESS(status)) {
        pKey->DeleteFromFailedCreate();
    }

    return status;
}

} // extern "C"
