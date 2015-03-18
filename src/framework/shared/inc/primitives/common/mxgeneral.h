/*++

Copyright (c) Microsoft Corporation

ModuleName:

    MxGeneral.h

Abstract:

    Mode agnostic definitions for general OS
    functions used in framework code

    See MxGeneralKm.h and MxGeneralUm.h for mode
    specific implementations

Author:



Revision History:



--*/

#pragma once

//
// Placeholder macro for a no-op
//
#define DO_NOTHING()                            (0)

//
// We need to make these static functions of the class
// to force common definition to apply to um and km versions
//
// If we don't do this, um and km definitions can diverge
//
class Mx
{
public:

    //
    // IsUM/IsKM don't change at runtime
    // but defining them as functions makes it more convenient to check
    // for UM/KM as compared to using ifdef's in certain places
    //
    // Since they are forceinlined and return a constant value,
    // optimized code is no different than using an ifdef
    //
    // See FxPoolAllocator in WdfPool.cpp for example of such usage
    //
    FORCEINLINE
    static
    BOOLEAN
    IsUM(
        );

    FORCEINLINE
    static
    BOOLEAN
    IsKM(
        );

    FORCEINLINE
    static
    MxThread
    MxGetCurrentThread(
        );

    FORCEINLINE
    static
    MdEThread
    GetCurrentEThread(
        );

    NTSTATUS
    static
    MxTerminateCurrentThread(
        __in NTSTATUS Status
        );

    FORCEINLINE
    static
    KIRQL
    MxGetCurrentIrql(
        );

    __drv_maxIRQL(HIGH_LEVEL)
    __drv_raisesIRQL(NewIrql)
    FORCEINLINE
    static
    VOID
    MxRaiseIrql(
        __in KIRQL                              NewIrql,
        __out __deref __drv_savesIRQL PKIRQL    OldIrql
        );

    __drv_maxIRQL(HIGH_LEVEL)
    FORCEINLINE
    static
    VOID
    MxLowerIrql(
        __in __drv_restoresIRQL __drv_nonConstant  KIRQL  NewIrql
        );

    FORCEINLINE
    static
    VOID
    MxQueryTickCount(
        __out PLARGE_INTEGER  TickCount
        );

    FORCEINLINE
    static
    ULONG 
    MxQueryTimeIncrement(
        );








    static
    VOID
    MxBugCheckEx(
        __in ULONG  BugCheckCode,
        __in ULONG_PTR  BugCheckParameter1,
        __in ULONG_PTR  BugCheckParameter2,
        __in ULONG_PTR  BugCheckParameter3,
        __in ULONG_PTR  BugCheckParameter4
    );

    FORCEINLINE
    static
    VOID
    MxDbgBreakPoint(
        );

    static
    VOID
    MxDbgPrint(
        __drv_formatString(printf)
        __in PCSTR DebugMessage,
        ...
        );

    FORCEINLINE
    static
    VOID
    MxAssert(
        __in BOOLEAN Condition
        );

    FORCEINLINE
    static
    VOID
    MxAssertMsg(
        __in LPSTR Message,
        __in BOOLEAN Condition
        );

    _Acquires_lock_(_Global_critical_region_)
    FORCEINLINE
    static
    VOID
    MxEnterCriticalRegion(
        );

    _Releases_lock_(_Global_critical_region_) //implies _Requires_lock_held_(_Global_critical_region_)
    FORCEINLINE
    static
    VOID
    MxLeaveCriticalRegion(
        );

    FORCEINLINE
    static
    VOID
    MxDelayExecutionThread(
        __in KPROCESSOR_MODE  WaitMode,
        __in BOOLEAN  Alertable,
        __in PLARGE_INTEGER  Interval
        );

    //
    // Mode agnostic function to get address of a system function
    // Should be used only for Rtl* functions applicable both to
    // kernel mode and user mode
    //
    // User mode version is assumed to reside in ntdll.dll
    //
    // The argument type is MxFuncName so that it can be defined
    // as LPCWSTR in kernel mode and LPCSTR in user mode
    // which is what MmGetSystemRoutineAddress and GetProcAddress
    // expect respectively
    //
    FORCEINLINE
    static
    PVOID
    MxGetSystemRoutineAddress(
        __in MxFuncName FuncName
        );

    FORCEINLINE
    static
    VOID
    MxReferenceObject(
        __in PVOID Object
        );

    FORCEINLINE
    static
    VOID
    MxDereferenceObject(
        __in PVOID Object
        );

    FORCEINLINE
    static
    VOID
    MxInitializeRemoveLock(
        __in MdRemoveLock  Lock,
        __in ULONG  AllocateTag,
        __in ULONG  MaxLockedMinutes,
        __in ULONG  HighWatermark
        );

    FORCEINLINE
    static
    NTSTATUS
    MxAcquireRemoveLock(
        __in MdRemoveLock  RemoveLock,
        __in_opt PVOID  Tag 
        );

