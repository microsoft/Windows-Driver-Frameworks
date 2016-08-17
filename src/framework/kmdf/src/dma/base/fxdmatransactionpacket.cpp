/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDmaTransactionPacket.cpp

Abstract:

    WDF DMA Transaction Object for Packet-based DMA

Environment:

    Kernel mode only.

Notes:


Revision History:

--*/

#include "FxDmaPCH.hpp"

extern "C" {
#include "FxDmaTransactionPacket.tmh"
}

FxDmaPacketTransaction::FxDmaPacketTransaction(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in USHORT ObjectSize,
    __in USHORT ExtraSize,
    __in FxDmaEnabler *DmaEnabler
    ) :
    FxDmaTransactionBase(FxDriverGlobals, ObjectSize, ExtraSize, DmaEnabler)
{
    m_MapRegistersNeeded  = 0;
    m_MapRegisterBase     = NULL;
    m_MapRegisterBaseSet = FALSE;
    m_DeviceAddressOffset = 0;
    m_MapRegistersReserved = 0;

    m_IsCancelled = FALSE;

    m_TransferState.CurrentStagingThread = NULL;
    m_TransferState.RerunStaging = FALSE;
    m_TransferState.RerunCompletion = FALSE;
    m_TransferState.CompletionStatus = UNDEFINED_DMA_COMPLETION_STATUS;
}

_Must_inspect_result_
NTSTATUS
FxDmaPacketTransaction::_Create(
    __in  PFX_DRIVER_GLOBALS      FxDriverGlobals,
    __in  PWDF_OBJECT_ATTRIBUTES  Attributes,
    __in  FxDmaEnabler*           DmaEnabler,
    __out WDFDMATRANSACTION*      Transaction
    )
{
    FxDmaPacketTransaction* pTransaction;
    WDFOBJECT hTransaction;
    NTSTATUS status;

    pTransaction = new (FxDriverGlobals, Attributes, DmaEnabler->GetTransferContextSize())
                FxDmaPacketTransaction(FxDriverGlobals, 
                                       sizeof(FxDmaPacketTransaction), 
                                       DmaEnabler->GetTransferContextSize(),
                                       DmaEnabler);

    if (pTransaction == NULL) {
        status =  STATUS_INSUFFICIENT_RESOURCES;
        DoTraceLevelMessage(
            FxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
            "Could not allocate memory for WDFTRANSACTION, %!STATUS!", status);
        return status;
    }

    //
    // Commit and apply the attributes
    //
    status = pTransaction->Commit(Attributes, &hTransaction, DmaEnabler);
    if (NT_SUCCESS(status)) {
        *Transaction = (WDFDMATRANSACTION)hTransaction;
    }
    else {
        //
        // This will properly clean up the target's state and free it
        //
        pTransaction->DeleteFromFailedCreate();
    }

    return status;
}

_Must_inspect_result_
NTSTATUS
FxDmaPacketTransaction::InitializeResources(
    VOID
    )
{
    KIRQL oldIrql;
    m_DeviceAddressOffset = 0;
    LockTransferState(&oldIrql);
    m_IsCancelled = FALSE;
    UnlockTransferState(oldIrql);
    return STATUS_SUCCESS;
}

VOID
FxDmaPacketTransaction::ReleaseResources(
    __in BOOLEAN ForceRelease
    )
{
    //
    // If the map register base hasn't been assigned, then just 
    // skip this.
    //

    if (IsMapRegisterBaseSet() == FALSE) {
        return;
    }

    //
    // Map registers are reserved.  Unless the caller is forcing
    // us to free them, just return.  Otherwise updated the 
    // number of map registers that FreeMapRegistersAndAdapter 
    // is going to look at.
    //
    if ((m_MapRegistersReserved > 0) && (ForceRelease == FALSE))
    {
        return;
    }

    //
    // Free the map registers and release the device.
    //
    FreeMapRegistersAndAdapter();

    ClearMapRegisterBase();

    ReleaseDevice();

    m_AdapterInfo = NULL;
    m_MapRegistersReserved = 0;
}

