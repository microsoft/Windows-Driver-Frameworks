//
//    Copyright (C) Microsoft.  All rights reserved.
//
#include "pnppriv.hpp"

#include <initguid.h>
#include <wdmguid.h>

extern "C" {
#if defined(EVENT_TRACING)
#include "FxPkgPnpKM.tmh"
#endif
}

NTSTATUS
FxPkgPnp::FilterResourceRequirements(
    __in IO_RESOURCE_REQUIREMENTS_LIST **IoList
    )
/*++

Routine Description:

    This routine traverses one or more alternate _IO_RESOURCE_LISTs in the input
    IO_RESOURCE_REQUIREMENTS_LIST looking for interrupt descriptor and applies
    the policy set by driver in the interrupt object to the resource descriptor.

    LBI - Line based interrupt
    MSI - Message Signalled interrupt

    Here are the assumptions made about the order of descriptors.

    - An IoRequirementList can have one or more alternate IoResourceList
    - Each IoResourceList can have one or more resource descriptors
    - A descriptor can be default (unique), preferred, or alternate descriptors
    - A preferred descriptor can have zero or more alternate descriptors (P, A, A, A..)
    - In an IoResourceList, there can be one or more LBI descriptors
      (non-pci devices)(P,A,P,A)
    - In an IoResourceList, there can be only one preferred MSI 2.2
      (single or multi message) descriptor
    - In an IoResourceList, there cannot be MSI2.2 and MSI-X descriptors
    - In an IoResourceList, there can be one or more MSI-X descriptor
    - An alternate descriptor cannot be a very first descriptor in the list


    Now with that assumption, this routines parses the list looking for interrupt
    descriptor.

    - If it finds a LBI, it starts with the very first interrupt object and applies
      the policy set by the driver to the resource descriptor.
    - If it's finds an MSI2.2 then it starts with the first interrupt object and applies
      the policy. If the MSI2.2 is a multi-message one then it loops thru looking for
      as many interrupt object as there are messages. It doesn't fail the IRP, if the
      interrupt objects are less than the messages.
    - If there is an alternate descriptor then it applies the same policy from the
      interrupt object that it used for the preceding preferred descriptor.
    - Framework always uses FULLY_SPECIFIED connection type for both LBI and MSI
      interrupts including MSI-X
    - Framework will apply the policy on the descriptor set by the driver only
      if the policy is already not included in the resource descriptor. This is
      to allow the policy set in the registry to take precedence over the hard
      coded driver policy.
    - If the driver registers filter resource requirement and applies the policy
      on its own (by escaping to WDM) then framework doesn't override that.

Arguments:

    IoList - Pointer to the list part of an IRP_MN_FILTER_RESOURCE_REQUIREMENTS.

Return Value:

    NTSTATUS

--*/
{
    ULONG altResListIndex;
    PIO_RESOURCE_REQUIREMENTS_LIST pIoRequirementList;
    PIO_RESOURCE_LIST pIoResList;

    pIoRequirementList = *IoList;

    if (pIoRequirementList == NULL) {
        return STATUS_SUCCESS;
    }

    if (IsListEmpty(&m_InterruptListHead)) {
        //
        // No interrupt objects created to filter resource requirements.
        //
        return STATUS_SUCCESS;
    }

    pIoResList = pIoRequirementList->List;

    //
    // Parse one or more alternative resource lists.
    //
    for (altResListIndex = 0;
         altResListIndex < pIoRequirementList->AlternativeLists;
         altResListIndex++) {
        PLIST_ENTRY pIntListEntryForMSI;
        PLIST_ENTRY pIntListEntryForLBI;
        BOOLEAN multiMessageMSI22Found;
        BOOLEAN previousDescMSI;
        ULONG descIndex;

        multiMessageMSI22Found = FALSE;
        previousDescMSI = FALSE;

        pIntListEntryForMSI = &m_InterruptListHead;
        pIntListEntryForLBI = &m_InterruptListHead;

        //
        // Traverse each _IO_RESOURCE_LISTs looking for interrupt descriptors
        // and call FilterResourceRequirements method so that it can apply
        // policy set on the interrupt object into the resource-descriptor.
        //

        for (descIndex = 0; descIndex < pIoResList->Count; descIndex++) {
            ULONG messageCount;
            PIO_RESOURCE_DESCRIPTOR pIoDesc;
            FxInterrupt* pInterruptInstance;

            pIoDesc = &pIoResList->Descriptors[descIndex];

            switch (pIoDesc->Type) {
            case CmResourceTypeInterrupt:

                if (FxInterrupt::_IsMessageInterrupt(pIoDesc->Flags)) {

                    previousDescMSI = TRUE;

                    //
                    // We will advance to the next interrupt object if the resource
                    // is not an alternate resource descriptor. A resource list can
                    // have a preferred and zero or more alternate resource descriptors
                    // for the same resource. We need to apply the same policy on the
                    // alternate desc that we applied on the preferred one in case one
                    // of the alernate desc is selected for this device. An alternate
                    // resource descriptor can't be the first descriptor in a list.
                    //
                    if ((pIoDesc->Option & IO_RESOURCE_ALTERNATIVE) == 0) {
                        pIntListEntryForMSI = pIntListEntryForMSI->Flink;
                    }

                    if (pIntListEntryForMSI == &m_InterruptListHead) {
                        DoTraceLevelMessage(
                            GetDriverGlobals(), TRACE_LEVEL_WARNING, TRACINGPNP,
                            "Not enough interrupt objects created for MSI by WDFDEVICE 0x%p ",
                            m_Device->GetHandle());
                        break;
                    }

                    pInterruptInstance = CONTAINING_RECORD(pIntListEntryForMSI, FxInterrupt, m_PnpList);
                    messageCount = pIoDesc->u.Interrupt.MaximumVector - pIoDesc->u.Interrupt.MinimumVector + 1;

                    if (messageCount > 1) {
                        //
                        //  PCI spec guarantees that there can be only one preferred/default
                        //  MSI 2.2 descriptor in a single list.
                        //
                        if ((pIoDesc->Option & IO_RESOURCE_ALTERNATIVE) == 0) {
#if DBG
                            ASSERT(multiMessageMSI22Found == FALSE);
#else
                            UNREFERENCED_PARAMETER(multiMessageMSI22Found);
#endif
                            multiMessageMSI22Found = TRUE;

                        }
                    }
                    else {
                        //
                        //  This is either single message MSI 2.2 or MSI-X interrupts
                        //
                        DO_NOTHING();
                    }

                    pInterruptInstance->FilterResourceRequirements(pIoDesc);
                }
                else {

                    //
                    // We will advance to next interrupt object if the desc is not an alternate
                    // descriptor. For non PCI devices, the first LBI interrupt desc can't be an
                    // alternate descriptor.
                    //
                    if ((pIoDesc->Option & IO_RESOURCE_ALTERNATIVE) == 0) {
                        pIntListEntryForLBI = pIntListEntryForLBI->Flink;
                    }

                    //
                    // An LBI can be first alternate resource if there are preceding MSI(X) descriptors
                    // listed in the list. In that case, this descriptor is the alternate interrupt resource
                    // for all of the MSI messages. As a result, we will use the first interrupt object from
                    // the list if this ends up being assigned by the system instead of MSI.
                    //
                    if (previousDescMSI) {
                        ASSERT(pIoDesc->Option & IO_RESOURCE_ALTERNATIVE);
                        pIntListEntryForLBI = m_InterruptListHead.Flink;
                        previousDescMSI = FALSE;
                    }

                    //
                    // There can be one or more LBI interrupts and each LBI interrupt
                    // could have zero or more alternate descriptors.
                    //
                    if (pIntListEntryForLBI == &m_InterruptListHead) {
                        DoTraceLevelMessage(
                            GetDriverGlobals(), TRACE_LEVEL_WARNING, TRACINGPNP,
                            "Not enough interrupt objects created for LBI by WDFDEVICE 0x%p ",
                            m_Device->GetHandle());
                        break;
                    }

                    pInterruptInstance = CONTAINING_RECORD(pIntListEntryForLBI, FxInterrupt, m_PnpList);

                    pInterruptInstance->FilterResourceRequirements(pIoDesc);
                }

                break;

            default:
                break;
            }
        }

        //
        // Since the Descriptors is a variable length list, you cannot get to the next
        // alternate list by doing pIoRequirementList->List[altResListIndex].
        // Descriptors[descIndex] will now point to the end of the descriptor list.
        // If there is another alternate list, it would be begin there.
        //
        pIoResList = (PIO_RESOURCE_LIST) &pIoResList->Descriptors[descIndex];
    }

    return STATUS_SUCCESS;
}

