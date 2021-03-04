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
#include "wdfWmi.h"
#include "wdfChildList.h"
#include "wdfpdo.h"
#include "wdffdo.h"
#include "wdfiotarget.h"
#include "wdfcontrol.h"
#include "wdfcx.h"
#include "wdfio.h"
#include "wdfqueryinterface.h"

#include "FxIrpQueue.hpp"
#include "FxCallback.hpp"
#if (FX_CORE_MODE == FX_CORE_USER_MODE)
#include "FxIrpUm.hpp"
#else if ((FX_CORE_MODE)==(FX_CORE_KERNEL_MODE))
#include "FxIrpKm.hpp"
#endif
#include "FxTransactionedList.hpp"

#include "FxCollection.hpp"
#include "FxDeviceInitShared.hpp"
#include "FxDeviceToMxInterface.hpp"
#include "FxRequestContext.hpp"
#include "FxRequestContextTypes.h"
#include "FxRequestBase.hpp"
#include "FxRequestBuffer.hpp"
#include "IfxMemory.hpp"
#include "FxIoTarget.hpp"
#include "FxIoTargetRemote.hpp"
#include "FxIoTargetSelf.hpp"

#include "FxVersionedStructures.h"
