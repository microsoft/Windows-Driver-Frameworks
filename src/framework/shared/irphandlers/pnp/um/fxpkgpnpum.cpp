//
//    Copyright (C) Microsoft.  All rights reserved.
//
#include "..\pnppriv.hpp"

#include <initguid.h>
#include <wdmguid.h>

extern "C" {
#if defined(EVENT_TRACING)
#include "FxPkgPnpUM.tmh"
#endif
}

NTSTATUS
FxPkgPnp::FilterResourceRequirements(
    __in IO_RESOURCE_REQUIREMENTS_LIST **IoList
    )
{
    UNREFERENCED_PARAMETER(IoList);
    ASSERTMSG("Not implemented for UMDF\n", FALSE);

    return STATUS_NOT_IMPLEMENTED;
}

_Must_inspect_result_
NTSTATUS
FxPkgPnp::AllocateDmaEnablerList(
    VOID
    )
{
    ASSERTMSG("Not implemented for UMDF\n", FALSE);

    return STATUS_NOT_IMPLEMENTED;
}

VOID
FxPkgPnp::AddDmaEnabler(
    __in FxDmaEnabler* Enabler
    )
{
    UNREFERENCED_PARAMETER(Enabler);
    ASSERTMSG("Not implemented for UMDF\n", FALSE);
}

VOID
FxPkgPnp::RemoveDmaEnabler(
    __in FxDmaEnabler* Enabler
    )
{
    UNREFERENCED_PARAMETER(Enabler);
    ASSERTMSG("Not implemented for UMDF\n", FALSE);
}

VOID
FxPkgPnp::WriteStateToRegistry(
    __in HANDLE RegKey,
    __in PUNICODE_STRING ValueName,
    __in ULONG Value
    )
{
    UNREFERENCED_PARAMETER(RegKey);

    //
    // Failure to save the user's idle/wake settings is not critical and we 
    // will continue on regardless. Hence we ignore the return value.
    //
    (void) UmToMx::WriteStateToRegistry(m_Device->GetDeviceObject(),
                                        ValueName, 
                                        Value);
    
    return;
}
 
NTSTATUS
FxPkgPnp::UpdateWmiInstanceForS0Idle(
    __in FxWmiInstanceAction Action
    )
{
    NTSTATUS status;

    //
    // send an IOCTL to redirector to add/remove S0Idle WMI instance 
    //
    status = UmToMx::UpdateWmiInstanceForS0Idle(
        m_Device->GetDeviceObject(), 
        Action);
    
    return status;;
}

VOID
FxPkgPnp::ReadRegistryS0Idle(
    __in PCUNICODE_STRING ValueName,
    __out BOOLEAN *Enabled
    )
{
    NTSTATUS status;
    ULONG value;

    status = UmToMx::ReadStateFromRegistry(
        m_Device->GetDeviceObject(), 
        ValueName, 
        &value);

    //
    // Modify value of Enabled only if success
    //
    if (NT_SUCCESS(status)) {
        //
        // Normalize the ULONG value into a BOOLEAN
        //
        *Enabled = (value == FALSE) ? FALSE : TRUE;
    }
}

NTSTATUS
FxPkgPnp::UpdateWmiInstanceForSxWake(
    __in FxWmiInstanceAction Action
    )
{
    NTSTATUS status;

    //
    // send an IOCTL to redirector to add/remove S0Idle WMI instance 
    //
    status = UmToMx::UpdateWmiInstanceForSxWake(
        m_Device->GetDeviceObject(), 
        Action);
    
    return status;;
}

VOID
FxPkgPnp::ReadRegistrySxWake(
    __in PCUNICODE_STRING ValueName,
    __out BOOLEAN *Enabled
    )
{
    NTSTATUS status;
    ULONG value;

    status = UmToMx::ReadStateFromRegistry(
        m_Device->GetDeviceObject(), 
        ValueName, 
        &value);

    //
    // Modify value of Enabled only if success
    //
    if (NT_SUCCESS(status)) {
        //
        // Normalize the ULONG value into a BOOLEAN
        //
        *Enabled = (value == FALSE) ? FALSE : TRUE;
    }
}

VOID
PnpPassThroughQIWorker(
    __in    MxDeviceObject* Device,
    __inout FxIrp* Irp,
    __inout FxIrp* ForwardIrp
    )
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(ForwardIrp);
    ASSERTMSG("Not implemented for UMDF\n", FALSE);
}


VOID
FxPkgPnp::RevokeDmaEnablerResources(
    __in FxDmaEnabler * /* DmaEnabler */
    )
{
    // Do nothing
}

