/*++
Copyright (c) Microsoft. All rights reserved.

Module Name:

    PoxInterface.cpp

Abstract:

    This module implements the power-framework-related logic in WDF.

--*/

#include "pnppriv.hpp"

extern "C" {
#if defined(EVENT_TRACING)
#include "PoxInterface.tmh"
#endif
}

FxPoxInterface::FxPoxInterface(
    __in FxPkgPnp* PkgPnp
    )
{
    m_PkgPnp = PkgPnp;
    m_PoHandle = NULL;
    m_DevicePowerRequired = TRUE;
    m_DevicePowerRequirementMachine = NULL;
    m_CurrentIdleTimeoutHint = 0;
    m_NextIdleTimeoutHint = 0;
    m_DirectedTransitionActive = FALSE;
}

FxPoxInterface::~FxPoxInterface(
    VOID
    )
{
    if (NULL != m_DevicePowerRequirementMachine) {
        delete m_DevicePowerRequirementMachine;
    }
}

NTSTATUS
FxPoxInterface::CreateDevicePowerRequirementMachine(
    VOID
    )
{
    NTSTATUS status;
    FxDevicePwrRequirementMachine * fxDprMachine = NULL;

    ASSERT(NULL == m_DevicePowerRequirementMachine);

    fxDprMachine = new (m_PkgPnp->GetDriverGlobals())
                            FxDevicePwrRequirementMachine(this);
    if (NULL == fxDprMachine) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(),
            TRACE_LEVEL_ERROR, TRACINGPNP,
            "WDFDEVICE 0x%p !devobj 0x%p failed to allocate "
            "FxDevicePwrRequirementMachine. %!STATUS!.",
            m_PkgPnp->GetDevice()->GetHandle(),
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            status);
        goto exit;
    }

    status = fxDprMachine->Initialize(m_PkgPnp->GetDriverGlobals());
    if (FALSE == NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(),
            TRACE_LEVEL_ERROR, TRACINGPNP,
            "WDFDEVICE 0x%p !devobj 0x%p Device Power Requirement State Machine"
            " Initialize() failed, %!STATUS!",
            m_PkgPnp->GetDevice()->GetHandle(),
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            status);
        goto exit;
    }

    status = fxDprMachine->Init(
                            m_PkgPnp,
                            FxDevicePwrRequirementMachine::_ProcessEventInner
                            );
    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(),
            TRACE_LEVEL_ERROR, TRACINGPNP,
            "WDFDEVICE 0x%p !devobj 0x%p Device Power Requirement State Machine"
            " Init() failed, %!STATUS!",
            m_PkgPnp->GetDevice()->GetHandle(),
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            status);
        goto exit;
    }

    m_DevicePowerRequirementMachine = fxDprMachine;

    status = STATUS_SUCCESS;

exit:
    if (FALSE == NT_SUCCESS(status)) {
        if (NULL != fxDprMachine) {
            delete fxDprMachine;
        }
    }
    return status;
}

