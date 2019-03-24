/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDeviceInitApiUm.cpp

Abstract:

    This module exposes the "C" interface to the FxDevice object.

Author:



Environment:

    User mode only

Revision History:

--*/

#include "coreprivshared.hpp"

extern "C" {
#include "FxDeviceInitApiUm.tmh"
}

//
// Extern "C" the entire file
//
extern "C" {

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
WDFEXPORT(WdfDeviceInitEnableHidInterface)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit
    )
{
    DDI_ENTRY();

    FxPointerNotNull(GetFxDriverGlobals(DriverGlobals), DeviceInit);

    DeviceInit->DevStack->SetHidInterfaceSupport();
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
WDFEXPORT(WdfDeviceInitSetCompanionEventCallbacks)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_COMPANION_EVENT_CALLBACKS CompanionEventCallbacks
    )
{
    DDI_ENTRY();

    PFX_DRIVER_GLOBALS pFxDriverGlobals;

    FxPointerNotNull(GetFxDriverGlobals(DriverGlobals), DeviceInit);
    pFxDriverGlobals = DeviceInit->DriverGlobals;

    FxPointerNotNull(pFxDriverGlobals, CompanionEventCallbacks);

    if (CompanionEventCallbacks->Size != sizeof(WDF_COMPANION_EVENT_CALLBACKS)) {

        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "CompanionEventCallbacks size %d is invalid, exptected %d",
            CompanionEventCallbacks->Size, sizeof(WDF_COMPANION_EVENT_CALLBACKS)
            );

        FxVerifierDbgBreakPoint(pFxDriverGlobals);

        return;
    }

    //
    // Following the PnpPowerEventCallbacks pattern:
    //
    // Driver's CompanionEventCallbacks structure may be from a previous 
    // version and therefore may be different in size than the current version 
    // that framework is using. Therefore, copy only CompanionEventCallbacks->Size
    // bytes and not sizeof(CompanionEventCallbacks) bytes.
    //
    RtlCopyMemory(&DeviceInit->CompanionInit.CompanionEventCallbacks,
                  CompanionEventCallbacks,
                  CompanionEventCallbacks->Size);
}

} // extern "C"

