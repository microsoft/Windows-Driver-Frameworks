//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

extern "C" {
#include "FxDmaTransactionPacket.hpp.tmh"
}

class FxDmaPacketTransaction : public FxDmaTransactionBase {

protected:
    FxDmaPacketTransaction(
        __in PFX_DRIVER_GLOBALS FxDriverGlobals,
        __in USHORT ObjectSize,
        __in USHORT ExtraSize,
        __in FxDmaEnabler *DmaEnabler
        );

public:

    _Must_inspect_result_
    virtual
    NTSTATUS
    InitializeResources(
        VOID
        );

    _Must_inspect_result_
    NTSTATUS
    ReserveAdapter(
        __in     ULONG               NumberOfMapRegisters,
        __in     WDF_DMA_DIRECTION   Direction,
        __in     PFN_WDF_RESERVE_DMA Callback,
        __in_opt PVOID               Context
        );

    VOID
    ReleaseAdapter(
        VOID
        );

    _Must_inspect_result_
    virtual
    NTSTATUS
    StartTransfer(
        VOID
        );

    _Must_inspect_result_
    virtual
    NTSTATUS
    TransferCompleted(
        VOID
        );

    _Must_inspect_result_
    virtual
    NTSTATUS
    StageTransfer(
        VOID
        );

    virtual
    VOID
    ReleaseResources(
        __in BOOLEAN ForceRelease
        );

    _Must_inspect_result_
    static
    NTSTATUS
    _Create(
        __in  PFX_DRIVER_GLOBALS      FxDriverGlobals,
        __in  PWDF_OBJECT_ATTRIBUTES  Attributes,
        __in  FxDmaEnabler*           DmaEnabler,
        __out WDFDMATRANSACTION*      Transaction
        );

    VOID
    SetDeviceAddressOffset(
        __in ULONG Offset
        )
    {
        m_DeviceAddressOffset = Offset;
    }

protected:

    //
    // Number of map registers to be used in this transfer.
    // This value is the least of the number of map registers
    // needed to satisfy the current transfer request, and the
    // number of available map registers returned by IoGetDmaAdapter.
    //
    ULONG                        m_MapRegistersNeeded;

    //
    // Opaque-value represents the map registers that the system has
    // assigned for this transfer operation. We pass this value in
    // FlushAdapterBuffers, FreeMapRegisters, and MapTransfer.
    //
    PVOID                         m_MapRegisterBase;

    //
    // TRUE when the map register base above is valid.  The HAL can give
    // us a NULL map register base when double buffering isn't required, 
    // so we can't just do a NULL test on m_MapRegisterBase to know if 
    // the DMA channel is allocated.
    //
    BOOLEAN                       m_MapRegisterBaseSet;

    //
    // For system-DMA this provides the offset from the original 
    // DeviceAddress used to compute the device register to or from which 
    // DMA should occur.
    //
    ULONG                         m_DeviceAddressOffset;
    
    //
    // 0 if the transaction has not reserved the enabler.  Otherwise
    // this is the number of map registers requested for the reservation.
    // This value persists across a reuse and reinitialization of the 
    // transaction, and is only cleared when the enabler is released.
    //
    ULONG                         m_MapRegistersReserved;

    //
    // These fields are used to defer completion or staging while another
    // thread/CPU is already staging.  They are protected by the 
    // transfer state lock.
    //
    // These values are cleared when checked, not in InitializeResources.  
    // It's possible for a transaction being staged to complete on another 
    // CPU, get reused and reinitialized.  Clearing these values in 
    // InitializeResources would destroy the state the prior call to 
    // StageTransfer depends on.
    //
    struct {

        //
        // Non-null when a staging operation is in progress on some CPU.  
        // When set any attempt to call the DMA completion routine or 
        // stage the transfer again (due to a call to TransferComplete)
        // will be deferred to this thread.
        //
        PKTHREAD                      CurrentStagingThread;

        //
        // Indicates that a nested or concurrent attempt to stage
        // the transaction was deferred.  The CurrentStagingThread
        // will restage the transaction.
        //
        BOOLEAN                       RerunStaging;

