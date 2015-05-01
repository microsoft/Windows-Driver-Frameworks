/*++

Copyright (c) Microsoft Corporation

ModuleName:

    MxTimer.h

Abstract:

    Mode agnostic definiton of timer

    See MxTimerKm.h and MxTimerUm.h for
    mode specific implementations
    
Author:


Revision History:



--*/

#pragma once

class MxTimer
{
private:
    //
    // Handle to the timer object
    //
    MdTimer m_Timer;

public:

    FORCEINLINE
    MxTimer(
        VOID
        );

    FORCEINLINE
    ~MxTimer(
        VOID
        );

    CHECK_RETURN_IF_USER_MODE
    FORCEINLINE
    NTSTATUS
    Initialize(
        __in_opt PVOID TimerContext,
        __in MdDeferredRoutine TimerCallback,
        __in LONG Period
        );

    CHECK_RETURN_IF_USER_MODE
    FORCEINLINE
    NTSTATUS
    InitializeEx(
        __in_opt PVOID TimerContext,
        __in MdExtCallbackType TimerCallback,
        __in LONG Period,
        __in ULONG TolerableDelay,
        __in BOOLEAN UseHighResolutionTimer
        );

    FORCEINLINE
    VOID
    Start(
        __in LARGE_INTEGER DueTime,
        __in ULONG TolerableDelay = 0
        );

    FORCEINLINE
    BOOLEAN
    StartWithReturn(
        __in LARGE_INTEGER DueTime,
        __in ULONG TolerableDelay = 0
        );

    _Must_inspect_result_
    FORCEINLINE
    BOOLEAN
    Stop(
        VOID
        );

    FORCEINLINE
    VOID
    FlushQueuedDpcs(
        VOID
        );
};


