/*++

Copyright (c) Microsoft Corporation

Module Name:

    corepriv.hpp

Abstract:

    This is the main driver framework.

Author:



Environment:

    Both kernel and user mode

Revision History:

--*/

#pragma once

#if ((FX_CORE_MODE)==(FX_CORE_USER_MODE))
#define FX_IS_USER_MODE (TRUE)
#define FX_IS_KERNEL_MODE (FALSE)
#elif ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
#define FX_IS_USER_MODE (FALSE)
#define FX_IS_KERNEL_MODE (TRUE)
#endif

extern "C" {
#include "mx.h"
}

#include "FxMin.hpp"

#include "wdfmemory.h"
#include "wdfrequest.h"
#include "wdfdevice.h"
#include "wdfdevicepri.h"
#include "wdfiotargetpri.h"
#include "wdfwmi.h"
#include "wdfChildList.h"
#include "wdfpdo.h"
#include "wdffdo.h"
#include "wdfiotarget.h"
#include "wdfcontrol.h"
#include "wdfcx.h"
#include "wdfio.h"
#include "wdfqueryinterface.h"
#include "wdftriage.h"

//
// Companion headers
//
#if (FX_CORE_MODE == FX_CORE_USER_MODE)
#include "wdfcompanion.h"
#endif

#if (FX_CORE_MODE == FX_CORE_KERNEL_MODE)
#include "wdfcompaniontarget.h"
#endif

#if (FX_CORE_MODE == FX_CORE_USER_MODE)
#include "FxIrpUm.hpp"
#else
#include "FxIrpKm.hpp"
#endif

// <FxSystemWorkItem.hpp>
typedef
VOID
(*PFN_WDF_SYSTEMWORKITEM) (
    IN PVOID             Parameter
    );

#include "FxIrpQueue.hpp"


// </FxSystemWorkItem.hpp>


#include "FxProbeAndLock.h"
#include "FxPackage.hpp"
#include "FxCollection.hpp"
#include "FxDeviceInitShared.hpp"

#include "IfxMemory.hpp"
#include "FxCallback.hpp"
#include "FxRequestContext.hpp"
#include "FxRequestContextTypes.h"
#include "FxRequestBase.hpp"
#include "FxMemoryObject.hpp"
#include "FxMemoryBuffer.hpp"

#include "FxMemoryBufferFromPool.hpp"

#include "FxMemoryBufferPreallocated.hpp"

#include "FxTransactionedList.hpp"

//
// MERGE temp: We may not need these include files here,
// temporarily including them to verify they compile in shared code
//
#include "FxRequestValidateFunctions.hpp"
#include "FxRequestCallbacks.hpp"

// support
#include "StringUtil.hpp"
#include "FxAutoString.hpp"
#include "FxString.hpp"
#include "FxDeviceText.hpp"
#include "FxCallback.hpp"
#include "FxDisposeList.hpp"
#include "FxSystemThread.hpp"

#include "FxIrpPreprocessInfo.hpp"
#include "FxPnpCallbacks.hpp"

// device init
#include "FxCxDeviceInit.hpp"
#include "FxCxDeviceInfo.hpp"
#include "FxDeviceInit.hpp"

#include "FxDeviceToMxInterface.hpp"

// request
#include "FxRequestMemory.hpp"
#include "FxRequest.hpp"
#include "FxRequestBuffer.hpp"
#include "FxSyncRequest.hpp"

// io target
#include "FxIoTarget.hpp"
#include "FxIoTargetSelf.hpp"

#include "FxSystemWorkItem.hpp"
#include "FxCallbackMutexLock.hpp"
#include "FxDriver.hpp"

#include "FxDeviceInterface.hpp"
#include "FxQueryInterface.hpp"

#include "FxCallbackSpinLock.hpp"
#include "FxDefaultIrpHandler.hpp"
#include "FxWmiIrpHandler.hpp"

// packages
#include "FxPkgIo.hpp"
#include "FxPkgPnp.hpp"
#include "FxPkgFdo.hpp"
#include "FxPkgPdo.hpp"
#include "FxPkgGeneral.hpp"
#include "FxFileObject.hpp"
#include "FxIoQueue.hpp"
#include "FxDevice.hpp"
#include "FxTelemetry.hpp"

#include "FxChildList.hpp"

#include "FxLookasideList.hpp"

/*#if FX_IS_KERNEL_MODE
#include "wdfrequest.h"
#endif*/

#include "FxVersionedStructures.h"