        //
        // Indicates that a nested or concurrent attempt to call the 
        // DMA completion routine occurred.  The CurrentStagingThread 
        // will call the DMA completion routine when it unwinds providing 
        // the saved CompletionStatus
        //
        BOOLEAN                       RerunCompletion;
        DMA_COMPLETION_STATUS         CompletionStatus;

    } m_TransferState;

    //
    // Indicates that the DMA transfer has been cancelled.
    // Set during StopTransfer and cleared during InitializeResources
    // Checked during StageTransfer.  Stop and Initialize should never
    // race (it's not valid for the driver to stop an uninitialized 
    // transaction).  Stop and Stage can race but in that case Stop 
    // will mark the transaction context such that MapTransfer fails 
    // (and that's protected by a HAL lock).  
    //
    // So this field does not need to be volatile or interlocked, but
    // it needs to be sized to allow atomic writes.
    //
    ULONG                           m_IsCancelled;


    virtual
    VOID
    Reuse(
        VOID
        )
    {
        return;
    }

protected:

    inline
    SetMapRegisterBase(
        __in PVOID Value
        )
    {
        NT_ASSERTMSG("Map register base is already set", 
                     m_MapRegisterBaseSet == FALSE);

        m_MapRegisterBase = Value;
        m_MapRegisterBaseSet = TRUE;
    }

    inline
    ClearMapRegisterBase(
        VOID
        )
    {
        NT_ASSERTMSG("Map register base was not set", 
                     m_MapRegisterBaseSet == TRUE);
        m_MapRegisterBaseSet = FALSE;
    }

    inline
    IsMapRegisterBaseSet(
        VOID
        )
    {
        return m_MapRegisterBaseSet;
    }

    inline
    PVOID
    GetMapRegisterBase(
        VOID
        )
    {
        NT_ASSERTMSG("Map register base is not set", 
                     m_MapRegisterBaseSet == TRUE);
        return m_MapRegisterBase;
    }

    virtual
    IO_ALLOCATION_ACTION
    GetAdapterControlReturnValue(
        VOID
        )
    {
        return DeallocateObjectKeepRegisters;
    }

    virtual
    BOOLEAN
    PreMapTransfer(
        VOID
        )
    {
        return TRUE;
    }

    _Acquires_lock_(this)
    VOID
    __drv_raisesIRQL(DISPATCH_LEVEL)
#pragma prefast(suppress:__WARNING_FAILING_TO_ACQUIRE_MEDIUM_CONFIDENCE, "transferring lock name to 'this->TransferStateLock'")
#pragma prefast(suppress:__WARNING_FAILING_TO_RELEASE_MEDIUM_CONFIDENCE, "transferring lock name to 'this->TransferStateLock'")
    LockTransferState(
        __out __drv_deref(__drv_savesIRQL) KIRQL *OldIrql
        )
    {
        Lock(OldIrql);
    }

    _Requires_lock_held_(this)
    _Releases_lock_(this)
    VOID
#pragma prefast(suppress:__WARNING_FAILING_TO_RELEASE_MEDIUM_CONFIDENCE, "transferring lock name from 'this->TransferStateLock'")
    UnlockTransferState(
        __in __drv_restoresIRQL KIRQL OldIrql
        )
    {
#pragma prefast(suppress:__WARNING_CALLER_FAILING_TO_HOLD, "transferring lock name from 'this->TransferStateLock'")
        Unlock(OldIrql);
    }

    virtual
    PDMA_COMPLETION_ROUTINE
    GetTransferCompletionRoutine(
        VOID
        )
    {
        return NULL;
    }

    static 
    IO_ALLOCATION_ACTION
    _AdapterControl(
        __in PDEVICE_OBJECT  DeviceObject,
        __in PIRP            Irp,
        __in PVOID           MapRegisterBase,
        __in PVOID           Context
        );

    _Must_inspect_result_
    NTSTATUS
    AcquireDevice(
        VOID
        )
    {
        if (m_DmaEnabler->UsesDmaV3() == FALSE)
        {
            return m_DmaEnabler->GetDevice()->AcquireDmaPacketTransaction();
        }
        else
        {
            return STATUS_SUCCESS;
        }
    }

    FORCEINLINE
    VOID
    ReleaseDevice(
        VOID
        )
    {
        if (m_DmaEnabler->UsesDmaV3() == FALSE)
        {
            m_DmaEnabler->GetDevice()->ReleaseDmaPacketTransaction();
        }
    }

