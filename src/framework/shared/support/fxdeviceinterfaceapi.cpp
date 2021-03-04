/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDeviceInterfaceAPI.cpp

Abstract:

    This module implements the device interface object external APIs

Author:




Environment:

    kernel and user mode

Revision History:

--*/

#include "FxSupportPch.hpp"

extern "C" {
#include "FxDeviceInterfaceAPI.tmh"
}

//
// extern "C" the entire file
//
extern "C" {

_Must_inspect_result_
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
WDFEXPORT(WdfDeviceCreateDeviceInterface)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    WDFDEVICE Device,
    __in
    CONST GUID *InterfaceClassGUID,
    __in_opt
    PCUNICODE_STRING ReferenceString
    )
/*++

Routine Description:
    Creates a device interface associated with the passed in device object

Arguments:
    Device - Handle which represents the device exposing the interface

    InterfaceGUID - GUID describing the interface being exposed

    ReferenceString - OPTIONAL string which allows the driver writer to
        distinguish between different exposed interfaces

Return Value:
    STATUS_SUCCESS or appropriate NTSTATUS code

  --*/
{
    DDI_ENTRY();

    SINGLE_LIST_ENTRY **ppPrev, *pCur;
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    FxDeviceInterface *pDeviceInterface;
    FxDevice *pDevice;
    FxPkgPnp* pPkgPnp;
    NTSTATUS status;

    FxObjectHandleGetPtrAndGlobals(GetFxDriverGlobals(DriverGlobals),
                                   Device,
                                   FX_TYPE_DEVICE,
                                   (PVOID*) &pDevice,
                                   &pFxDriverGlobals);

    FxPointerNotNull(pFxDriverGlobals, InterfaceClassGUID);

    status = FxVerifierCheckIrqlLevel(pFxDriverGlobals, PASSIVE_LEVEL);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (ReferenceString != NULL) {
        status = FxValidateUnicodeString(pFxDriverGlobals, ReferenceString);
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    if (pDevice->IsLegacy()) {
        status = STATUS_INVALID_DEVICE_REQUEST;

        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGERROR,
            "WDFDEVICE %p is not a PNP device, device interface creation not "
            "allowed %!STATUS!", Device, status);

        return status;
    }

    pDeviceInterface = new(pFxDriverGlobals, PagedPool) FxDeviceInterface();

    if (pDeviceInterface == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;

        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGERROR,
            "WDFDEVICE %p DeviceInterface object creation failed, %!STATUS!",
            Device, status);

        return status;
    }

    status = pDeviceInterface->Initialize(pFxDriverGlobals,
                                          InterfaceClassGUID,
                                          ReferenceString);

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGERROR,
            "WDFDEVICE %p, DeviceInterface object initialization failed, %!STATUS!",
            Device, status );

        goto Done;
    }

    pPkgPnp = pDevice->m_PkgPnp;

    pPkgPnp->m_DeviceInterfaceLock.AcquireLock(pFxDriverGlobals);

    status = pDeviceInterface->Register(pDevice);

    if (NT_SUCCESS(status)) {
        //
        // Insert into the end of the list
        //
        ppPrev = &pPkgPnp->m_DeviceInterfaceHead.Next;
        pCur = pPkgPnp->m_DeviceInterfaceHead.Next;
        while (pCur != NULL) {
            ppPrev = &pCur->Next;
            pCur = pCur->Next;
        }

        *ppPrev = &pDeviceInterface->m_Entry;
    }

    pPkgPnp->m_DeviceInterfaceLock.ReleaseLock(pFxDriverGlobals);

Done:
    if (!NT_SUCCESS(status)) {
        delete pDeviceInterface;
        pDeviceInterface = NULL;
    }

    return status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