_Must_inspect_result_
NTSTATUS
FxDmaPacketTransaction::ReserveAdapter(
    __in     ULONG               NumberOfMapRegisters,
    __in     WDF_DMA_DIRECTION   DmaDirection,
    __in     PFN_WDF_RESERVE_DMA Callback,
    __in_opt PVOID               Context
    )
{
    NTSTATUS status;
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();
    WDFDMATRANSACTION dmaTransaction = GetHandle();

    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                            "Enter WDFDMATRANSACTION %p", dmaTransaction);
    }

    //
    // If caller doesn't supply a map register count then we get the count
    // out of the transaction.  So the transaction must be initialized.
    //
    // Otherwise the transaction can't be executing.
    //
    if (NumberOfMapRegisters == 0) {
        if (m_State != FxDmaTransactionStateInitialized) {
            status = STATUS_INVALID_PARAMETER;

            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                                "RequiredMapRegisters cannot be 0 because "
                                "WDFDMATRANSACTION %p is not initialized ("
                                "state is %!FxDmaTransactionState!) - %!STATUS!",
                                GetHandle(),
                                m_State,
                                status);
            FxVerifierBugCheck(pFxDriverGlobals,               // globals
                               WDF_DMA_FATAL_ERROR,            // specific type
                               (ULONG_PTR) GetObjectHandle(),  // parm 2
                               (ULONG_PTR) m_State);           // parm 3
        }
    }
    else if (m_State != FxDmaTransactionStateCreated &&
             m_State != FxDmaTransactionStateInitialized && 
             m_State != FxDmaTransactionStateReleased) {

        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                    "WDFDMATRANSACTION %p state %!FxDmaTransactionState! "
                    "is invalid", dmaTransaction, m_State);

        FxVerifierBugCheck(pFxDriverGlobals,               // globals
                           WDF_DMA_FATAL_ERROR,            // specific type
                           (ULONG_PTR) GetObjectHandle(),  // parm 2
                           (ULONG_PTR) m_State);           // parm 3
    }

    //
    // Must not already have reserved map registers
    //
    if (m_MapRegistersReserved != 0) 
    {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                    "WDFDMATRANSACTION %p already has allocated map registers.",
                    dmaTransaction);

        FxVerifierBugCheck(pFxDriverGlobals,               // globals
                           WDF_DMA_FATAL_ERROR,            // specific type
                           (ULONG_PTR) GetObjectHandle(),  // parm 2
                           (ULONG_PTR) m_State);           // parm 3
    }

    //
    // Get the adapter
    //
    if (DmaDirection == WdfDmaDirectionReadFromDevice) {
        m_AdapterInfo = m_DmaEnabler->GetReadDmaDescription();
    } else {
        m_AdapterInfo = m_DmaEnabler->GetWriteDmaDescription();
    }

    //
    // Save the number of map registers being reserved.
    //
    if (NumberOfMapRegisters != 0) {
        
        // 
        // Use the number the caller passed us
        //
        m_MapRegistersReserved = NumberOfMapRegisters;
    }
    else if (m_DmaEnabler->IsBusMaster() == FALSE) {

        //
        // For system DMA use all the map registers we have
        //
        m_MapRegistersReserved = m_AdapterInfo->NumberOfMapRegisters;

    } else {

        //
        // Compute the number of map registers required based on
        // the MDL and length passed in
        //
        status = _CalculateRequiredMapRegisters(
                    m_StartMdl,
                    m_StartOffset,
                    (ULONG) m_TransactionLength,
                    m_AdapterInfo->NumberOfMapRegisters,
                    NULL,
                    &m_MapRegistersReserved
                    );

        if (!NT_SUCCESS(status)) {
            ReleaseForReuse(TRUE);
            goto End;
        }
    }

    //
    // Initialize the DmaTransaction object with enough data to 
    // trick StartTransfer into allocating the adapter channel for us.
    //
    m_DmaDirection       = DmaDirection;
    m_StartMdl           = NULL;
    m_StartOffset        = 0;
    m_CurrentFragmentMdl = NULL;
    m_CurrentFragmentOffset = 0;
    m_Remaining          = 0;
    m_TransactionLength  = 0;

    //
    // Save the callback and context
    //
    m_DmaAcquiredFunction.Method.ReserveDma = Callback;
    m_DmaAcquiredContext = Context;

    //
    // If needed, initialize the transfer context.
    //
    if (m_DmaEnabler->UsesDmaV3()) {
        m_DmaEnabler->InitializeTransferContext(GetTransferContext(), 
                                                m_DmaDirection);
    }

    status = InitializeResources();
    if (NT_SUCCESS(status)) {
        //
        // Set the state to reserved so _AdapterControl knows which 
        // callback to invoke.
        //
        m_State = FxDmaTransactionStateReserved;
    } else {
        ReleaseForReuse(TRUE);
        goto End;
    }

    //
    // Start the adapter channel allocation through StartTransfer
    //
    status = StartTransfer();

