/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxIrpKm.hpp

Abstract:

    This module implements km definitions for FxIrp functions.

Author:



Environment:

    Kernel mode only

Revision History:

--*/

//
// All the functions in this file should use FORCEINLINE so that KMDF gets
// inlining. FxIrp.hpp does not use FORCEINLINE on the functions because it
// doesn't work for UMDF (see comments in FxIrp.hpp).
//

#ifndef _FXIRPKM_HPP_
#define _FXIRPKM_HPP_

typedef PIRP MdIrp;

typedef DRIVER_CANCEL MdCancelRoutineType, *MdCancelRoutine;
typedef IO_COMPLETION_ROUTINE MdCompletionRoutineType, *MdCompletionRoutine;
typedef REQUEST_POWER_COMPLETE MdRequestPowerCompleteType, *MdRequestPowerComplete;

typedef
NTSTATUS
(*PFX_COMPLETION_ROUTINE)(
    __in FxDevice *Device,
    __in FxIrp *Irp,
    __in PVOID Context
    );

typedef
VOID
(*PFX_CANCEL_ROUTINE)(
    __in FxDevice *Device,
    __in FxIrp *Irp,
    __in PVOID CancelContext
    );

#include "FxIrp.hpp"



FORCEINLINE
MdIrp
FxIrp::GetIrp(
    VOID
    )
{
    return m_Irp;
}

FORCEINLINE
VOID
FxIrp::CompleteRequest(
    __in CCHAR PriorityBoost
    )
{
    IoCompleteRequest(m_Irp, PriorityBoost);
    m_Irp = NULL;
}

FORCEINLINE
NTSTATUS
FxIrp::CallDriver(
    __in MdDeviceObject DeviceObject
    )
{
    return IoCallDriver(DeviceObject, m_Irp);
}

FORCEINLINE
NTSTATUS
FxIrp::PoCallDriver(
    __in MdDeviceObject DeviceObject
    )
{
    return ::PoCallDriver(DeviceObject, m_Irp);
}

FORCEINLINE
VOID
FxIrp::StartNextPowerIrp(
    )
{
    PoStartNextPowerIrp(m_Irp);
}

FORCEINLINE
MdCompletionRoutine
FxIrp::GetNextCompletionRoutine(
    VOID
    )
{
    return this->GetNextIrpStackLocation()->CompletionRoutine;
}


FORCEINLINE
VOID
FxIrp::SetCompletionRoutine(
    __in MdCompletionRoutine CompletionRoutine,
    __in PVOID Context,
    __in BOOLEAN InvokeOnSuccess,
    __in BOOLEAN InvokeOnError,
    __in BOOLEAN InvokeOnCancel
    )
{
    IoSetCompletionRoutine(
        m_Irp,
        CompletionRoutine,
        Context,
        InvokeOnSuccess,
        InvokeOnError,
        InvokeOnCancel
        );
}

FORCEINLINE
VOID
FxIrp::SetCompletionRoutineEx(
    __in MdDeviceObject DeviceObject,
    __in MdCompletionRoutine CompletionRoutine,
    __in PVOID Context,
    __in BOOLEAN InvokeOnSuccess,
    __in BOOLEAN InvokeOnError,
    __in BOOLEAN InvokeOnCancel
    )
{
    if (!NT_SUCCESS(IoSetCompletionRoutineEx(
            DeviceObject,
            m_Irp,
            CompletionRoutine,
            Context,
            InvokeOnSuccess,
            InvokeOnError,
            InvokeOnCancel))) {

        IoSetCompletionRoutine(
            m_Irp,
            CompletionRoutine,
            Context,
            InvokeOnSuccess,
            InvokeOnError,
            InvokeOnCancel
            );
    }
}

FORCEINLINE
MdCancelRoutine 
FxIrp::SetCancelRoutine(
    __in_opt MdCancelRoutine  CancelRoutine
    )
{
    return IoSetCancelRoutine(m_Irp, CancelRoutine);
}