    _Must_inspect_result_
    NTSTATUS
    AllocateAdapterChannel(
        __in BOOLEAN MapRegistersReserved
        )
    {
        NTSTATUS status;
        KIRQL irql;

        KeRaiseIrql(DISPATCH_LEVEL, &irql);

        if (GetDriverGlobals()->FxVerifierOn) {

            if (MapRegistersReserved == FALSE) {
                DoTraceLevelMessage(
                    GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGDMA,
                    "Allocating %d map registers for "
                    "WDFDMATRANSACTION %p",
                    m_MapRegistersNeeded,
                    GetHandle()
                    );
            }
            else {
                DoTraceLevelMessage(
                    GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGDMA,
                    "Using %d reserved map registers for "
                    "WDFDMATRANSACTION %p",
                    m_MapRegistersNeeded,
                    GetHandle()
                    );
            }
        }

        if (m_DmaEnabler->UsesDmaV3()) {
            PDMA_OPERATIONS dmaOperations = 
                m_AdapterInfo->AdapterObject->DmaOperations;

            if (MapRegistersReserved == FALSE)
            {
                status = dmaOperations->AllocateAdapterChannelEx(
                                        m_AdapterInfo->AdapterObject,
                                        m_DmaEnabler->m_FDO,
                                        GetTransferContext(),
                                        m_MapRegistersNeeded,
                                        m_Flags,
                                        _AdapterControl,
                                        this,
                                        NULL
                                        );
            }
            else {
                _AdapterControl(m_DmaEnabler->m_FDO,
                                NULL,
                                GetMapRegisterBase(),
                                this);
                status = STATUS_SUCCESS;
            }
        }
        else {

            ASSERTMSG("Prereserved map registers are not compatible with DMA V2", 
                      MapRegistersReserved == FALSE);

            status = m_AdapterInfo->AdapterObject->DmaOperations->
                        AllocateAdapterChannel(m_AdapterInfo->AdapterObject,
                                    m_DmaEnabler->m_FDO,
                                    m_MapRegistersNeeded,
                                    _AdapterControl,
                                    this);
        }

        KeLowerIrql(irql);

        if (!NT_SUCCESS(status))
        {
            DoTraceLevelMessage(
                GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGDMA,
                "Allocating DMA resources (%d map registers) for WDFDMATRANSACTION %p "
                "returned %!STATUS!",
                m_MapRegistersNeeded,
                GetHandle(),
                status
                );
        }

        return status;
    }