End:
    if (!NT_SUCCESS(status)) {
        m_State = FxDmaTransactionStateTransferFailed;
        m_DmaAcquiredFunction.Method.ReserveDma = NULL;
        m_DmaAcquiredContext = NULL;
        m_MapRegistersReserved = 0;

        if (m_EncodedRequest != NULL) {
            ReleaseButRetainRequest();
        }
    }

    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                            "Exit WDFDMATRANSACTION %p, %!STATUS!",
                            dmaTransaction, status);
    }

    return status;
}

VOID
FxDmaPacketTransaction::ReleaseAdapter(
    VOID
    )
{
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();
    WDFDMATRANSACTION dmaTransaction = GetHandle();
    
    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                            "Enter WDFDMATRANSACTION %p", dmaTransaction);
    }

    //
    // Must not be in invalid, created, transfer or deleted state
    //
    if (m_State == FxDmaTransactionStateInvalid ||
        m_State == FxDmaTransactionStateCreated ||
        m_State == FxDmaTransactionStateTransfer ||
        m_State == FxDmaTransactionStateDeleted) {

        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                    "WDFDMATRANSACTION %p state %!FxDmaTransactionState! "
                    "is invalid", dmaTransaction, m_State);

        FxVerifierBugCheck(pFxDriverGlobals,               // globals
                           WDF_DMA_FATAL_ERROR,            // specific type
                           (ULONG_PTR) GetObjectHandle(),  // parm 2
                           (ULONG_PTR) m_State);           // parm 3
    }

    //
    // The caller wants to free the reserved map registers, so force their
    // release.
    //
    ReleaseForReuse(TRUE);

    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                            "Exit WDFDMATRANSACTION %p",
                            dmaTransaction);
    }

    return;
}