NTSTATUS
FxPoxInterface::InitializeComponents(
    VOID
    )
{
    NTSTATUS status;
    PPOX_SETTINGS poxSettings = NULL;
    WDFDEVICE fxDevice = NULL;


    if (FALSE == m_PkgPnp->m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                                m_TimeoutMgmt.UsingSystemManagedIdleTimeout()) {
        //
        // Driver-managed idle timeout. Nothing to do.
        //
        return STATUS_SUCCESS;
    }

    //
    // We create the device power requirement state machine only if system-
    // managed idle timeout is being used.
    //
    if (NULL == m_DevicePowerRequirementMachine) {
        status = CreateDevicePowerRequirementMachine();
        if (FALSE == NT_SUCCESS(status)) {
            goto exit;
        }
    }

    ASSERT(NULL != m_DevicePowerRequirementMachine);

    //
    // Register with the power framework
    //
    status = PoxRegisterDevice();

    if (FALSE == NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            m_PkgPnp->GetDriverGlobals(),
            TRACE_LEVEL_ERROR, TRACINGPNP,
            "WDFDEVICE 0x%p !devobj 0x%p FxPox::PoxRegisterDevice failed. "
            "%!STATUS!.",
            m_PkgPnp->GetDevice()->GetHandle(),
            m_PkgPnp->GetDevice()->GetDeviceObject(),
            status);
        goto exit;
    }

    //
    // At the time of registration, all components are active. When we start the
    // power framework's device power management (see below), all components are
    // moved to the idle state by default. Take an extra reference on the
    // component to prevent this from happening. The power policy state machine
    // will evaluate the S0-idle policy later and ask us to drop this reference
    // if the policy requires it.
    //
    PoxActivateComponent();

    //
    // Tell the power framework to start its device power management. This will
    // drop a reference on the component, but the component will still remain
    // active because of the extra reference we took above.
    //
    PoxStartDevicePowerManagement();

    //
    // If the client driver has specified power framework settings, retrieve
    // them.
    //
    poxSettings = GetPowerFrameworkSettings();

    //
    // If the driver wanted to receive the POHANDLE, invoke their callback now
    //
    if ((NULL != poxSettings) &&
        (NULL != poxSettings->EvtDeviceWdmPostPoFxRegisterDevice)) {

        fxDevice = m_PkgPnp->GetDevice()->GetHandle();

        status = poxSettings->EvtDeviceWdmPostPoFxRegisterDevice(
                                  fxDevice,
                                  m_PoHandle
                                  );
        if (FALSE == NT_SUCCESS(status)) {

            DoTraceLevelMessage(
                m_PkgPnp->GetDriverGlobals(),
                TRACE_LEVEL_ERROR, TRACINGPNP,
                "WDFDEVICE 0x%p !devobj 0x%p. The client driver has failed the "
                "EvtDeviceWdmPostPoFxRegisterDevice callback with %!STATUS!.",
                m_PkgPnp->GetDevice()->GetHandle(),
                m_PkgPnp->GetDevice()->GetDeviceObject(),
                status);

            //
            // Notify the driver that the POHANDLE is about to become invalid
            //
            if (NULL != poxSettings->EvtDeviceWdmPrePoFxUnregisterDevice) {
                poxSettings->EvtDeviceWdmPrePoFxUnregisterDevice(
                                fxDevice,
                                m_PoHandle
                                );
            }

            //
            // Unregister with the power framework
            //
            PoxUnregisterDevice();
            goto exit;
        }
    }

    //
    // Tell the device power requirement state machine that we have registered
    // with the power framework
    //
    m_DevicePowerRequirementMachine->ProcessEvent(DprEventRegisteredWithPox);

exit:
    return status;
}

VOID
FxPoxInterface::UninitializeComponents(
    VOID
    )
{
    PPOX_SETTINGS poxSettings = NULL;

    if (FALSE == m_PkgPnp->m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                                m_TimeoutMgmt.UsingSystemManagedIdleTimeout()) {
        //
        // Driver-managed idle timeout. Nothing to do.
        //
        return;
    }

    ASSERT(NULL != m_DevicePowerRequirementMachine);

    //
    // If the client driver has specified power framework settings, retrieve
    // them.
    //
    poxSettings = GetPowerFrameworkSettings();

    //
    // Notify the client driver that the POHANDLE is about to become invalid
    //
    if ((NULL != poxSettings) &&
        (NULL != poxSettings->EvtDeviceWdmPrePoFxUnregisterDevice)) {

        poxSettings->EvtDeviceWdmPrePoFxUnregisterDevice(
                       m_PkgPnp->GetDevice()->GetHandle(),
                       m_PoHandle
                       );
    }

    //
    // Unregister with the power framework
    //
    PoxUnregisterDevice();

    //
    // Tell the device power requirement state machine that we have unregistered
    // with the power framework
    //
    m_DevicePowerRequirementMachine->ProcessEvent(
                                        DprEventUnregisteredWithPox
                                        );
    return;
}

VOID
FxPoxInterface::RequestComponentActive(
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

    PoxActivateComponent();
    return;
}

BOOLEAN
FxPoxInterface::DeclareComponentIdle(
    VOID
    )
{
    BOOLEAN canPowerDown;

    if (FALSE == m_PkgPnp->m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                                m_TimeoutMgmt.UsingSystemManagedIdleTimeout()) {
        //
        // Driver-managed idle timeout. We can power down immediately, without
        // waiting for device-power-not-required notification.
        //
        canPowerDown = TRUE;
    } else {
        //
        // System-managed idle timeout
        //
        PoxIdleComponent();

        //
        // We must wait for device-power-not-required notification before
        // powering down.
        //
        canPowerDown = FALSE;
    }

    return canPowerDown;
}