_Must_inspect_result_
NTSTATUS
FxPkgPnp::AllocateDmaEnablerList(
    VOID
    )
{
    FxSpinLockTransactionedList* pList;
    NTSTATUS status;
    KIRQL irql;

    if (m_DmaEnablerList != NULL) {
        return STATUS_SUCCESS;
    }

    Lock(&irql);
    if (m_DmaEnablerList == NULL) {
        pList = new (GetDriverGlobals()) FxSpinLockTransactionedList();

        if (pList != NULL) {
            m_DmaEnablerList = pList;
            status = STATUS_SUCCESS;
        }
        else {
            status = STATUS_INSUFFICIENT_RESOURCES;
        }
    }
    else {
        //
        // Already have a DMA list
        //
        status = STATUS_SUCCESS;
    }
    Unlock(irql);

    return status;
}

VOID
FxPkgPnp::AddDmaEnabler(
    __in FxDmaEnabler* Enabler
    )
{
    DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                        "Adding DmaEnabler %p, WDFDMAENABLER %p",
                        Enabler, Enabler->GetObjectHandle());

    m_DmaEnablerList->Add(GetDriverGlobals(), &Enabler->m_TransactionLink);
}

VOID
FxPkgPnp::RemoveDmaEnabler(
    __in FxDmaEnabler* Enabler
    )
{
    DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                        "Removing DmaEnabler %p, WDFDMAENABLER %p",
                        Enabler, Enabler->GetObjectHandle());

    m_DmaEnablerList->Remove(GetDriverGlobals(), &Enabler->m_TransactionLink);
}