    FORCEINLINE
    NTSTATUS
    MapTransfer(
        __out_bcount_opt(ScatterGatherListCb) 
                 PSCATTER_GATHER_LIST     ScatterGatherList,
        __in     ULONG                    ScatterGatherListCb,
        __in_opt PDMA_COMPLETION_ROUTINE  CompletionRoutine,
        __in_opt PVOID                    CompletionContext,
        __out    ULONG                   *TransferLength
        )
    {
        //
        // Cache globals & object handle since call to MapTransferEx could 
        // result in a DmaComplete callback before returning.
        //
        PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();
        WDFDMATRANSACTION handle = GetHandle();
#if DBG
        ULONG_PTR mapRegistersRequired;

        mapRegistersRequired = ADDRESS_AND_SIZE_TO_SPAN_PAGES(
                                    GetStartVaFromOffset(m_CurrentFragmentMdl, 
                                                         m_CurrentFragmentOffset),
                                    m_CurrentFragmentLength
                                    );
        NT_ASSERTMSG("Mapping requires too many map registers", 
                     mapRegistersRequired <= m_MapRegistersNeeded);
#endif

        NTSTATUS status;

        //
        // Assume we're going to transfer the entire current fragment.  
        // MapTransfer may say otherwise.
        //

        *TransferLength = (ULONG) m_CurrentFragmentLength;

        //
        // Map the transfer.
        //

        if (pFxDriverGlobals->FxVerifierOn) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                "Mapping transfer for WDFDMATRANSACTION %p.  "
                                "MDL %p, Offset %I64x, Length %x, MapRegisterBase %p",
                                handle,
                                m_CurrentFragmentMdl,
                                m_CurrentFragmentOffset,
                                *TransferLength,
                                GetMapRegisterBase());
        }

        if (m_DmaEnabler->UsesDmaV3()) {

            PDMA_OPERATIONS dmaOperations = 
                m_AdapterInfo->AdapterObject->DmaOperations;

            status = dmaOperations->MapTransferEx(
                                 m_AdapterInfo->AdapterObject,
                                 m_CurrentFragmentMdl,
                                 GetMapRegisterBase(),
                                 m_CurrentFragmentOffset,
                                 m_DeviceAddressOffset,
                                 TransferLength,
                                 (BOOLEAN) m_DmaDirection,
                                 ScatterGatherList,
                                 ScatterGatherListCb,
                                 CompletionRoutine,
                                 CompletionContext
                                 );

            NT_ASSERTMSG(
                "With these parameters, MapTransferEx should never fail", 
                NT_SUCCESS(status) || status == STATUS_CANCELLED
                );
        }
        else {
            NT_ASSERTMSG("cannot use DMA completion routine with DMAv2", 
                         CompletionRoutine == NULL);

            NT_ASSERTMSG(
                "scatter gather list length must be large enough for at least one element",
                (ScatterGatherListCb >= (sizeof(SCATTER_GATHER_LIST) + 
                                         sizeof(SCATTER_GATHER_ELEMENT)))
                );

            //
            // This matches the assertion above.  There's no way to explain to 
            // prefast that this code path requires the caller to provide a buffer
            // of sufficient size to store the SGL.  The only case which doesn't 
            // provide any buffer is system-mode DMA and that uses DMA v3 and so 
            // won't go through this path.
            //

            __assume((ScatterGatherListCb >= (sizeof(SCATTER_GATHER_LIST) + 
                                              sizeof(SCATTER_GATHER_ELEMENT))));

            ScatterGatherList->NumberOfElements = 1;
            ScatterGatherList->Reserved = 0;
            ScatterGatherList->Elements[0].Address = 
                m_AdapterInfo->AdapterObject->DmaOperations->
                        MapTransfer(m_AdapterInfo->AdapterObject,
                                    m_CurrentFragmentMdl,
                                    GetMapRegisterBase(),
                                    GetStartVaFromOffset(m_CurrentFragmentMdl, 
                                                         m_CurrentFragmentOffset),
                                    TransferLength,
                                    (BOOLEAN) m_DmaDirection);
            ScatterGatherList->Elements[0].Length = *TransferLength;

            status = STATUS_SUCCESS;
        }

        if (pFxDriverGlobals->FxVerifierOn) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                "MapTransfer mapped next %d bytes of "
                                "WDFDMATRANSACTION %p - status %!STATUS!",
                                *TransferLength,
                                handle, 
                                status);
        }

        return status;
    }

    FORCEINLINE
    NTSTATUS
    FlushAdapterBuffers(
        VOID
        )
    {
        PDMA_OPERATIONS dmaOperations = 
            m_AdapterInfo->AdapterObject->DmaOperations;

        NTSTATUS status;

#if DBG
        ULONG_PTR mapRegistersRequired;

        mapRegistersRequired = ADDRESS_AND_SIZE_TO_SPAN_PAGES(
                                    GetStartVaFromOffset(m_CurrentFragmentMdl, 
                                                         m_CurrentFragmentOffset),
                                    m_CurrentFragmentLength
                                    );
        NT_ASSERTMSG("Mapping requires too many map registers", 
                     mapRegistersRequired <= m_MapRegistersNeeded);
#endif

        if (GetDriverGlobals()->FxVerifierOn) {
            DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                "Flushing DMA buffers for WDFDMATRANSACTION %p.  "
                                "MDL %p, Offset %I64x, Length %I64x",
                                GetHandle(),
                                m_CurrentFragmentMdl,
                                m_CurrentFragmentOffset,
                                m_CurrentFragmentLength);
        }

        if (m_DmaEnabler->UsesDmaV3()) {
            status = dmaOperations->FlushAdapterBuffersEx(
                                              m_AdapterInfo->AdapterObject,
                                              m_CurrentFragmentMdl,
                                              GetMapRegisterBase(),
                                              m_CurrentFragmentOffset,
                                              (ULONG) m_CurrentFragmentLength,
                                              (BOOLEAN) m_DmaDirection
                                              );
        }
        else if (dmaOperations->FlushAdapterBuffers(
                                m_AdapterInfo->AdapterObject,
                                m_CurrentFragmentMdl,
                                GetMapRegisterBase(),
                                GetStartVaFromOffset(m_CurrentFragmentMdl, 
                                                     m_CurrentFragmentOffset),
                                (ULONG) m_CurrentFragmentLength,
                                (BOOLEAN) m_DmaDirection) == FALSE) {
                status = STATUS_UNSUCCESSFUL;
        }
        else {
            status = STATUS_SUCCESS;
        }

        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGDMA,
                                "Flushing DMA buffers for WDFDMATRANSACTION %p ("
                                "MDL %p, Offset %I64x, Length %I64x)"
                                "completed with %!STATUS!", 
                                GetHandle(),
                                m_CurrentFragmentMdl,
                                m_CurrentFragmentOffset,
                                m_CurrentFragmentLength,
                                status);
        }

        return status;
    }

    virtual
    VOID
    FreeMapRegistersAndAdapter(
        VOID
        )
    {
        KIRQL irql;

        PVOID mapRegisterBase = GetMapRegisterBase();

        //
        // It's illegal to free a NULL map register base, even if the HAL gave it 
        // to us.
        //
        if (mapRegisterBase == NULL) {
            if (GetDriverGlobals()->FxVerifierOn) {
                DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                    "Skipping free of %d map registers for WDFDMATRANSACTION %p "
                                    "because base was NULL", 
                                    m_MapRegistersNeeded,
                                    GetHandle());
            }

            return;
        }

        //
        // Free the map registers
        //
        KeRaiseIrql(DISPATCH_LEVEL, &irql);

        if (GetDriverGlobals()->FxVerifierOn) {
            DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                "Freeing %d map registers for WDFDMATRANSACTION %p "
                                "(base %p)", 
                                m_MapRegistersNeeded,
                                GetHandle(),
                                mapRegisterBase);
        }

        //
        // If we pre-reserved map registers then Reserved contains 
        // the number to free.  Otherwise Needed is the number allocated
        // for the last transaction, which is the number to free.
        //
        m_AdapterInfo->AdapterObject->DmaOperations->
                    FreeMapRegisters(m_AdapterInfo->AdapterObject,
                                     mapRegisterBase,
                                     (m_MapRegistersReserved > 0 ? 
                                        m_MapRegistersReserved : 
                                        m_MapRegistersNeeded));
        KeLowerIrql(irql);

        return;
    }

    virtual
    VOID
    CallEvtDmaCompleted(
        __in DMA_COMPLETION_STATUS /* Status */
        )
    {
        //
        // Packet mode DMA doesn't support cancellation or 
        // completion routines.  So this should never run.
        //
        ASSERTMSG("EvtDmaCompleted is not a valid callback for "
                  "a packet-mode transaction", 
                  FALSE);
        return;
    }

    virtual
    VOID
    SetNewSgListBuffer(
        _In_ PVOID Buffer,
        _In_ ULONG Size
        )
    {
        PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

        UNREFERENCED_PARAMETER(Buffer);
        UNREFERENCED_PARAMETER(Size);

        //
        // Packet mode always uses a 1-element SG list
        // allocated on the stack, so this should never run.
        //
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
            "Cannot call SetNewSgListBuffer for Packet-based "
            "WDFDMATRANSACTION 0x%p",
            GetHandle());
        FxVerifierDbgBreakPoint(pFxDriverGlobals);
    }

    virtual
    ULONG
    GetSgListBufferSize(
        VOID
        )
    {
        PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

        //
        // Packet mode always uses a 1-element SG list
        // allocated on the stack, so this should never run.
        //
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
            "Cannot call GetSgListBufferSize for Packet-based "
            "WDFDMATRANSACTION 0x%p",
            GetHandle());
        FxVerifierDbgBreakPoint(pFxDriverGlobals);

        return (ULONG)-1;
    }

    virtual
    ULONG
    GetNumberOfAvailableMapRegisters(
        VOID
        )
    {
        return m_MapRegistersReserved > 0 ?
            m_MapRegistersReserved :
            m_AdapterInfo->NumberOfMapRegisters;
    }

};
