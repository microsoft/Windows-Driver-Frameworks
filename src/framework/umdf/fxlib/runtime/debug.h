/*++
 
  Copyright (C) Microsoft Corporation, All Rights Reserved

  Module Name:

    debug.h

  Abstract:

    Contains verification macros for use throughout the WUDF components.

  Author:



--*/

#pragma once

typedef struct IUMDFPlatform IUMDFPlatform;

typedef 
enum WdfDriverStopType
    {
        WdfInternalError	= 0,
        WdfDriverError	= ( WdfInternalError + 1 ) ,
        WdfCallerError	= ( WdfDriverError + 1 ) ,
        WdfExternalError	= ( WdfCallerError + 1 ) ,
        WdfUnhandledExceptionError	= ( WdfExternalError + 1 ) ,
        WdfDriverStopTypeMax	= ( WdfUnhandledExceptionError + 1 ) 
    } 	WdfDriverStopType;

typedef enum WdfComponentType {
    WdfComponentInvalid = 0,
    WdfComponentPlatform,
    WdfComponentReflector,
    WdfComponentDriverManager,
    WdfComponentHost,
    WdfComponentFramework,
    WdfComponentTest,
    WdfComponentMax
} WdfComponentType;

//
// Macros for generating error numbers.  Error numbers allow us to track a
// UMDFDriverStop to a specific location in the code (by inspecting the code).
//
//   Component - the component reporting the error (dm, host, etc.. use the 
//                                                  values defined in 
//                                                  WdfPlatform.h)
//
//   Kind - the kind of error - driver, internal, external
//
//   Class - defined by the component to provide broad categorization of 
//           errors.  For example, the Fx could define a class of error - 
//           MissingCallback - and then use specific error codes within that
//           to indicate the specific callback that's not present.
//
//    Error - defined by the component.  Within the class it should provide 
//            enough information to know exactly where in the code the 
//            error occurred (by inspection).
//

//
// Generate a complete error number.
//
//  6 6 6 6 5 5 5 5 5 5 5 5 5 5 4 4 4 4 4 4 4 4 4 4 3 3 3 3 3 3 3 3 
//  3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2
// -----------------------------------------------------------------
// |  Component    |  Driver Stop  |             Class             |
// |               |   Type        |                               |
// |               |               |           0 - 65536           |
// -----------------------------------------------------------------
//
//  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 
//  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// -----------------------------------------------------------------
// |                                                               |
// |                       Error Number                            |
// |                                                               |
// -----------------------------------------------------------------
//

#define WUDF_ERROR_NUMBER(Component, Kind, Class, Error) \
    ((((ULONGLONG) (Component))  << 56) |                        \
     (((ULONGLONG) (Kind))       << 48) |                        \
     (((ULONGLONG) (Class))      << 32) |                        \
     (((ULONGLONG) (Error)))                                     \
     )

//
// The global enum of error classes.  New classes of error should be added
// here.  There are several overlapping component specific ranges.
// 

typedef enum WdfErrorClass {
    WdfErrorClass_Undefined = 0,

    //
    // One of your arguments was bad.
    //

    WdfErrorClass_BadArgument,

    //
    // The call was made with unexpected state
    //

    WdfErrorClass_BadState,

    //
    // The result you got back from the call was incorrect or invalid
    //

    WdfErrorClass_BadResponse,

    //
    // The component that called you to do something shouldn't have done so.
    // For example, if something hasn't been opened and the caller tries to
    // close it.
    //

    WdfErrorClass_BadAction,

    //
    // A reference has been leaked someplace.
    //

    WdfErrorClass_LostReference,

    WdfErrorClass_GlobalMax,

    //
    // Platform specific error classes can go in here.
    //

    WdfErrorClass_PlatformStart = WdfErrorClass_GlobalMax,

    WdfErrorClass_PlatformMax,

    //
    // Redirector specific error classes go in here.
    //

    WdfErrorClass_RdStart = WdfErrorClass_GlobalMax,

    WdfErrorClass_RdMax,

    //
    // Driver manager specific error classes.

    WdfErrorClass_DmStart = WdfErrorClass_GlobalMax,

    WdfErrorClass_DmMax,

    //
    // Host specific error classes.
    //

    WdfErrorClass_HostStart = WdfErrorClass_GlobalMax,

    WdfErrorClass_HostMax,

    //
    // Framework specific error classes.
    //

    WdfErrorClass_FxStart = WdfErrorClass_GlobalMax,

    WdfErrorClass_FxMax,

    //
    // Test specific error classes.
    //

    WdfErrorClass_TestStart = WdfErrorClass_GlobalMax,

    WdfErrorClass_TestMax

} WdfErrorClass, *PWdfErrorClass;