_Must_inspect_result_
NTSTATUS
FxDmaPacketTransaction::StartTransfer(
    VOID
    )
{
    NTSTATUS           status;
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();
    WDFDMATRANSACTION  dmaTransaction;

    //
    // Client driver could complete and delete the object in
    // EvtProgramDmaFunction. So, save the handle because we need it
    // for tracing after we invoke the callback.
    //
    dmaTransaction = GetHandle();

    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                            "Enter WDFDMATRANSACTION %p",
                            dmaTransaction);

        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA, 
                            "Starting WDFDMATRANSACTION %p - MDL %#p, "
                            "offset %I64x, length %I64x",
                            dmaTransaction,
                            m_StartMdl,
                            m_StartOffset,
                            m_TransactionLength);
    }

    //
    // Reference the device when using DMA v2.  For DMA v3 we can support
    // concurrent attempts to allocate the channel.
    //
    status = AcquireDevice();
    if (!NT_SUCCESS(status)) {

        NT_ASSERTMSG("AcquireDevice should never fail when DMAv3 is in use", 
                     m_DmaEnabler->UsesDmaV3());

        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                            "Only one transaction can be queued "
                            "at one time on a packet based WDFDMAENABLER %p "
                            "%!STATUS!", m_DmaEnabler->GetHandle(),
                            status);
        FxVerifierDbgBreakPoint(pFxDriverGlobals);
        return status;
    }

    //
    // Calculate the initial DMA transfer length.
    //
    m_CurrentFragmentLength = FxSizeTMin(m_Remaining, m_MaxFragmentLength);

    m_CurrentFragmentOffset = m_StartOffset;

    if (m_State == FxDmaTransactionStateReserved) {
        //
        // Caller is simply reserving the DMA adapter for later use.  Ask for 
        // as many map registers as the driver requested.
        //
        m_MapRegistersNeeded = m_MapRegistersReserved;

        ASSERT(m_MapRegistersNeeded <= m_AdapterInfo->NumberOfMapRegisters);
    
        status = AllocateAdapterChannel(FALSE);

    }
    else {

        if (m_DmaEnabler->IsBusMaster() == FALSE) {

            //
            // Use as many map registers as we were granted.
            //
            m_MapRegistersNeeded = m_AdapterInfo->NumberOfMapRegisters;
        } else {

            //
            // If the transfer is the size of the transaction then use the offset
            // to determine the number of map registers needed.  If it's smaller
            // then use the worst-case offset to make sure we ask for enough MR's
            // to account for a bigger offset in one of the later transfers.
            //
            // Example:
            //  Transaction is 8 KB and is page aligned
            //      if max transfer is >= 8KB then this will be one transfer and only 
            //      requires two map registers.  Even if the driver completes a partial 
            //      transfer and we have to do the rest in a second transfer it will 
            //      fit within two map registers becuase the overall transaction does
            //      (and a partial transfer can't take more map registers than the 
            //       whole transaction would).
            //
            //      If max transfer is 2KB then this nominally requires 4 2KB transfers.
            //      In this case however, a partial completion of one of those transfers
            //      would leave us attempting a second 2KB transfer starting on an 
            //      unaligned address.  For example, we might transfer 2KB, then 1KB
            //      then 2KB.  Even though the first transfer was page aligned, the 
            //      3rd transfer isn't and could cross a page boundary, requiring two
            //      map registers rather than one.
            //
            // To account for this second case, ignore the actual MDL offset and 
            // instead compute the maximum number of map registers than an N byte
            // transfer could take (with worst-case alignment).
            //
            //
            m_MapRegistersNeeded = 
                (ULONG) ADDRESS_AND_SIZE_TO_SPAN_PAGES(
                    ((m_CurrentFragmentLength == m_Remaining) ? 
                        GetStartVaFromOffset(m_CurrentFragmentMdl, 
                                             m_CurrentFragmentOffset) : 
                        (PVOID)(ULONG_PTR) (PAGE_SIZE -1)), 
                    m_CurrentFragmentLength
                    );


            ASSERT(m_MapRegistersNeeded <= m_AdapterInfo->NumberOfMapRegisters);
        }

        //
        // NOTE: the number of map registers needed for this transfer may 
        //       exceed the number that we've reserved.  StageTransfer will
        //       take care of fragmenting the transaction accordingly.
        //
        status = AllocateAdapterChannel(m_MapRegistersReserved > 0);
    }

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                            "AllocateAdapterChannel failed for "
                            "WDFDMATRANSACTION %p, %!STATUS!",
                            dmaTransaction, status);
        ReleaseDevice();
    }

    //
    // Before AllocateAdapterChannel returns, _AdapterControl can get called
    // on another thread and the driver could delete the transaction object.
    // So don't touch the object after this point.
    //
    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                        "Exit WDFDMATRANSACTION %p, "
                        "%!STATUS!", dmaTransaction, status);
    }

    return status;
}