FORCEINLINE
NTSTATUS
FxIrp::_IrpSynchronousCompletion(
    __in MdDeviceObject DeviceObject,
    __in PIRP OriginalIrp,
    __in PVOID Context
    )
{
    FxCREvent* event = (FxCREvent*) Context;

    UNREFERENCED_PARAMETER(DeviceObject);

    if (OriginalIrp->PendingReturned) {
        //
        // No need to propagate the pending returned bit since we are handling
        // the request synchronously
        //
        event->Set();
    }

    return STATUS_MORE_PROCESSING_REQUIRED;
}

FORCEINLINE
NTSTATUS
FxIrp::SendIrpSynchronously(
    __in MdDeviceObject DeviceObject
    )
{
    NTSTATUS status;
    FxCREvent event;

    SetCompletionRoutine(_IrpSynchronousCompletion,
                         event.GetSelfPointer(),
                         TRUE,
                         TRUE,
                         TRUE);

    status = CallDriver(DeviceObject);

    if (status == STATUS_PENDING) {
        event.EnterCRAndWaitAndLeave();
        status = m_Irp->IoStatus.Status;
    }

    return status;
}

FORCEINLINE
VOID
FxIrp::CopyToNextIrpStackLocation(
    __in PIO_STACK_LOCATION Stack
    )
{
  PIO_STACK_LOCATION nextIrpSp = IoGetNextIrpStackLocation(m_Irp);
    
  RtlCopyMemory(nextIrpSp,
          Stack,
          FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine)
          );
  nextIrpSp->Control = 0;
}


FORCEINLINE
VOID
FxIrp::CopyCurrentIrpStackLocationToNext(
    VOID
    )
{
    IoCopyCurrentIrpStackLocationToNext(m_Irp);
}

FORCEINLINE
VOID
FxIrp::SetNextIrpStackLocation(
    VOID
    )
{
    IoSetNextIrpStackLocation(m_Irp);    
}

FORCEINLINE
UCHAR
FxIrp::GetMajorFunction(
    VOID
    )
{
    return IoGetCurrentIrpStackLocation(m_Irp)->MajorFunction;
}

FORCEINLINE
UCHAR
FxIrp::GetMinorFunction(
    VOID
    )
{
    return IoGetCurrentIrpStackLocation(m_Irp)->MinorFunction;
}

FORCEINLINE
UCHAR
FxIrp::GetCurrentStackFlags(
    VOID
    )
{
    return IoGetCurrentIrpStackLocation(m_Irp)->Flags;
}

FORCEINLINE
MdFileObject
FxIrp::GetCurrentStackFileObject(
    VOID
    )
{
    return IoGetCurrentIrpStackLocation(m_Irp)->FileObject;
}

FORCEINLINE
KPROCESSOR_MODE
FxIrp::GetRequestorMode(
    VOID
    )
{
    return m_Irp->RequestorMode;
}

FORCEINLINE
VOID
FxIrp::SetContext(
    __in ULONG Index,
    __in PVOID Value
    )
{
    m_Irp->Tail.Overlay.DriverContext[Index] = Value;
}

FORCEINLINE
PVOID
FxIrp::GetContext(
    __in ULONG Index
    )
{
    return m_Irp->Tail.Overlay.DriverContext[Index];
}

FORCEINLINE
VOID
FxIrp::SetFlags(
    __in ULONG Flags
    )
{
    m_Irp->Flags = Flags;
}

FORCEINLINE
ULONG
FxIrp::GetFlags(
    VOID
    )
{
    return m_Irp->Flags;
}

FORCEINLINE
PIO_STACK_LOCATION
FxIrp::GetCurrentIrpStackLocation(
    VOID
    )
{
    return IoGetCurrentIrpStackLocation(m_Irp);
}

FORCEINLINE
PIO_STACK_LOCATION
FxIrp::GetNextIrpStackLocation(
    VOID
    )
{
    return IoGetNextIrpStackLocation(m_Irp);
}

PIO_STACK_LOCATION
FORCEINLINE
FxIrp::_GetAndClearNextStackLocation(
    __in MdIrp Irp
    )
{
    RtlZeroMemory(IoGetNextIrpStackLocation(Irp),
                  FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine));
    return IoGetNextIrpStackLocation(Irp);
}

