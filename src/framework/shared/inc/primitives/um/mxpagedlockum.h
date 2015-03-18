/*++

Copyright (c) Microsoft Corporation

ModuleName:

    MxPagedLockUm.h

Abstract:

    User mode implementation of paged lock defined in
    MxPagedLock.h
    
Author:



Revision History:



--*/

#pragma once

typedef struct { 
    CRITICAL_SECTION Lock; 
    bool Initialized; 
    DWORD OwnerThreadId; 
} MdPagedLock;

#include "MxPagedLock.h"

FORCEINLINE
MxPagedLock::MxPagedLock(
    )
{
    m_Lock.Initialized = false;
    m_Lock.OwnerThreadId = 0;    
}

_Must_inspect_result_
FORCEINLINE
NTSTATUS
MxPagedLockNoDynam::Initialize(
    )
{
    if (InitializeCriticalSectionAndSpinCount(&m_Lock.Lock, 0)) {
        m_Lock.Initialized = true;
        
        return S_OK;
    }
    else {
        DWORD err = GetLastError();
        return WinErrorToNtStatus(err);
    }    
}

FORCEINLINE
VOID
#pragma prefast(suppress:__WARNING_UNMATCHED_DEFN, "Can't apply kernel mode annotations.");
MxPagedLockNoDynam::Acquire(
    )
{
    EnterCriticalSection(&m_Lock.Lock);

    DWORD threadId = GetCurrentThreadId();

    if (threadId == m_Lock.OwnerThreadId) {
        Mx::MxAssertMsg("Recursive acquision of the lock is not allowed", FALSE);
    }

    m_Lock.OwnerThreadId = GetCurrentThreadId();            
}

FORCEINLINE
VOID
MxPagedLockNoDynam::AcquireUnsafe(
    )
{
    MxPagedLockNoDynam::Acquire();
}

FORCEINLINE
BOOLEAN
#pragma prefast(suppress:__WARNING_UNMATCHED_DEFN, "Can't apply kernel mode annotations.");
MxPagedLockNoDynam::TryToAcquire(
    )
{
    return TryEnterCriticalSection(&m_Lock.Lock) == TRUE ? TRUE : FALSE;
}


FORCEINLINE
VOID
#pragma prefast(suppress:__WARNING_UNMATCHED_DEFN, "Can't apply kernel mode annotations.");
MxPagedLockNoDynam::Release(
    )
{
    m_Lock.OwnerThreadId = 0;

    LeaveCriticalSection(&m_Lock.Lock);    
}

FORCEINLINE
VOID
MxPagedLockNoDynam::ReleaseUnsafe(
    )
{
    MxPagedLockNoDynam::Release();
}

FORCEINLINE
VOID
MxPagedLockNoDynam::Uninitialize(
    )
{
    DeleteCriticalSection(&m_Lock.Lock);
    m_Lock.Initialized = false;
}

FORCEINLINE
MxPagedLock::~MxPagedLock(
    )
{
    if (m_Lock.Initialized) {
        this->Uninitialize();
    }
}
