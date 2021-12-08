/*++

Copyright (c) Microsoft Corporation

ModuleName:

    MxMemoryUm.h

Abstract:

    User mode implementation of memory
    class defined in MxMemory.h

Author:



Revision History:



--*/

#pragma once

#include "MxMemory.h"

__inline
PVOID
MxMemory::MxAllocatePool2(
    _In_ POOL_FLAGS  PoolFlags,
    _In_ SIZE_T  NumberOfBytes,
    _In_ ULONG  Tag
    )
{
    UNREFERENCED_PARAMETER(PoolFlags);
    UNREFERENCED_PARAMETER(Tag);

    return ::HeapAlloc(
                GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                NumberOfBytes
                );
}

__inline
PVOID
MxMemory::MxAllocatePoolWithTag(
    __in POOL_TYPE  PoolType,
    __in SIZE_T  NumberOfBytes,
    __in ULONG  Tag
    )
{
    UNREFERENCED_PARAMETER(PoolType);
    UNREFERENCED_PARAMETER(Tag);

    return ::HeapAlloc(
                GetProcessHeap(),
                0,
                NumberOfBytes
                );
}

__inline
VOID
MxMemory::MxFreePool(
    __in PVOID Ptr
    )
{
    ::HeapFree(
            GetProcessHeap(),
            0,
            Ptr
            );
}

