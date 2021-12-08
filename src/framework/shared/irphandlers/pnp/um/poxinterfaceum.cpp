//
//    Copyright (C) Microsoft.  All rights reserved.
//
#include "pnppriv.hpp"

#include <initguid.h>
#include <wdmguid.h>

extern "C" {
#if defined(EVENT_TRACING)
#include "PoxInterfaceUm.tmh"
#endif
}

VOID
FxPoxInterface::PoxStartDevicePowerManagement(
    )
{
    m_PkgPnp->GetDevice()->GetDeviceStack2()->PoFxStartDevicePowerManagement();

    return;
}

NTSTATUS
FxPoxInterface::PoxRegisterDevice(
    )
{
    HRESULT hr;
    BOOLEAN directedTransitionsEnabled;
    ULONGLONG deviceFlags;

    deviceFlags = m_PkgPnp->m_PowerPolicyMachine.m_Owner->
        m_IdleSettings.m_TimeoutMgmt.GetPoFxDeviceFlags();

    directedTransitionsEnabled = m_PkgPnp->m_PowerPolicyMachine.m_Owner->
        m_IdleSettings.m_TimeoutMgmt.GetDirectedPowerTransitionSupport();

    hr = m_PkgPnp->GetDevice()->GetDeviceStack2()->PoFxRegisterDevice(deviceFlags, directedTransitionsEnabled);

    if (S_OK == hr)
    {
        return STATUS_SUCCESS;
    }
    else
    {
        PUMDF_VERSION_DATA driverVersion = m_PkgPnp->GetDevice()->GetDeviceStack()->GetMinDriverVersion();

        return CHostFxUtil::NtStatusFromHr(
                hr,
                driverVersion->MajorNumber,
                driverVersion->MinorNumber,
                m_PkgPnp->GetDevice()->GetDeviceStack()->ShouldPreserveIrpCompletionStatusCompatibility()
                );
    }

}

VOID
FxPoxInterface::PoxUnregisterDevice(
    VOID
    )
{
    m_PkgPnp->GetDevice()->GetDeviceStack2()->PoFxUnregisterDevice();
    return;
}

VOID
FxPoxInterface::PoxActivateComponent(
    VOID
    )
{
    m_PkgPnp->GetDevice()->GetDeviceStack2()->PoFxActivateComponent();
    return;
}

VOID
FxPoxInterface::PoxIdleComponent(
    VOID
    )
{
    m_PkgPnp->GetDevice()->GetDeviceStack2()->PoFxIdleComponent();
    return;
}

VOID
FxPoxInterface::PoxReportDevicePoweredOn(
    VOID
    )
{
    m_PkgPnp->GetDevice()->GetDeviceStack2()->PoFxReportDevicePoweredOn();
    return;
}

VOID
FxPoxInterface::PoxSetDeviceIdleTimeout(
    __in ULONGLONG IdleTimeout
    )
{
    m_PkgPnp->GetDevice()->GetDeviceStack2()->PoFxSetDeviceIdleTimeout(IdleTimeout);
    return;
}


VOID
FxPoxInterface::PowerRequiredCallbackInvoked(
    VOID
    )
{
    PowerRequiredCallbackWorker(FALSE /* InvokedFromPoxCallback */);
    return;
}

VOID
FxPoxInterface::PowerNotRequiredCallbackInvoked(
    VOID
    )
{
    //
    // Notice that it is important that we first acknowledge the callback
    // because once we queue an event in the state machine, the state machine
    // could end up calling PoFxUnregister from the same thread
    //
    m_PkgPnp->GetDevice()->GetDeviceStack2()->PoFxCompleteDevicePowerNotRequired();

    //
    // The contract with the reflector is that the reflector guarantees to
    // not send this event from the PoFx callback
    //
    PowerNotRequiredCallbackWorker(FALSE /* InvokedFromPoxCallback */);

    return;
}

VOID
FxPoxInterface::SimulateDevicePowerRequiredInReflector(
    VOID
    )
{
    if (FALSE == m_PkgPnp->m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                                m_TimeoutMgmt.UsingSystemManagedIdleTimeout()) {
        //
        // Driver-managed idle timeout. Nothing to do.
        //
        return;
    }

    m_PkgPnp->GetDevice()->GetDeviceStack2()->PoFxSimulateDevicePowerRequiredInReflector();
}

VOID
FxPoxInterface::DirectedPowerDownCallbackInvoked(
    VOID
    )
{
    DoTraceLevelMessage(
        m_PkgPnp->GetDriverGlobals(),
        TRACE_LEVEL_INFORMATION,
        TRACINGPNP,
        "WDFDEVICE 0x%p PO_FX_DIRECTED_POWER_DOWN_CALLBACK invoked.",
        m_PkgPnp->GetDevice()->GetHandle());

    DirectedPowerDownCallbackWorker(FALSE /* InvokedFromPoxCallback */);
    return;
}

VOID
FxPoxInterface::DirectedPowerUpCallbackInvoked(
    VOID
    )
{
    DoTraceLevelMessage(
        m_PkgPnp->GetDriverGlobals(),
        TRACE_LEVEL_INFORMATION,
        TRACINGPNP,
        "WDFDEVICE 0x%p PO_FX_DIRECTED_POWER_UP_CALLBACK invoked.",
        m_PkgPnp->GetDevice()->GetHandle());

    //
    // The contract with the reflector is that the reflector guarantees to
    // not send this event from the PoFx callback
    //
    DirectedPowerUpCallbackWorker(FALSE /* InvokedFromPoxCallback */);
    return;
}

VOID
FxPoxInterface::PoxCompleteDirectedPowerDownTransition(
    VOID
    )
{
    m_PkgPnp->GetDevice()->GetDeviceStack2()->PoFxCompleteDirectedPowerDown();
    return;
}

VOID
FxPoxInterface::PoxCompleteDirectedPowerUpTransition(
    VOID
    )
{
    //
    // For directed power up transitions, the completion is implicit as a
    // result of the device reporting itself as powered on (i.e. calling
    // PoFxReportDevicePoweredOn). Thus no further action is required here.
    //
    return;
}

