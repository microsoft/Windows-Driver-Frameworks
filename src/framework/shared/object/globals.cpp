
/*++

Copyright (c) Microsoft Corporation

Module Name:

    globals.cpp

Abstract:

    This contains all Driver Frameworks configuration globals.

Author:




Environment:

    Both kernel and user mode

Revision History:













--*/


#include "fxobjectpch.hpp"

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
#include "FeatureStagingSupport.h"
#endif

#include <FeatureStaging-WDF.h>

// Tracing support
extern "C" {
#if defined(EVENT_TRACING)
#include "globals.tmh"
#endif
}

extern "C" {

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
VOID
VerifierPageLockHandle (
    VOID
    );
#ifdef ALLOC_PRAGMA
#pragma alloc_text(WDF_FX_VF_SECTION_NAME, VerifierPageLockHandle)
#endif
#endif

//
// Private methods.
//

VOID
FxLibraryGlobalsQueryRegistrySettings(
    VOID
    );

VOID
FxRegistrySettingsInitialize(
    __inout PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in PCUNICODE_STRING RegistryPath,
    __in BOOLEAN WindowsVerifierOn
    );

_Must_inspect_result_
FxObjectDebugInfo*
FxVerifierGetObjectDebugInfo(
    __in HANDLE Key,
    __in PFX_DRIVER_GLOBALS  FxDriverGlobals
    );

NTSTATUS
FxVerifierReadObjectDebugInfo(
    _In_ HANDLE Key,
    _In_ PFX_DRIVER_GLOBALS  FxDriverGlobals,
    _Inout_ FxObjectDebugInfo **Info,
    _In_ PCWSTR KeyName,
    _In_ FxObjectDebugInfoFlags DebugFlag,
    _In_opt_ PCWSTR DefaultSettings
    );

VOID
FxVerifierQueryTrackPower(
    __in HANDLE Key,
    __out FxTrackPowerOption* TrackPower
    );

VOID
FxVerifierQueryStateSeparationDetection(
    _In_ HANDLE Key,
    _Out_ FxStateSeparationDetectionOption* StateSeparationDetection
    );

VOID
FxOverrideDefaultVerifierSettings(
    __in    HANDLE Key,
    __in    LPWSTR Name,
    _Inout_ PBOOLEAN OverrideValue
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryRegistryValuesEx(
    _In_     ULONG RelativeTo,
    _In_     PCWSTR Path,
    _Inout_ _At_(*(*QueryTable).EntryContext, _Pre_unknown_)
        PRTL_QUERY_REGISTRY_TABLE QueryTable,
    _In_opt_ PVOID Context,
    _In_opt_ PVOID Environment
    );

//
// Global allocation tracker
//
FX_POOL FxPoolFrameworks;

FxLibraryGlobalsType FxLibraryGlobals = { 0 };

//
// These are defined in FxObjectInfo.cpp to account for the facts that
//     1. FxObjectInfo array is different for UMDF and KMDF,
//     2. and not all the types are available in the common code
//
extern const FX_OBJECT_INFO FxObjectsInfo[];
extern ULONG FxObjectsInfoCount;

//
// Prevent compiler/linker/BBT from optimizing the global variable away
//
#if defined(_M_IX86)
#pragma comment(linker, "/include:_FxObjectsInfoCount")
#else
#pragma comment(linker, "/include:FxObjectsInfoCount")
#endif


_Must_inspect_result_
BOOLEAN
FxVerifyObjectTypeInTable(
    __in USHORT ObjectType
    )
{
    ULONG i;

    for (i = 0; i < FxObjectsInfoCount; i++) {
        if (ObjectType == FxObjectsInfo[i].ObjectType) {
            return TRUE;
        }
        else if (ObjectType > FxObjectsInfo[i].ObjectType) {
            continue;
        }

        return FALSE;
    }

    return FALSE;
}

_Must_inspect_result_
NTSTATUS
FxVerifyAllocateDebugInfo(
    _Inout_ FxObjectDebugInfo** Info,
    _In_ LPCWSTR HandleNameList,
    _In_ PFX_DRIVER_GLOBALS FxDriverGlobals,
    _In_ FxObjectDebugInfoFlags DebugFlag
    )

/*++

Routine Description:
    Allocates an array of FxObjectDebugInfo's if 'Info' is NULL.  The length
    of this array is the same length as FxObjectsInfo.  The array is sorted
    the same as FxObjectDebugInfo, ObjectInfo is ascending in the list.

    If HandleNameList's first string is "*", we treat this as a wildcard and
    track all external handles.

Arguments:
    Info - preallocated FxObjectDebugInfo struct. If NULL one will be allocated

    HandleNameList - a multi-sz of handle names.  It is assumed the multi sz is
        well formed.

    DebugFlag - Flag to set based on the passed in HandleNameList

Return Value:
    NT_SUCCESS(Status) if pInfo is valid and the list is in use

--*/

{
    FxObjectDebugInfo* pInfo;
    LPCWCHAR pCur;
    ULONG i;
    BOOLEAN all;

    ASSERT(Info != NULL);
    pInfo = *Info;

    //
    // check to see if the multi sz is empty
    //
    if (*HandleNameList == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    if (pInfo == NULL) {

        ULONG length = sizeof(FxObjectDebugInfo) * FxObjectsInfoCount;

        //
        // Freed with ExFreePool in FxFreeDriverGlobals.  Must be non paged because
        // objects can be allocated at IRQL > PASSIVE_LEVEL.
        //
        pInfo = (FxObjectDebugInfo*) MxMemory::MxAllocatePool2(POOL_FLAG_NON_PAGED,
                                                           length,
                                                           FxDriverGlobals->Tag);
        if (pInfo == NULL) {
            return STATUS_MEMORY_NOT_ALLOCATED;
        }
    }

    ASSERT(pInfo != NULL);

    all = *HandleNameList == L'*' ? TRUE : FALSE;

    //
    // Iterate over all of the objects in our internal array.  We iterate over
    // this array instead of the multi sz list b/c this way we only convert
    // each ANSI string to UNICODE once.
    //
    for (i = 0; i < FxObjectsInfoCount; i++) {
        UNICODE_STRING objectName;
        WCHAR ubuffer[40];
        STRING string;

        pInfo[i].ObjectType = FxObjectsInfo[i].ObjectType;

        //
        // If this is an internal object, just continue past it
        //
        if (FxObjectsInfo[i].HandleName == NULL) {
            continue;
        }

        //
        // Short circuit if we are wildcarding
        //
        if (all) {
            pInfo[i].u.DebugFlags |= DebugFlag;
            continue;
        }

        RtlInitAnsiString(&string, FxObjectsInfo[i].HandleName);

        RtlZeroMemory(ubuffer, sizeof(ubuffer));
        objectName.Buffer = ubuffer;
        objectName.Length = 0;
        objectName.MaximumLength = sizeof(ubuffer);

        //
        // Conversion failed, just continue.  Failure is not critical to
        // returning the list.
        //
        if (!NT_SUCCESS(RtlAnsiStringToUnicodeString(&objectName,
                                                     &string,
                                                     FALSE))) {
            continue;
        }

        //
        // Now iterate over the multi sz list, comparing handle strings in the
        // list against the current object name.
        //
        pCur = HandleNameList;

        while (*pCur != UNICODE_NULL) {
            UNICODE_STRING handleName;

            RtlInitUnicodeString(&handleName, pCur);

            //
            // Increment to the next string now.  Add one so that we skip past
            // terminating null for this sz as well.
            // Length is the number of bytes, not the number of characters.
            //
            pCur += handleName.Length / sizeof(WCHAR) + 1;

            //
            // Case insensitive compare
            //
            if (RtlCompareUnicodeString(&handleName, &objectName, TRUE) == 0) {
                pInfo[i].u.DebugFlags |= DebugFlag;
                break;
            }
        }
    }

    if (*Info == NULL) {
        *Info = pInfo;
    }

    return STATUS_SUCCESS;
}

VOID
FxDriverGlobalsInitializeDebugExtension(
    __inout PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in_opt    HANDLE Key
    )
{
    FxDriverGlobalsDebugExtension* pExtension;

    //
    // The wdf subkey may not be present for inbox drivers that do not use inf.
    // Since Mdl tracking doen't need regsitry info we go ahead and allocate
    // debug extension for use in Mdl tracking. Tag tracker depends on registry
    // info and it won't be available if registry info is not present.
    //

    pExtension = (FxDriverGlobalsDebugExtension*) MxMemory::MxAllocatePool2(
        POOL_FLAG_NON_PAGED, sizeof(FxDriverGlobalsDebugExtension), FxDriverGlobals->Tag);

    if (pExtension == NULL) {
        return;
    }

    pExtension->AllocatedTagTrackersLock.Initialize();

    InitializeListHead(&pExtension->AllocatedTagTrackersListHead);

    pExtension->TrackPower = FxTrackPowerNone;
    pExtension->StateSeparationDetection = FxStateSeparationDetectionNone;

    FxDriverGlobals->DebugExtension = pExtension;

    if (Key != NULL) {
        pExtension->ObjectDebugInfo = FxVerifierGetObjectDebugInfo(
                                                        Key,
                                                        FxDriverGlobals
                                                        );
        FxVerifierQueryTrackPower(Key, &pExtension->TrackPower);
        FxVerifierQueryStateSeparationDetection(Key, &pExtension->StateSeparationDetection);
    }

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
    KeInitializeSpinLock(&pExtension->AllocatedMdlsLock);
#endif
}

PCSTR
FxObjectTypeToHandleName(
    __in WDFTYPE ObjectType
    )
{
    ULONG i;

    for (i = 0; i < FxObjectsInfoCount; i++) {
        if (ObjectType == FxObjectsInfo[i].ObjectType) {
            return FxObjectsInfo[i].HandleName;
        }
        else if (ObjectType > FxObjectsInfo[i].ObjectType) {
            continue;
        }

        return NULL;
    }

    return NULL;
}

_Must_inspect_result_
BOOLEAN
FxVerifierIsDebugInfoFlagSetForType(
    _In_ FxObjectDebugInfo* DebugInfo,
    _In_ WDFTYPE ObjectType,
    _In_ FxObjectDebugInfoFlags Flag
    )

/*++

Routine Description:
    For a given object type and flag, returns to the caller if the flag is set.

Arguments:
    DebugInfo - array of object debug info to search through

    ObjectType - the type of the object to check

    FxObjectDebugInfoFlags - Flag to check for

Return Value:
    TRUE if objects of the given type had its flag set.

--*/

{
    ULONG i;

    //
    // Array size of DebugInfo is the same size as FxObjectsInfo
    //
    for (i = 0; i < FxObjectsInfoCount; i++) {
        if (ObjectType == DebugInfo[i].ObjectType) {
            return FLAG_TO_BOOL(DebugInfo[i].u.DebugFlags,
                                Flag);
        }
        else if (ObjectType > FxObjectsInfo[i].ObjectType) {
            continue;
        }

        return FALSE;
    }

    return FALSE;
}


VOID
FxVerifyObjectTableIsSorted(
    VOID
    )
{
    ULONG i;
    USHORT prevType;

    prevType = FxObjectsInfo[0].ObjectType;

    for (i = 1; i < FxObjectsInfoCount; i++) {
        if (prevType >= FxObjectsInfo[i].ObjectType) {
            ASSERTMSG("FxObjectsInfo table is not in sorted order\n",
                         prevType < FxObjectsInfo[i].ObjectType);
        }

        prevType = FxObjectsInfo[i].ObjectType;
    }
}

VOID
FxLibraryGlobalsQueryRegistrySettings(
    VOID
    )
{
    FxAutoRegKey hWdf;
    NTSTATUS status = STATUS_SUCCESS;
    DECLARE_CONST_UNICODE_STRING(path,            WDF_REGISTRY_BASE_PATH);
    DECLARE_CONST_UNICODE_STRING(ifrDisabledName, WDF_GLOBAL_VALUE_IFRDISABLED);
    DECLARE_CONST_UNICODE_STRING(ssDisabledName,  WDF_GLOBAL_VALUE_SLEEPSTUDY_DISABLED);


    ULONG ifrDisabled = 0;
    ULONG ssDisabled = 0;


    status = FxRegKey::_OpenKey(NULL, &path, &hWdf.m_Key, KEY_READ);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    status = FxRegKey::_QueryULong(hWdf.m_Key, &ifrDisabledName, &ifrDisabled);
    if ((NT_SUCCESS(status)) && (ifrDisabled == 1)) {
        FxLibraryGlobals.IfrDisabled = TRUE;
    }

    FxLibraryGlobals.SleepStudyDisabled = FALSE;
    status = FxRegKey::_QueryULong(hWdf.m_Key, &ssDisabledName, &ssDisabled);
    if ((NT_SUCCESS(status)) && (ssDisabled == 1)) {
        FxLibraryGlobals.SleepStudyDisabled = TRUE;
    }











exit:
    return;
}

_Must_inspect_result_
NTSTATUS
FxLibraryGlobalsCommission(
    VOID
    )
{
    NTSTATUS status;

    //
    // Global initialization for mode-agnostic primitives library
    //
    Mx::MxGlobalInit();















#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
    FxLibraryGlobals.IsUserModeFramework = FALSE;
#else
    FxLibraryGlobals.IsUserModeFramework = TRUE;
#endif

    //
    // IFR is enabled by default
    //
    FxLibraryGlobals.IfrDisabled = FALSE;

    //
    // Logging Sleep Study Blockers is enabled by default
    //
    FxLibraryGlobals.SleepStudyDisabled = FALSE;

    //
    // Directed Power Transition support
    //
    FxLibraryGlobals.WdfDirectedPowerTransitionEnabled = FALSE;

    //
    // Query global WDF settings (both KMDF and UMDF).
    //
    FxLibraryGlobalsQueryRegistrySettings();

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
    UNICODE_STRING funcName;


    RtlInitUnicodeString(&funcName, L"IoConnectInterruptEx");
    FxLibraryGlobals.IoConnectInterruptEx = (PFN_IO_CONNECT_INTERRUPT_EX)
        MmGetSystemRoutineAddress(&funcName);

    RtlInitUnicodeString(&funcName, L"IoDisconnectInterruptEx");
    FxLibraryGlobals.IoDisconnectInterruptEx = (PFN_IO_DISCONNECT_INTERRUPT_EX)
        MmGetSystemRoutineAddress(&funcName);

    RtlInitUnicodeString(&funcName, L"IoReportInterruptActive");
    FxLibraryGlobals.IoReportInterruptActive =
      (PFN_IO_REPORT_INTERRUPT_ACTIVE) MmGetSystemRoutineAddress(&funcName);

    RtlInitUnicodeString(&funcName, L"IoReportInterruptInactive");
    FxLibraryGlobals.IoReportInterruptInactive =
      (PFN_IO_REPORT_INTERRUPT_INACTIVE) MmGetSystemRoutineAddress(&funcName);

#endif //((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))

    FxLibraryGlobals.OsVersionInfo.dwOSVersionInfoSize = sizeof(FxLibraryGlobals.OsVersionInfo);
    RtlGetVersion((PRTL_OSVERSIONINFOW) &FxLibraryGlobals.OsVersionInfo);

    //
    // Initialize power management-related stuff.
    //
    RtlZeroMemory(&FxLibraryGlobals.MachineSleepStates[0],
                  sizeof(FxLibraryGlobals.MachineSleepStates));

    //
    // Insure that the FxObject is layed-up correctly.
    //
    FxVerifyObjectTableIsSorted();

    //
    // Initialize the list of FxDriverGlobals.
    // This is essentially the list of drivers on this WDF version.
    //
    InitializeListHead(&FxLibraryGlobals.FxDriverGlobalsList);
    FxLibraryGlobals.FxDriverGlobalsListLock.Initialize();

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
    //
    // Register for the global (library) bugcheck callbacks.
    //
    FxInitializeBugCheckDriverInfo();

    //
    // Init driver usage tracker. This tracker is used by the debug dump
    // callback routines for finding the driver's dump log file to write
    // in the minidump. Ignore any tracker's errors.
    //
    (VOID)FxLibraryGlobals.DriverTracker.Initialize();

    //
    // Initialize enhanced-verifier section handle
    //
    FxLibraryGlobals.VerifierSectionHandle = NULL;
    FxLibraryGlobals.VerifierSectionHandleRefCount = 0;

    //
    // Retrieve a pointer to the data structure that cotains trace routines
    // corresponding to WdfNotifyRoutinesClass from the SystemTraceProvider
    // that we'll use for perf tracing of WDF operations. The trace
    // routines inside the structuyre are present only when tracing is enabled
    // by some trace client (e.g. tracelog or xperf)  for WDF specific perf
    // groups. Note that no unregistration is necessary.
    //
    status = WmiQueryTraceInformation(WdfNotifyRoutinesClass,
                                      &FxLibraryGlobals.PerfTraceRoutines,
                                      sizeof(PWMI_WDF_NOTIFY_ROUTINES),
                                      NULL,
                                      NULL);

    if (!NT_SUCCESS(status)) {
        //
        // WDF trace routines are available only on win8+, so failure is
        // expected on pre-Win8 OS. Use the dummy routines on failure.
        //
        RtlZeroMemory(&FxLibraryGlobals.DummyPerfTraceRoutines,
                      sizeof(WMI_WDF_NOTIFY_ROUTINES));
        FxLibraryGlobals.DummyPerfTraceRoutines.Size =
            sizeof(WMI_WDF_NOTIFY_ROUTINES);
        FxLibraryGlobals.PerfTraceRoutines =
            &FxLibraryGlobals.DummyPerfTraceRoutines;
        status = STATUS_SUCCESS;
    }

    //
    // The Size member of WMI_WDF_NOTIFY_ROUTINES allows versioning. When
    // the WMI_WDF_NOTIFY_ROUTINES structure is revised with additional
    // members in future OS versions, the Size member will allow validating
    // the various versions, and initializeing the structure correctly.
    //
    ASSERT(FxLibraryGlobals.PerfTraceRoutines->Size >=
        sizeof(WMI_WDF_NOTIFY_ROUTINES));

#else
    status = STATUS_SUCCESS;
#endif

    return status;
}

VOID
FxLibraryGlobalsDecommission(
    VOID
    )
{
    //
    // Assure the all driver's FxDriverGlobals have been freed.
    //
    ASSERT(IsListEmpty(&FxLibraryGlobals.FxDriverGlobalsList));

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
    //
    // Cleanup for the driver usage tracker.
    //
    FxLibraryGlobals.DriverTracker.Uninitialize();

    //
    // Deregister from the global (library) bugcheck callbacks.
    //
    FxUninitializeBugCheckDriverInfo();
#endif

    FxLibraryGlobals.FxDriverGlobalsListLock.Uninitialize();

    return;
}

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
//
// This function is only used to lock down verifier section code in memory.
// It uses the #pragma alloc_text(...) style for paging.
//
VOID
VerifierPageLockHandle (
    VOID
    )
{
    PAGED_CODE_LOCKED();
    DO_NOTHING();
}

VOID
LockVerifierSection(
    _In_ PFX_DRIVER_GLOBALS FxDriverGlobals,
    _In_ PCUNICODE_STRING RegistryPath
    )
{
    LONG count;

    //
    // This asserts  makes sure the struct is not pack(1) and the counter
    // is correctly aligned on a 32 bit boundary.
    //
    C_ASSERT((FIELD_OFFSET(FxLibraryGlobalsType, VerifierSectionHandleRefCount)
              % __alignof(LONG)) == 0);

    count = InterlockedIncrement(&FxLibraryGlobals.VerifierSectionHandleRefCount);
    ASSERT(count > 0);

    //
    // If verifier section is unlocked, lock it in.
    //
    if(FxLibraryGlobals.VerifierSectionHandle == NULL) {
        //
        //First time verifier section is being locked.
        //
        DoTraceLevelMessage(FxDriverGlobals, TRACE_LEVEL_INFORMATION, TRACINGDRIVER,
                            "First time Locking (%d) in Verifier Paged Memory "
                            "from  %!wZ! from driver globals %p",
                            count, RegistryPath, FxDriverGlobals);
        //
        // VerifierLockHandle is a function that we use to lock in all the code from it's section
        // since all the verifier code is in the same section as VerifierLockHandle.
        //
        FxLibraryGlobals.VerifierSectionHandle = MmLockPagableCodeSection(VerifierPageLockHandle);
    }
    else {
        MmLockPagableSectionByHandle(FxLibraryGlobals.VerifierSectionHandle);
        DoTraceLevelMessage(FxDriverGlobals, TRACE_LEVEL_INFORMATION, TRACINGDRIVER,
                            "Increment Lock counter (%d) for Verifier Paged Memory "
                            "from  %!wZ! from driver globals %p",
                            count, RegistryPath, FxDriverGlobals);
    }
}

VOID
UnlockVerifierSection(
    _In_ PFX_DRIVER_GLOBALS FxDriverGlobals
    )
{
    if( FxLibraryGlobals.VerifierSectionHandle != NULL) {
        LONG count;

        count = InterlockedDecrement(&FxLibraryGlobals.VerifierSectionHandleRefCount);
        ASSERT(count >= 0);

        MmUnlockPagableImageSection(FxLibraryGlobals.VerifierSectionHandle);
        DoTraceLevelMessage(FxDriverGlobals, TRACE_LEVEL_INFORMATION, TRACINGDRIVER,
                            "Decrement UnLock counter (%d) for Verifier Paged Memory "
                            "with driver globals %p",
                            count, FxDriverGlobals);
    }
}
#endif

_Must_inspect_result_
NTSTATUS
FxInitialize(
    __inout     PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in        MdDriverObject DriverObject,
    __in        PCUNICODE_STRING RegistryPath,
    __in_opt    PWDF_DRIVER_CONFIG DriverConfig
    )

/*++

Routine Description:

    This is the global framework initialization routine.

    This is called when the framework is loaded, and by
    any drivers that use the framework.

    It is safe to call if already initialized, to handle
    cases where multiple drivers are sharing a common
    kernel DLL.

    This method is used instead of relying on C++ static class
    constructors in kernel mode.

Arguments:


Returns:

    NTSTATUS

--*/

{
    NTSTATUS status;
    BOOLEAN windowsVerifierOn = FALSE;

    UNREFERENCED_PARAMETER(DriverConfig);

    //
    // Check if windows driver verifier is on for this driver
    // We need this when initializing wdf verifier
    //
    windowsVerifierOn = IsWindowsVerifierOn(DriverObject);

    //
    // Get registry values first since these effect the
    // rest of initialization
    //
    FxRegistrySettingsInitialize(FxDriverGlobals,
                                 RegistryPath,
                                 windowsVerifierOn);

    //
    // Initialize IFR logging
    //
    FxIFRStart(FxDriverGlobals, RegistryPath, DriverObject);

    DoTraceLevelMessage(FxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDRIVER,
                        "Initialize globals for %!wZ!", RegistryPath);

    //
    // Only first one initializes the frameworks globals
    //
    status = FxPoolPackageInitialize(FxDriverGlobals);
    if (!NT_SUCCESS(status)) {
        //
        // FxPoolPackageInitialize logs a message in case of failure so
        // we don't need to log failure here.
        //
        FxIFRStop(FxDriverGlobals);
        return status;
    }

    //
    // Lock verifier package
    //
    FxVerifierLockInitialize(FxDriverGlobals);

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
    //
    // Cache driver info for bugcheck callback.
    //
    FxCacheBugCheckDriverInfo(FxDriverGlobals);

    //
    // Register for bugcheck callbacks.
    //
    FxRegisterBugCheckCallback(FxDriverGlobals, DriverObject);
#endif

    if  (NULL != RegistryPath) {
        if  (FALSE == FxDriverGlobals->IsCorrectVersionRegistered(RegistryPath))
            FxDriverGlobals->RegisterClientVersion(RegistryPath);
    }

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
    if(FxDriverGlobals->FxVerifierOn){
        LockVerifierSection(FxDriverGlobals, RegistryPath);
    }
#endif

    return STATUS_SUCCESS;
}

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
BOOLEAN
IsDriverVerifierActive(
    _In_ MdDriverObject DriverObject
    )
/*++

Routine Description:

    This function checks whether WDF verification is turned on or not.

Arguments:

    DriverObject - Driver to test if WDF verification turned on.

Returns:

    TRUE if WDF verification is turned on. False otherwise.

--*/
{
    BOOLEAN isWDFRuleClassTurnedOn = FALSE;

    //
    // This is defined in VRF_RULE_CLASS_ID for WDF verification.
    //

    const ULONG VrfWDFRuleClass = 33;

    if (MmIsDriverVerifying (DriverObject) > 0) {
        isWDFRuleClassTurnedOn = VfIsRuleClassEnabled (VrfWDFRuleClass);
    }

    return isWDFRuleClassTurnedOn;
}
#endif

BOOLEAN
IsWindowsVerifierOn(
    _In_ MdDriverObject DriverObject
    )
{
    BOOLEAN windowsVerifierOn = FALSE;


#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
    //
    // Check if windows driver verifier is on for this driver
    // We need this when initializing wdf verifier
    //
    windowsVerifierOn = IsDriverVerifierActive(DriverObject);

#else
    UNREFERENCED_PARAMETER(DriverObject);

    //
    // For user-mode we check if app verifier's verifier.dll is loaded in this
    // process (since app verifier doesn't provide any other way to detect its
    // presence).
    //
    windowsVerifierOn = (GetModuleHandleW(L"verifier.dll") == NULL ? FALSE : TRUE);
#endif

    return windowsVerifierOn;
}

VOID
FxDestroy(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals
    )

/*++

Routine Description:

    This is the global framework uninitialization routine.

    It is here for symmetry, and to allow a shared DLL based frameworks
    to unload safely.

Arguments:


Returns:

    NTSTATUS

--*/

{
    //
    // Release the last reference.
    //
    FxDriverGlobals->RELEASE(FxDestroy);

    //
    // Wait for everyone else to be done.
    //
    Mx::MxEnterCriticalRegion();
    FxDriverGlobals->DestroyEvent.WaitFor(Executive, KernelMode, FALSE, NULL);
    Mx::MxLeaveCriticalRegion();

    //
    // Lock verifier package
    //
    FxVerifierLockDestroy(FxDriverGlobals);

    //
    // Cleanup frameworks structures
    //
    FxPoolPackageDestroy(FxDriverGlobals);

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
    //
    // Deregister from the bugcheck callbacks.
    //
    FxUnregisterBugCheckCallback(FxDriverGlobals);

    //
    // Purge driver info from bugcheck data.
    //
    FxPurgeBugCheckDriverInfo(FxDriverGlobals);
#endif

#if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
    //
    // unlock verifier image sections
    //
    if(FxDriverGlobals->FxVerifierOn){
        UnlockVerifierSection(FxDriverGlobals);
    }
#endif

    return;
}

_Must_inspect_result_
PWDF_DRIVER_GLOBALS
FxAllocateDriverGlobals(
    VOID
    )
{
    PFX_DRIVER_GLOBALS  pFxDriverGlobals;
    KIRQL               irql;
    NTSTATUS            status;

    pFxDriverGlobals = (PFX_DRIVER_GLOBALS)
        MxMemory::MxAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(FX_DRIVER_GLOBALS), FX_TAG);

    if (pFxDriverGlobals == NULL) {
        return NULL;
    }

    pFxDriverGlobals->Refcnt = 1;

    status = pFxDriverGlobals->DestroyEvent.Initialize(NotificationEvent, FALSE);
#if (FX_CORE_MODE==FX_CORE_USER_MODE)
    if (!NT_SUCCESS(status)) {
        MxMemory::MxFreePool(pFxDriverGlobals);
        return NULL;
    }
#else
    UNREFERENCED_PARAMETER(status);
#endif

    //
    // Initialize this new FxDriverGlobals structure.
    //
    FxLibraryGlobals.FxDriverGlobalsListLock.Acquire(&irql);
    InsertHeadList(&FxLibraryGlobals.FxDriverGlobalsList,
                   &pFxDriverGlobals->Linkage);
    FxLibraryGlobals.FxDriverGlobalsListLock.Release(irql);

    pFxDriverGlobals->WdfHandleMask                  = FxHandleValueMask;
    pFxDriverGlobals->WdfVerifierAllocateFailCount   = (ULONG) -1;
    pFxDriverGlobals->Driver                         = NULL;
    pFxDriverGlobals->DebugExtension                 = NULL;
    pFxDriverGlobals->LibraryGlobals                 = &FxLibraryGlobals;
    pFxDriverGlobals->WdfLogHeader                   = NULL;

    //
    // Verifier settings.  Off by default.
    //
    pFxDriverGlobals->SetVerifierState(FALSE);

    //
    // By default don't apply latest-version restricted verifier checks
    // to downlevel version drivers.
    //
    pFxDriverGlobals->FxVerifyDownlevel              = FALSE;

    //
    // Verbose is separate knob
    //
    pFxDriverGlobals->FxVerboseOn                    = FALSE;

    //
    // Do not parent queue presented requests.
    // This performance optimization is on by default.
    //
    pFxDriverGlobals->FxRequestParentOptimizationOn  = TRUE;

    //
    // Enhanced verifier options. Off by default
    //
    pFxDriverGlobals->FxEnhancedVerifierOptions = 0;

    //
    // If FxVerifierDbgBreakOnError is true, WaitForSignal interrupts the
    // execution of the system after waiting for the specified number
    // of seconds. Developer will have an opportunity to validate the state
    // of the driver when breakpoint is hit. Developer can continue to wait
    // by entering 'g' in the debugger.
    //
    pFxDriverGlobals->FxVerifierDbgWaitForSignalTimeoutInSec = 60;

    //
    // Timeout used by the wake interrupt ISR in WaitForSignal to catch
    // scenarios where the interrupt ISR is blocked because the device stack
    // is taking too long to power up
    //
    pFxDriverGlobals->DbgWaitForWakeInterruptIsrTimeoutInSec = 60;

    //
    // Minidump log related settings.
    //
    pFxDriverGlobals->FxForceLogsInMiniDump          = FALSE;

#if (FX_CORE_MODE==FX_CORE_KERNEL_MODE)
    pFxDriverGlobals->FxTrackDriverForMiniDumpLog    = TRUE;
    pFxDriverGlobals->IsUserModeDriver               = FALSE;
#else
    pFxDriverGlobals->FxTrackDriverForMiniDumpLog    = FALSE;
    pFxDriverGlobals->IsUserModeDriver               = TRUE;
#endif

#if (FX_CORE_MODE==FX_CORE_KERNEL_MODE)
    //
    // Minidump driver info related settings.
    //
    pFxDriverGlobals->BugCheckDriverInfoIndex        = 0;
#endif

    //
    // By default disable the support for device simulation framework (DSF).
    //
    pFxDriverGlobals->FxDsfOn  = FALSE;

    pFxDriverGlobals->FxVerifyLeakDetection = NULL;
    pFxDriverGlobals->FxVerifyTagTrackingEnabled = FALSE;

    //
    // Allocate a telemetry context if a telemetry client is enabled, for any level/keyword.
    //
    pFxDriverGlobals->TelemetryContext = NULL;
    if (TraceLoggingProviderEnabled(g_TelemetryProvider, 0 ,0)) {
        AllocAndInitializeTelemetryContext(&(pFxDriverGlobals->TelemetryContext));
    }

    return &pFxDriverGlobals->Public;
}

VOID
FxFreeDriverGlobals(
    __in PWDF_DRIVER_GLOBALS DriverGlobals
    )
{
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    KIRQL irql;

    pFxDriverGlobals = GetFxDriverGlobals(DriverGlobals);

    FxLibraryGlobals.FxDriverGlobalsListLock.Acquire(&irql);
    RemoveEntryList(&pFxDriverGlobals->Linkage);
    InitializeListHead(&pFxDriverGlobals->Linkage);
    FxLibraryGlobals.FxDriverGlobalsListLock.Release(irql);

    if (pFxDriverGlobals->DebugExtension != NULL) {

        FxFreeAllocatedMdlsDebugInfo(pFxDriverGlobals->DebugExtension);

        if (pFxDriverGlobals->DebugExtension->ObjectDebugInfo != NULL) {
            MxMemory::MxFreePool(pFxDriverGlobals->DebugExtension->ObjectDebugInfo);
            pFxDriverGlobals->DebugExtension->ObjectDebugInfo = NULL;
        }

        pFxDriverGlobals->DebugExtension->AllocatedTagTrackersLock.Uninitialize();

        MxMemory::MxFreePool(pFxDriverGlobals->DebugExtension);
        pFxDriverGlobals->DebugExtension = NULL;
    }

    if (pFxDriverGlobals->FxVerifyLeakDetection != NULL) {
        MxMemory::MxFreePool(pFxDriverGlobals->FxVerifyLeakDetection);
    }

    //
    // Cleanup event b/c d'tor is not called for MxAllocatePoolWithTag.
    //
    pFxDriverGlobals->DestroyEvent.Uninitialize();

    if (NULL != pFxDriverGlobals->TelemetryContext) {
        MxMemory::MxFreePool(pFxDriverGlobals->TelemetryContext);
        pFxDriverGlobals->TelemetryContext = NULL;
    }

    MxMemory::MxFreePool(pFxDriverGlobals);
}

_Must_inspect_result_
FxObjectDebugInfo*
FxVerifierGetObjectDebugInfo(
    __in HANDLE Key,
    __in PFX_DRIVER_GLOBALS  FxDriverGlobals
    )

/*++

Routine Description:
    Attempts to open values under the passed in key and create an array of
    FxObjectDebugInfo.

Arguments:
    Key - Registry key to query the value for

Return Value:
    NULL or a pointer which should be freed by the caller using ExFreePool

--*/

{
    FxObjectDebugInfo *pInfo = NULL;
    NTSTATUS status;

    //
    // Read TrackHandles settings
    //
    FxVerifierReadObjectDebugInfo(Key,
                                  FxDriverGlobals,
                                  &pInfo,
                                  L"TrackHandles",
                                  FxObjectDebugTrackReferences,
                                  NULL);
    if (pInfo != NULL) {
        FxDriverGlobals->FxVerifyTagTrackingEnabled = TRUE;
    }

    //
    // To enable leak detection, a reg key must be present and not equal to 0xFFFFFFFF
    //
    DECLARE_CONST_UNICODE_STRING(valueName, L"ObjectLeakDetectionLimit");
    ULONG Limit;
    if (!NT_SUCCESS(FxRegKey::_QueryULong(Key, &valueName, &Limit))
        || Limit == FX_OBJECT_LEAK_DETECTION_DISABLED) {

        return pInfo;
    }

    FxDriverGlobals->FxVerifyLeakDetection = (FxObjectDebugLeakDetection*)
                            MxMemory::MxAllocatePool2(POOL_FLAG_NON_PAGED,
                                sizeof(FxObjectDebugLeakDetection),
                                FxDriverGlobals->Tag);
    if (FxDriverGlobals->FxVerifyLeakDetection == NULL) {
        return pInfo;
    }

    FxDriverGlobals->FxVerifyLeakDetection->Limit = Limit;
    FxDriverGlobals->FxVerifyLeakDetection->LimitScaled = Limit;
    FxDriverGlobals->FxVerifyLeakDetection->ObjectCnt = 0;
    FxDriverGlobals->FxVerifyLeakDetection->DeviceCnt = 0;
    FxDriverGlobals->FxVerifyLeakDetection->Enabled = TRUE;

    //
    // Read REG key for "object types to count"
    //
    status = FxVerifierReadObjectDebugInfo(Key,
                                           FxDriverGlobals,
                                           &pInfo,
                                           L"ObjectsForLeakDetection",
                                           FxObjectDebugTrackObjectCount,
                                           FX_OBJECT_LEAK_DETECTION_DEFAULT_TYPES);

    if (!NT_SUCCESS(status) || NULL == pInfo) {
        MxMemory::MxFreePool(FxDriverGlobals->FxVerifyLeakDetection);
        FxDriverGlobals->FxVerifyLeakDetection = NULL;
    }

    return pInfo;
}


NTSTATUS
FxVerifierReadObjectDebugInfo(
    _In_ HANDLE Key,
    _In_ PFX_DRIVER_GLOBALS  FxDriverGlobals,
    _Inout_  FxObjectDebugInfo **Info,
    _In_ PCWSTR KeyName,
    _In_ FxObjectDebugInfoFlags DebugFlag,
    _In_opt_ PCWSTR DefaultSettings
    )

/*++

Routine Description:
    Attempts to open a value under the passed in key. Will allocate an array of
    FxObjectDebugInfo if provided pointer is NULL.

Arguments:
    Key - Registry key to query the value for

    FxDriverGlobals - globals

    Info - optional preallocated debug info structure.

    KeyName - Key to read from

    DebugFlag - flag to set in the debug info structure.

    DefaultSettings - settings to use in the event the registery key is not
        present or malformed.

Return Value:
    NT_SUCCESS if the registry key was read and memory was allocated.

--*/

{
    PVOID dataBuffer = NULL;
    NTSTATUS status;
    ULONG length, type;
    UNICODE_STRING valueName;

    type = REG_MULTI_SZ;
    length = 0;
    RtlInitUnicodeString(&valueName, KeyName);

    //
    // Find out how big a buffer we need to allocate if the value is present
    //
    status = FxRegKey::_QueryValue(FxDriverGlobals,
                                   Key,
                                   &valueName,
                                   length,
                                   NULL,
                                   &length,
                                   &type);

    //
    // We expect the list to be bigger then a standard partial, so if it is
    // not, just bail now.
    //
    if (status != STATUS_BUFFER_OVERFLOW && status != STATUS_BUFFER_TOO_SMALL) {
        goto exit;
    }

    //
    // Pool can be paged b/c we are running at PASSIVE_LEVEL and we are going
    // to free it at the end of this function.
    //
    dataBuffer = MxMemory::MxAllocatePool2(POOL_FLAG_PAGED, length, FxDriverGlobals->Tag);
    if (dataBuffer == NULL) {
        status = STATUS_MEMORY_NOT_ALLOCATED;
        goto exit;
    }

    //
    // Requery now that we have a big enough buffer
    //
    status = FxRegKey::_QueryValue(FxDriverGlobals,
                                   Key,
                                   &valueName,
                                   length,
                                   dataBuffer,
                                   &length,
                                   &type);
    if (NT_SUCCESS(status)) {
        //
        // Verify that the data from the registry is a valid multi-sz string.
        //
        status = FxRegKey::_VerifyMultiSzString(FxDriverGlobals,
                                                &valueName,
                                                (PWCHAR) dataBuffer,
                                                length);
    }

    if (NT_SUCCESS(status)) {
#pragma prefast(push)
#pragma prefast(suppress:__WARNING_PRECONDITION_NULLTERMINATION_VIOLATION, "FxRegKey::_VerifyMultiSzString makes sure the string is NULL-terminated")
        status = FxVerifyAllocateDebugInfo(Info, (LPCWSTR) dataBuffer, FxDriverGlobals, DebugFlag);
#pragma prefast(pop)

    }

exit:
    if (NULL != dataBuffer) {
        MxMemory::MxFreePool(dataBuffer);
    }

    if (!NT_SUCCESS(status) && DefaultSettings != NULL){
        status = FxVerifyAllocateDebugInfo(Info, DefaultSettings, FxDriverGlobals, DebugFlag);
    }

    return status;
}

VOID
FxVerifierQueryTrackPower(
    __in HANDLE Key,
    __out FxTrackPowerOption* TrackPower
    )
{
    NTSTATUS status;
    ULONG value = 0;
    DECLARE_CONST_UNICODE_STRING(valueName, L"TrackPower");

    status = FxRegKey::_QueryULong(Key, &valueName, &value);
    if (NT_SUCCESS(status) && value < FxTrackPowerMaxValue) {
        *TrackPower = (FxTrackPowerOption)value;
    }
    else {
        *TrackPower = FxTrackPowerNone;
    }
}






















VOID
FxOverrideDefaultVerifierSettings(
    __in    HANDLE Key,
    __in    LPWSTR Name,
    _Inout_ PBOOLEAN OverrideValue
    )
{
    UNICODE_STRING valueName;
    ULONG value = 0;

    RtlInitUnicodeString(&valueName, Name);

    if (NT_SUCCESS(FxRegKey::_QueryULong(Key,
                                         (PCUNICODE_STRING)&valueName,
                                         &value))) {
        if (value) {
            *OverrideValue = TRUE;
        } else {
            *OverrideValue = FALSE;
        }
    }

}


VOID
FxRegistrySettingsInitialize(
    __inout PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in PCUNICODE_STRING RegistryPath,
    __in BOOLEAN WindowsVerifierOn
    )

/*++

Routine Description:

    Initialize Driver Framework settings from the driver
    specific registry settings under

    (KMDF)
    \REGISTRY\MACHINE\SYSTEM\ControlSetxxx\Services\<driver>\Parameters\Wdf

    (UMDF)
    \REGISTRY\MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\WUDF\Services\<driver>

Arguments:

    RegistryPath - Registry path passed to DriverEntry

--*/

{
    NTSTATUS status;
    RTL_QUERY_REGISTRY_TABLE paramTable[10];
    ULONG verifierOnValue;
    ULONG verifyDownlevelValue;
    ULONG verboseValue;
    ULONG allocateFailValue;
    ULONG forceLogsInMiniDump;
    ULONG trackDriverForMiniDumpLog;
    ULONG requestParentOptimizationOn;
    ULONG dsfValue;
    ULONG removeLockOptionFlags;
    ULONG zero = 0;
    ULONG max = 0xFFFFFFFF;
    ULONG defaultTrue = (ULONG) TRUE;
    ULONG i;
    ULONG timeoutValue = 0;
    FxAutoRegKey hDriver, hWdf;

#if (FX_CORE_MODE==FX_CORE_KERNEL_MODE)
    DECLARE_CONST_UNICODE_STRING(parametersPath, L"Wdf");
    UNREFERENCED_PARAMETER(RegistryPath);

    status = OpenDriverParamsKeyForRead(FxDriverGlobals, &hDriver.m_Key);

#else
    DECLARE_CONST_UNICODE_STRING(parametersPath, L"Parameters\\Wdf");

    //
    // UMDF may not provide this registry path
    //
    if (NULL == RegistryPath) {
        return;
    }

    status = FxRegKey::_OpenKey(NULL, RegistryPath, &hDriver.m_Key, KEY_READ);
#endif

    if (!NT_SUCCESS(status)) {
        return;
    }

    status = FxRegKey::_OpenKey(hDriver.m_Key, &parametersPath, &hWdf.m_Key, KEY_READ);
    if (!NT_SUCCESS(status)) {
        //
        // For version >= 1.9 we enable WDF verifier automatically when driver
        // verifier or app verifier is enabled. Since inbox drivers may not have
        // WDF subkey populated as they may not use INF, we need to enable
        // verifier even if we fail to open wdf subkey (if DriverVerifier is on).
        //
        if (FxDriverGlobals->IsVersionGreaterThanOrEqualTo(1,9)) {
            //
            // Verifier settings are all or nothing.  We currently do not support
            // turning on individual sub-verifiers.
            //
            FxDriverGlobals->SetVerifierState(WindowsVerifierOn);
            if (FxDriverGlobals->FxVerifierOn) {
                FxDriverGlobalsInitializeDebugExtension(FxDriverGlobals, NULL);
            }
        }

        return;
    }

    RtlZeroMemory (&paramTable[0], sizeof(paramTable));
    i = 0;

    #define ADD_TABLE_ENTRY(ValueName, Value, Default) \
        paramTable[i].Flags = \
            RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_TYPECHECK; \
        paramTable[i].Name = L##ValueName; \
        paramTable[i].EntryContext = &Value; \
        paramTable[i].DefaultType = \
            (REG_DWORD << RTL_QUERY_REGISTRY_TYPECHECK_SHIFT) | REG_NONE; \
        paramTable[i].DefaultData = &Default; \
        paramTable[i].DefaultLength = sizeof(ULONG); \
        i++; \
        ASSERT(i < sizeof(paramTable) / sizeof(paramTable[0]));

    verboseValue = 0;
    ADD_TABLE_ENTRY("VerboseOn", verboseValue, zero);

    allocateFailValue = (ULONG) -1;
    ADD_TABLE_ENTRY("VerifierAllocateFailCount", allocateFailValue, max);

    verifierOnValue = 0;

    //
    // If the client version is 1.9 or above, the default (i.e when
    // the key is not present) VerifierOn state is tied to the
    // driver verifier.
    //
    if (FxDriverGlobals->IsVersionGreaterThanOrEqualTo(1,9)) {
        verifierOnValue = WindowsVerifierOn;
    }

    ADD_TABLE_ENTRY("VerifierOn", verifierOnValue, verifierOnValue);

    verifyDownlevelValue = 0;
    ADD_TABLE_ENTRY("VerifyDownLevel", verifyDownlevelValue, zero);

    forceLogsInMiniDump = 0;
    ADD_TABLE_ENTRY("ForceLogsInMiniDump", forceLogsInMiniDump, zero);

    //
    // Track driver for minidump log:
    //  Default for KMDF is on.
    //  Default for UMDF is off.
    //
#if (FX_CORE_MODE==FX_CORE_KERNEL_MODE)
    trackDriverForMiniDumpLog = (ULONG) TRUE;
    ADD_TABLE_ENTRY("TrackDriverForMiniDumpLog", trackDriverForMiniDumpLog, defaultTrue);
#else
    trackDriverForMiniDumpLog = 0;
    ADD_TABLE_ENTRY("TrackDriverForMiniDumpLog", trackDriverForMiniDumpLog, zero);
#endif

    requestParentOptimizationOn = (ULONG) TRUE;
    ADD_TABLE_ENTRY("RequestParentOptimizationOn", requestParentOptimizationOn, defaultTrue);

    dsfValue = 0;
    ADD_TABLE_ENTRY("DsfOn", dsfValue, zero);

    removeLockOptionFlags = 0;
    ADD_TABLE_ENTRY("RemoveLockOptionFlags", removeLockOptionFlags, zero);

    //
    // The last entry's QueryRoutine and Name fields must be NULL,
    // because that marks the end of the query table.
    //
    ASSERT(i < sizeof(paramTable) / sizeof(paramTable[0]));
    ASSERT(paramTable[i].QueryRoutine == NULL);
    ASSERT(paramTable[i].Name == NULL);

    status = RtlQueryRegistryValuesEx(
        RTL_REGISTRY_OPTIONAL | RTL_REGISTRY_HANDLE,
        (PWSTR) hWdf.m_Key,
        &paramTable[0],
        NULL,
        NULL
        );

    //
    // Only examine key values on success
    //
    if (NT_SUCCESS(status)) {

        if (verboseValue) {
            FxDriverGlobals->FxVerboseOn = TRUE;
        }
        else {
            FxDriverGlobals->FxVerboseOn = FALSE;
        }

        if (allocateFailValue != (ULONG) -1) {
            FxDriverGlobals->WdfVerifierAllocateFailCount = (LONG) allocateFailValue;
        }
        else {
            FxDriverGlobals->WdfVerifierAllocateFailCount = -1;
        }

        //
        // Verifier settings are all or nothing.  We currently do not support
        // turning on individual sub-verifiers.
        //
        FxDriverGlobals->SetVerifierState(verifierOnValue ? TRUE : FALSE);

        if (FxDriverGlobals->FxVerifierOn) {
            FxDriverGlobalsInitializeDebugExtension(FxDriverGlobals, hWdf.m_Key);
        }

        //
        // Update FxVerifyDownLevel independent of FxVerifyOn because for UMDF
        // verifer is always on so it does not consume FxVerifyOn value
        //
        if (verifyDownlevelValue) {
            FxDriverGlobals->FxVerifyDownlevel = TRUE;
        }
        else {
            FxDriverGlobals->FxVerifyDownlevel = FALSE;
        }

        //
        // See if there exists an override in the registry for WDFVERIFY state.
        // We query for this separately so that we can establish a default state
        // based on verifierOnValue, and then know if the value was present in
        // the registry to override the default.
        //
        FxOverrideDefaultVerifierSettings(hWdf.m_Key,
                                          L"VerifyOn",
                                          &FxDriverGlobals->FxVerifyOn);

        if (FxDriverGlobals->FxVerifyOn) {
            FxDriverGlobals->Public.DriverFlags |= WdfVerifyOn;
        }

        FxOverrideDefaultVerifierSettings(hWdf.m_Key,
                                          L"DbgBreakOnError",
                                          &FxDriverGlobals->FxVerifierDbgBreakOnError);

        FxOverrideDefaultVerifierSettings(hWdf.m_Key,
                                          L"DbgBreakOnDeviceStateError",
                                          &FxDriverGlobals->FxVerifierDbgBreakOnDeviceStateError);

        if (FxDriverGlobals->FxVerifierDbgBreakOnError) {
            timeoutValue = 0;
            DECLARE_CONST_UNICODE_STRING(timeoutName, L"DbgWaitForSignalTimeoutInSec");

            //
            // Get the override value for the WaitForSignal's timeout if present.
            //
            if (NT_SUCCESS(FxRegKey::_QueryULong(hWdf.m_Key,
                                                 &timeoutName,
                                                 &timeoutValue))) {

                FxDriverGlobals->FxVerifierDbgWaitForSignalTimeoutInSec = timeoutValue;
            }
        }

        timeoutValue = 0;
        DECLARE_CONST_UNICODE_STRING(timeoutName, L"DbgWaitForWakeInterruptIsrTimeoutInSec");

        //
        // Get the override value for the Wake Interrupt ISR timeout if present.
        // Since the wake interrupt feature is only supported for 1.13 and higher,
        // avoid querying the reg key for older versions
        //
        if (FxDriverGlobals->IsVersionGreaterThanOrEqualTo(1,13) &&
            NT_SUCCESS(FxRegKey::_QueryULong(hWdf.m_Key,
                                             &timeoutName,
                                             &timeoutValue))) {

            FxDriverGlobals->DbgWaitForWakeInterruptIsrTimeoutInSec = timeoutValue;
        }

        FxDriverGlobals->FxForceLogsInMiniDump =
                            (forceLogsInMiniDump) ? TRUE : FALSE;

        FxDriverGlobals->FxTrackDriverForMiniDumpLog =
                            (trackDriverForMiniDumpLog) ? TRUE : FALSE;

        FxDriverGlobals->FxRequestParentOptimizationOn =
                            (requestParentOptimizationOn) ? TRUE : FALSE;

        FxDriverGlobals->FxDsfOn = (dsfValue) ? TRUE : FALSE;

        FxDriverGlobals->RemoveLockOptionFlags = removeLockOptionFlags;
    }

    return;
}

VOID
FX_DRIVER_GLOBALS::WaitForSignal(
    __in MxEvent* Event,
    __in PCSTR ReasonForWaiting,
    __in WDFOBJECT Handle,
    __in ULONG WarningTimeoutInSec,
    __in ULONG WaitSignalFlags
    )
{
    LARGE_INTEGER timeOut;
    NTSTATUS status;

    ASSERT(Mx::MxGetCurrentIrql() == PASSIVE_LEVEL);

    timeOut.QuadPart = WDF_REL_TIMEOUT_IN_SEC(((ULONGLONG)WarningTimeoutInSec));

    do {
        status = Event->WaitFor(Executive,
                        KernelMode,
                        FALSE, // Non alertable
                        timeOut.QuadPart ? &timeOut : NULL);

        if(status == STATUS_TIMEOUT) {
            DbgPrint("Thread 0x%p is %s 0x%p\n",
		      Mx::MxGetCurrentThread(),
                      ReasonForWaiting,
                      Handle);

            if ((WaitSignalFlags & WaitSignalAlwaysBreak) ||
                ((WaitSignalFlags & WaitSignalBreakUnderVerifier) &&
                 FxVerifierDbgBreakOnError) ||
                ((WaitSignalFlags & WaitSignalBreakUnderDebugger) &&
                 IsDebuggerAttached())) {

                DbgBreakPoint();
            }
        } else {
            ASSERT(NT_SUCCESS(status));
            break;
        }
    } WHILE(TRUE);
}

_Must_inspect_result_
BOOLEAN
FX_DRIVER_GLOBALS::IsVersionGreaterThanOrEqualTo(
    _In_ ULONG Major,
    _In_ ULONG Minor
    )
{
    if ((WdfBindInfo->Version.Major > Major) ||
                (WdfBindInfo->Version.Major == Major &&
                  WdfBindInfo->Version.Minor >= Minor)) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

_Must_inspect_result_
BOOLEAN
FX_DRIVER_GLOBALS::IsMinorVersionGreaterThanOrEqualTo(
    _In_ ULONG Minor
    )
{
#if (FX_CORE_MODE==FX_CORE_KERNEL_MODE)
    ASSERT(WdfBindInfo->Version.Major == 1);
#else
    ASSERT(WdfBindInfo->Version.Major == 2);

    //
    // This is needed for e.g. KMDF checking version >= 1.11
    //
    // UMDF 2.0 = KMDF 1.13. Thus any check below 13 is always satisfied.
    // Starting with minor version = 15, KMDF and UMDF are updated together.
    //
    if (Minor <= 13) {
        return TRUE;
    }
#endif

    return (WdfBindInfo->Version.Minor >= Minor);
}

} // extern "C"

