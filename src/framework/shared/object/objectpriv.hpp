/*++

Copyright (c) Microsoft Corporation

Module Name:

    objectpriv.hpp

Abstract:

    Private header file for shared\object directory
    It is then included in objectpch.hpp

Author:



Environment:

    Both kernel and user mode

Revision History:

--*/


extern "C" {
#include "mx.h"
}

//
// Root WDF key, for both UMDF and KMDF settings.
//
#if (FX_CORE_MODE==FX_CORE_KERNEL_MODE)
#define WDF_REGISTRY_BASE_PATH                      L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Wdf"
#else
#define WDF_REGISTRY_BASE_PATH                      L"System\\CurrentControlSet\\Control\\Wdf"
#endif

#define WDF_GLOBAL_VALUE_IFRDISABLED                L"WdfGlobalLogsDisabled"        // REG_DWORD

#define WDF_GLOBAL_VALUE_SLEEPSTUDY_DISABLED        L"WdfGlobalSleepStudyDisabled"  // REG_DWORD










