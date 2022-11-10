/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    DevicePowerIrpTracker.cpp

Abstract:

    This module implements FxDevicePowerIrpTracker for power policy owner.

Author:

Environment:

    Both kernel and user mode

Revision History:

--*/

#include "pnppriv.hpp"

extern "C" {

#if defined(EVENT_TRACING)
#include "DevicePowerIrpTracker.tmh"
#endif

}


VOID
FxDevicePowerIrpTracker::SaveStateFromSystemPowerIrp(
    _In_ FxIrp *Irp
    )
{
    BOOLEAN                     systemPowerUp;
    SYSTEM_POWER_STATE          sleepState;
    SYSTEM_POWER_STATE_CONTEXT  context;
    POWER_ACTION                powerAction;
    POWER_ACTION                shutdownType;

    ASSERT(Irp->GetMajorFunction() == IRP_MJ_POWER);
    ASSERT(Irp->GetMinorFunction() == IRP_MN_SET_POWER);
    ASSERT(Irp->GetParameterPowerType() == SystemPowerState);

    systemPowerUp   = (Irp->GetParameterPowerStateSystemState() == PowerSystemWorking);
    shutdownType    = Irp->GetParameterPowerShutdownType();
    context         = Irp->GetParameterPowerSystemPowerStateContext();

    sleepState = (SYSTEM_POWER_STATE)
        (systemPowerUp ? context.CurrentSystemState : context.EffectiveSystemState);

    /*
    | -------------- | ----- | ------------------ | -------- | -------- | -------- | --------
    | Scenarios      | State | ShutdownType       | Current  | Target   | Effective| Comment
    |                |       |                    | SysState | SysState | SysState |
    | -------------- | ----- | ------------------ | -------- | -------- | -------- | --------
    | Sleep          | S3    | Sleep              | S0       | S3       | S3       |
    |   Wake         | S0    | Sleep              | S3       | S0       | S0       |
    | Hybrid Sleep   | S4    | Hibernate          | S0       | S3       | S4       | Sleep with a Hibernation file
    |   Wake         | S0    | Sleep              | S3       | S0       | S0       | a.k.a. Fast S4
    |   Wake/PwrLost | S0    | Sleep              | S4       | S0       | S0       |
    | Hibernate      | S4    | Hibernate          | S0       | S4       | S4       |
    |   Wake         | S0    | Sleep              | S4       | S0       | S0       |
    | Hybrid Shutdown| S4    | Hibernate          | S0       | S5       | S4       | App closed, user logged off as if shutdown
    |   Fast Startup | S0    | Sleep              | S4       | S0       | S0       | a.k.a. Hiber Boot
    | Shutdown       | S5    | Shutdown/Reset/Off | S0       | S5       | S5       |
    |   System Boot  |       |                    |          |          |          | No S-IRP for boot up
    | -------------- | ----- | ------------------ | -------- | -------- | -------- | --------
    */

    switch (sleepState) {

    case PowerSystemSleeping1:
    case PowerSystemSleeping2:
    case PowerSystemSleeping3:
        powerAction = PowerActionSleep;
        break;

    case PowerSystemHibernate:
        powerAction = PowerActionHibernate;
        break;

    case PowerSystemShutdown:
        ASSERT(systemPowerUp == FALSE);
        powerAction = shutdownType;
        break;

    case PowerSystemWorking:
        //
        // Someone failed IRP_MN_QUERY_POWER and we follow that up with an S0 IRP
        //
        ASSERT(systemPowerUp == TRUE);
        powerAction = PowerActionNone;
        break;

    default:
        ASSERT(FALSE);
        powerAction = PowerActionNone;
        break;
    }

    if (systemPowerUp) {
        m_S0PowerAction = powerAction;
    }
    else {
        m_SxPowerAction = powerAction;
    }
}

VOID
FxDevicePowerIrpTracker::StartTrackingDevicePowerIrp(
    _In_ RequestDIrpReason Reason
    )