IO_ALLOCATION_ACTION
FxDmaPacketTransaction::_AdapterControl(
    __in PDEVICE_OBJECT  DeviceObject,
    __in PIRP            Irp,
    __in PVOID           MapRegisterBase,
    __in PVOID           Context
    )
{
    FxDmaPacketTransaction * pDmaTransaction;
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    IO_ALLOCATION_ACTION action;
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(DeviceObject);

    pDmaTransaction = (FxDmaPacketTransaction*) Context;
    ASSERT(pDmaTransaction);

    pFxDriverGlobals = pDmaTransaction->GetDriverGlobals();

    //
    // Cache the return value while we can still touch the transaction
    //
    action = pDmaTransaction->GetAdapterControlReturnValue();

    //
    // Save the MapRegister base, unless it was previously set 
    // during a reserve.
    //
    if (pDmaTransaction->IsMapRegisterBaseSet() == FALSE) {
        pDmaTransaction->SetMapRegisterBase(MapRegisterBase);
    }
    else {
        NT_ASSERTMSG("Caller was expected to use existing map register base", 
                     MapRegisterBase == pDmaTransaction->m_MapRegisterBase);
    }

    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                            "Map registers for WDFDMATRANSACTION %p allocated "
                            "(base %p)",
                            pDmaTransaction->GetHandle(),
                            MapRegisterBase);
    }

    //
    // NOTE: KMDF used to call KeFlushIoBuffers() here to "ensure the 
    //       data buffers were flushed."  However KeFlushIoBuffers did 
    //       nothing on x86 & amd64 (which are cache coherent WRT DMA) 
    //       and calling FlushAdapterBuffers() does any necessary 
    //       flushing anyway.  Plus on non-cache-coherent architectures
    //       (such as ARM) the flush operation has to be cache-line aligned
    //       to avoid cache line tearing.  So the flush is not necessary
    //       and has been removed.

    //
    // Check the state of the transaction.  If it's reserve then call the 
    // reserve callback and return.  Otherwise stage the first fragment.
    //
    if (pDmaTransaction->m_State == FxDmaTransactionStateReserved)
    {
        FxDmaTransactionProgramOrReserveDma callback;
        
        //
        // Save off and clear the callback before calling it.
        //
        callback = pDmaTransaction->m_DmaAcquiredFunction;
        pDmaTransaction->m_DmaAcquiredFunction.Clear();

        ASSERTMSG("Mismatch between map register counts", 
                  (pDmaTransaction->m_MapRegistersReserved ==
                   pDmaTransaction->m_MapRegistersNeeded));

        //
        // Invoke the callback.  Note that from here the driver may initialize
        // and execute the transaction.
        //
        callback.InvokeReserveDma(
            pDmaTransaction->GetHandle(), 
            pDmaTransaction->m_DmaAcquiredContext
            );
    }
    else {

        //
        // Stage next fragment
        //
        status = pDmaTransaction->StageTransfer();

        if (!NT_SUCCESS(status)) {

            DMA_COMPLETION_STATUS dmaStatus = 
                (status == STATUS_CANCELLED ? DmaCancelled : DmaError);
            FxDmaSystemTransaction* systemTransaction = (FxDmaSystemTransaction*) pDmaTransaction;

            //
            // Map transfer failed.  There will be no DMA completion callback 
            // and no call to EvtProgramDma.  And we have no way to hand this 
            // status back directly to the driver.  Fake a DMA completion with 
            // the appropriate status.
            //
            // This should only happen for system DMA (and there most likely 
            // only during cancelation, though we leave the possibility that 
            // the DMA extension may fail the transfer)
            //
            ASSERTMSG("Unexpected failure of StageTransfer for packet based "
                      "DMA", 
                       (pDmaTransaction->GetDmaEnabler()->IsBusMaster() == false));

            if (pFxDriverGlobals->FxVerifierOn) {
                DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                    "Invoking DmaCompleted callback %p (context %p) "
                                    "for WDFDMATRANSACTION %p (status %x) "
                                    "due to staging failure (%!STATUS!)",
                                    systemTransaction->m_TransferCompleteFunction.Method,
                                    systemTransaction->m_TransferCompleteContext,
                                    pDmaTransaction->GetHandle(),
                                    dmaStatus,
                                    status);
            }

            pDmaTransaction->CallEvtDmaCompleted(
                status == STATUS_CANCELLED ? DmaCancelled : DmaError 
                );
        }
    }

    //
    // Indicate that MapRegs are to be kept
    //
    return action;
}