VOID
FxPkgPnp::WriteStateToRegistry(
    __in HANDLE RegKey,
    __in PUNICODE_STRING ValueName,
    __in ULONG Value
    )
{
    ZwSetValueKey(RegKey, ValueName, 0, REG_DWORD, &Value, sizeof(Value));
}

NTSTATUS
FxPkgPnp::UpdateWmiInstanceForS0Idle(
    __in FxWmiInstanceAction Action
    )
{
    FxWmiProvider* pProvider;
    NTSTATUS status;

    switch(Action) {
    case AddInstance:
        if (m_PowerPolicyMachine.m_Owner->m_IdleSettings.WmiInstance == NULL) {
            FxWmiInstanceInternalCallbacks cb;

            cb.SetInstance = _S0IdleSetInstance;
            cb.QueryInstance = _S0IdleQueryInstance;
            cb.SetItem = _S0IdleSetItem;

            status = RegisterPowerPolicyWmiInstance(
                &GUID_POWER_DEVICE_ENABLE,
                &cb,
                &m_PowerPolicyMachine.m_Owner->m_IdleSettings.WmiInstance);

            if (!NT_SUCCESS(status)) {
                return status;
            }
        }
        else {
            pProvider = m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                WmiInstance->GetProvider();

            //
            // Enable the WMI GUID by adding the instance back to the provider's
            // list.  If there is an error, ignore it.  It just means we were
            // racing with another thread removing or adding the instance.
            //
            (void) pProvider->AddInstance(
                m_PowerPolicyMachine.m_Owner->m_IdleSettings.WmiInstance,
                TRUE
                );
        }
        break;
        
    case RemoveInstance:
        if (m_PowerPolicyMachine.m_Owner->m_IdleSettings.WmiInstance != NULL) {
            //
            // Disable the WMI guid by removing it from the provider's list of
            // instances.
            //
            pProvider = m_PowerPolicyMachine.m_Owner->m_IdleSettings.
                WmiInstance->GetProvider();

            pProvider->RemoveInstance(
                m_PowerPolicyMachine.m_Owner->m_IdleSettings.WmiInstance
                );
        }
        break;
        
    default:
        ASSERT(FALSE);
        break;
    }
  
    return STATUS_SUCCESS;;
}

VOID
FxPkgPnp::ReadRegistryS0Idle(
    __in  PCUNICODE_STRING ValueName,
    __out BOOLEAN *Enabled
    )
{
    NTSTATUS status;
    FxAutoRegKey hKey;

    status = m_Device->OpenSettingsKey(&hKey.m_Key, STANDARD_RIGHTS_READ);

    //
    // Modify the value of Enabled only if success
    //
    if (NT_SUCCESS(status)) {
        ULONG value;

        status = FxRegKey::_QueryULong(
            hKey.m_Key, ValueName, &value);

        if (NT_SUCCESS(status)) {
            //
            // Normalize the ULONG value into a BOOLEAN
            //
            *Enabled = (value == FALSE) ? FALSE : TRUE;
        }
    } 
}