FORCEINLINE
VOID
FxIrp::SkipCurrentIrpStackLocation(
    VOID
    )
{






    IoSkipCurrentIrpStackLocation(m_Irp);
}

FORCEINLINE
VOID
FxIrp::MarkIrpPending(
    )
{
    IoMarkIrpPending(m_Irp);
}

FORCEINLINE
BOOLEAN
FxIrp::PendingReturned(
    )
{
    return m_Irp->PendingReturned;
}

FORCEINLINE
VOID
FxIrp::PropagatePendingReturned(
    VOID
    )
{
    if (PendingReturned() && m_Irp->CurrentLocation <= m_Irp->StackCount) {
        MarkIrpPending();
    }
}

FORCEINLINE
VOID
FxIrp::SetStatus(
    __in NTSTATUS Status
    )
{
    m_Irp->IoStatus.Status = Status;
}

FORCEINLINE
NTSTATUS
FxIrp::GetStatus(
    )
{
    return m_Irp->IoStatus.Status;
}

FORCEINLINE
BOOLEAN
FxIrp::Cancel(
    VOID
    )
{
    return IoCancelIrp(m_Irp);
}

FORCEINLINE
VOID
FxIrp::SetCancel(
    __in BOOLEAN Cancel
    )
{
    m_Irp->Cancel = Cancel;
}

FORCEINLINE
BOOLEAN
FxIrp::IsCanceled(
    )
{
    return m_Irp->Cancel ? TRUE : FALSE;
}

FORCEINLINE
KIRQL
FxIrp::GetCancelIrql(
    )
{
    return m_Irp->CancelIrql;
}

FORCEINLINE
VOID
FxIrp::SetInformation(
    ULONG_PTR Information
    )
{
    m_Irp->IoStatus.Information = Information;
}

FORCEINLINE
ULONG_PTR
FxIrp::GetInformation(
    )
{
    return m_Irp->IoStatus.Information;
}

FORCEINLINE
CCHAR
FxIrp::GetCurrentIrpStackLocationIndex(
    )
{
    return m_Irp->CurrentLocation;
}

FORCEINLINE
CCHAR
FxIrp::GetStackCount(
    )
{
    return m_Irp->StackCount;
}

FORCEINLINE
PLIST_ENTRY
FxIrp::ListEntry(
    )
{
    return &m_Irp->Tail.Overlay.ListEntry;
}

FORCEINLINE
PVOID
FxIrp::GetSystemBuffer(
    )
{
    return m_Irp->AssociatedIrp.SystemBuffer;
}

FORCEINLINE
PVOID
FxIrp::GetOutputBuffer(
    )
{
    //
    // In kernel mode, for buffered I/O, the output and input buffers are 
    // at same location.
    //
    return GetSystemBuffer();
}

FORCEINLINE
VOID
FxIrp::SetSystemBuffer(
    __in PVOID Value
    )
{
    m_Irp->AssociatedIrp.SystemBuffer = Value;
}


FORCEINLINE
PMDL
FxIrp::GetMdl(
    )
{
    return m_Irp->MdlAddress;
}

FORCEINLINE
PMDL*
FxIrp::GetMdlAddressPointer(
    )
{
    return &m_Irp->MdlAddress;
}

FORCEINLINE
VOID
FxIrp::SetMdlAddress(
    __in PMDL Value
    )
{
    m_Irp->MdlAddress = Value;
}


FORCEINLINE
PVOID
FxIrp::GetUserBuffer(
    )
{
    return m_Irp->UserBuffer;
}


FORCEINLINE
VOID
FxIrp::SetUserBuffer(
    __in PVOID Value
    )
{
    m_Irp->UserBuffer = Value;
}
 
FORCEINLINE
VOID
FxIrp::Reuse(
    __in NTSTATUS Status
    )
{
    IoReuseIrp(m_Irp, Status);
}

FORCEINLINE
VOID
FxIrp::SetMajorFunction(
    __in UCHAR MajorFunction
    )
{
    this->GetNextIrpStackLocation()->MajorFunction = MajorFunction;
}

