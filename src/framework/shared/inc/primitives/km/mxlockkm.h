/*++

Copyright (c) Microsoft Corporation

ModuleName:

    MxLockKm.h

Abstract:

    Kernel mode implementation of lock
    class defined in MxLock.h

Author:



Revision History:



--*/

#pragma once

#include "DbgMacros.h"

typedef KSPIN_LOCK MdLock;

#include "MxLock.h"

FORCEINLINE
MxLock::MxLock(
    )
{
    CLEAR_DBGFLAG_INITIALIZED;

    MxLock::Initialize();
}

FORCEINLINE
VOID
MxLockNoDynam::Initialize(
    )
{
    KeInitializeSpinLock(&m_Lock);

    SET_DBGFLAG_INITIALIZED;
}

_Acquires_lock_(this->m_Lock)
__drv_maxIRQL(DISPATCH_LEVEL)
__drv_setsIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
MxLockNoDynam::Acquire(
    __out __drv_deref(__drv_savesIRQL) KIRQL * OldIrql
    )
{
    ASSERT_DBGFLAG_INITIALIZED;
    
    KeAcquireSpinLock(&m_Lock, OldIrql);
}

_Acquires_lock_(this->m_Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
MxLockNoDynam::AcquireAtDpcLevel(
    )
{
    ASSERT_DBGFLAG_INITIALIZED;
    
    KeAcquireSpinLockAtDpcLevel(&m_Lock);
}

_Releases_lock_(this->m_Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
MxLockNoDynam::Release(
    __drv_restoresIRQL KIRQL NewIrql
    )
{
    ASSERT_DBGFLAG_INITIALIZED;

    KeReleaseSpinLock(&m_Lock, NewIrql);    
}

_Releases_lock_(this->m_Lock)
__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
MxLockNoDynam::ReleaseFromDpcLevel(
    )
{
    ASSERT_DBGFLAG_INITIALIZED;
    
    KeReleaseSpinLockFromDpcLevel(&m_Lock);
}

FORCEINLINE
VOID
MxLockNoDynam::Uninitialize(
    )
{
    CLEAR_DBGFLAG_INITIALIZED;
}

FORCEINLINE
MxLock::~MxLock(
    )
{
    CLEAR_DBGFLAG_INITIALIZED;
}