_Must_inspect_result_
NTSTATUS
FxDmaPacketTransaction::StageTransfer(
    VOID
    )
{
    PSCATTER_GATHER_LIST sgList;

    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    UCHAR_MEMORY_ALIGNED sgListBuffer[sizeof(SCATTER_GATHER_LIST)
                            + sizeof(SCATTER_GATHER_ELEMENT)];
    WDFDMATRANSACTION dmaTransaction;

    KIRQL oldIrql;
    BOOLEAN stagingNeeded;

    NTSTATUS status = STATUS_SUCCESS;

    //
    // Client driver could complete and delete the object in
    // EvtProgramDmaFunction. So, save the handle because we need it
    // for tracing after we invoke the callback.
    //
    pFxDriverGlobals = GetDriverGlobals();
    dmaTransaction = GetHandle();
    
    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                        "Enter WDFDMATRANSACTION %p ", dmaTransaction);
    }

    //
    // For packet base DMA, current and startMDL will always be
    // same.  For V2 DMA we don't support MDL chains.  For V3 DMA
    // we use the HAL's support for MDL chains and don't walk through 
    // the MDL chain on our own.
    //
    ASSERT(m_CurrentFragmentMdl == m_StartMdl);

    LockTransferState(&oldIrql);

    if (m_TransferState.CurrentStagingThread != NULL) {

        //
        // Staging in progress.  Indicate that another staging will 
        // be needed.
        //
        m_TransferState.RerunStaging = TRUE;

        stagingNeeded = FALSE;

        if (pFxDriverGlobals->FxVerifierOn) {
            DoTraceLevelMessage(
                pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                "Staging next fragment of WDFDMATRANSACTION %p "
                "deferred",
                dmaTransaction
                );
        }
    }
    else {
        //
        // Staging isn't in progress anyplace else.  Indicate that it's 
        // running now so that any parallel attempt is blocked.
        //
        m_TransferState.CurrentStagingThread = KeGetCurrentThread();

        ASSERTMSG("The thread which was staging didn't clear "
                  "RerunStaging", 
                  (m_TransferState.RerunStaging == FALSE));

        stagingNeeded = TRUE;
    }

    UnlockTransferState(oldIrql);

    //
    // Take a reference on the transaction so that we can safely 
    // manipulate the transfer state even after it's destroyed.
    //
    AddRef(&pFxDriverGlobals);

    //
    // Loop for as long as staging is required
    //
    while (stagingNeeded) {

        //
        // Calculate length for this packet.
        //
        m_CurrentFragmentLength = FxSizeTMin(m_Remaining, m_MaxFragmentLength);

        //
        // Calculate address for this packet.
        //
        m_CurrentFragmentOffset = m_StartOffset + m_Transferred;

        //
        // If this is a single-transfer transaction, then this is the first transfer
        // we're about to make. It was already validated in Initialize and doesn't
        // need any adjustments here.
        //
        if (m_RequireSingleTransfer == FALSE) {
            //
            // Adjust the fragment length for the number of reserved map registers.
            //
            if ((m_MapRegistersReserved > 0) && 
                (m_MapRegistersNeeded > m_MapRegistersReserved))
            {
                size_t currentOffset = m_CurrentFragmentOffset;
                size_t currentPageOffset;
                PMDL mdl;

                for (mdl = m_CurrentFragmentMdl; mdl != NULL; mdl = mdl->Next)
                {
                    //
                    // For packet/system transfers of chained MDLs, m_CurrentFragmentMdl 
                    // is never adjusted, and m_CurrentFragmentOFfset is the offset 
                    // into the entire chain.
                    //
                    // Locate the MDL which contains the current fragment.
                    //
                    ULONG mdlBytes = MmGetMdlByteCount(mdl);
                    if (mdlBytes >= currentOffset)
                    {
                        //
                        // This MDL is larger than the remaining offset, so it 
                        // contains the start address.
                        //
                        break;
                    }

                    currentOffset -= mdlBytes;
                }

                ASSERT(mdl != NULL);

                //
                // Compute page offset from current MDL's initial page offset
                // and the offset into that MDL
                //

                currentPageOffset = BYTE_OFFSET(MmGetMdlByteOffset(mdl) + 
                                                currentOffset);

                //
                // Compute the maximum number of bytes we can transfer with 
                // the number of map registers we have reserved, taking into 
                // account the offset of the first page.
                //
                size_t l =  ((PAGE_SIZE * (m_MapRegistersReserved - 1)) + 
                             (PAGE_SIZE - currentPageOffset));

                m_CurrentFragmentLength = FxSizeTMin(m_CurrentFragmentLength, l);
            }
        }

        m_Remaining -= m_CurrentFragmentLength;

        if (pFxDriverGlobals->FxVerifierOn) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                "Initiating %s transfer for WDFDMATRANSACTION %p.  "
                                "Mdl %p, Offset %I64x, Length %I64x",
                                m_Transferred == 0 ? "first" : "next",
                                GetHandle(),
                                m_CurrentFragmentMdl,
                                m_Transferred, 
                                m_CurrentFragmentLength);
        }

        //
        // Check for a pending cancellation.  This can happen if the cancel 
        // occurred between DMA completion and FlushAdapterBuffers - 
        // FlushAdapterBuffers will clear the canceled bit in the transfer 
        // context (TC), which would allow MapTransfer to succeed.
        //
        // An unprotected check of IsCancelled here is safe.  A concurrent 
        // cancel at this point would mark the TC cancelled such that 
        // MapTransfer will fail.
        //
        if (m_IsCancelled == TRUE) {
            status = STATUS_CANCELLED;
            goto End;
        }

        //
        // Profile specific work before mapping the transfer.  if this 
        // fails consider 'this' invalid.
        //
        if (PreMapTransfer() == FALSE) {
            status = STATUS_SUCCESS;
            goto End;
        }

        //
        // Map this packet for transfer.
        //

        // 
        // For packet based DMA we use a single entry on-stack SGL.  This 
        // allows us to map multiple packet-based requests concurrently and 
        // we know packet base DMA only requires a single SGL
        //
        // NOTE: It turns out the HAL doesn't handle chained MDLs in packet
        //       mode correctly.  It makes each MDL continguous, but returns
        //       a list of SG elements - one for each MDL.  That's scatter
        //       gather DMA, not packet DMA.
        //
        //       So it's actually very important in Win8 that we only use a 
        //       single entry SGL when calling MapTransferEx.  This ensures
        //       we only map the first MDL in the chain and thus get a 
        //       contiguous buffer
        //
        // For system DMA we use the SystemSGList stored in the DMA enabler
        // We can use a shared one in that case because we won't be mapping
        // multiple system DMA requests concurrently (HAL doesn't allow it)
        //
        FxDmaEnabler* enabler = GetDmaEnabler();
        size_t sgListSize;

        if (enabler->IsBusMaster()) {
            sgList = (PSCATTER_GATHER_LIST)sgListBuffer;
            sgListSize = sizeof(sgListBuffer);
        } else {
            sgList = enabler->m_SGList.SystemProfile.List;
            sgListSize = enabler->m_SGListSize;
        }

        ULONG mappedBytes;

        status = MapTransfer(sgList, 
                             (ULONG) sgListSize, 
                             GetTransferCompletionRoutine(), 
                             this, 
                             &mappedBytes);

        NT_ASSERTMSG("Unexpected failure of MapTransfer", 
                     ((NT_SUCCESS(status) == TRUE) || 
                      (status == STATUS_CANCELLED)));

        if (NT_SUCCESS(status)) {

            NT_ASSERTMSG("unexpected number of mapped bytes", 
                         ((mappedBytes > 0) && 
                          (mappedBytes <= m_CurrentFragmentLength)));

            //
            // Adjust the remaining byte count if the HAL mapped less data than we 
            // requested.
            //
            if (mappedBytes < m_CurrentFragmentLength) {
                m_Remaining += m_CurrentFragmentLength - mappedBytes;
                m_CurrentFragmentLength = mappedBytes;
            }

            //
            // Do client PFN_WDF_PROGRAM_DMA callback.
            //
            if (m_DmaAcquiredFunction.Method.ProgramDma != NULL) {

                if (pFxDriverGlobals->FxVerifierOn) {
                    DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                        "Invoking ProgramDma callback %p (context %p) for "
                                        "WDFDMATRANSACTION %p.",
                                        m_DmaAcquiredFunction.Method.ProgramDma,
                                        m_DmaAcquiredContext,
                                        GetHandle()
                                        );
                }

                //
                // Call program DMA
                //
                (VOID) m_DmaAcquiredFunction.InvokeProgramDma(
                                                GetHandle(),
                                                m_DmaEnabler->m_DeviceBase->GetHandle(),
                                                m_DmaAcquiredContext,
                                                m_DmaDirection,
                                                sgList
                                                );
            }
        }

