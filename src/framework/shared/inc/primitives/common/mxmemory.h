/*++

Copyright (c) Microsoft Corporation

ModuleName:

    MxMemory.h

Abstract:

    Mode agnostic definition of memory
    allocation/deallocation functions

    See MxMemoryKm.h and MxMemoryUm.h for mode
    specific implementations

Author:



Revision History:



--*/

#pragma once

class MxMemory
{
public:

    //
    // MxAllocatePool2 zero-initializes memory by default. New code should
    // ideally always use MxAllocatePool2 instead of MxAllocatePoolWithTag.
    //
    __inline
    static
    PVOID
    MxAllocatePool2(
        _In_ POOL_FLAGS  PoolFlags,
        _In_ SIZE_T  NumberOfBytes,
        _In_ ULONG  Tag
        );

    __inline
    static
    PVOID
    MxAllocatePoolWithTag(
        __in POOL_TYPE  PoolType,
        __in SIZE_T  NumberOfBytes,
        __in ULONG  Tag
        );

    __inline
    static
    VOID
    MxFreePool(
        __in PVOID Ptr
        );
};