/*++

    This is the core macro for the verify building blocks.  It tests an 
    assertion and, if false, calls UmdfDriverStop.  It passes UmdfDriverStop: 
        * the component reporting the error 
        * the 64-bit error number 
        * Function name and line number
        * an output string

    This is macro magic.  The first and second parameters are used to
    generate a parameter list for the inline functions WudfExtractErrorKind
    (which returns just the kind parameteter) and WudfComputeErrorNumber
    (which combines all 4 numbers into a 64-bit value.

    The second parameter is used as both the boolean test (the comma
    operator returns whatever value is on the right, which is the test portion)
    and to build a parameter list for WudfExtractMessage() which returns
    just the text message.

    If the UmdfDriverStop path is taken, the function WudfNoReturn() is also invoked.  
    This is a no-op inline which is decorated as noreturn in prefast builds.
    This allows the use of __VERIFY(...) to work around false prefast warnings 
    and adds in an runtime test to ensure that your assertion is correct. 

  Arguments:

    Component - a single value of type WdfComponentType.  This is provided
                by the component specific macros.

    Platform - the platform pointer needed for calling UmdfDriverStop.  This 
               is provided by the component specific macros.

    ErrorCodeParts - a 3-element, comma delimited list containing:
                        Kind, Class, ErrorNumber

    MsgTestParts - a 2-element, comma delimited list containing:
                        Output message, boolean test

  Return Value:

    None.
    
*/

//
// Macro defined to no-op for now.  For use with future expansion.
//

#define DEFINE_VERIFY(x, y, z)

#ifndef _PREFAST_

//
// This macro has been constructed to ensure that none of the parameters
// are evaluated more than once.
//

#pragma warning(disable:4189)
#pragma warning(disable:4100)


typedef
VOID
(*PWUDF_STATIC_DRIVERSTOP_METHOD)(
    _In_ WdfDriverStopType Type,
    _In_ ULONGLONG ErrorNumber,
    _In_ PCWSTR Location,
    _In_ PCSTR Message
    );

VOID
WudfVerify(
    _In_ WdfComponentType Component,
    _In_ IUMDFPlatform *Platform,
    _In_z_ PCWSTR File,
    _In_ ULONG Line,
    _In_z_ PCSTR Function,
    _In_ WdfDriverStopType Kind,
    _In_ WdfErrorClass Class,
    _In_ ULONG Error,
    _In_ PCSTR Message,
    _In_ bool test,
    _In_ PSTR ActiveDriver = NULL
    );

VOID
WudfVerifyStatic(
    _In_ WdfComponentType Component,
    _In_ PWUDF_STATIC_DRIVERSTOP_METHOD DriverStop,
    _In_z_ PCWSTR File,
    _In_ ULONG Line,
    _In_z_ PCSTR Function,
    _In_ WdfDriverStopType Kind,
    _In_ WdfErrorClass Class,
    _In_ ULONG Error,
    _In_ PCSTR Message,
    _In_ bool test
    );

#else

_inline
VOID
__declspec(noreturn)
WudfNoReturn(
    void
    )
{
}