NTSTATUS
FxPkgPnp::UpdateWmiInstanceForSxWake(
    __in FxWmiInstanceAction Action
    )
{
    FxWmiProvider* pProvider;
    NTSTATUS status;

    switch(Action) {
    case AddInstance:
        if (m_PowerPolicyMachine.m_Owner->m_WakeSettings.WmiInstance == NULL) {
            FxWmiInstanceInternalCallbacks cb;

            cb.SetInstance = _SxWakeSetInstance;
            cb.QueryInstance = _SxWakeQueryInstance;
            cb.SetItem = _SxWakeSetItem;

            status = RegisterPowerPolicyWmiInstance(
                &GUID_POWER_DEVICE_WAKE_ENABLE,
                &cb,
                &m_PowerPolicyMachine.m_Owner->m_WakeSettings.WmiInstance);

            if (!NT_SUCCESS(status)) {
                return status;
            }
        } else {
            pProvider = m_PowerPolicyMachine.m_Owner->m_WakeSettings.
                WmiInstance->GetProvider();

            //
            // Enable the WMI GUID by adding the instance back to the provider's
            // list.  If there is an error, ignore it.  It just means we were
            // racing with another thread removing or adding the instance.
            //
            (void) pProvider->AddInstance(
                m_PowerPolicyMachine.m_Owner->m_WakeSettings.WmiInstance,
                TRUE
                );
        }
        break;
        
    case RemoveInstance:
        if (m_PowerPolicyMachine.m_Owner->m_WakeSettings.WmiInstance != NULL) {
            //
            // Disable the WMI guid by removing it from the provider's list of
            // instances.
            //
            pProvider = m_PowerPolicyMachine.m_Owner->m_WakeSettings.
                WmiInstance->GetProvider();

            pProvider->RemoveInstance(
                m_PowerPolicyMachine.m_Owner->m_WakeSettings.WmiInstance
                );
        }
        break;

    default:
        ASSERT(FALSE);
        break;
    }

    return STATUS_SUCCESS;
}

VOID
FxPkgPnp::ReadRegistrySxWake(
    __in  PCUNICODE_STRING ValueName,
    __out BOOLEAN *Enabled
    )
{
    FxAutoRegKey hKey;
    NTSTATUS status;

    status = m_Device->OpenSettingsKey(&hKey.m_Key, STANDARD_RIGHTS_READ);

    //
    // Modify the value of Enabled only if success
    //
    if (NT_SUCCESS(status)) {
        ULONG value;

        status = FxRegKey::_QueryULong(
            hKey.m_Key, ValueName, &value);
        
        if (NT_SUCCESS(status)) {
            //
            // Normalize the ULONG value into a BOOLEAN
            //
            *Enabled = (value == FALSE) ? FALSE : TRUE;
        }
    }
}

VOID
PnpPassThroughQIWorker(
    __in    MxDeviceObject* Device,
    __inout FxIrp* Irp,
    __inout FxIrp* ForwardIrp
    )
{
    PIO_STACK_LOCATION pFwdStack, pCurStack;

    pCurStack = Irp->GetCurrentIrpStackLocation();

    ForwardIrp->SetStatus(STATUS_NOT_SUPPORTED);
    
    pFwdStack = ForwardIrp->GetNextIrpStackLocation();
    pFwdStack->MajorFunction = Irp->GetMajorFunction();
    pFwdStack->MinorFunction = Irp->GetMinorFunction();

    RtlCopyMemory(&pFwdStack->Parameters.QueryInterface,
                  &pCurStack->Parameters.QueryInterface,
                  sizeof(pFwdStack->Parameters.QueryInterface));

    ForwardIrp->SetInformation(Irp->GetInformation());
    ForwardIrp->SendIrpSynchronously(Device->GetObject());

    pFwdStack = ForwardIrp->GetNextIrpStackLocation();

    RtlCopyMemory(&pCurStack->Parameters.QueryInterface,
                  &pFwdStack->Parameters.QueryInterface,
                  sizeof(pCurStack->Parameters.QueryInterface));
}