    FORCEINLINE
    static
    VOID
    MxReleaseRemoveLock(
        __in MdRemoveLock  RemoveLock,
        __in PVOID  Tag 
        );

    FORCEINLINE
    static
    VOID
    MxReleaseRemoveLockAndWait(
        __in MdRemoveLock  RemoveLock,
        __in PVOID  Tag 
        );

    FORCEINLINE
    static
    BOOLEAN
    MxHasEnoughRemainingThreadStack(
        VOID
        );

    
    _Releases_lock_(_Global_cancel_spin_lock_) //implies _Requires_lock_held_(_Global_cancel_spin_lock_)
    __drv_requiresIRQL(DISPATCH_LEVEL)
    FORCEINLINE
    static
    VOID
    ReleaseCancelSpinLock(
        __in __drv_restoresIRQL __drv_useCancelIRQL KIRQL  Irql
        );

    FORCEINLINE
    static
    NTSTATUS
    CreateCallback(
        __out PCALLBACK_OBJECT  *CallbackObject,
        __in POBJECT_ATTRIBUTES  ObjectAttributes,
        __in BOOLEAN  Create,
        __in BOOLEAN  AllowMultipleCallbacks
        );

    FORCEINLINE
    static
    PVOID
    RegisterCallback(
        __in PCALLBACK_OBJECT  CallbackObject,
        __in MdCallbackFunction  CallbackFunction,
        __in PVOID  CallbackContext
        );

    FORCEINLINE
    static
    VOID
    UnregisterCallback(
        __in PVOID  CbRegistration
        );

    static
    VOID
    MxGlobalInit(
        VOID
        );

    FORCEINLINE
    static
    VOID
    MxUnlockPages(
        __in PMDL Mdl
        );

    FORCEINLINE
    static
    PVOID
    MxGetSystemAddressForMdlSafe(
        __inout PMDL Mdl,
        __in    ULONG Priority
        );

    FORCEINLINE
    static
    VOID
    MxBuildMdlForNonPagedPool(
        __inout PMDL Mdl
        );

    FORCEINLINE
    static
    PVOID
    MxGetDriverObjectExtension(
        __in MdDriverObject DriverObject,
        __in PVOID ClientIdentificationAddress
        );
    
    FORCEINLINE
    static
    NTSTATUS
    MxAllocateDriverObjectExtension(
        _In_  MdDriverObject DriverObject,
        _In_  PVOID ClientIdentificationAddress,
        _In_  ULONG DriverObjectExtensionSize,
        // When successful, this always allocates already-aliased memory.
        _Post_ _At_(*DriverObjectExtension, _When_(return==0,
        __drv_aliasesMem __drv_allocatesMem(Mem) _Post_notnull_))
        _When_(return == 0, _Outptr_result_bytebuffer_(DriverObjectExtensionSize))
        PVOID *DriverObjectExtension
        );

    FORCEINLINE
    static
    MdDeviceObject
    MxGetAttachedDeviceReference(
        __in MdDeviceObject DriverObject
        );

    FORCEINLINE
    static
    VOID
    MxDeleteSymbolicLink(
        __in PUNICODE_STRING Link
        );

    FORCEINLINE
    static
    VOID
    MxDeleteNPagedLookasideList(
        _In_ PNPAGED_LOOKASIDE_LIST LookasideList
        );

    FORCEINLINE
    static
    VOID
    MxDeletePagedLookasideList(
        _In_ PPAGED_LOOKASIDE_LIST LookasideList
        );

    FORCEINLINE
    static
    VOID
    MxInitializeNPagedLookasideList(
        _Out_     PNPAGED_LOOKASIDE_LIST Lookaside,
        _In_opt_  PALLOCATE_FUNCTION Allocate,
        _In_opt_  PFREE_FUNCTION Free,
        _In_      ULONG Flags,
        _In_      SIZE_T Size,
        _In_      ULONG Tag,
        _In_      USHORT Depth
        );

    FORCEINLINE
    static
    VOID
    MxInitializePagedLookasideList(
        _Out_     PPAGED_LOOKASIDE_LIST Lookaside,
        _In_opt_  PALLOCATE_FUNCTION Allocate,
        _In_opt_  PFREE_FUNCTION Free,
        _In_      ULONG Flags,
        _In_      SIZE_T Size,
        _In_      ULONG Tag,
        _In_      USHORT Depth
        );

    FORCEINLINE
    static
    VOID
    MxDeleteDevice(
        _In_ MdDeviceObject Device
        );

    static
    VOID
    MxDetachDevice(
        _Inout_ MdDeviceObject Device
        );

    FORCEINLINE
    static
    MdDeviceObject
    MxAttachDeviceToDeviceStack(
        _In_ MdDeviceObject SourceDevice,
        _In_ MdDeviceObject TargetDevice
        );