FORCEINLINE
VOID
FxIrp::SetMinorFunction(
    __in UCHAR MinorFunction
    )
{
    this->GetNextIrpStackLocation()->MinorFunction = MinorFunction;
}

FORCEINLINE
SYSTEM_POWER_STATE_CONTEXT
FxIrp::GetParameterPowerSystemPowerStateContext(
    )
{
    return (this->GetCurrentIrpStackLocation())->
        Parameters.Power.SystemPowerStateContext;
}

FORCEINLINE
POWER_STATE_TYPE
FxIrp::GetParameterPowerType(
    )
{
    return (this->GetCurrentIrpStackLocation())->Parameters.Power.Type;
}

FORCEINLINE
POWER_STATE
FxIrp::GetParameterPowerState(
    )
{
    return (this->GetCurrentIrpStackLocation())->Parameters.Power.State;
}

FORCEINLINE
DEVICE_POWER_STATE
FxIrp::GetParameterPowerStateDeviceState(
    )
{
    return (this->GetCurrentIrpStackLocation())->
        Parameters.Power.State.DeviceState;
}

FORCEINLINE
SYSTEM_POWER_STATE
FxIrp::GetParameterPowerStateSystemState(
    )
{
    return (this->GetCurrentIrpStackLocation())->
        Parameters.Power.State.SystemState;
}

FORCEINLINE
POWER_ACTION
FxIrp::GetParameterPowerShutdownType(
    )
{
    return (this->GetCurrentIrpStackLocation())->
        Parameters.Power.ShutdownType;
}

FORCEINLINE
DEVICE_RELATION_TYPE
FxIrp::GetParameterQDRType(
    )
{
    return (this->GetCurrentIrpStackLocation())->
        Parameters.QueryDeviceRelations.Type;
}

FORCEINLINE
VOID
FxIrp::SetParameterQDRType(
    DEVICE_RELATION_TYPE DeviceRelation
	)
{
     this->GetNextIrpStackLocation()->
        Parameters.QueryDeviceRelations.Type = DeviceRelation;
}

FORCEINLINE
PDEVICE_CAPABILITIES
FxIrp::GetParameterDeviceCapabilities(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.DeviceCapabilities.Capabilities;
}

FORCEINLINE
MdDeviceObject
FxIrp::GetDeviceObject(
    VOID
    )
{
  return this->GetCurrentIrpStackLocation()->DeviceObject;
}

FORCEINLINE
VOID
FxIrp::SetCurrentDeviceObject(
    __in MdDeviceObject DeviceObject
    )
{
  this->GetCurrentIrpStackLocation()->DeviceObject = DeviceObject;
}

FORCEINLINE
VOID
FxIrp::SetParameterDeviceCapabilities(
    __in PDEVICE_CAPABILITIES DeviceCapabilities
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.DeviceCapabilities.Capabilities = DeviceCapabilities;
}

FORCEINLINE
LONGLONG
FxIrp::GetParameterWriteByteOffsetQuadPart(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.Write.ByteOffset.QuadPart;
}

FORCEINLINE
VOID
FxIrp::SetNextParameterWriteByteOffsetQuadPart(
    __in LONGLONG DeviceOffset
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.Write.ByteOffset.QuadPart = DeviceOffset;
}

FORCEINLINE
VOID
FxIrp::SetNextParameterWriteLength(
    __in ULONG IoLength
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.Write.Length = IoLength;
}

FORCEINLINE
PVOID*
FxIrp::GetNextStackParameterOthersArgument1Pointer(
    )
{
    PIO_STACK_LOCATION nextStack;

    nextStack = this->GetNextIrpStackLocation();

    return &nextStack->Parameters.Others.Argument1;
}

FORCEINLINE
VOID
FxIrp::SetNextStackParameterOthersArgument1(
    __in PVOID Argument1
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.Others.Argument1 = Argument1;
}

FORCEINLINE
PVOID*
FxIrp::GetNextStackParameterOthersArgument2Pointer(
    )
{
    PIO_STACK_LOCATION nextStack;

    nextStack = this->GetNextIrpStackLocation();

    return &nextStack->Parameters.Others.Argument2;
}

