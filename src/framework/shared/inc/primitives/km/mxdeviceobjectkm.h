/*++

Copyright (c) Microsoft Corporation

ModuleName:

    MxDeviceObjectKm.h

Abstract:

    Kernel Mode implementation of Device Object defined in MxDeviceObject.h

--*/

#pragma once

#include "MxDeviceObject.h"

FORCEINLINE
CCHAR
MxDeviceObject::GetStackSize(
    VOID
    )
{
    return m_DeviceObject->StackSize;
}

FORCEINLINE
VOID
MxDeviceObject::SetStackSize(
    _In_ CCHAR Size
    )
{
    m_DeviceObject->StackSize = Size;
}

FORCEINLINE
VOID
MxDeviceObject::ReferenceObject(
    )
{
    ObReferenceObject(m_DeviceObject);
}

FORCEINLINE
MdDeviceObject
MxDeviceObject::GetAttachedDeviceReference(
    VOID
    )
{
    return IoGetAttachedDeviceReference(m_DeviceObject);
}

FORCEINLINE
VOID
MxDeviceObject::DereferenceObject(
    )
{
    ObDereferenceObject(m_DeviceObject);
}

FORCEINLINE
ULONG
MxDeviceObject::GetFlags(
    VOID
    )
{
#pragma warning(disable:28129)
    return m_DeviceObject->Flags;
}

FORCEINLINE
VOID
MxDeviceObject::SetFlags(
    ULONG Flags
    )
{
#pragma warning(disable:28129)
    m_DeviceObject->Flags = Flags;
}

FORCEINLINE
POWER_STATE
MxDeviceObject::SetPowerState(
    __in POWER_STATE_TYPE  Type,
    __in POWER_STATE  State
    )
{
    return PoSetPowerState(m_DeviceObject, Type, State);
}

FORCEINLINE
VOID
MxDeviceObject::InvalidateDeviceRelations(
    __in DEVICE_RELATION_TYPE Type
    )
{
    IoInvalidateDeviceRelations(m_DeviceObject, Type);
}

FORCEINLINE
VOID
MxDeviceObject::InvalidateDeviceState(
    __in MdDeviceObject Fdo
    )
{
    //
    // UMDF currently needs Fdo for InvalidateDeviceState
    // FDO is not used in km. 
    //
    // m_DeviceObject holds PDO that is what is used below.
    //
    
    UNREFERENCED_PARAMETER(Fdo);
    
    IoInvalidateDeviceState(m_DeviceObject);
}

FORCEINLINE
PVOID
MxDeviceObject::GetDeviceExtension(
    VOID
    )
{
    return m_DeviceObject->DeviceExtension;
}

FORCEINLINE
VOID
MxDeviceObject::SetDeviceExtension(
    PVOID Value
    )
{
    m_DeviceObject->DeviceExtension = Value;
}

FORCEINLINE
DEVICE_TYPE
MxDeviceObject::GetDeviceType(
    VOID
    )
{
    return m_DeviceObject->DeviceType;
}

FORCEINLINE
ULONG
MxDeviceObject::GetCharacteristics(
    VOID
    )
{
    return m_DeviceObject->Characteristics;
}

FORCEINLINE
VOID
MxDeviceObject::SetDeviceType(
    DEVICE_TYPE Value
    )
{
    m_DeviceObject->DeviceType = Value;
}

FORCEINLINE
VOID
MxDeviceObject::SetCharacteristics(
    ULONG Characteristics
    )
{
    m_DeviceObject->Characteristics = Characteristics;
}

FORCEINLINE
VOID
MxDeviceObject::SetAlignmentRequirement(
    _In_ ULONG Value
    )
{
    m_DeviceObject->AlignmentRequirement = Value;
}

FORCEINLINE
ULONG
MxDeviceObject::GetAlignmentRequirement(
    VOID
    )
{
    return m_DeviceObject->AlignmentRequirement;
}
