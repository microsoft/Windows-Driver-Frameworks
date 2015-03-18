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

DECLSPEC_NOINLINE
VOID
WudfVerify(
    _In_ WdfComponentType Component,
    _In_ IUMDFPlatform *Platform,
    _In_ PCWSTR Location,
    _In_ WdfDriverStopType Kind,
    _In_ WdfErrorClass Class,
    _In_ ULONG Error,
    _In_ PCSTR Message,
    _In_ bool test,
    _In_ PSTR ActiveDriver
)
{
    if(!test) {

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

        hr = Platform->QueryInterface(IID_IUMDFPlatform2, (PVOID*)&platformEx);
        if (SUCCEEDED(hr)) {            
            platformEx->UmdfDriverStopWithActiveDriver(
                Kind,
                WUDF_ERROR_NUMBER(Component, Kind, Class, Error),
                Location,
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
    __in WdfComponentType Component,
    __in PWUDF_STATIC_DRIVERSTOP_METHOD DriverStop,
    __in PCWSTR Location,
    __in WdfDriverStopType Kind,
    __in WdfErrorClass Class,
    __in ULONG Error,
    __in PCSTR Message,
    __in bool test
    )
{
    if(!test) {
        DriverStop(Kind, 
                   WUDF_ERROR_NUMBER(Component, Kind, Class, Error),
                   Location,
                   Message);
    }
}

#endif
