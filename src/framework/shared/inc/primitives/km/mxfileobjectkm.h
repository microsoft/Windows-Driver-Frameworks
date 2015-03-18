/*++

Copyright (c) Microsoft Corporation

ModuleName:

    MxFileObjectKm.h

Abstract:

    Kernel Mode implementation of File Object defined in MxFileObject.h

--*/

#pragma once

#include "MxFileObject.h"

FORCEINLINE
PUNICODE_STRING
MxFileObject::GetFileName(
    _Inout_opt_ PUNICODE_STRING Filename
   )
{
    UNREFERENCED_PARAMETER(Filename);
    
    return  &m_FileObject->FileName;
}

FORCEINLINE
PLARGE_INTEGER
MxFileObject::GetCurrentByteOffset(
   VOID
   )
{
   return &m_FileObject->CurrentByteOffset;
}

FORCEINLINE
ULONG
MxFileObject::GetFlags(
   VOID
   )
{
   return m_FileObject->Flags;
}

FORCEINLINE
VOID
MxFileObject::SetFsContext(
    _In_ PVOID Value
    )
{
    m_FileObject->FsContext = Value;
}

FORCEINLINE
VOID
MxFileObject::SetFsContext2(
    _In_ PVOID Value
    )
{
    m_FileObject->FsContext2 = Value;
}

FORCEINLINE
PVOID
MxFileObject::GetFsContext(
    VOID
    )
{
    return m_FileObject->FsContext;
}

FORCEINLINE
PVOID
MxFileObject::GetFsContext2(
    VOID
    )
{
    return m_FileObject->FsContext2;
}