VOID
FxPkgPnp::RevokeDmaEnablerResources(
    __in FxDmaEnabler *DmaEnabler
    )
{
    DmaEnabler->RevokeResources();
}

VOID
FxPkgPnp::QueryForD3ColdInterface(
    VOID
    )
{
    MxDeviceObject deviceObject;
    PDEVICE_OBJECT topOfStack;
    PDEVICE_OBJECT pdo;
    FxAutoIrp irp;
    NTSTATUS status;

    //
    // This function can be invoked multiple times, particularly if filters
    // send IRP_MN_QUERY_CAPABILITIES.  So bail out if the interface has already
    // been acquired.
    //

    if ((m_D3ColdInterface.InterfaceDereference != NULL) ||
        (m_D3ColdInterface.GetIdleWakeInfo != NULL) ||
        (m_D3ColdInterface.SetD3ColdSupport != NULL)) {
        return;
    }

    pdo = m_Device->GetPhysicalDevice();

    if (pdo == NULL) {
        return;
    }

    //
    // Get the top of stack device object, even though normal filters and the
    // FDO may not have been added to the stack yet to ensure that this
    // query-interface is seen by bus filters.  Specifically, in a PCI device
    // which is soldered to the motherboard, ACPI will be on the stack and it
    // needs to see this IRP.
    //
    topOfStack = IoGetAttachedDeviceReference(pdo);
    deviceObject.SetObject(topOfStack);
    if (deviceObject.GetObject() != NULL) {
        irp.SetIrp(FxIrp::AllocateIrp(deviceObject.GetStackSize()));
        if (irp.GetIrp() == NULL) {

            DoTraceLevelMessage(
                GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGPNP,
                "Failed to allocate IRP to get D3COLD_SUPPORT_INTERFACE from !devobj %p", 
                pdo);
        } else {

            //
            // Initialize the Irp
            //
            irp.SetStatus(STATUS_NOT_SUPPORTED);

            irp.ClearNextStack();
            irp.SetMajorFunction(IRP_MJ_PNP);
            irp.SetMinorFunction(IRP_MN_QUERY_INTERFACE);
            irp.SetParameterQueryInterfaceType(&GUID_D3COLD_SUPPORT_INTERFACE);
            irp.SetParameterQueryInterfaceVersion(D3COLD_SUPPORT_INTERFACE_VERSION);
            irp.SetParameterQueryInterfaceSize(sizeof(m_D3ColdInterface));
            irp.SetParameterQueryInterfaceInterfaceSpecificData(NULL);
            irp.SetParameterQueryInterfaceInterface((PINTERFACE)&m_D3ColdInterface);

            status = irp.SendIrpSynchronously(deviceObject.GetObject());

            if (!NT_SUCCESS(status)) {
                DoTraceLevelMessage(
                    GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                    "!devobj %p declined to supply D3COLD_SUPPORT_INTERFACE", 
                    pdo);

                RtlZeroMemory(&m_D3ColdInterface, sizeof(m_D3ColdInterface));
            }
        }
    }
    ObDereferenceObject(topOfStack);
}

VOID
FxPkgPnp::DropD3ColdInterface(
    VOID
    )
{
    if (m_D3ColdInterface.InterfaceDereference != NULL) {
        m_D3ColdInterface.InterfaceDereference(m_D3ColdInterface.Context);
    }

    RtlZeroMemory(&m_D3ColdInterface, sizeof(m_D3ColdInterface));
}

