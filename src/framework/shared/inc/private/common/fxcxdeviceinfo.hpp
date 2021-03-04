//
//    Copyright (C) Microsoft.  All rights reserved.
//
#ifndef _FXCXDEVICEINFO_H_
#define _FXCXDEVICEINFO_H_

#include "FxDeviceCallbacks.hpp"

struct FxCxDeviceInfo : public FxStump {
    FxCxDeviceInfo(PFX_DRIVER_GLOBALS FxDriverGlobals) :
        Driver(NULL),
        IoInCallerContextCallback(FxDriverGlobals),
        Index(0)
    {
        InitializeListHead(&ListEntry);
        RtlZeroMemory(&RequestAttributes, sizeof(RequestAttributes));
        RtlZeroMemory(&CxPnpPowerCallbackContexts, sizeof(CxPnpPowerCallbackContexts));
    }

    ~FxCxDeviceInfo()
    {
        ASSERT(IsListEmpty(&ListEntry));

        for (ULONG loop = 0; loop < ARRAYSIZE(CxPnpPowerCallbackContexts); loop++) {
            if (CxPnpPowerCallbackContexts[loop] != NULL) {
                delete CxPnpPowerCallbackContexts[loop];
            }
        }
    }

    FxCxPnpPowerCallbackContext*
    GetCxPnpPowerCallbackContexts(FxCxCallbackType CallbackType)
    /*
      A CX can register callbacks for either ArmWakeFromSx or ArmWakeFromSxWithReason,
      but not both. They are called "equal types". When new equal types are
      introduced, add them here, and then update their InvokeCxCallback /
      InvokeCxCleanupCallback like FxPowerDeviceArmWakeFromSx does:
          FxPowerDeviceArmWakeFromSx::InvokeCxCallback()
          FxPowerDeviceArmWakeFromSx::InvokeCxCleanupCallback()
    */
    {
        FxCxPnpPowerCallbackContext* result;
        FxCxCallbackType equalType;

        result = CxPnpPowerCallbackContexts[CallbackType];
        if (result != NULL) {
            return result;
        }

        switch (CallbackType) {

        // FxCxCallbackArmWakeFromSx vs. FxCxCallbackArmWakeFromSxWithReason

        case FxCxCallbackArmWakeFromSx:
            equalType = FxCxCallbackArmWakeFromSxWithReason;
            break;

        case FxCxCallbackArmWakeFromSxWithReason:
            equalType = FxCxCallbackArmWakeFromSx;
            break;

        // FxCxCallbackSmIoRestart vs. FxCxCallbackSmIoRestartEx

        case FxCxCallbackSmIoRestart:
            equalType = FxCxCallbackSmIoRestartEx;
            break;

        case FxCxCallbackSmIoRestartEx:
            equalType = FxCxCallbackSmIoRestart;
            break;

        // FxCxCallbackSmIoSuspend vs. FxCxCallbackSmIoSuspendEx

        case FxCxCallbackSmIoSuspend:
            equalType = FxCxCallbackSmIoSuspendEx;
            break;

        case FxCxCallbackSmIoSuspendEx:
            equalType = FxCxCallbackSmIoSuspend;
            break;

        default:
            return NULL;
        }
        return CxPnpPowerCallbackContexts[equalType];
    }

    LIST_ENTRY                  ListEntry;
    FxDriver*                   Driver;
    FxIoInCallerContext         IoInCallerContextCallback;
    WDF_OBJECT_ATTRIBUTES       RequestAttributes;
    CCHAR                       Index;

    PFxCxPnpPowerCallbackContext CxPnpPowerCallbackContexts[FxCxCallbackMax];
};

#endif // _FXCXDEVICEINFO_H_
