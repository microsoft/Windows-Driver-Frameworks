/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxVerifierBugcheck.cpp

Abstract:

    This file contains definitions of verifier bugcheck functions
    These definitions are split from tracing.cpp in kmdf\src\core

Author:




Environment:

    Both kernel and user mode

Revision History:


--*/

#include "fxobjectpch.hpp"

// We use DoTraceMessage
extern "C" {
#if defined(EVENT_TRACING)
#include "FxVerifierBugcheck.tmh"
#endif
}

//=============================================================================
//
//=============================================================================


VOID
__declspec(noreturn)
FxVerifierBugCheckWorker(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in WDF_BUGCHECK_CODES WdfBugCheckCode,
    __in_opt ULONG_PTR BugCheckParameter2,
    __in_opt ULONG_PTR BugCheckParameter3
    )
/*++

Routine Description:
    Wrapper for system BugCheck.

    Note this functions is marked "__declspec(noreturn)"

Arguments:

Returns:

--*/
{
    //
    // Indicate to the BugCheck callback filter which IFR to dump.
    //
    FxDriverGlobals->FxForceLogsInMiniDump = TRUE;

    Mx::MxBugCheckEx(WDF_VIOLATION,
                 WdfBugCheckCode,
                 BugCheckParameter2,
                 BugCheckParameter3,
                 (ULONG_PTR) FxDriverGlobals );
}

VOID
__declspec(noreturn)
FxVerifierNullBugCheck(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in PVOID ReturnAddress
    )
/*++

Routine Description:

    Calls KeBugCheckEx indicating a WDF DDI was passed a NULL parameter.

    Note this functions is marked "__declspec(noreturn)"

Arguments:

Returns:

--*/
{
    
    DoTraceLevelMessage( FxDriverGlobals, TRACE_LEVEL_FATAL, TRACINGERROR,
                         "NULL Required Parameter Passed to a DDI\n"
                         "FxDriverGlobals 0x%p",
                         FxDriverGlobals
                         );

    FxVerifierBugCheck(FxDriverGlobals,
                       WDF_REQUIRED_PARAMETER_IS_NULL,  // Bugcheck code.
                       0,                               // Parameter 2
                       (ULONG_PTR)ReturnAddress         // Parameter 3
                       );
}

VOID
__declspec(noreturn)
FxVerifierDriverReportedBugcheck(
    _In_ PFX_DRIVER_GLOBALS FxDriverGlobals,
    _In_ ULONG      BugCheckCode,
    _In_ ULONG_PTR  BugCheckParameter1,
    _In_ ULONG_PTR  BugCheckParameter2,
    _In_ ULONG_PTR  BugCheckParameter3,
    _In_ ULONG_PTR  BugCheckParameter4
    )
/*++

Routine Description:

    For KMDF this will bugcheck the system while for UMDF a watson report 
    is filed that will blame the driver for the host failure.

Arguments:
    FxDriverGlobals - Unreferenced for KMDF. For UMDF a watson report is filed
                blaming the driver associated with the globals

    BugCheckCode - Specifies a value that indicates the reason for the bug check.

    BugCheckParameter1 - Additional information pertaining to the bugcheck.

    BugCheckParameter2 - Additional information pertaining to the bugcheck.

    BugCheckParameter3 - Additional information pertaining to the bugcheck.

    BugCheckParameter4 - Additional information pertaining to the bugcheck.

Returns: VOID

--*/
{
#if (FX_CORE_MODE == FX_CORE_USER_MODE)
    FX_VERIFY_WITH_NAME(DRIVER(BadAction, BugCheckCode), TRAPMSG("A UMDF driver "
        "reported a fatal error"), FxDriverGlobals->Public.DriverName);
#else
    UNREFERENCED_PARAMETER(FxDriverGlobals);
#pragma prefast(suppress:__WARNING_USE_OTHER_FUNCTION, "WDF wrapper to KeBugCheckEx.");
    Mx::MxBugCheckEx(BugCheckCode,
        BugCheckParameter1,
        BugCheckParameter2,
        BugCheckParameter3,
        BugCheckParameter4);
#endif //(FX_CORE_MODE == FX_CORE_USER_MODE)
}