VOID
FxPoxInterface::UpdateIdleTimeoutHint(
    VOID
    )
{
    ULONGLONG idleTimeoutHint;

    if (FALSE == m_PkgPnp->m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                                m_TimeoutMgmt.UsingSystemManagedIdleTimeoutAndPofxTimer()) {
        //
        // Driver-managed idle timeout, or System-managed idle timeout with WDF timer.
        // Nothing to do
        //
        return;
    }

    if (m_NextIdleTimeoutHint != m_CurrentIdleTimeoutHint) {
        m_CurrentIdleTimeoutHint = m_NextIdleTimeoutHint;

        //
        // Convert the idle timeout from milliseconds to 100-nanosecond units
        //
        idleTimeoutHint = ((ULONGLONG) m_CurrentIdleTimeoutHint) * 10 * 1000;
        PoxSetDeviceIdleTimeout(idleTimeoutHint);
    }

    return;
}


NTSTATUS
FxPoxInterface::NotifyDevicePowerDown(
    VOID
    )
{
    KIRQL irql;
    BOOLEAN canPowerOff;

    if (FALSE == m_PkgPnp->m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                                m_TimeoutMgmt.UsingSystemManagedIdleTimeout()) {
        //
        // Driver-managed idle timeout. We don't have to take power framework's
        // device power requirement into consideration. Just return success.
        //
        return STATUS_SUCCESS;
    }

    //
    // Acquire the lock to ensure that device power requirement doesn't change.
    //
    m_DevicePowerRequiredLock.Acquire(&irql);
    if (FALSE == m_DevicePowerRequired) {
        //
        // Send an event to the device power requirement state machine to tell
        // it that we are about to go to Dx.
        //
        // This path is also entered when a directed power down transition is
        // in progress. Note from PoFx's perspective, the device will be in
        // device-power-required state as PoFx acquires an active reference on
        // the device prior to issuing a a directed transition. However, we
        // simulate a device-power-not-required prior to reaching here and
        // thus our internal state should map to device-power-not-required.
        //
        // We send the event inside a lock in order to handle the race condition
        // when the power framework notifies us that device power is required at
        // the same time that we are about to go to Dx. By sending the event
        // inside the lock, we ensure that the DprEventDeviceGoingToDx event is
        // always queued to device power requirement state machine before the
        // DprEventPoxRequiresPower.
        //
        // This allows for a clean design in the device power requirement state
        // machine by ensuring that it does not have to handle a non-intuitive
        // sequence, i.e. DprEventPoxRequiresPower followed by
        // DprEventDeviceGoingToDx. This sequence is non-intuitive because it
        // doesn't make sense for a device to go to Dx after it has been
        // informed that device power is required. Avoiding this non-intuitive
        // sequence via locking enables a clean design for the device power
        // requirement state machine.
        //
        m_DevicePowerRequirementMachine->ProcessEvent(DprEventDeviceGoingToDx);
        canPowerOff = TRUE;

    } else {

        //
        // When a directed power down transition is requested, a
        // device-power-not-required is simulated prior to reaching this
        // state. Thus the internal state is always expected to map to
        // device-power-not-required for those transitions.
        //
        ASSERT(FALSE == IsDirectedTransitionInProgress());

        canPowerOff = FALSE;
    }
    m_DevicePowerRequiredLock.Release(irql);

    return canPowerOff ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}

VOID
FxPoxInterface::DeviceIsPoweredOn(
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

    //
    // System-managed idle timeout. Notify the device power requirement state
    // machine that we are back in D0.
    //
    m_DevicePowerRequirementMachine->ProcessEvent(
                                        DprEventDeviceReturnedToD0
                                        );
    return;
}

PPOX_SETTINGS
FxPoxInterface::GetPowerFrameworkSettings(
    VOID
    )
{
    PPOX_SETTINGS poxSettings = NULL;

    if (m_PkgPnp->m_PowerPolicyMachine.m_Owner->
         m_IdleSettings.m_TimeoutMgmt.DriverSpecifiedPowerFrameworkSettings()) {

        poxSettings = m_PkgPnp->m_PowerPolicyMachine.m_Owner->
                       m_IdleSettings.m_TimeoutMgmt.GetPowerFrameworkSettings();

        ASSERT(NULL != poxSettings);
    }

    return poxSettings;
}

VOID
FxPoxInterface::DprProcessEventFromPoxCallback(
    __in FxDevicePwrRequirementEvents Event
    )
{
    KIRQL irql;

    //
    // We should not run the state machine from within a power framework
    // callback because we might end up reaching a state where we unregister
    // with the power framework. Unregistering from a callback leads to a
    // deadlock. Therefore, we raise IRQL before queueing an event to the state
    // machine. Raising IRQL causes the event processing to be deferred to a
    // worker thread.
    //

    //
    // This path should only be invoked for kernel mode. For user mode, this
    // condition is avoided by reflector guranteeing that it queues a worker
    // item to send a Pofx event corresponding to any PoFx callback
    //
    ASSERT(FX_IS_KERNEL_MODE);

    Mx::MxRaiseIrql(DISPATCH_LEVEL, &irql);
    m_DevicePowerRequirementMachine->ProcessEvent(Event);
    Mx::MxLowerIrql(irql);
}

