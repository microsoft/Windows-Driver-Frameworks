/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    PnpStateMachine.cpp

Abstract:

    This module implements the PnP state machine for the driver framework.
    This code was split out from FxPkgPnp.cpp.

Author:




Environment:

    User mode only

Revision History:

--*/

#include "..\pnppriv.hpp"

extern "C" {
#if defined(EVENT_TRACING)
#include "PnpStateMachineUM.tmh"
#endif
}









BOOLEAN
FxPkgPnp::PnpCheckAndIncrementRestartCount(
    VOID
    )
/*++

Routine Description:
    This routine determines if this device should ask the bus driver to
    reenumerate the device.   This is determined by how many times the entire
    stack has asked for a restart within a given period.  This is stack wide
    because the settings are stored in a key in the device node itself (which all
    devices share).

    The period and number of times a restart are attempted are defined as constants
    (m_RestartTimePeriodMaximum, m_RestartCountMaximum)in this class.   They are
    current defined as a period of 10 seconds and a restart max count of 5.

    The settings are stored in a volatile key so that they do not persist across
    machine reboots.  Persisting across reboots makes no sense if we restrict the
    number of restarts w/in a short period.

    The rules are as follows
    1)  if the key does not exist, treat this as the beginning of the period
        and ask for a reenumeration
    2)  if the key exists
        a)  if the beginning of the period and the restart count cannot be read
            do not ask for a reenumeration
        b)  if the beginning of the period is after the current time, either the
            current tick count has wrapped or the key has somehow survived a
            reboot.  Either way, treat this as a reset of the period and ask
            for a reenumeration
        c)  if the current time is after the period start time and within the
            restart period, increment the restart count.  if the count is <=
            the max restart count, ask for a reenumeration.  If it exceeds the
            max, do not ask for a reenumeration.
        d)  if the current time is after the period stat time and exceeds the
            maximum period, reset the period and count and ask for a reenumeration.

Considerations:
    A normal surprise removal will cause this routine to ask for a restart.  We
    do not exclude normal surprise removes from our logic because you can also
    receive a surprise remove by invalidating your device relations and marking
    the device as failed or removed.

    Furthermore, all this will do is increment the restart count.  If the device
    is plugged in and successfully started and surprise removed multiple times
    within the restart period, the reenumeration and restart of the device will
    not be affected by the restart count.  All that will be affected is if the
    device fails start within that period and we have exceeded the restart count,
    we will not ask for a restart.

Arguments:
    None

Return Value:
    TRUE if a restart should be requested.

  --*/

{
    return FALSE;
}

BOOLEAN
FxPkgPnp::ShouldProcessPnpEventOnDifferentThread(
    __in KIRQL CurrentIrql,
    __in BOOLEAN CallerSpecifiedProcessingOnDifferentThread
    )
/*++
Routine Description:

    This function returns whether the PnP state machine should process the 
    current event on the same thread or on a different one.
   
    This function has been added to work around a bug in the state machines. 
    The idle state machine always calls PnpProcessEvent with the idle state 
    machine lock held. Some events sent by the idle state machine can cause the
    Pnp state machine to invoke FxPowerIdleMachine::IoDecrement(). 
    FxPowerIdleMachine::IoDecrement() will try to acquire the idle state 
    machine lock, which is already being held, so it will result in a recursive
    acquire of the idle state machine lock.
    
    The above bug only affects UMDF, but not KMDF. In KMDF, the idle state 
    machine lock is a spinlock. When PnpProcessEvent is called, it is called 
    with the spinlock held and hence at dispatch level. Note that if called at
    a non-passive IRQL, PnpProcessEvent will always queue a work item to 
    process the event at passive IRQL later. Queuing a work item forces 
    processing to happen on a different thread and hence we don't attempt to
    recursively acquire the spinlock. On the other hand, with UMDF we are 
    always at passive IRQL and hence we process the event on the same thread 
    and run into the recursive acquire problem.
    







Arguments:

    CurrentIrql - The current IRQL
    
    CallerSpecifiedProcessingOnDifferentThread - Whether or not caller of 
        PnpProcessEvent specified that the event be processed on a different 
        thread.

Returns:
    TRUE if the PnP state machine should process the event on a different
       thread.
       
    FALSE if the PnP state machine should process the event on the same thread

--*/
{
    //
    // For UMDF, we ignore the IRQL and just do what the caller of 
    // PnpProcessEvent wants.
    //
    UNREFERENCED_PARAMETER(CurrentIrql);

    return CallerSpecifiedProcessingOnDifferentThread;
}

_Must_inspect_result_
NTSTATUS
FxPkgPnp::CreatePowerThreadIfNeeded(
    VOID
    )
/*++
Routine description:
    If needed, creates a thread for processing power IRPs
    
Arguments:
    None
    
Return value:
    An NTSTATUS code indicating success or failure of this function
--*/
{
    //
    // For UMDF, we never need a power thread, so just return success.
    //
    return STATUS_SUCCESS;
}

NTSTATUS
FxPkgPnp::PnpPrepareHardwareInternal(
    VOID
    )
{
    //
    // Update maximum interrupt thread count now that we know how many
    // total interrupts we have.
    //
    return GetDevice()->UpdateInterruptThreadpoolLimits();
}

