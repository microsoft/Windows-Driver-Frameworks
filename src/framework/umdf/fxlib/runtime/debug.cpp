/*++
 
  Copyright (c) Microsoft Corporation.  All rights reserved.

  Module Name: 

    debug.cpp

  Abstract:

    This module contains some helper functions used in the verify macros to 
    invoke the DriverStop functionality.

  Author:



--*/

#include "fxmin.hpp"

#ifndef _PREFAST_

//
// Size needs to be large enough for these added together (all WCHAR)
//      *) File location (MAX_PATH) + 
//      *) ':' 
//      *) 6 digit line number
//      *) "("
//      *) FunctionName (MAX_PATH)
//      *) ")"
//      *) NULL
//
#define LOCATION_PATH_MAX       ((MAX_PATH * 2 + 10)* sizeof(WCHAR))
#define LOCATION_PATH_FORMAT    (L"%s:%u(%S)")

HRESULT
WudfBuildFullFileLocation(
    _Out_writes_bytes_(LocationSize) PWSTR Location,
    _In_   ULONG LocationSize,
    _In_z_ PCWSTR File,
    _In_   ULONG Line,
    _In_z_ PCSTR Function
    )
/*++

Routine Description:

    Called to create unique string describing the location of the verifier
    call.

    Location should be set to:
        filename with full path ":" line number "(" function name ")"

    Format is needed to maintain Watson logic for generating
    bug hash, which ultimately leads to bug bucketization. A change
    in format will result in bucket bugs getting a new bucket and disconnecting
    from current bucket.

Arguments:

    Location - string to write the unique location/line number/function to
    LocationSize - size of Location that can be written to
    File - Full file name and path
    Line - line number
    Function - Function name

Return Value:

    S_OK if Location is been created correctly.

    --*/
{

    return StringCbPrintf(Location, LocationSize, LOCATION_PATH_FORMAT,
                          File, Line, Function);
}

DECLSPEC_NOINLINE
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
    _In_ PSTR ActiveDriver
)
/*++

Routine Description:

    Invoked by the UMDF components to validate assumptions, similar to an
    assert.

    If an assumption fails then a UmdfDriverStop is issued, which ultimataly 
    kills the driver after generating a watson report.

    NOTE: Updating this function requires the update of !Analyze. The tool
    looks at this frame in crashes to extraction the crash location.
    The code that needs to be updated is in 
    %SDXROOT%\sdktools\debuggers\exts\extdll\uanalysis.cpp
    The function name is IMPL_ATTRIBUTE_EXTRACTOR(UMDFVerifierFailure)

Arguments:

    Component - The WDF component issueing the call.
    Platform - Pointer to the platform interface.
    File - File name this API is being called from.
    Line - Line number that this API is being called from.
    Function - Function name that this API is being called from.
    Kind - The nature of the error (internal/external).
    Class - The type of issue that occured; bad state/action/etc.
    Error - Error code from the failing API causing this check.
    Message - A string containing an error specific message.
    test - Results of test condition being evaluated.
    ActiveDriver - A string containing the framework determined active driver 

Return Value:

    None

    --*/
{
    if(!test) {
        
        // 
        // !Analyze is looking for a variable named 'Location' with a capital 
        // 'L'. To minimize !Analyze impact we ensure this variable 
        // contains the full location of the verifier failure when
        // UmdfDriverStopWithActiveDriver is called.
        //
        PCWSTR callerLocation;
        WCHAR Location[LOCATION_PATH_MAX] = {0};
        HRESULT hr;
        IUMDFPlatform2 *platformEx = NULL;
        
        //
        // Capture the context record so that user can go to the context
        // of the caller
        //

        CONTEXT driverStopContextRecord;

        //
        // x86 -    For this to work correctly the calling function should be 
        //          non-FPO
        //          RtlCaptureContext assumes that:
        //          it is called from a 'C' procedure with
        //          the old ebp at [ebp], the return address at [ebp+4], and
        //          old esp = ebp + 8.
        //
        // x86 -    esp is not adjusted to be of previous caller
        //
        // AMD64 -  rcx (this is what the argument is passed through)
        //          and rsp would be modified before the capture
        //          as a side effect of the RtlCaptureContext call (in addition
        //          to any modifications the current function might do)
        //
        
        RtlCaptureContext(&driverStopContextRecord);

        
        if (SUCCEEDED(WudfBuildFullFileLocation(Location, 
                                                sizeof(Location),
                                                File,
                                                Line,
                                                Function))){
            callerLocation = Location;
        }
        else {
             
            callerLocation = File;
            ASSERT(0);                  // Not expected
        }
                
        hr = Platform->QueryInterface(IID_IUMDFPlatform2, (PVOID*)&platformEx);
        if (SUCCEEDED(hr)) {            
            platformEx->UmdfDriverStopWithActiveDriver(
                Kind,
                WUDF_ERROR_NUMBER(Component, Kind, Class, Error),
                callerLocation,
                Message,
                &driverStopContextRecord,
                ActiveDriver
                );
            
            //
            // Release the reference taken during QueryInterface
            //
            platformEx->Release();
            platformEx = NULL;
        } else {
            ASSERT("Unable to access the IUMDFPlatform2 COM interface from an "
                   "internal facing component");
        }
    }
}

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
    )
/*++

Routine Description:

    Invoked by the UMDF components to validate assumptions, similar to an
    assert.

    If an assumption fails then a UmdfDriverStop is issued, which ultimataly 
    kills the driver after generating a watson report.

Arguments:

    Component - The WDF component issueing the call.
    DriverStop - UmdfDriverStop function pointer
    File - File name this API is being called from.
    Line - Line number that this API is being called from.
    Function - Function name that this API is being called from.
    Kind - The nature of the error (internal/external).
    Class - The type of issue that occured; bad state/action/etc.
    Error - Error code from the failing API causing this check.
    Message - A string containing an error specific message.
    test - Results of test condition being evaluated.

Return Value:

    None

    --*/
{
    if(!test) {
        PCWSTR callerLocation;
        WCHAR Location[LOCATION_PATH_MAX] = {0};
        
        if (SUCCEEDED(WudfBuildFullFileLocation(Location, 
                                                sizeof(Location),
                                                File,
                                                Line,
                                                Function))){
        
            callerLocation = Location;
        }
        else {
            callerLocation = File;
            ASSERT(0);                  // Not expected
        }

        DriverStop(Kind, 
                   WUDF_ERROR_NUMBER(Component, Kind, Class, Error),
                   callerLocation,
                   Message);
    }
}

#endif
