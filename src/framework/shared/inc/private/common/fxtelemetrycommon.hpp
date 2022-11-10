/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxTelemetryCommon.hpp

Abstract:

    This is header file for telemetry methods common to all WDF components

Author:



Environment:

     Both kernel and user mode

Revision History:

Notes:

--*/

#pragma once

#include <traceloggingprovider.h>
#include <telemetry\MicrosoftTelemetry.h>

// WDF01000.sys
#define KMDF_FX_TRACE_LOGGING_PROVIDER_NAME    "Microsoft.Wdf.KMDF.Fx"

// WUDFX0200.dll
#define UMDF_FX_TRACE_LOGGING_PROVIDER_NAME    "Microsoft.Wdf.UMDF.Fx"

// WudfHost.exe
#define UMDF_HOST_TRACE_LOGGING_PROVIDER_NAME  "Microsoft.Wdf.UMDF.Host"

// WudfCompanionHost.exe
#define COMPANION_HOST_TRACE_LOGGING_PROVIDER_NAME  "Microsoft.Wdf.Companion.Host"

// WdfLdr.sys
#define KMDF_LDR_TRACE_LOGGING_PROVIDER_NAME   "Microsoft.Wdf.KMDF.Ldr"

// UMDF driver manager, now running inside services.exe
#define UMDF_DM_TRACE_LOGGING_PROVIDER_NAME    "Microsoft.Wdf.UMDF.Dm"

// WUDFPlatform.dll
#define UMDF_PLATFORM_TRACE_LOGGING_PROVIDER_NAME   "Microsoft.Wdf.UMDF.Platform"