WdfDeviceSetDeviceInterfaceStateWorker(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    WDFDEVICE Device,
    __in
    CONST GUID *InterfaceClassGUID,
    __in_opt
    PCUNICODE_STRING RefString,
    __in
    BOOLEAN State,
    __in
    WDF_TRI_STATE AutoEnableOnFirstStart
    )
{
    DDI_ENTRY();

    PSINGLE_LIST_ENTRY ple;
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    NTSTATUS status;
    FxDevice* pDevice;
    FxPkgPnp* pPkgPnp;

    FxObjectHandleGetPtrAndGlobals(GetFxDriverGlobals(DriverGlobals),
                                   Device,
                                   FX_TYPE_DEVICE,
                                   (PVOID*) &pDevice,
                                   &pFxDriverGlobals);

    FxPointerNotNull(pFxDriverGlobals, InterfaceClassGUID);

    status = FxVerifierCheckIrqlLevel(pFxDriverGlobals, PASSIVE_LEVEL);
    if (!NT_SUCCESS(status)) {
        FxVerifierDbgBreakPoint(pFxDriverGlobals);
        return;
    }

    if (RefString != NULL) {
        status = FxValidateUnicodeString(pFxDriverGlobals, RefString);
        if (!NT_SUCCESS(status)) {
            FxVerifierDbgBreakPoint(pFxDriverGlobals);
            return;
        }
    }

    if (pDevice->IsLegacy()) {
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGERROR,
            "WDFDEVICE %p is not a PNP device, device interfaces not allowed",
            Device);
        FxVerifierDbgBreakPoint(pFxDriverGlobals);
        return;
    }

    pPkgPnp = pDevice->m_PkgPnp;

    pPkgPnp->m_DeviceInterfaceLock.AcquireLock(pFxDriverGlobals);

    //
    // Iterate over the interfaces and see if we have a match
    //
    for (ple = pPkgPnp->m_DeviceInterfaceHead.Next; ple != NULL; ple = ple->Next) {
        FxDeviceInterface *pDI;

        pDI = FxDeviceInterface::_FromEntry(ple);

        if (FxIsEqualGuid(&pDI->m_InterfaceClassGUID, InterfaceClassGUID)) {
            if (RefString != NULL) {
                if ((RefString->Length == pDI->m_ReferenceString.Length)
                    &&
                    (RtlCompareMemory(RefString->Buffer,
                                      pDI->m_ReferenceString.Buffer,
                                      RefString->Length) == RefString->Length)) {
                    //
                    // They match, carry on
                    //
                    DO_NOTHING();
                }
                else {
                    //
                    // The ref strings do not match, continue on in the search
                    // of the collection.
                    //
                    continue;
                }
            }
            else if (pDI->m_ReferenceString.Length > 0) {
                //
                // Caller didn't specify a ref string but this interface has
                // one, continue on in the search through the collection.
                //
                continue;
            }

            //
            // Set the state and break out of the loop because we found our
            // interface.
            //
            pDI->SetState(State);

            if (AutoEnableOnFirstStart == WdfFalse) {
                pDI->m_AutoEnableOnFirstStart = FALSE;
            }
            else if (AutoEnableOnFirstStart == WdfTrue) {
                pDI->m_AutoEnableOnFirstStart = TRUE;
            }

            break;
        }
    }

    pPkgPnp->m_DeviceInterfaceLock.ReleaseLock(pFxDriverGlobals);
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
WDFEXPORT(WdfDeviceSetDeviceInterfaceState)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    CONST GUID *InterfaceClassGUID,
    _In_opt_
    PCUNICODE_STRING RefString,
    _In_
    BOOLEAN IsInterfaceEnabled
    )
/*++

Routine Description:
    Enables or disables a device interface for a specified device.

Arguments:
    Device - Handle which represents the device exposing the interface

    InterfaceClassGUID - GUID describing the interface being exposed

    RefString - OPTIONAL string which allows the driver writer to
        distinguish between different exposed interfaces

    IsInterfaceEnabled - if TRUE, enables the specified device interface
        instance or, if FALSE, disables it.

Remarks:
    The API should only be called after the device has started. If you want to
    call the API during adding device, use WdfDeviceSetDeviceInterfaceStateEx instead.

Return Value:
    VOID

  --*/
{
    WdfDeviceSetDeviceInterfaceStateWorker(
        DriverGlobals,
        Device,
        InterfaceClassGUID,
        RefString,
        IsInterfaceEnabled,
        WdfUseDefault);
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
WDFEXPORT(WdfDeviceSetDeviceInterfaceStateEx)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    CONST GUID *InterfaceClassGUID,
    _In_opt_
    PCUNICODE_STRING RefString,
    _In_
    BOOLEAN IsInterfaceEnabled
    )
