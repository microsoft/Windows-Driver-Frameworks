/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxObjectInfoKm.cpp

Abstract:

    This file contains object info split from globals.cpp

    This is because objects incorporated in KMDF and UMDF will differ

Author:




Environment:

    Kernel mode only

Revision History:

--*/

#include "fxobjectpch.hpp"

#include "FxMemoryBufferPreallocated.hpp"
#include "FxUserObject.hpp"
#include "FxUsbDevice.hpp"
#include "FxUsbPipe.hpp"
#include "FxUsbInterface.hpp"
#include "WudfRdNonPnP.hpp"
#include "FxCompanionTarget.hpp"

#include "FxObjectInfoDataKm.hpp"