/*++

Routine Description:
    The routine is called right before D-IRP is requested.

    Right now it only handles RequestD0ForS0 and RequestDxForSx by saving reason
    to an internal buffer. The expectation is that when the D-IRP has finished
    processing (not IRP completion, but PwrPolPowerUp / PwrPolPowerDown implying
    all callbacks like EvtDeviceD0Entry/D0Exit, EvtDeviceSelfManagedIoSuspend /
    Restart have been invoked), a follow up call to StopTrackingDevicePowerIrp
    will be made to reset the buffer to ReasonInvalid. GetSystemPowerAction
    reads the buffer to know whether RequestD0ForS0 / RequestDxForSx is happening.

Arguments:
    Reason - The reason why D-IRP is requested, e.g. idle out, system wakes up

Return Value:
    None

  --*/
{
    FxPowerPolicyOwnerSettings* owner;
    FxDevice* device;

    owner = (FxPowerPolicyOwnerSettings*)
                CONTAINING_RECORD(
                    this,
                    FxPowerPolicyOwnerSettings,
                    m_DevicePowerIrpTracker);

    device = owner->m_PkgPnp->GetDevice();















    switch (Reason) {

        case RequestD0ForS0:
        case RequestDxForSx:
            m_DIrpRequestedForSIrp = Reason;
            break;

        default:
            ASSERT(FALSE);
            break;
    }
}

VOID
FxDevicePowerIrpTracker::StopTrackingDevicePowerIrp(
    VOID
    )
/*++

Routine Description:
    The routine is called when finishing processing D-IRP. This can be either of:

     - Failure from PoRequestPowerIrp, implying no D-IRP was sent to the device
     - For RequestD0ForS0, PwrPolPowerUp is received
     - For RequestDxForSx, PwrPolPowerDown is received
     - Other failure happens when handling RequestD0ForS0 or RequestDxForSx

    The routine might be called twice, one for failure from PoRequestPowerIrp,
    the other when the state machine moves the a common failure handling state.

Arguments:


Return Value:
    None

  --*/
{
    m_DIrpRequestedForSIrp = RequestDIrpReasonInvalid;
}

POWER_ACTION
FxDevicePowerIrpTracker::GetSystemPowerAction(
    VOID
    )
{
    switch (m_DIrpRequestedForSIrp) {

        case RequestD0ForS0:
            return m_S0PowerAction;

        case RequestDxForSx:
            return m_SxPowerAction;

        default:
            return PowerActionNone;
    }
}

VOID
FxDevicePowerIrpTracker::LogRequestDIrpReason(
    _In_ RequestDIrpReason Reason,
    _In_ BOOLEAN           PowerUp
    )
/*++

Routine Description:
    Record the reason of requesting D-IRP into an FIFO history which can be
    reviewed using `!wdfkd.wdfdevice _device_ ff` debugger command later.

Arguments:
    Reason - The reason why D-IRP is requested, e.g. idle out, system wakes up
    PowerUp - TRUE the device is powering up, FALSE power down

Return Value:
    None

  --*/
{
    if (Reason == RequestD0ForOther) {
        ASSERT(PowerUp);
        Reason = (RequestDIrpReason) InterlockedOr(&m_D0IrpReasonHint.AsLong, 0);

































    }

    //
    // Clear the saved D0 reason hint but only when we're requesting D0 IRP.
    //
    // For example, right after idle times out but before requesting Dx IRP, driver
    // calls WdfDeviceStopIdle which SaveRequestD0IrpReasonHint(RequestD0ForStopIdle).
    // The device will still enter Dx and then exit immediately. If D0 reason hint
    // is cleared when requesting Dx, later when requesting D0, WDF will assert that
    // D0 reason hint is invalid. Thus, D0 reason hint should not be cleared on Dx.
    //
    if (PowerUp) {
        InterlockedExchange(&m_D0IrpReasonHint.AsLong, RequestD0ForOther);
    }

    AddToHistory(Reason);
}
