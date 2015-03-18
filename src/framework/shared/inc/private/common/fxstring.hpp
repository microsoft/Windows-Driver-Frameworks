/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxString.hpp

Abstract:

    This module implements a simple string class to operate on
    unicode strings.

Author:




Environment:

    Both kernel and user mode

Revision History:


--*/
#ifndef _FXSTRING_H_
#define _FXSTRING_H_

class FxString : public FxObject {
public:
    //
    // Length describes the length of the string in bytes (not WCHARs)
    // MaximumLength describes the size of the buffer in bytes
    //
    UNICODE_STRING m_UnicodeString;

public:
    FxString(
        __in PFX_DRIVER_GLOBALS FxDriverGlobals
        );

    ~FxString();

    VOID
    FORCEINLINE
    ReleaseString(
        __out PUNICODE_STRING ReleaseTo
        )
    {
        RtlCopyMemory(ReleaseTo, &m_UnicodeString, sizeof(UNICODE_STRING));
        RtlZeroMemory(&m_UnicodeString, sizeof(m_UnicodeString));
    }

    FORCEINLINE
    operator PUNICODE_STRING(
        )
    {
        return &m_UnicodeString;
    }

    PUNICODE_STRING
    FORCEINLINE
    GetUnicodeString(
        VOID
        )
    {
        return &m_UnicodeString;
    }

    _Must_inspect_result_
    NTSTATUS
    Assign(
        __in PCWSTR SourceString
        );

    _Must_inspect_result_
    NTSTATUS
    Assign(
        __in const UNICODE_STRING* UnicodeString
        );

    FORCEINLINE
    USHORT
    Length(
        VOID
        )
    {
        return m_UnicodeString.Length;
    }

    FORCEINLINE
    USHORT
    ByteLength(
        __in BOOLEAN IncludeNull
        )
    {
        if (IncludeNull) {
            return m_UnicodeString.Length + sizeof(UNICODE_NULL);
        }
        else {
            return m_UnicodeString.Length;
        }
    }

    FORCEINLINE
    USHORT
    CharacterLength(
        VOID
        )
    {
        return m_UnicodeString.Length / sizeof(WCHAR);
    }

    FORCEINLINE
    USHORT
    MaximumLength(
        VOID
        )
    {
        return m_UnicodeString.MaximumLength;
    }

    FORCEINLINE
    USHORT
    MaximumByteLength(
        VOID
        )
    {
        return m_UnicodeString.MaximumLength;
    }

    FORCEINLINE
    USHORT
    MaximumCharacterLength(
        VOID
        )
    {
        return m_UnicodeString.MaximumLength / sizeof(WCHAR);
    }

    FORCEINLINE
    PWCHAR
    Buffer(
        VOID
        )
    {
        return m_UnicodeString.Buffer;
    }
};

#endif // _FXSTRING_H_