/*++

Routine Description:
    Enables or disables a device interface for a specified device.

Arguments:
    Device - Handle which represents the device exposing the interface

    InterfaceClassGUID - GUID describing the interface being exposed

    RefString - OPTIONAL string which allows the driver writer to
        distinguish between different exposed interfaces

    IsInterfaceEnabled - if TRUE, enables the specified device interface
        instance or, if FALSE, disables it.

Remarks:
    The API can be called during adding device, or after device has started.

    If called during adding device, EnableInterface should be FALSE, which
    means the interface won't be automatically enabled when the device starts.
    Passing TRUE is redundant as interfaces will be auto enabled by default.

    If called after device has started, the behavior is the same as
    WdfDeviceSetDeviceInterfaceState.

Return Value:
    VOID

  --*/
{
    WdfDeviceSetDeviceInterfaceStateWorker(
        DriverGlobals,
        Device,
        InterfaceClassGUID,
        RefString,
        IsInterfaceEnabled,
        WdfFalse);
}

_Must_inspect_result_
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
WDFEXPORT(WdfDeviceRetrieveDeviceInterfaceString)(
    __in
    PWDF_DRIVER_GLOBALS DriverGlobals,
    __in
    WDFDEVICE Device,
    __in
    CONST GUID* InterfaceClassGUID,
    __in_opt
    PCUNICODE_STRING RefString,
    __in
    WDFSTRING String
    )
/*++

Routine Description:
    Returns the symbolic link value of the registered device interface.

Arguments:


Return Value:


  --*/

{
    DDI_ENTRY();

    PSINGLE_LIST_ENTRY ple;
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    FxDevice* pDevice;
    FxPkgPnp* pPkgPnp;
    FxString* pString;
    NTSTATUS status;

    FxObjectHandleGetPtrAndGlobals(GetFxDriverGlobals(DriverGlobals),
                                   Device,
                                   FX_TYPE_DEVICE,
                                   (PVOID*) &pDevice,
                                   &pFxDriverGlobals );

    FxPointerNotNull(pFxDriverGlobals, InterfaceClassGUID);

    status = FxVerifierCheckIrqlLevel(pFxDriverGlobals, PASSIVE_LEVEL);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (RefString != NULL) {
        status = FxValidateUnicodeString(pFxDriverGlobals, RefString);
        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    if (pDevice->IsLegacy()) {
        status = STATUS_INVALID_DEVICE_REQUEST;

        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGERROR,
            "WDFDEVICE %p is not a PNP device, device interface creation not "
            "allowed %!STATUS!", Device, status);

        return status;
    }

    FxObjectHandleGetPtr(pFxDriverGlobals,
                         String,
                         FX_TYPE_STRING,
                         (PVOID*) &pString);

    pPkgPnp = pDevice->m_PkgPnp;

    status = STATUS_OBJECT_NAME_NOT_FOUND;

    pPkgPnp->m_DeviceInterfaceLock.AcquireLock(pFxDriverGlobals);

    //
    // Iterate over the interfaces and see if we have a match
    //
    for (ple = pPkgPnp->m_DeviceInterfaceHead.Next;
         ple != NULL;
         ple = ple->Next) {
        FxDeviceInterface *pDI;

        pDI = FxDeviceInterface::_FromEntry(ple);

        if (FxIsEqualGuid(&pDI->m_InterfaceClassGUID, InterfaceClassGUID)) {
            if (RefString != NULL) {
                if ((RefString->Length == pDI->m_ReferenceString.Length)
                    &&
                    (RtlCompareMemory(RefString->Buffer,
                                      pDI->m_ReferenceString.Buffer,
                                      RefString->Length) == RefString->Length)) {
                    //
                    // They match, carry on
                    //
                    DO_NOTHING();
                }
                else {
                    //
                    // The ref strings do not match, continue on in the search
                    // of the collection.
                    //
                    continue;
                }
            }
            else if (pDI->m_ReferenceString.Length > 0) {
                //
                // Caller didn't specify a ref string but this interface has
                // one, continue on in the search through the collection.
                //
                continue;
            }

            status = pDI->GetSymbolicLinkName(pString);

            break;
        }
    }

    pPkgPnp->m_DeviceInterfaceLock.ReleaseLock(pFxDriverGlobals);

    return status;
}

} // extern "C"
