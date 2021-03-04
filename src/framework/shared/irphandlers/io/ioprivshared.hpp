/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    iopriv.hpp

Abstract:

    This module defines private interfaces for the I/O package

Author:



Environment:

    Both kernel and user mode

Revision History:

--*/

#ifndef _IOPRIV_H_
#define _IOPRIV_H_

#if ((FX_CORE_MODE)==(FX_CORE_USER_MODE))
#define FX_IS_USER_MODE (TRUE)
#define FX_IS_KERNEL_MODE (FALSE)
#elif ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
#define FX_IS_USER_MODE (FALSE)
#define FX_IS_KERNEL_MODE (TRUE)
#endif

/*#if defined(MODE_AGNSOTIC_FXPKGIO_NOT_IN_SHARED_FOLDER)
#include <Fx.hpp>
#else
// common header file for all irphandler\* files
#include "irphandlerspriv.hpp"
#endif

#if FX_IS_USER_MODE
#define PWDF_REQUEST_PARAMETERS  PVOID
#define PFN_WDF_REQUEST_CANCEL   PVOID
#define PIO_CSQ_IRP_CONTEXT      PVOID
#endif*/

extern "C" {
#include "mx.h"
}

#include "FxMin.hpp"

#include "wdfmemory.h"
#include "wdfrequest.h"
#include "wdfio.h"
#include "wdfdevice.h"
#include "wdfWmi.h"
#include "wdfChildList.h"
#include "wdfpdo.h"
#include "wdffdo.h"
#include "FxIrpQueue.hpp"
#include "FxCallback.hpp"

// <FxSystemWorkItem.hpp>
__drv_functionClass(EVT_SYSTEMWORKITEM)
__drv_maxIRQL(PASSIVE_LEVEL)
__drv_maxFunctionIRQL(DISPATCH_LEVEL)
__drv_sameIRQL
typedef
VOID
EVT_SYSTEMWORKITEM(
    __in PVOID Parameter
    );

typedef EVT_SYSTEMWORKITEM *PFN_WDF_SYSTEMWORKITEM;

// </FxSystemWorkItem.hpp>

#include "FxSystemThread.hpp"

#include "FxCallbackSpinlock.hpp"
#include "FxCallbackMutexLock.hpp"
#include "FxTransactionedList.hpp"

#if (FX_CORE_MODE == FX_CORE_KERNEL_MODE)
#include "FxIrpKm.hpp"
#else
#include "FxIrpUm.hpp"
#endif


#include "FxPackage.hpp"
#include "FxCollection.hpp"
#include "FxDeviceInitShared.hpp"
#include "FxDeviceToMxInterface.hpp"

#include "IfxMemory.hpp"
#include "FxCallback.hpp"
#include "FxRequestContext.hpp"
#include "FxRequestContextTypes.h"
#include "FxRequestBase.hpp"
#include "FxMemoryObject.hpp"
#include "FxMemoryBufferPreallocated.hpp"
#include "FxRequestMemory.hpp"
#include "FxRequest.hpp"
#include "FxRequestBuffer.hpp"
#include "FxSyncRequest.hpp"

#include "irphandlerspriv.hpp"
#include "FxPkgPnp.hpp"
#include "FxPkgIo.hpp"
#include "FxIoQueue.hpp"
#include "FxIoQueueCallbacks.hpp"


#include "FxVersionedStructures.h"

#endif  //_IOPRIV_H_