    FORCEINLINE 
    static
    NTSTATUS
    MxCreateDeviceSecure(
        _In_      MdDriverObject DriverObject,
        _In_      ULONG DeviceExtensionSize,
        _In_opt_  PUNICODE_STRING DeviceName,
        _In_      DEVICE_TYPE DeviceType,
        _In_      ULONG DeviceCharacteristics,
        _In_      BOOLEAN Exclusive,
        _In_      PCUNICODE_STRING DefaultSDDLString,
        _In_opt_  LPCGUID DeviceClassGuid,
        _Out_     MdDeviceObject *DeviceObject
        );

    FORCEINLINE
    static
    NTSTATUS 
    MxCreateDevice(
        _In_      MdDriverObject DriverObject,
        _In_      ULONG DeviceExtensionSize,
        _In_opt_  PUNICODE_STRING DeviceName,
        _In_      DEVICE_TYPE DeviceType,
        _In_      ULONG DeviceCharacteristics,
        _In_      BOOLEAN Exclusive,
        _Out_     MdDeviceObject *DeviceObject
    );

    FORCEINLINE
    static
    NTSTATUS
    MxCreateSymbolicLink(
        _In_ PUNICODE_STRING SymbolicLinkName,
        _In_ PUNICODE_STRING DeviceName
        );

    FORCEINLINE
    static
    VOID
    MxFlushQueuedDpcs(
        );

    FORCEINLINE
    static
    NTSTATUS
    MxOpenKey(
        _Out_ PHANDLE KeyHandle,
        _In_ ACCESS_MASK DesiredAccess,
        _In_ POBJECT_ATTRIBUTES ObjectAttributes
        );

    FORCEINLINE
    static
    NTSTATUS
    MxSetDeviceInterfaceState(
        _In_ PUNICODE_STRING SymbolicLinkName,
        _In_ BOOLEAN Enable
        );

    FORCEINLINE
    static
    NTSTATUS
    MxRegisterDeviceInterface(
        _In_      PDEVICE_OBJECT PhysicalDeviceObject,
        _In_      const GUID *InterfaceClassGuid,
        _In_opt_  PUNICODE_STRING ReferenceString,
        _Out_     PUNICODE_STRING SymbolicLinkName
        );

    FORCEINLINE
    static
    NTSTATUS
    MxDeleteKey(
        _In_ HANDLE KeyHandle
        );

    FORCEINLINE
    static
    VOID 
    MxInitializeMdl(
        _In_  PMDL MemoryDescriptorList,
        _In_  PVOID BaseVa,
        _In_  SIZE_T Length
        );

    FORCEINLINE
    static
    PVOID
    MxGetMdlVirtualAddress(
        _In_ PMDL Mdl
        );

    FORCEINLINE
    static    
    VOID 
    MxBuildPartialMdl(
        _In_     PMDL SourceMdl,
        _Inout_  PMDL TargetMdl,
        _In_     PVOID VirtualAddress,
        _In_     ULONG Length
    );

    FORCEINLINE
    static    
    VOID 
    MxQuerySystemTime(
        _Out_ PLARGE_INTEGER CurrentTime
        );

    FORCEINLINE
    static    
    NTSTATUS 
    MxSetValueKey(
        _In_      HANDLE KeyHandle,
        _In_      PUNICODE_STRING ValueName,
        _In_opt_  ULONG TitleIndex,
        _In_      ULONG Type,
        _In_opt_  PVOID Data,
        _In_      ULONG DataSize
        );

    FORCEINLINE
    static    
    NTSTATUS 
    MxQueryValueKey(
        _In_       HANDLE KeyHandle,
        _In_       PUNICODE_STRING ValueName,
        _In_       KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
        _Out_opt_  PVOID KeyValueInformation,
        _In_       ULONG Length,
        _Out_      PULONG ResultLength
    );
    FORCEINLINE
    static
    NTSTATUS
    MxReferenceObjectByHandle(
        __in HANDLE Handle,
        __in ACCESS_MASK DesiredAccess,
        __in_opt POBJECT_TYPE ObjectType,
        __in KPROCESSOR_MODE AccessMode,
        __out  PVOID *Object,
        __out_opt POBJECT_HANDLE_INFORMATION HandleInformation
        );

    FORCEINLINE
    static
    NTSTATUS
    MxUnRegisterPlugPlayNotification(
        __in __drv_freesMem(Pool) PVOID NotificationEntry
        );

    FORCEINLINE
    static
    NTSTATUS
    MxClose (
        __in HANDLE Handle
        );
    
    FORCEINLINE
    static
    KIRQL
    MxAcquireInterruptSpinLock(
        _Inout_ PKINTERRUPT Interrupt
        );

    FORCEINLINE
    static
    VOID
    MxReleaseInterruptSpinLock(
        _Inout_ PKINTERRUPT Interrupt,
        _In_ KIRQL OldIrql
        );
    
    FORCEINLINE
    static
    BOOLEAN 
    MxInsertQueueDpc(
      __inout   PRKDPC Dpc,
      __in_opt  PVOID SystemArgument1,
      __in_opt  PVOID SystemArgument2
    );
};