_Function_class_(EX_WNF_CALLBACK)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FxPkgPnp::_SleepStudyWnfCallback(
    _In_ PMxWnfSubscriptionContext SubscriptionContext,
    _In_ PVOID CallbackContext
) {
    FxPkgPnp* This = (FxPkgPnp*) CallbackContext;

#if DBG
    ASSERT(SubscriptionContext == This->m_SleepStudy->WnfContext);
#else
    UNREFERENCED_PARAMETER(SubscriptionContext);
#endif

    This->SleepStudyEvaluateDripsConstraint(FALSE);
    return STATUS_SUCCESS;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
FxPkgPnp::SleepStudyEvaluateParticipation(
    VOID
    )
/*++

Routine Description:
    This routine is invoked by the power policy state machine. Its purpose 
    is to see if the driver is a constraint to DRIPS, if it is then notify 
    WDF sub components that the driver is participating in the sleep study.

Arguments:

    N/A

Return Value:
    None

--*/
{
    NTSTATUS status;
    POWER_PLATFORM_INFORMATION platformInfo = {0};
    WNF_STATE_NAME wnfStateName = WNF_PO_DRIPS_DEVICE_CONSTRAINTS_REGISTERED;
    PSLEEP_STUDY_INTERFACE sleepStudy = NULL;

    if ((IsPowerPolicyOwner() == FALSE) ||
        FxLibraryGlobals.SleepStudyDisabled == TRUE) {
        //
        // Sleep Study is not supported
        //
        ASSERT(m_SleepStudy == NULL);
        status = STATUS_NOT_SUPPORTED;
        goto Done;
    }

    status = ZwPowerInformation(PlatformInformation, 
                                NULL, 
                                0,
                                &platformInfo, 
                                sizeof(platformInfo));
    if (!NT_SUCCESS(status) || platformInfo.AoAc == FALSE) {
        // 
        // Sleep Study is only supported on AOAC systems
        //
        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_ERROR,
                TRACINGPNP,
                "ZwPowerInformation failed aquiring AOAC state, disabling "
                "Sleep Study for WDFDEVICE 0x%p", m_Device->GetHandle());
        }
        else {
            status = STATUS_NOT_SUPPORTED;
        }
        goto Done;
    }

    sleepStudy = (PSLEEP_STUDY_INTERFACE) MxMemory::MxAllocatePoolWithTag(
                                                NonPagedPool, 
                                                sizeof(SLEEP_STUDY_INTERFACE),
                                                SLEEPSTUDY_POOL_TAG);
    if (sleepStudy == NULL) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_ERROR,
            TRACINGPNP,
            "Unable to allocate SLEEP_STUDY_INTERFACE, disabling Sleep Study for "
            "WDFDEVICE 0x%p", m_Device->GetHandle());
        status = STATUS_MEMORY_NOT_ALLOCATED;
        goto Done;
    }

    RtlZeroMemory(sleepStudy, sizeof(SLEEP_STUDY_INTERFACE));

    //
    // m_SleepStudy may be accessed asynchronously, so first we must ensure 
    // its initialized prior to assigning it to m_SleepStudy
    //
    m_SleepStudy = sleepStudy;

    //
    // Register for Wnf callback, callback may already be inflight when this
    // returns
    //
    status = MxWnf::MxSubscribeWnfStateChange(&m_SleepStudy->WnfContext,
                                              &wnfStateName,
                                              _SleepStudyWnfCallback,
                                              this,
                                              (PVOID) SLEEPSTUDY_POOL_TAG);

    if (NT_SUCCESS(status)) {
        //
        // Manualy check to see if the WNF state has been set incase the async 
        // notification fired already and we missed it.
        //
        SleepStudyEvaluateDripsConstraint(TRUE);
    }
    else {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_ERROR,
            TRACINGPNP,
            "MxSubscribeWnfStateChange failed, disabling Sleep Study for "
            "WDFDEVICE 0x%p, %!STATUS!",
            m_Device->GetHandle(), status);
    }

Done:
    if (!NT_SUCCESS(status)) {
        m_SleepStudyTrackReferences = FALSE;
    }

    // 
    // NOTE: do not set m_SleepStudy to null once it is initialized. 
    // Asynchronously WdfDeviceResume/StopIdle may call and access internal 
    // structures if m_SleepStudy is valid. m_SleepStudy can only be freed 
    // during the destruction of the device.
    //
}

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
FxPkgPnp::SleepStudyEvaluateDripsConstraint(
    _In_ BOOLEAN IgnoreWnfQueryFailure
    )