FORCEINLINE
PVOID*
FxIrp::GetNextStackParameterOthersArgument4Pointer(
    )
{
    PIO_STACK_LOCATION nextStack;

    nextStack = this->GetNextIrpStackLocation();

    return &nextStack->Parameters.Others.Argument4;
}

FORCEINLINE
PCM_RESOURCE_LIST
FxIrp::GetParameterAllocatedResources(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.StartDevice.AllocatedResources;
}

FORCEINLINE
VOID
FxIrp::SetParameterAllocatedResources(
    __in PCM_RESOURCE_LIST AllocatedResources
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.StartDevice.AllocatedResources = AllocatedResources;
}

FORCEINLINE
PCM_RESOURCE_LIST
FxIrp::GetParameterAllocatedResourcesTranslated(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.StartDevice.AllocatedResourcesTranslated;
}

FORCEINLINE
VOID
FxIrp::SetParameterAllocatedResourcesTranslated(
    __in PCM_RESOURCE_LIST AllocatedResourcesTranslated
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.StartDevice.AllocatedResourcesTranslated = 
            AllocatedResourcesTranslated;
}

FORCEINLINE
LCID
FxIrp::GetParameterQueryDeviceTextLocaleId(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.QueryDeviceText.LocaleId;
}

FORCEINLINE
DEVICE_TEXT_TYPE
FxIrp::GetParameterQueryDeviceTextType(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.QueryDeviceText.DeviceTextType;
}

FORCEINLINE
BOOLEAN
FxIrp::GetParameterSetLockLock(
    )
{
    return this->GetCurrentIrpStackLocation()->Parameters.SetLock.Lock;
}

FORCEINLINE
BUS_QUERY_ID_TYPE
FxIrp::GetParameterQueryIdType(
    )
{
    return this->GetCurrentIrpStackLocation()->Parameters.QueryId.IdType;
}

FORCEINLINE
PINTERFACE
FxIrp::GetParameterQueryInterfaceInterface(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.QueryInterface.Interface;
}

FORCEINLINE
const GUID*
FxIrp::GetParameterQueryInterfaceType(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.QueryInterface.InterfaceType;
}

FORCEINLINE
MdFileObject
FxIrp::GetFileObject(
    VOID
    )
{
    return this->GetCurrentIrpStackLocation()->FileObject;
}

FORCEINLINE
USHORT
FxIrp::GetParameterQueryInterfaceVersion(
    )
{
    return this->GetCurrentIrpStackLocation()->Parameters.QueryInterface.Version;
}

FORCEINLINE
USHORT
FxIrp::GetParameterQueryInterfaceSize(
    )
{
    return this->GetCurrentIrpStackLocation()->Parameters.QueryInterface.Size;
}

FORCEINLINE
PVOID
FxIrp::GetParameterQueryInterfaceInterfaceSpecificData(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.QueryInterface.InterfaceSpecificData;
}

FORCEINLINE
DEVICE_USAGE_NOTIFICATION_TYPE
FxIrp::GetParameterUsageNotificationType(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.UsageNotification.Type;
}

FORCEINLINE
BOOLEAN
FxIrp::GetParameterUsageNotificationInPath(
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.UsageNotification.InPath;
}

FORCEINLINE
VOID
FxIrp::SetParameterUsageNotificationInPath(
    __in BOOLEAN InPath
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.UsageNotification.InPath = InPath;
}

FORCEINLINE
BOOLEAN 
FxIrp::GetNextStackParameterUsageNotificationInPath(
    )
{
    return this->GetNextIrpStackLocation()->
        Parameters.UsageNotification.InPath;
}



FORCEINLINE
ULONG
FxIrp::GetParameterIoctlCode(
    VOID
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.DeviceIoControl.IoControlCode;
}

FORCEINLINE
ULONG
FxIrp::GetParameterIoctlCodeBufferMethod(
    VOID
    )
{
    return METHOD_FROM_CTL_CODE(GetParameterIoctlCode());
}