#define WudfVerify(_Component, _Object, _File, _Line, _Function, _ErrorCodeParts, _Test, _String) \
{                                                                                   \
__pragma(warning(push));                                                            \
__pragma(warning(disable:4127));                                                    \
                                                                                    \
    if (!(_Test)) {                                                                 \
                                                                                    \
__pragma(warning(pop));                                                             \
        WudfNoReturn();                                                             \
    }                                                                               \
}

#define WudfVerifyStatic WudfVerify

#endif

#define _WDF_WIDEN2(x) L ## x  
#define _WDF_WIDEN(x) _WDF_WIDEN2(x)  
  
#define _WDF_MKWSTR2(x) L ## #x  
#define _WDF_MKWSTR(x) _WDF_MKWSTR2(x)  
  
#define __WFILE__       _WDF_WIDEN(__FILE__)  
#define __WFUNCTION__   _WDF_WIDEN(__FUNCTION__)  
#define __WLINE__       _WDF_MKWSTR(__LINE__)  
  

//
// Component specific macros.  These take into account the appropriate way
// to get the platform object for the given component.  For example, the host
// has a single global platform object, so it uses the known name of the
// global.  
//

#define FX_VERIFY_1_9(ErrorCodeParts, MsgTestParts, DownLevel)    \
    if (m_pDriverGlobals->IsVersionGreaterThanOrEqualTo(1, 9)  || \
       (DownLevel ? m_pDriverGlobals->IsDownLevelVerificationEnabled() : FALSE))\
    { \
        WudfVerify(WdfComponentFramework,                   \
                 g_IUMDFPlatform,                           \
                 __WFILE__,                                 \
                 __LINE__,                                  \
                 __FUNCTION__,                              \
                 ErrorCodeParts,                            \
                 MsgTestParts                               \
                 );                                         \
    }
    
#define STATIC_FX_VERIFY_1_11(ErrorCodeParts, MsgTestParts, DownLevel, DriverGlobals) \
        if (DriverGlobals->IsVersionGreaterThanOrEqualTo(1, 11)  || \
           (DownLevel ? DriverGlobals->IsDownLevelVerificationEnabled() : FALSE))\
        { \
            WudfVerify(WdfComponentFramework,                   \
                     g_IUMDFPlatform,                           \
                     __WFILE__,                                 \
                     __LINE__,                                  \
                     __FUNCTION__,                              \
                     ErrorCodeParts,                            \
                     MsgTestParts                               \
                     );                                         \
        }
        
#define FX_VERIFY(ErrorCodeParts, MsgTestParts)         \
    WudfVerify(WdfComponentFramework,                   \
             g_IUMDFPlatform,                           \
             __WFILE__,                                 \
             __LINE__,                                  \
             __FUNCTION__,                              \
             ErrorCodeParts,                            \
             MsgTestParts                               \
             )

#define FX_VERIFY_WITH_NAME(ErrorCodeParts, MsgTestParts, DriverName) \
    WudfVerify(WdfComponentFramework,                                 \
               g_IUMDFPlatform,                                       \
               __WFILE__,                                             \
               __LINE__,                                              \
               __FUNCTION__,                                          \
               ErrorCodeParts,                                        \
               MsgTestParts,                                          \
               DriverName                                             \
               )

#define FX_FOLDABLE_VERIFY(ErrorCodeParts, MsgTestParts)    \
    WudfVerify(WdfComponentFramework,                       \
             g_IUMDFPlatform,                               \
             L"FOLDED",                                     \
             ErrorCodeParts,                                \
             MsgTestParts                                   \
             )

#define HOST_VERIFY(ErrorCodeParts, MsgTestParts)   \
    WudfVerify(WdfComponentHost,                    \
             g_pPlatform,                           \
             __WFILE__,                             \
             __LINE__,                              \
             __FUNCTION__,                          \
             ErrorCodeParts,                        \
             MsgTestParts                           \
             )
    
#define DM_VERIFY(ErrorCodeParts, MsgTestParts) \
    WudfVerify(WdfComponentDriverManager,       \
             g_Platform,                        \
             __WFILE__,                         \
             __LINE__,                          \
             __FUNCTION__,                      \
             ErrorCodeParts,                    \
             MsgTestParts                       \
             )

