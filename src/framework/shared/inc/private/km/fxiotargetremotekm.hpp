/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    FxIoTargetRemoteKm.hpp

Abstract:

    Kernel-mode specific definitions of FxIoTargetRemote 
    
Author:


Environment:

    kernel mode only

Revision History:

--*/

#ifndef _FXIOTARGETREMOTEKM_H_
#define _FXIOTARGETREMOTEKM_H_

FORCEINLINE
NTSTATUS
FxIoTargetRemote::InitRemoteModeSpecific(
    __in FxDeviceBase* Device
    )
{
    UNREFERENCED_PARAMETER(Device);
    
    //
    // Nothing mode-specific work to do for KM here.
    //
    DO_NOTHING();

    return STATUS_SUCCESS;
}

FORCEINLINE
VOID
FxIoTargetRemote::RemoveModeSpecific(
    VOID
    )
{
    //
    // Nothing mode-specific work to do for KM here.
    //
    DO_NOTHING();
}

FORCEINLINE
_Must_inspect_result_
NTSTATUS
FxIoTargetRemote::OpenLocalTargetByFile(
    _In_ PWDF_IO_TARGET_OPEN_PARAMS OpenParams
    )
{
    UNREFERENCED_PARAMETER(OpenParams);
    
    //
    // Nothing mode-specific work to do for KM here.
    //
    DO_NOTHING();

    return STATUS_SUCCESS;
}

FORCEINLINE
HANDLE
FxIoTargetRemote::GetTargetHandle(
    VOID
    )
{
    return m_TargetHandle;
}

#endif // _FXIOTARGETREMOTEKM_H_