VOID
FxPoxInterface::SimulateDevicePowerRequired(
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

    //
    // System-managed idle timeout. Notify the device power requirement state
    // machine that device power is required.
    //
    PowerRequiredCallbackWorker(FALSE /* InvokedFromPoxCallback */);
    return;
}

VOID
FxPoxInterface::SimulateDevicePowerNotRequired(
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

    //
    // System-managed idle timeout. Notify the device power requirement state
    // machine that device power is not required.
    //
    PowerNotRequiredCallbackWorker(FALSE /* InvokedFromPoxCallback */);
    return;
}

VOID
FxPoxInterface::PowerRequiredCallbackWorker(
    __in BOOLEAN InvokedFromPoxCallback
    )
{
    KIRQL irql;

    //
    // Make a note of the fact that device power is required
    //
    m_DevicePowerRequiredLock.Acquire(&irql);
    m_DevicePowerRequired = TRUE;
    m_DevicePowerRequiredLock.Release(irql);

    //
    // Send the device-power-required event to the device power requirement
    // state machine.
    //
    if (InvokedFromPoxCallback) {
        DprProcessEventFromPoxCallback(DprEventPoxRequiresPower);
    } else {
        m_DevicePowerRequirementMachine->ProcessEvent(DprEventPoxRequiresPower);
    }
    return;
}

VOID
FxPoxInterface::PowerNotRequiredCallbackWorker(
    __in BOOLEAN InvokedFromPoxCallback
    )
{
    KIRQL irql;

    //
    // Make a note of the fact that device power is not required
    //
    m_DevicePowerRequiredLock.Acquire(&irql);
    m_DevicePowerRequired = FALSE;
    m_DevicePowerRequiredLock.Release(irql);

    //
    // Send the device-power-not-required event to the device power
    // requirement state machine.
    //
    if (InvokedFromPoxCallback) {
        DprProcessEventFromPoxCallback(DprEventPoxDoesNotRequirePower);
    } else {
        m_DevicePowerRequirementMachine->ProcessEvent(
                                            DprEventPoxDoesNotRequirePower
                                            );
    }
    return;
}

VOID
FxPoxInterface::DirectedPowerUpCallbackWorker(
    _In_ BOOLEAN InvokedFromPoxCallback
    )
/*++

Routine Description:
    This routine initiates the directed power up sequence for the device.

Arguments:
    InvokedFromPoxCallback - Supplies a flag indicating if the invocation was
        due to a PoFx callback (TRUE) or generated internally (FALSE).

Return Value:
    None.

--*/
{
    //
    // Mark the directed transition as now in progress.
    //
    SetDirectedTransitionInProgress();

    //
    // Send the device-power-not-required event to the device power
    // requirement state machine.
    //
    if (InvokedFromPoxCallback) {
        DprProcessEventFromPoxCallback(DprEventPoxDirectedPowerUp);
    } else {
        m_DevicePowerRequirementMachine->ProcessEvent(
                                            DprEventPoxDirectedPowerUp
                                            );
    }
    return;
}

VOID
FxPoxInterface::DirectedPowerDownCallbackWorker(
    _In_ BOOLEAN InvokedFromPoxCallback
    )
/*++

Routine Description:
    This routine initiates the directed power up sequence for the device.

Arguments:
    InvokedFromPoxCallback - Supplies a flag indicating if the invocation was
        due to a PoFx callback (TRUE) or generated internally (FALSE).

Return Value:
    None.

--*/
{
    //
    // Mark the directed transition as now in progress.
    //
    SetDirectedTransitionInProgress();

    //
    // Send the device-power-not-required event to the device power
    // requirement state machine.
    //
    if (InvokedFromPoxCallback) {
        DprProcessEventFromPoxCallback(DprEventPoxDirectedPowerDown);
    } else {
        m_DevicePowerRequirementMachine->ProcessEvent(
                                            DprEventPoxDirectedPowerDown
                                            );
    }
    return;
}

VOID
FxPoxInterface::NotifyDeviceDirectedPoweredDown(
    VOID
    )