/*++

Routine Description:
    This function evaluates the WnfState for 
    WNF_PO_DRIPS_DEVICE_CONSTRAINTS_REGISTERED. This function can be called 
    twice, once after initially registering for the async WNF notification.
    This is to cover the case the notification fired prior to the driver 
    starting. The 2nd is when the async notification fires. If 
    IgnoreWnfQueryFailure is TRUE then a failure of MxQueryWnfStateData is ok 
    (the data is not populated yet).

Arguments:

    IgnoreWnfQueryFailure  - a failure of MxQueryWnfStateData is ok when TRUE

Return Value:
    None

--*/
{
    UCHAR constraintsRegistered = 0;
    ULONG bufferSize = sizeof(constraintsRegistered);
    NTSTATUS status = STATUS_SUCCESS;
    BOOLEAN isDripsConstraint;
    MdDeviceObject pdo;
    LONG initLib;
    
    //
    // Retrieve notification data at PASSIVE
    //
    status = MxWnf::MxQueryWnfStateData(m_SleepStudy->WnfContext,
                                        &constraintsRegistered,
                                        &bufferSize);
    if (!NT_SUCCESS(status)) {
        if (IgnoreWnfQueryFailure == TRUE) {
            //
            // Failure is ignored so we will try again.
            //
            status = STATUS_SUCCESS;
            goto Done;
        }
        else {
            DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_ERROR,
                TRACINGPNP,
                "MxQueryWnfStateData failed, disabling Sleep Study for "
                "WDFDEVICE 0x%p, %!STATUS!",
                m_Device->GetHandle(), status);
            goto Done;
        }
    }
    else if (constraintsRegistered == 0) {
        if (IgnoreWnfQueryFailure == TRUE) {
            //
            // Constraints are not registered yet, keep waiting.
            // Leave status successful so we keep tracking references. 
            //
            goto Done;
        }
        status = STATUS_NOT_SUPPORTED;
        goto Done;
    }

    ASSERT(constraintsRegistered == 1);
    
    isDripsConstraint = FALSE;
    pdo = m_Device->GetPhysicalDevice(); 

    //
    // Now see if this driver is a constraint
    //






    if(!NT_SUCCESS(status) || isDripsConstraint == FALSE) {
        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_ERROR,
                TRACINGPNP,
                "ZwPowerInformation failed, disabling Sleep Study for "
                "WDFDEVICE 0x%p, %!STATUS!",
                m_Device->GetHandle(), status);
        }
        status = STATUS_NOT_SUPPORTED;
        goto Done;
    }

    ASSERT(isDripsConstraint == TRUE);

    initLib = InterlockedCompareExchange(&m_SleepStudy->LibInitializing, 1, 0);

    if (initLib == 0) {
        // 
        // We won the race to initialize sleepstudy. We can get here either from
        // manually polling the WnfState after registering a subscriber or from
        // the async WNF notification indicating the state has been updated.
        //

        status = SleepstudyHelper_Initialize(&m_SleepStudy->SleepStudyLibContext, 
                                                (PVOID) m_Device);
        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_ERROR,
                TRACINGPNP,
                "SleepstudyHelper_Initialize failed, disabling for "
                "WDFDEVICE 0x%p, %!STATUS!",
                m_Device->GetHandle(), status);
        }
        //
        // Notify Components that Sleep Study is enabled
        //
        status = SleepStudyRegisterBlockingComponents();
    }

Done:
    if (!NT_SUCCESS(status)) {
        m_SleepStudyTrackReferences = FALSE;
    }
}

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
FxPkgPnp::SleepStudyStopEvaluation(
    VOID
    )
/*++

Routine Description:
    This function terminates the async WNF notification that may enable sleep 
    study components within WDF. It synchronized the thread shutdown to 
    guarantee after completion the WNF notification can no longer fire. This 
    also means and wnfContext is no longer needed.

    Its called from the Power Policy state machine when its stopping.

Arguments:

    N/A

Return Value:
    None

--*/
{
    KIRQL irql;
    PMxWnfSubscriptionContext wnfContext = NULL;

    if (m_SleepStudy == NULL) {
        return;
    }

    Lock(&irql);

    ASSERT(m_SleepStudy->WnfContext != NULL);

    wnfContext = m_SleepStudy->WnfContext;
    m_SleepStudy->WnfContext = NULL;
    Unlock(irql);

    MxWnf::MxUnsubscribeWnfStateChange(&wnfContext);
}

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
FxPkgPnp::SleepStudyStop(
    VOID
    )
{
    if (m_SleepStudy) {
        ASSERT(m_SleepStudy->WnfContext == NULL);

        if (m_SleepStudy->ComponentPowerRef != NULL) {
            SleepstudyHelper_UnregisterComponent(m_SleepStudy->ComponentPowerRef);
            m_SleepStudy->ComponentPowerRef = NULL;
        }

        if (m_SleepStudy->SleepStudyLibContext != NULL) {
            SleepstudyHelper_Uninitialize(m_SleepStudy->SleepStudyLibContext);
            m_SleepStudy->SleepStudyLibContext = NULL;
        }

        MxMemory::MxFreePool(m_SleepStudy);
        m_SleepStudy = NULL;
    }
}

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS 
FxPkgPnp::SleepStudyRegisterBlockingComponents(
    VOID
    )