FORCEINLINE
ULONG
FxIrp::GetParameterIoctlOutputBufferLength(
    VOID
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.DeviceIoControl.OutputBufferLength;
}

FORCEINLINE
ULONG
FxIrp::GetParameterIoctlInputBufferLength(
    VOID
    )
{
    return this->GetCurrentIrpStackLocation()->
        Parameters.DeviceIoControl.InputBufferLength;
}

FORCEINLINE
VOID
FxIrp::SetParameterIoctlCode(
    __in ULONG DeviceIoControlCode
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.DeviceIoControl.IoControlCode = DeviceIoControlCode;
}

FORCEINLINE
VOID
FxIrp::SetParameterIoctlInputBufferLength(
    __in ULONG InputBufferLength
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.DeviceIoControl.InputBufferLength = InputBufferLength;
}

FORCEINLINE
VOID
FxIrp::SetParameterIoctlOutputBufferLength(
    __in ULONG OutputBufferLength
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.DeviceIoControl.OutputBufferLength = OutputBufferLength;
}

FORCEINLINE
VOID
FxIrp::SetParameterIoctlType3InputBuffer(
    __in PVOID Type3InputBuffer
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.DeviceIoControl.Type3InputBuffer = Type3InputBuffer;
}

FORCEINLINE
PVOID
FxIrp::GetParameterIoctlType3InputBuffer(
    VOID
    )
{
    return this->GetCurrentIrpStackLocation()->
               Parameters.DeviceIoControl.Type3InputBuffer;
}

FORCEINLINE
VOID
FxIrp::SetParameterQueryInterfaceInterface(
    __in PINTERFACE Interface
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.QueryInterface.Interface = Interface;
}

FORCEINLINE
VOID
FxIrp::SetParameterQueryInterfaceType(
    __in const GUID* InterfaceType
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.QueryInterface.InterfaceType = InterfaceType;
}

FORCEINLINE
VOID
FxIrp::SetParameterQueryInterfaceVersion(
    __in USHORT Version
    )
{
    this->GetNextIrpStackLocation()->Parameters.QueryInterface.Version = Version;
}

FORCEINLINE
VOID
FxIrp::SetParameterQueryInterfaceSize(
    __in USHORT Size
    )
{
    this->GetNextIrpStackLocation()->Parameters.QueryInterface.Size = Size;
}

FORCEINLINE
VOID
FxIrp::SetParameterQueryInterfaceInterfaceSpecificData(
    __in PVOID InterfaceSpecificData
    )
{
    this->GetNextIrpStackLocation()->
        Parameters.QueryInterface.InterfaceSpecificData = InterfaceSpecificData;
}

FORCEINLINE
VOID
FxIrp::SetNextStackFlags(
    __in UCHAR Flags
    )
{
    this->GetNextIrpStackLocation()->Flags = Flags;
}

FORCEINLINE
VOID
FxIrp::SetNextStackFileObject(
    _In_ MdFileObject FileObject
    )
{
    this->GetNextIrpStackLocation()->FileObject = FileObject;
}


FORCEINLINE
VOID
FxIrp::ClearNextStack(
    VOID
    )
{
    PIO_STACK_LOCATION stack;

    stack = this->GetNextIrpStackLocation();
    RtlZeroMemory(stack, sizeof(IO_STACK_LOCATION));
}




FORCEINLINE
VOID
FxIrp::ClearNextStackLocation(
    VOID
    )
{
    RtlZeroMemory(this->GetNextIrpStackLocation(),
                  FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine));
}

FORCEINLINE
VOID
FxIrp::InitNextStackUsingStack(
    __in FxIrp* Irp
    )
{
    PIO_STACK_LOCATION srcStack, destStack;

    srcStack = Irp->GetCurrentIrpStackLocation();
    destStack = this->GetNextIrpStackLocation();

    *destStack = *srcStack;
}

_Must_inspect_result_
FORCEINLINE
MdIrp
FxIrp::AllocateIrp(
    _In_ CCHAR StackSize,
    _In_opt_ FxDevice* Device
    )
{
    UNREFERENCED_PARAMETER(Device);
    
    return IoAllocateIrp(StackSize, FALSE);
}

