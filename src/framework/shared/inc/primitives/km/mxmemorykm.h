/*++

Copyright (c) Microsoft Corporation

ModuleName:

    MxMemoryKm.h

Abstract:

    Kernel mode implementation of memory
    class defined in MxMemory.h

Author:



Revision History:



--*/

#pragma once

#include "MxMemory.h"

__inline
VOID
MxValidatePoolFlags(
    _In_ POOL_FLAGS  PoolFlags
    )
{
#if DBG
    //
    // Validate WDF internal code does not pass in a POOL_TYPE accidentally.
    //
    // POOL_TYPE is an enum, and POOL_FLAGS is a ULONG64. While the compiler is
    // able to detect the error if POOL_FLAGS is passed to MxAllocatePoolWithTag,
    // it cannot do so if POOL_TYPE is passed to MxAllocatePool2.
    //
    // Validation is done based on the following facts.
    //
    //  1) the caller must pass one of the following flags:
    //
    // #define POOL_FLAG_NON_PAGED               0x0000000000000040UI64
    // #define POOL_FLAG_NON_PAGED_EXECUTE       0x0000000000000080UI64
    // #define POOL_FLAG_PAGED                   0x0000000000000100UI64
    //
    //  2) POOL_TYPE is very unlikely to use any of the above flags.
    //
    // enum POOL_TYPE {
    //     NonPagedPool        = 0,
    //     NonPagedPoolExecute = NonPagedPool,
    //     PagedPool           = 1,
    //     NonPagedPoolSession = 32,   // 0x0020
    //     POOL_COLD_ALLOCATION= 256,  // 0x0100
    //     NonPagedPoolNx      = 512,  // 0x0200
    // };
    //
    //  3) MxAllocatePool2 is used by WDF for internal usage only. When client
    // driver calls WDF API and passs PoolType to allocate memory, it goes to
    // MxAllocatePoolWithTag instead. As param to MxAllocatePool2 is provided
    // by WDF, which never uses POOL_COLD_ALLOCATION, the validation is safe.
    //

    #define VALID_POOL_FLAG_POOLS (POOL_FLAG_PAGED | POOL_FLAG_NON_PAGED | POOL_FLAG_NON_PAGED_EXECUTE)

    ASSERT((PoolFlags & VALID_POOL_FLAG_POOLS) != 0);

#else
    UNREFERENCED_PARAMETER(PoolFlags);
#endif
}

__inline
PVOID
MxMemory::MxAllocatePool2(
    _In_ POOL_FLAGS  PoolFlags,
    _In_ SIZE_T  NumberOfBytes,
    _In_ ULONG  Tag
    )
{
    MxValidatePoolFlags(PoolFlags);

    return ExAllocatePool2(PoolFlags, NumberOfBytes, Tag);
}

__inline
PVOID
MxMemory::MxAllocatePoolWithTag(
    __in POOL_TYPE  PoolType,
    __in SIZE_T  NumberOfBytes,
    __in ULONG  Tag
    )
{
    #pragma warning( suppress : 4996 28751 )
    return ExAllocatePoolWithTag(PoolType, NumberOfBytes, Tag);
}

__inline
VOID
MxMemory::MxFreePool(
    __in PVOID Ptr
    )
{
    ExFreePool(Ptr);
}