#define PLATFORM_VERIFY(Platform, ErrorCodeParts, MsgTestParts) \
    WudfVerify(WdfComponentPlatform,                            \
             Platform,                                          \
             __WFILE__,                                         \
             __LINE__,                                          \
             __FUNCTION__,                                      \
             ErrorCodeParts,                                    \
             MsgTestParts                                       \
             )

#define WRAPPER_VERIFY(ErrorCodeParts, MsgTestParts)            \
    WudfVerifyStatic(WdfComponentPlatform,                      \
             WdfPlatform::UmdfDriverStop,                       \
             __WFILE__,                                         \
             __LINE__,                                          \
             __FUNCTION__,                                      \
             ErrorCodeParts,                                    \
             MsgTestParts                                       \
             )

#define LPC_VERIFY(ErrorCodeParts, MsgTestParts)                \
    WudfVerifyStatic(WdfComponentPlatform,                      \
             WdfPlatform::UmdfDriverStop,                       \
             __WFILE__,                                         \
             __LINE__,                                          \
             __FUNCTION__,                                      \
             ErrorCodeParts,                                    \
             MsgTestParts                                       \
             )

#define RD_VERIFY_MSG(msg, test)                                        \
    ((!(test)) ?                                                        \
        (RtlAssert(#test, __FILE__, __LINE__, msg), WudfNoReturn()) :   \
                                                    TRUE)

#define RD_VERIFY(test) \
    RD_VERIFY_MSG("Failed Verification: ", test)

//
// Error-code part macros.  These are used to define the type of error.  They
// are used as the ErrorCodeParts parameter for *_VERIFY.
//

#define INTERNAL                WdfInternalError, WdfErrorClass_Undefined, 0
#define EXTERNAL(Class, Error)  WdfExternalError, WdfErrorClass_##Class, Error
#define CALLER(Class, Error)    WdfExternalError, WdfErrorClass_##Class, Error
#define DRIVER(Class, Error)    WdfDriverError, WdfErrorClass_##Class, Error

//
// Use to flag spots where you haven't yet bothered to assign error numbers.
//

#define TODO                    __LINE__

//
// MsgTestPart macros.  Use these as the second parmameter to create various
// types of tests.  If the test result is false then a UMDFDriverStop will occur.
//
//
// The TODO varients are useful for converting existing code.
//

#ifndef _PREFAST_

template< class I > 
bool
CheckQueryInterface(I ptr)
{
    IUnknown * unk;
    
    HRESULT hr = ptr->QueryInterface(__uuidof(*ptr), (void**)&unk);
    if (SUCCEEDED(hr))
    {
        unk->Release();
        return true;
    }
    return false;
}

#define CHECK_NULL(x)           (#x ## " should be NULL"), (NULL == x)
#define CHECK_NOT_NULL(x)       (#x ## " should not be NULL"), (NULL != x)
#define CHECK_QI(hr, ptr)       "Unexpected failure in QueryInterface", (SUCCEEDED(hr) && (NULL != ptr))
#define CHECK_HR(hr)            "Unexpected failure", (SUCCEEDED(hr))
#define CHECK_HANDLE(h)         (#h ## " is not a valid handle"), ((NULL != (h)) && (INVALID_HANDLE_VALUE != (h)))
#define CHECK(msg, test)        msg, ((test) ? true : false)
#define CHECK_TODO(test)        "TODO: Write something interesting here", ((test) ? true : false)
#define TRAPMSG(msg)            msg, false
#define UNIMPLEMENTED           __FUNCTION__ " is not implemented", false
#define VERIFY(cond)            #cond, ((cond) ? true : false)
#define CHECK_IS_IUNKNOWN(ptr)  "COM pointer failed QI for IUnknown", (NULL != ptr) && CheckQueryInterface<IUnknown>(ptr)
#define CHECK_IS_INTERFACE(ptr) "COM pointer failed QI for its interface", (NULL != ptr) && CheckQueryInterface(ptr)
#define CHECK_BOOL(x)           (#x ## " is not a valid BOOL"), ((x) == TRUE || (x) == FALSE)
#define CHECK_WUDF_TRI_STATE(x) (#x ## " is not a valid WDF_TRI_STATE"),\
                                ((x) == WudfUseDefault || (x) == WudfTrue || (x) == WudfFalse)
#else

#define CHECK_NULL(x)           (NULL == x)
#define CHECK_NOT_NULL(x)       (NULL != x)
#define CHECK_QI(hr, ptr)       (SUCCEEDED(hr) && (NULL != ptr))
#define CHECK_HR(hr)            (SUCCEEDED(hr))
#define CHECK_HANDLE(h)         ((NULL != (h)) && (INVALID_HANDLE_VALUE != (h)))
#define CHECK(msg, test)        ((test) ? true : false)
#define CHECK_TODO(test)        ((test) ? true : false)
#define TRAPMSG(msg)            false
#define UNIMPLEMENTED           false
#define VERIFY(cond)            ((cond) ? true : false)
#define CHECK_IS_IUNKNOWN(ptr)  (NULL != ptr)
#define CHECK_IS_INTERFACE(ptr) (NULL != ptr)
#define CHECK_BOOL(x)           ((x) == TRUE || (x) == FALSE)
#define CHECK_WUDF_TRI_STATE(x) ((x) == WudfUseDefault || (x) == WudfTrue || (x) == WudfFalse)

#endif

#define INVOKE_DRIVER_CALLBACK(object, callback, result, parameters)    \
{                                                                       \
    __try {                                                             \
        result = object->callback parameters;                           \
    }                                                                   \
    __except(EXECUTE_EXCEPTION_HANDLER)                                 \
    {                                                                   \
        g_pPlatform->UmdfDriverStop(                                          \
            WdfDriverError,                                             \
            GetExceptionCode(),                                         \
            L#callback,                                                 \
            __LINE__,                                                   \
            "Unhandled exception in driver callback."                   \
            );                                                          \
            WudfNoReturn();                                             \
    }

#define START_DRIVER_CALLBACK(callbackName)                             \
{                                                                       \
    PCWSTR __callbackName = callbackName;                               \
    __try 

#define END_DRIVER_CALLBACK                                             \
    __except(EXECUTE_EXCEPTION_HANDLER) {                               \
        g_pPlatform->UmdfDriverStop(                                          \
            WdfDriverError,                                             \
            GetExceptionCode(),                                         \
            __callbackName,                                             \
            __LINE__,                                                   \
            "Unhandled exception in driver callback."                   \
            );                                                          \
            WudfNoReturn();                                             \
    }

//
//

#ifndef WUDF_KERNEL

extern "C" {

#if WUDF_ENABLE_KERNEL_BREAKIN

    BOOL
    WudfIsUserDebuggerPresent(
        VOID
        );

    BOOL
    WudfIsKernelDebuggerPresent(
        VOID
        );

#endif

    BOOL
    WudfIsAnyDebuggerPresent(
        VOID
        );

    typedef enum _WUDF_BREAK_POINT_TYPE {
        WudfUserBreakin = 1,
        WudfKernelBreakin = 2,
        WudfUserOrKernelBreakin = (WudfUserBreakin | WudfKernelBreakin)
    } WUDF_BREAK_POINT_TYPE;

    VOID
    WudfDebugBreakPoint(
        _In_ WUDF_BREAK_POINT_TYPE Type
        );

    typedef enum _WUDF_DEBUGGER_TYPE {
        WudfUserDebugger = 0,
        WudfUserOrKernelDebugger = 1
    } WUDF_DEBUGGER_TYPE;

    VOID
    WudfWaitForDebugger(
        _In_ ULONG TimeoutSeconds,
        _In_ WUDF_DEBUGGER_TYPE Type
        );
}

#endif

extern "C" IUMDFPlatform *g_IUMDFPlatform;
extern "C" IWudfHost2 *g_IWudfHost2;
