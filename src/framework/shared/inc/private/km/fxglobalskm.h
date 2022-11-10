/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxGlobalsKm.h

Abstract:

    This module contains kernel-mode specific globals definitions
    for the frameworks.

    For common definitions common between km and um please see
    FxGlobals.h

Author:



Environment:

    Kernel mode only

Revision History:


--*/
#ifdef __cplusplus
extern "C" {
#endif

extern PCHAR WdfLdrType;

#define WDF_LDR_STATIC_TYPE_STR     "WdfStatic"

// forward definitions
typedef struct _FX_DRIVER_GLOBALS *PFX_DRIVER_GLOBALS;
typedef struct _FX_DUMP_DRIVER_INFO_ENTRY *PFX_DUMP_DRIVER_INFO_ENTRY;

struct FxMdlDebugInfo {
    PMDL Mdl;
    FxObject* Owner;
    PVOID Caller;
};

#define NUM_MDLS_IN_INFO (16)

struct FxAllocatedMdls {
    FxMdlDebugInfo Info[NUM_MDLS_IN_INFO];
    ULONG Count;
    struct FxAllocatedMdls* Next;
};

#define DDI_ENTRY_IMPERSONATION_OK()
#define DDI_ENTRY()

typedef
NTSTATUS
(*PFN_IO_CONNECT_INTERRUPT_EX)(
    __inout PIO_CONNECT_INTERRUPT_PARAMETERS Parameters
    );

typedef
NTSTATUS
(*PFN_IO_DISCONNECT_INTERRUPT_EX)(
    __in PIO_DISCONNECT_INTERRUPT_PARAMETERS Parameters
    );

typedef
VOID
(*PFN_IO_REPORT_INTERRUPT_ACTIVE) (
    _In_ PIO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS Parameters
    );

typedef
VOID
(*PFN_IO_REPORT_INTERRUPT_INACTIVE) (
    _In_ PIO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS Parameters
    );

VOID
FxRegisterBugCheckCallback(
    __inout PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in    PDRIVER_OBJECT DriverObject
    );

VOID
FxUnregisterBugCheckCallback(
    __inout PFX_DRIVER_GLOBALS FxDriverGlobals
    );

VOID
FxInitializeBugCheckDriverInfo();

VOID
FxUninitializeBugCheckDriverInfo();

VOID
FxCacheBugCheckDriverInfo(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals
    );

VOID
FxPurgeBugCheckDriverInfo(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals
    );

typedef struct _FX_DRIVER_TRACKER_CACHE_AWARE {
    //
    // Internal data types.
    //
private:
    typedef struct _FX_DRIVER_TRACKER_ENTRY {
         volatile PFX_DRIVER_GLOBALS FxDriverGlobals;
    } FX_DRIVER_TRACKER_ENTRY, *PFX_DRIVER_TRACKER_ENTRY;

    //
    // Public interface.
    //
public:
    _Must_inspect_result_
    NTSTATUS
    Initialize();

    VOID
    Uninitialize();

    _Must_inspect_result_
    NTSTATUS
    Register(
        __in PFX_DRIVER_GLOBALS FxDriverGlobals
        );

    VOID
    Deregister(
        __in PFX_DRIVER_GLOBALS FxDriverGlobals
        );

    //
    // Tracks the driver usage on the current processor.
    // KeGetCurrentProcessorNumberEx is called directly because the procgrp.lib
    // provides the downlevel support for Vista, XP and Win2000.
    //
    __inline
    VOID
    TrackDriver(
        __in PFX_DRIVER_GLOBALS FxDriverGlobals
        )
    {
        ASSERT(m_PoolToFree != NULL);

        GetProcessorDriverEntryRef(
            KeGetCurrentProcessorIndex())->FxDriverGlobals =
                FxDriverGlobals;
    }

    //
    // Returns the tracked driver (globals) on the current processor.
    // KeGetCurrentProcessorNumberEx is called directly because the procgrp.lib
    // provides the downlevel support for Vista, XP and Win2000.
    //
    _Must_inspect_result_
    __inline
    PFX_DRIVER_GLOBALS
    GetCurrentTrackedDriver()
    {
        PFX_DRIVER_GLOBALS fxDriverGlobals = NULL;

        ASSERT(m_PoolToFree != NULL);

        fxDriverGlobals = GetProcessorDriverEntryRef(
            KeGetCurrentProcessorIndex())->FxDriverGlobals;

        return fxDriverGlobals;
    }

    //
    // Helper functions.
    //
private:
    //
    // Returns the per-processor cache-aligned driver usage ref structure for
    // given processor.
    //
    __inline
    PFX_DRIVER_TRACKER_ENTRY
    GetProcessorDriverEntryRef(
        __in ULONG Index
        )
    {
        return ((PFX_DRIVER_TRACKER_ENTRY) (((PUCHAR)m_DriverUsage) +
                    Index * m_EntrySize));
    }

    //
    // Data members.
    //
private:
    //
    // Pointer to array of cache-line aligned tracking driver structures.
    //
    PFX_DRIVER_TRACKER_ENTRY    m_DriverUsage;

    //
    // Points to pool of per-proc tracking entries that needs to be freed.
    //
    PVOID                       m_PoolToFree;

    //
    // Size of each padded tracking driver structure.
    //
    ULONG                       m_EntrySize;

    //
    // Indicates # of entries in the array of tracking driver structures.
    //
    ULONG                       m_Number;
} FX_DRIVER_TRACKER_CACHE_AWARE, *PFX_DRIVER_TRACKER_CACHE_AWARE;


#include "FxGlobals.h"


//
// This inline function tracks driver usage; This info is used by the
// debug dump callback routine for selecting which driver's log to save
// in the minidump. At this time we track the following OS to framework calls:
//  (a) IRP dispatch (general, I/O, PnP, WMI).
//  (b) Request's completion callbacks.
//  (c) Work Item's (& System Work Item's) callback handlers.
//  (d) Timer's callback handlers.
//
__inline
VOID
FX_TRACK_DRIVER(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals
    )
{
    if (FxDriverGlobals->FxTrackDriverForMiniDumpLog) {
        FxLibraryGlobals.DriverTracker.TrackDriver(FxDriverGlobals);
    }
}

_Must_inspect_result_
__inline
PVOID
FxAllocateFromNPagedLookasideListNoTracking (
    __in PNPAGED_LOOKASIDE_LIST Lookaside
    )

/*++

Routine Description:

    This function removes (pops) the first entry from the specified
    nonpaged lookaside list. This function was added to allow request allocated
    by a lookaside list to be freed by ExFreePool and hence do no tracking of statistics.

Arguments:

    Lookaside - Supplies a pointer to a nonpaged lookaside list structure.

Return Value:

    If an entry is removed from the specified lookaside list, then the
    address of the entry is returned as the function value. Otherwise,
    NULL is returned.

--*/

{

    PVOID Entry;

    Entry = InterlockedPopEntrySList(&Lookaside->L.ListHead);

    if (Entry == NULL) {
        Entry = (Lookaside->L.Allocate)(Lookaside->L.Type,
                                        Lookaside->L.Size,
                                        Lookaside->L.Tag);
    }

    return Entry;
}

__inline
VOID
FxFreeToNPagedLookasideListNoTracking (
    __in PNPAGED_LOOKASIDE_LIST Lookaside,
    __in PVOID Entry
    )
/*++

Routine Description:

    This function inserts (pushes) the specified entry into the specified
    nonpaged lookaside list. This function was added to allow request allocated
    by a lookaside list to be freed by ExFreePool and hence do no tracking of statistics.

Arguments:

    Lookaside - Supplies a pointer to a nonpaged lookaside list structure.

    Entry - Supples a pointer to the entry that is inserted in the
        lookaside list.

Return Value:

    None.

--*/

{
    if (ExQueryDepthSList(&Lookaside->L.ListHead) >= Lookaside->L.Depth) {
        (Lookaside->L.Free)(Entry);
    }
    else {
        InterlockedPushEntrySList(&Lookaside->L.ListHead,
                                  (PSLIST_ENTRY)Entry);
    }
}

__inline
PVOID
FxAllocateFromNPagedLookasideList (
    _In_ PNPAGED_LOOKASIDE_LIST Lookaside,
    _In_opt_ size_t ElementSize = 0
    )

/*++

Routine Description:

    This function removes (pops) the first entry from the specified
    nonpaged lookaside list.

Arguments:

    Lookaside - Supplies a pointer to a nonpaged lookaside list structure.

Return Value:

    If an entry is removed from the specified lookaside list, then the
    address of the entry is returned as the function value. Otherwise,
    NULL is returned.

--*/

{

    PVOID Entry;

    UNREFERENCED_PARAMETER(ElementSize);

    Lookaside->L.TotalAllocates += 1;

    Entry = InterlockedPopEntrySList(&Lookaside->L.ListHead);

    if (Entry == NULL) {
        Lookaside->L.AllocateMisses += 1;
        Entry = (Lookaside->L.Allocate)(Lookaside->L.Type,
                                        Lookaside->L.Size,
                                        Lookaside->L.Tag);
    }

    return Entry;
}

__inline
VOID
FxFreeToNPagedLookasideList (
    __in PNPAGED_LOOKASIDE_LIST Lookaside,
    __in PVOID Entry
    )
/*++

Routine Description:

    This function inserts (pushes) the specified entry into the specified
    nonpaged lookaside list.

Arguments:

    Lookaside - Supplies a pointer to a nonpaged lookaside list structure.

    Entry - Supples a pointer to the entry that is inserted in the
        lookaside list.

Return Value:

    None.

--*/

{
    Lookaside->L.TotalFrees += 1;

    if (ExQueryDepthSList(&Lookaside->L.ListHead) >= Lookaside->L.Depth) {
        Lookaside->L.FreeMisses += 1;
        (Lookaside->L.Free)(Entry);

    }
    else {
        InterlockedPushEntrySList(&Lookaside->L.ListHead,
                                  (PSLIST_ENTRY)Entry);
    }
}

_Must_inspect_result_
__inline
PVOID
FxAllocateFromPagedLookasideList (
    __in PPAGED_LOOKASIDE_LIST Lookaside
    )

/*++

Routine Description:

    This function removes (pops) the first entry from the specified
    paged lookaside list.

Arguments:

    Lookaside - Supplies a pointer to a paged lookaside list structure.

Return Value:

    If an entry is removed from the specified lookaside list, then the
    address of the entry is returned as the function value. Otherwise,
    NULL is returned.

--*/

{

    PVOID Entry;

    Lookaside->L.TotalAllocates += 1;

    Entry = InterlockedPopEntrySList(&Lookaside->L.ListHead);
    if (Entry == NULL) {
        Lookaside->L.AllocateMisses += 1;
        Entry = (Lookaside->L.Allocate)(Lookaside->L.Type,
                                        Lookaside->L.Size,
                                        Lookaside->L.Tag);
    }

    return Entry;
}

__inline
VOID
FxFreeToPagedLookasideList (
    __in PPAGED_LOOKASIDE_LIST Lookaside,
    __in PVOID Entry
    )
/*++

Routine Description:

    This function inserts (pushes) the specified entry into the specified
    paged lookaside list.

Arguments:

    Lookaside - Supplies a pointer to a paged lookaside list structure.

    Entry - Supples a pointer to the entry that is inserted in the
        lookaside list.

Return Value:

    None.

--*/

{
    Lookaside->L.TotalFrees += 1;

    if (ExQueryDepthSList(&Lookaside->L.ListHead) >= Lookaside->L.Depth) {
        Lookaside->L.FreeMisses += 1;
        (Lookaside->L.Free)(Entry);

    } else {
        InterlockedPushEntrySList(&Lookaside->L.ListHead,
                                  (PSLIST_ENTRY)Entry);
    }
}

_Must_inspect_result_
__inline
BOOLEAN
FxIsProcessorGroupSupported(
    VOID
    )
{
    //
    // Groups are supported in Win 7 and forward.
    //
    return TRUE;
}

#ifdef __cplusplus
}
#endif