End:
        //
        // Process any pending completion or nested staging.
        //
        {
            LockTransferState(&oldIrql);

            //
            // While staging we could either have deferred a call to the 
            // completion routine or deferred another call to stage the 
            // next fragment.  We should not ever have to do both - this 
            // would imply that the driver didn't wait for its DMA completion
            // routine to run when calling TransferComplete*.
            //
            ASSERTMSG("driver called TransferComplete with pending DMA "
                      "completion callback", 
                      !((m_TransferState.RerunCompletion == TRUE) && 
                        (m_TransferState.RerunStaging == TRUE)));

            //
            // Check for pending completion.  save the status away and clear it
            // before dropping the lock.
            //
            if (m_TransferState.RerunCompletion == TRUE) {
                DMA_COMPLETION_STATUS completionStatus;
                FxDmaSystemTransaction* systemTransaction = (FxDmaSystemTransaction*) this;

                //
                // Save the completion status for when we drop the lock.
                //
                completionStatus = m_TransferState.CompletionStatus;

                ASSERTMSG("completion needed, but status was not set or was "
                          "already cleared", 
                          completionStatus != UNDEFINED_DMA_COMPLETION_STATUS);

                ASSERTMSG("completion needed, but mapping failed so there shouldn't "
                          "be any parallel work going on", 
                          NT_SUCCESS(status));

                //
                // Clear the completion needed state.
                //
                m_TransferState.RerunCompletion = FALSE;
                m_TransferState.CompletionStatus = UNDEFINED_DMA_COMPLETION_STATUS;

                //
                // Drop the lock, call the completion routine, then take the 
                // lock again.
                //
                UnlockTransferState(oldIrql);
                if (pFxDriverGlobals->FxVerifierOn) {
                    DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                        "Invoking DmaCompleted callback %p (context %p) "
                                        "for WDFDMATRANSACTION %p (status %x) "
                                        "after deferral",
                                        systemTransaction->m_TransferCompleteFunction.Method,
                                        systemTransaction->m_TransferCompleteContext,
                                        GetHandle(),
                                        completionStatus);
                }
                CallEvtDmaCompleted(completionStatus);
                LockTransferState(&oldIrql);

                //
                // Staging is blocked, which means we aren't starting up the 
                // next transfer.  Therefore we cannot have a queued completion.
                //
                ASSERTMSG("RerunCompletion should not be set on an unstaged "
                          "transaction", 
                          m_TransferState.RerunCompletion == FALSE);
            }
            
            //
            // Capture whether another staging is needed.  If none is needed
            // then we can clear staging in progress.  
            //
            if (m_TransferState.RerunStaging == TRUE) {
                stagingNeeded = TRUE;
                m_TransferState.RerunStaging = FALSE;
            }
            else {
                m_TransferState.CurrentStagingThread = NULL;
                stagingNeeded = FALSE;
            }

            UnlockTransferState(oldIrql);
        }

#if DBG
        if (!NT_SUCCESS(status)) {
            ASSERTMSG("MapTransfer returned an error - there should not be any "
                      "deferred work.", 
                      (stagingNeeded == FALSE));
        }
#endif

    } // while(stagingNeeded)

    Release(&pFxDriverGlobals);

    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                            "Exit WDFDMATRANSACTION %p, "
                            "%!STATUS!", dmaTransaction,
                            status);
    }

    return status;
}

_Must_inspect_result_
NTSTATUS
FxDmaPacketTransaction::TransferCompleted(
    VOID
    )
{
    NTSTATUS status;
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

    //
    // Flush the buffers
    //
    status = FlushAdapterBuffers();

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                            "FlushAdapterBuffers on WDFDMATRANSACTION %p "
                            "failed, %!STATUS!",
                            GetHandle(), status);
        FxVerifierDbgBreakPoint(pFxDriverGlobals);
    }

    return status;
}