/*++

Routine Description:
    This routine notifies the device power requirement state machine that the
    device should be considered as directed power down. This only applies when
    a directed transition is in progress.

Arguments:
    None.

Return Value:
    None.

--*/
{
    BOOLEAN directedTransition;

    if (FALSE == m_PkgPnp->m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                                m_TimeoutMgmt.UsingSystemManagedIdleTimeout()) {
        //
        // Currently directed transitions are only supported for drivers using
        // system-managed idle timeout. Simply bail out for driver-managed
        // idle timeout cases.
        //
        return;
    }

    directedTransition = IsDirectedTransitionInProgress();
    if (FALSE != directedTransition) {
        //
        // Send the directed-power-down event to the device power requirement
        // state machine. A directed power down can only happen if a directed
        // transition is currently in progress.
        //
        m_DevicePowerRequirementMachine->ProcessEvent(
                                            DprEventDeviceDirectedPoweredDown
                                            );
    }

    return;
}

VOID
FxPoxInterface::NotifyPoxDirectedPowerDownCompletion(
    VOID
    )
/*++

Routine Description:
    This routine notifies completion of a directed power down transition to
    PoFx.

Arguments:
    None.

Return Value:
    None.

--*/
{
    //
    // Mark the directed transition as now complete.
    //
    ClearDirectedTransitionInProgress();

    //
    // Notify completion of the directed power transition to PoFx.
    //
    PoxCompleteDirectedPowerDownTransition();
    return;
}

VOID
FxPoxInterface::NotifyDeviceDirectedPoweredUp(
    VOID
    )
/*++

Routine Description:
    This routine notifies the device power requirement state machine that the
    device should be considered as directed power up. This only applies when a
    directed transition is in progress.

Arguments:
    None.

Return Value:
    STATUS_SUCCESS always.

--*/
{
    BOOLEAN directedTransition;

    if (FALSE == m_PkgPnp->m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                                m_TimeoutMgmt.UsingSystemManagedIdleTimeout()) {
        //
        // Currently directed transitions are only supported for drivers using
        // system-managed idle timeout. Simply bail out for driver-managed
        // idle timeout cases.
        //
        return;
    }

    directedTransition = IsDirectedTransitionInProgress();
    if (FALSE != directedTransition) {
        //
        // Send the directed-power-down event to the device power requirement
        // state machine. This event should only be posted if a directed
        // transition is currently in progress.
        //
        m_DevicePowerRequirementMachine->ProcessEvent(
                                            DprEventDeviceDirectedPoweredUp
                                            );
    }

    return;
}

VOID
FxPoxInterface::NotifyPoxDirectedPowerUpCompletion(
    VOID
    )
/*++

Routine Description:
    This routine performs actions required after a device has been powered up
    in a directed manner.

    Note for directed power up transitions, the completion is implicit as a
    result of the device reporting itself as powered on (i.e. calling
    PoFxReportDevicePoweredOn).

Arguments:
    None.

Return Value:
    None.

--*/
{
    //
    // Mark the directed transition as now complete.
    //
    ClearDirectedTransitionInProgress();

    //
    // Perform any post directed power up activities that are required by
    // PoFx.
    //
    PoxCompleteDirectedPowerUpTransition();
    return;
}

BOOLEAN
FxPoxInterface::IsDirectedTransitionInProgress(
    VOID
    )
/*++

Routine Description:
    This routine checks whether a directed power transition is currently in
    progress or not.

Arguments:
    None.

Return Value:
    TRUE if directed transition is in progress; FALSE otherwise.

--*/
{

    LONG value;

    value = InterlockedCompareExchange(&m_DirectedTransitionActive, 0, 0);
    if (0 == value) {
        return FALSE;
    } else {
        return TRUE;
    }
}

VOID
FxPoxInterface::SetDirectedTransitionInProgress(
    VOID
    )
/*++

Routine Description:
    This routine marks the directed power transition is currently being in
    progress.

Arguments:
    None.

Return Value:
    None.

--*/
{

    //
    // Currently directed transitions are only supported for drivers using
    // system-managed idle timeout. A directed transition should never
    // have been initiated otherwise.
    //
    ASSERT(FALSE != m_PkgPnp->m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                            m_TimeoutMgmt.UsingSystemManagedIdleTimeout());

    InterlockedExchange(&m_DirectedTransitionActive, 1);
    return;
}

VOID
FxPoxInterface::ClearDirectedTransitionInProgress(
    VOID
    )
/*++

Routine Description:
    This routine resets the directed transition state.

Arguments:
    None.

Return Value:
    None.

--*/
{

    //
    // Although directed transitions are only really supported for drivers using
    // system-managed idle timeout, any driver is allowed to reset this field.
    //
    InterlockedExchange(&m_DirectedTransitionActive, 0);
    return;
}