/*++

Routine Description:
    Registers a Sleep Study blocking component with the Sleep Study Library.

Arguments:
    N/A

Return Value:
    N/A failure is not fatial

  --*/
{

    NTSTATUS status;
    MdDeviceObject parentPdo, thisFdo;
    GUID parentGuid, thisGuid;
    SS_COMPONENT componentPowerRef;
    UNICODE_STRING friendlyName = {0};
    KIRQL irql;
    const WCHAR powerRefFriendlyName[] = L"WDF Power References for %wZ, Driver:%S";


    DECLARE_UNICODE_STRING_SIZE(pdoFriendlyName, FRIENDLY_NAME_MAX_LENGTH);
    
    ASSERT(m_SleepStudy != NULL && m_SleepStudy->ComponentPowerRef == NULL);

    parentPdo = GetDevice()->GetPhysicalDevice();
    thisFdo = GetDevice()->GetDeviceObject();

    SleepstudyHelper_GenerateGuid(SSH_PDO, (ULONG64) parentPdo, &parentGuid);
    SleepstudyHelper_GenerateGuid(SSH_FDO, (ULONG64) thisFdo, &thisGuid);

    status = SleepstudyHelper_GetPdoFriendlyName(parentPdo, &pdoFriendlyName);
    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_ERROR,
            TRACINGPNP,
            "Unable to get pdo friendly name, disabling Sleep Study for "
            "WDFDEVICE 0x%p, %!STATUS!", m_Device->GetHandle(), status);
        goto Done;
    }

    friendlyName.Length = 0;
    friendlyName.MaximumLength = sizeof(powerRefFriendlyName) +
                                 pdoFriendlyName.Length + 
                                 sizeof(GetDriverGlobals()->Public.DriverName);

    friendlyName.Buffer = (WCHAR*) MxMemory::MxAllocatePoolWithTag(
                NonPagedPool, 
                friendlyName.MaximumLength,
                SLEEPSTUDY_POOL_TAG);
    if (friendlyName.Buffer == NULL) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_ERROR,
            TRACINGPNP,
            "Unable to allocate friendly name, disabling Sleep Study for "
            "WDFDEVICE 0x%p", m_Device->GetHandle());
        status = STATUS_MEMORY_NOT_ALLOCATED;
        goto Done;
    }

    status = RtlUnicodeStringPrintf(&friendlyName, 
                                    powerRefFriendlyName, 
                                    &pdoFriendlyName, 
                                    GetDriverGlobals()->Public.DriverName);
    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_ERROR,
            TRACINGPNP,
            "Unable create Power Ref Friendly name, disabling Sleep Study for "
            "WDFDEVICE 0x%p, %!STATUS!", m_Device->GetHandle(), status);
        goto Done;
    }

    status = SleepstudyHelper_RegisterComponentEx(m_SleepStudy->SleepStudyLibContext,
                                                  parentGuid,
                                                  thisGuid,
                                                  &friendlyName,
                                                  &componentPowerRef);
    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            GetDriverGlobals(), TRACE_LEVEL_WARNING, TRACINGPNP,
            "WDFDEVICE 0x%p failed call to SleepstudyHelper_RegisterComponentEx, "
            "Sleep Study reports are disabled for the driver power references, "
            "%!STATUS!", 
            GetDevice()->GetHandle(), status);
        goto Done;
    }
    
    //
    // WDF components can now start forwarding calls to the Sleep Study
    //
    m_SleepStudy->ComponentPowerRef = componentPowerRef;

    //
    // See comment below
    //
    SleepstudyHelper_AcquireComponentLock(m_SleepStudy->ComponentPowerRef, &irql);

    if (m_SleepStudyPowerRefIoCount != 0) {
































        SleepstudyHelper_ComponentActiveLocked(m_SleepStudy->ComponentPowerRef);
    }

    SleepstudyHelper_ReleaseComponentLock(m_SleepStudy->ComponentPowerRef, irql);

Done:
    if (friendlyName.Buffer != NULL) {
        MxMemory::MxFreePool(friendlyName.Buffer);
    }
    return status;
}