FORCEINLINE
MdIrp
FxIrp::GetIrpFromListEntry(
    __in PLIST_ENTRY Ple
    )
{
    return CONTAINING_RECORD(Ple, IRP, Tail.Overlay.ListEntry);
}

FORCEINLINE
ULONG
FxIrp::GetParameterReadLength(
    VOID
    )
{
  return this->GetCurrentIrpStackLocation()->Parameters.Read.Length;
}

FORCEINLINE
ULONG
FxIrp::GetParameterWriteLength(
    VOID
    )
{
  return this->GetCurrentIrpStackLocation()->Parameters.Write.Length;
}

_Must_inspect_result_
FORCEINLINE
NTSTATUS
FxIrp::RequestPowerIrp(
    __in MdDeviceObject  DeviceObject,
    __in UCHAR  MinorFunction,
    __in POWER_STATE  PowerState,
    __in MdRequestPowerComplete  CompletionFunction,
    __in PVOID  Context
    )
{
    //
    // Prefast enforces that NULL is passed for IRP parameter (last parameter)  
    // since the IRP might complete before the function returns.
    //
    return PoRequestPowerIrp(
        DeviceObject,
        MinorFunction,
        PowerState,
        CompletionFunction,
        Context,
        NULL); 
}
  
FORCEINLINE
ULONG
FxIrp::GetCurrentFlags(
    VOID
    )
{
    return (this->GetCurrentIrpStackLocation())->Flags;
}

FORCEINLINE
PVOID
FxIrp::GetCurrentParametersPointer(
    VOID
    )
{
    return &(this->GetCurrentIrpStackLocation())->Parameters;
}

FORCEINLINE
MdEThread
FxIrp::GetThread(
    VOID
    )
{
    return  m_Irp->Tail.Overlay.Thread;
}

FORCEINLINE
BOOLEAN
FxIrp::Is32bitProcess(
    VOID
    )
{

#if defined(_WIN64)

#if BUILD_WOW64_ENABLED

    return IoIs32bitProcess(m_Irp);

#else // BUILD_WOW64_ENABLED

    return FALSE;

#endif // BUILD_WOW64_ENABLED

#else // defined(_WIN64)

    return TRUE;

#endif // defined(_WIN64)

}

FORCEINLINE
VOID
FxIrp::FreeIrp(
    VOID
    )
{
    IoFreeIrp(m_Irp);
}

FORCEINLINE
PIO_STATUS_BLOCK
FxIrp::GetStatusBlock(
    VOID
    )
{
    return &m_Irp->IoStatus;
}

FORCEINLINE
PVOID
FxIrp::GetDriverContext(
    VOID
    )
{
    return m_Irp->Tail.Overlay.DriverContext;
}

FORCEINLINE
ULONG
FxIrp::GetDriverContextSize(
    VOID
    )
{
    return sizeof(m_Irp->Tail.Overlay.DriverContext);
}

FORCEINLINE
VOID
FxIrp::CopyParameters(
    _Out_ PWDF_REQUEST_PARAMETERS Parameters
    )
{
    RtlMoveMemory(&Parameters->Parameters,
                  GetCurrentParametersPointer(),
                  sizeof(Parameters->Parameters));
}

FORCEINLINE
VOID
FxIrp::CopyStatus(
    _Out_ PIO_STATUS_BLOCK StatusBlock
    )
{
    RtlCopyMemory(StatusBlock,
                  GetStatusBlock(),
                  sizeof(*StatusBlock));
}

FORCEINLINE
BOOLEAN
FxIrp::HasStack(
    _In_ UCHAR StackCount
    )
{
    return (GetCurrentIrpStackLocationIndex() >= StackCount);
}

FORCEINLINE
BOOLEAN
FxIrp::IsCurrentIrpStackLocationValid(
    VOID
    )
{
    return (GetCurrentIrpStackLocationIndex() <= GetStackCount());
}

FORCEINLINE
FxAutoIrp::~FxAutoIrp()
{
    if (m_Irp != NULL) {
        IoFreeIrp(m_Irp);
    }
}

#endif // _FXIRPKM_HPP
