/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDriverApiKm.cpp

Abstract:

    This module contains the "C" interface for the FxDriver object.

Author:



Environment:

    User mode only

Revision History:

--*/

#include "coreprivshared.hpp"

// Tracing support
extern "C" {
#include <ntverp.h>
#include "FxDriverApiUm.tmh"
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
    NTSTATUS status;
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    FxDriver* pDriver;
    FxRegKey* pKey;
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
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = pDriver->InitFxRegKey(DesiredAccess,
                            UMINT::WdfPropertyStoreRootDriverParametersKey,
                            pKey);
    if (!NT_SUCCESS(status)) {
        pKey->DeleteFromFailedCreate();
        return status;
    }

    status = STATUS_SUCCESS;
    *Key = keyHandle;
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
    DDI_ENTRY();

    UNREFERENCED_PARAMETER(DriverGlobals);
    UNREFERENCED_PARAMETER(Driver);

    ASSERTMSG("Not implemented for UMDF\n", FALSE);

    return NULL;
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
    DDI_ENTRY();

    UNREFERENCED_PARAMETER(DriverGlobals);
    UNREFERENCED_PARAMETER(DriverObject);

    ASSERTMSG("Not implemented for UMDF\n", FALSE);

    return NULL;
}

VOID
WDFEXPORT(WdfDriverMiniportUnload)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    WDFDRIVER Driver
    )
{
    DDI_ENTRY();

    UNREFERENCED_PARAMETER(DriverGlobals);
    UNREFERENCED_PARAMETER(Driver);

    ASSERTMSG("Not implemented for UMDF\n", FALSE);
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
    DDI_ENTRY();

    UNREFERENCED_PARAMETER(DriverGlobals);
    UNREFERENCED_PARAMETER(Driver);
    UNREFERENCED_PARAMETER(Attributes);
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(AttachedDeviceObject);
    UNREFERENCED_PARAMETER(Pdo);
    UNREFERENCED_PARAMETER(Device);

    ASSERTMSG("Not implemented for UMDF\n", FALSE);

    return STATUS_NOT_IMPLEMENTED;
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
    NTSTATUS status;
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    FxDriver* pDriver;
    FxRegKey* pKey;
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
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = pDriver->InitFxRegKey(DesiredAccess,
                        UMINT::WdfPropertyStoreRootDriverPersistentStateKey,
                        pKey);
    if (!NT_SUCCESS(status)) {
        pKey->DeleteFromFailedCreate();
        return status;
    }

    status = STATUS_SUCCESS;
    *Key = keyHandle;

    return status;
}

_Must_inspect_result_
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
WDFEXPORT(WdfDriverRetrieveDriverDataDirectoryString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDRIVER Driver,
    _In_
    WDFSTRING String
    )
{
    DDI_ENTRY();
    PFX_DRIVER_GLOBALS  pFxDriverGlobals;
    FxDriver*           pDriver;
    FxString*           pString;
    NTSTATUS            status;

    pFxDriverGlobals = GetFxDriverGlobals(DriverGlobals);

    status = FxVerifierCheckIrqlLevel(pFxDriverGlobals, PASSIVE_LEVEL);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    FxObjectHandleGetPtr(pFxDriverGlobals,
                         Driver,
                         FX_TYPE_DRIVER,
                         (PVOID*) &pDriver);

    FxObjectHandleGetPtr(pFxDriverGlobals,
                         String,
                         FX_TYPE_STRING,
                         (PVOID*) &pString);

    return pDriver->GetDriverDataDirectory(pString);
}

} // extern "C"
