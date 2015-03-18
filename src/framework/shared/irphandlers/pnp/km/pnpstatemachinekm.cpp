/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    PnpStateMachine.cpp

Abstract:

    This module implements the PnP state machine for the driver framework.
    This code was split out from FxPkgPnp.cpp.

Author:




Environment:

    Kernel mode only

Revision History:

--*/

#include "..\pnppriv.hpp"
#include <wdmguid.h>

#include<ntstrsafe.h>

extern "C" {
#if defined(EVENT_TRACING)
#include "PnpStateMachineKM.tmh"
#endif
}

#define RESTART_START_TIME_NAME   L"StartTime"
#define RESTART_COUNT_NAME      L"Count"

const PWCHAR FxPkgPnp::m_RestartStartTimeName = RESTART_START_TIME_NAME;
const PWCHAR FxPkgPnp::m_RestartCountName = RESTART_COUNT_NAME;

const ULONG FxPkgPnp::m_RestartTimePeriodMaximum = 10;
const ULONG FxPkgPnp::m_RestartCountMaximum = 5;

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

    NTSTATUS status;
    FxAutoRegKey settings, restart;
    ULONG disposition, count;
    LARGE_INTEGER currentTickCount, startTickCount;
    BOOLEAN writeTick, writeCount;

    DECLARE_CONST_UNICODE_STRING(valueNameStartTime, RESTART_START_TIME_NAME);
    DECLARE_CONST_UNICODE_STRING(valueNameCount, RESTART_COUNT_NAME);
    DECLARE_CONST_UNICODE_STRING(keyNameRestart, L"Restart");


    status = m_Device->OpenSettingsKey(&settings.m_Key);

    if (!NT_SUCCESS(status)) {
        return FALSE;
    }

    count = 0;
    writeTick = FALSE;
    writeCount = FALSE;
    disposition = 0;

    //
    // We ask for a volatile key so that upon reboot, the count is purged and we
    // start a new fresh restart count.
    //
    status = FxRegKey::_Create(settings.m_Key,
                               &keyNameRestart,
                               &restart.m_Key,
                               KEY_ALL_ACCESS,
                               REG_OPTION_VOLATILE,
                               &disposition);

    if (!NT_SUCCESS(status)) {
        return FALSE;
    }

    Mx::MxQueryTickCount(&currentTickCount);

    //
    // If the key was created right now, there is nothing to check, just write out
    // the data.
    //
    if (disposition == REG_CREATED_NEW_KEY) {
        writeTick = TRUE;
        writeCount = TRUE;

        //
        // First restart
        //
        count = 1;
    }
    else {
        ULONG length, type;

        //
        // First try to get the start time of when we first attempted a restart
        //
        status = FxRegKey::_QueryValue(GetDriverGlobals(),
                                       restart.m_Key,
                                       &valueNameStartTime,
                                       sizeof(startTickCount.QuadPart),
                                       &startTickCount.QuadPart,
                                       &length,
                                       &type);

        if (NT_SUCCESS(status) &&
            length == sizeof(startTickCount.QuadPart) && type == REG_BINARY) {

            //
            // Now try to get the last restart count
            //
            status = FxRegKey::_QueryULong(restart.m_Key,
                                           &valueNameCount,
                                           &count);

            if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
                //
                // We read the start time, but not the count.  Assume there was
                // at least one previous restart.
                //
                count = 1;
                status = STATUS_SUCCESS;
            }
        }

        if (NT_SUCCESS(status)) {
            if (currentTickCount.QuadPart < startTickCount.QuadPart) {
                //
                // Somehow the key survived a reboot or the clock overlfowed
                // and the current time is less then the last time we started
                // timing restarts.  Either way, just treat this as the first
                // time we are restarting.
                //
                writeTick = TRUE;
                writeCount = TRUE;
                count = 1;
            }
            else {
                LONGLONG delta;

                //
                // Compute the difference in time in 100 ns units
                //
                delta = (currentTickCount.QuadPart - startTickCount.QuadPart) *
                         Mx::MxQueryTimeIncrement();

                if (delta < WDF_ABS_TIMEOUT_IN_SEC(m_RestartTimePeriodMaximum)) {
                    //
                    // We are within the time limit, see if we are within the
                    // count limit
                    count++;

                    //
                    // The count starts at one, so include the maximum in the
                    // compare.
                    //
                    if (count <= m_RestartCountMaximum) {
                        writeCount = TRUE;
                    }
                    else {
                        //
                        // Exceeded the restart count, do not attempt to restart
                        // the device.
                        //
                        status = STATUS_UNSUCCESSFUL;
                    }
                }
                else {
                    //
                    // Exceeded the time limit.  This is treated as a reset of
                    // the time limit, so we will try to restart and reset the
                    // start time and restart count.
                    //
                    writeTick = TRUE;
                    writeCount = TRUE;
                    count = 1;
                }
            }
        }
    }

    if (writeTick) {
        //
        // Write out the time and the count
        //
        status = ZwSetValueKey(restart.m_Key,
                               (PUNICODE_STRING)&valueNameStartTime,
                               0,
                               REG_BINARY,
                               &currentTickCount.QuadPart,
                               sizeof(currentTickCount.QuadPart));
    }

    if (NT_SUCCESS(status) && writeCount) {
        status = ZwSetValueKey(restart.m_Key,
                               (PUNICODE_STRING)&valueNameCount,
                               0,
                               REG_DWORD,
                               &count,
                               sizeof(count));
    }

    return NT_SUCCESS(status) ? TRUE : FALSE;
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






























Arguemnts:

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
    // For KMDF, we ignore what the caller of PnpProcessEvent specified (which
    // should always be FALSE, BTW) and base our decision on the current IRQL.
    // If we are running at PASSIVE_LEVEL, we process on the same thread else
    // we queue a work item.
    //
    UNREFERENCED_PARAMETER(CallerSpecifiedProcessingOnDifferentThread);

    ASSERT(FALSE == CallerSpecifiedProcessingOnDifferentThread);

    return (CurrentIrql == PASSIVE_LEVEL) ? FALSE : TRUE;
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
    NTSTATUS status = STATUS_SUCCESS;
    MxDeviceObject pTopOfStack;












    pTopOfStack.SetObject(
        m_Device->GetAttachedDeviceReference());

    ASSERT(pTopOfStack.GetObject() != NULL);

    if (pTopOfStack.GetObject() != NULL) {
        //
        // If the top of the stack is not power pageable, the stack needs a power
        // thread.  Query for it if we are not a PDO (and create it if the lower
        // stack does not support it), and create it if we are a PDO.
        //
        // Some stacks send a usage notification when processing a start device, so
        // a notification could have already traveled through the stack by the time
        // the start irp has completed back up to this device.
        //
        if ((pTopOfStack.GetFlags() & DO_POWER_PAGABLE) == 0 &&
            HasPowerThread() == FALSE) {
            status = QueryForPowerThread();

            if (!NT_SUCCESS(status)) {
                SetInternalFailure();
                SetPendingPnpIrpStatus(status);
            }
        }

        pTopOfStack.DereferenceObject();
        pTopOfStack.SetObject(NULL);
    }

    return status;
}

NTSTATUS
FxPkgPnp::PnpPrepareHardwareInternal(
    VOID
    )
/*++
Routine description:
    This is mode-specific routine for Prepare hardware
    
Arguments:
    None
    
Return value:
    none.
--*/
{
    //
    // nothing to do for KMDF.
    //
    return STATUS_SUCCESS;
}

