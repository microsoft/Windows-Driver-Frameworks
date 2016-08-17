/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDmaTransactionBase.cpp

Abstract:

    WDF DMA Transaction Object (base class)

Environment:

    Kernel mode only.

Notes:


Revision History:

--*/

#include "FxDmaPCH.hpp"

extern "C" {
#include "FxDmaTransactionBase.tmh"
}

FxDmaTransactionBase::FxDmaTransactionBase(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in USHORT ObjectSize,
    __in USHORT ExtraSize,
    __in FxDmaEnabler *DmaEnabler
    ) :
    FxNonPagedObject(
        FX_TYPE_DMA_TRANSACTION, 
        ExtraSize == 0 ? ObjectSize : COMPUTE_OBJECT_SIZE(ObjectSize, ExtraSize),
        FxDriverGlobals)
{
    m_DmaEnabler            = DmaEnabler;
    m_EncodedRequest        = NULL;

    //
    // Set more default values
    //
    Reset();

    m_State = FxDmaTransactionStateCreated;

    if (ExtraSize == 0) {
        m_TransferContext = NULL;
    } else {
        m_TransferContext = WDF_PTR_ADD_OFFSET_TYPE(
                                this,
                                COMPUTE_RAW_OBJECT_SIZE(ObjectSize), 
                                PVOID
                                );
    }

    MarkDisposeOverride(ObjectDoNotLock);
}

VOID
FxDmaTransactionBase::Reset(
    VOID
    )
{
    m_MaxFragmentLength = 0;

    m_TransactionLength = 0;
    m_DmaDirection = WdfDmaDirectionReadFromDevice;

    m_Remaining = 0;
    m_Transferred = 0;

    m_StartMdl = NULL;
    m_StartOffset = NULL;

    m_CurrentFragmentMdl = NULL;
    m_CurrentFragmentOffset = 0;
    m_CurrentFragmentLength = 0;

    m_RequireSingleTransfer = m_DmaEnabler->GetRequireSingleTransfer();

    m_Flags = 0;
    m_DmaAcquiredContext = NULL;
    m_DmaAcquiredFunction.Method.ProgramDma = NULL;
}

BOOLEAN
FxDmaTransactionBase::Dispose(
    VOID
    )
{
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

    //
    // Must not be in transfer state.
    //
    if (m_State == FxDmaTransactionStateTransfer) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                    "WDFDMATRANSACTION %p state %!FxDmaTransactionState! "
                    "is invalid", GetHandle(), m_State);

        if (pFxDriverGlobals->IsVerificationEnabled(1, 9, OkForDownLevel)) {
            FxVerifierBugCheck(pFxDriverGlobals,               // globals
                               WDF_DMA_FATAL_ERROR,            // type
                               (ULONG_PTR) GetObjectHandle(),  // parm 2
                               (ULONG_PTR) m_State);           // parm 3
        }
    }
    
    m_State = FxDmaTransactionStateDeleted;

    //
    // Release resources for this Dma Transaction.
    //
    ReleaseResources(TRUE);
    
    if (m_EncodedRequest != NULL) {
        ClearRequest();
    }

    return TRUE;
}

_Must_inspect_result_
NTSTATUS
FxDmaTransactionBase::Initialize(
    __in PFN_WDF_PROGRAM_DMA     ProgramDmaFunction,
    __in WDF_DMA_DIRECTION       DmaDirection,
    __in PMDL                    Mdl,
    __in size_t                  Offset,
    __in ULONG                   Length
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

    DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                        "Enter WDFDMATRANSACTION %p", GetHandle());
    //
    // Must be in Reserve, Created or Released state.
    //
    if (m_State != FxDmaTransactionStateCreated &&
        m_State != FxDmaTransactionStateReserved &&
        m_State != FxDmaTransactionStateReleased) {

        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                    "WDFDMATRANSACTION %p state %!FxDmaTransactionState! "
                    "is invalid", GetHandle(), m_State);

        FxVerifierBugCheck(pFxDriverGlobals,                // globals
                           WDF_DMA_FATAL_ERROR, // specific type
                           (ULONG_PTR) GetObjectHandle(),  // parm 2
                           (ULONG_PTR) m_State);           // parm 3
    }

    if (DmaDirection == WdfDmaDirectionReadFromDevice) {
        m_AdapterInfo = m_DmaEnabler->GetReadDmaDescription();
    } else {
        m_AdapterInfo = m_DmaEnabler->GetWriteDmaDescription();
    }

    //
    // Initialize the DmaTransaction object
    //

    m_MaxFragmentLength  = m_AdapterInfo->MaximumFragmentLength;
    m_DmaDirection       = DmaDirection;
    m_StartMdl           = Mdl;
    m_StartOffset        = Offset;
    m_CurrentFragmentMdl = Mdl;
    m_CurrentFragmentOffset = Offset;
    m_Remaining          = Length;
    m_TransactionLength  = Length;
    m_DmaAcquiredFunction.Method.ProgramDma = ProgramDmaFunction;

    //
    // If needed, initialize the transfer context.
    //

    if (m_DmaEnabler->UsesDmaV3()) {
        m_DmaEnabler->InitializeTransferContext(GetTransferContext(), 
                                                m_DmaDirection);
    }

    //
    // If the transaction needs to complete in a single transfer,
    // make sure that the total length is within the device's
    // transfer limits, we reserved enough map registers, the
    // device can handle enough SG list elements, and if necessary,
    // pre-allocate a larger SG list buffer.
    //
    if (m_RequireSingleTransfer) {
        //
        // The enabler's maximum transfer length may have been reduced
        // from the driver-specified value due to a HAL map registers
        // limit. This limit might be inaccurate, so disregard it for
        // single-transfer transactions. The transaction's resource
        // requirements will be validated by GetDmaTransferInfo.
        //
        if (m_MaxFragmentLength < m_DmaEnabler->m_MaximumLength) {
            m_MaxFragmentLength = m_DmaEnabler->m_MaximumLength;
        }

        status = PrepareForSingleTransfer();
    }

    if (NT_SUCCESS(status)) {
        status = InitializeResources();
    }
    
    if (NT_SUCCESS(status)) {
        m_State = FxDmaTransactionStateInitialized;
    } else {
        ReleaseForReuse(FALSE);
    }

    DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                        "Exit WDFDMATRANSACTION %p, %!STATUS!",
                        GetHandle(), status);

    return status;
}

_Must_inspect_result_
NTSTATUS
FxDmaTransactionBase::PrepareForSingleTransfer(
    VOID
    )
/*++

Routine Description:

    This function checks whether the transaction can be fulfilled
    by a single DMA transfer. It should only be called during
    transaction initialize. The transaction must have been marked
    with m_RequireSingleTransfer = TRUE by the client driver.

    The requirements are:

    1. Transaction's total length is at most the device's maximum
       transfer size.

    2. The number of map registers needed to map the transaction
       is not larger than the number the DMA adapter has reserved.

    3. The length of the Scatter/Gather list that describes the
       mapped transfer is within the device's supported limit.

    4. The preallocated SG list buffer is large enough to hold
       the SG list needed to describe the transfer. If it is not,
       then we can allocate a large enough buffer from non-paged
       system memory to replace it.

Arguments:

    None

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

    ASSERT(GetDmaEnabler()->UsesDmaV3());
    ASSERT(m_RequireSingleTransfer);

    if (m_TransactionLength > m_MaxFragmentLength) {
        status = STATUS_WDF_TOO_MANY_TRANSFERS;
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
            "WDFDMATRANSACTION %p cannot complete in a single transfer: "
            "transfer size %Iu, device limit is %Iu, %!STATUS!",
            GetHandle(), m_TransactionLength, m_MaxFragmentLength, status);
        return status;
    }

    DMA_TRANSFER_INFO info;
    info.Version = DMA_TRANSFER_INFO_VERSION1;

    status = m_AdapterInfo->AdapterObject->DmaOperations->GetDmaTransferInfo(
        m_AdapterInfo->AdapterObject,
        m_StartMdl,
        m_StartOffset,
        (ULONG)m_TransactionLength,
        m_DmaDirection == WDF_DMA_DIRECTION::WdfDmaDirectionWriteToDevice,
        &info);
    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
            "WDFDMATRANSACTION %p GetDmaTransferInfo failed %!STATUS!",
            GetHandle(), status);
        return status;
    }

    ULONG requiredMapRegisters = info.V1.MapRegisterCount;
    ULONG requiredSgListElements = info.V1.ScatterGatherElementCount;
    ULONG requiredSgListSize = info.V1.ScatterGatherListSize;

    if (requiredMapRegisters > GetNumberOfAvailableMapRegisters()) {
        status = STATUS_WDF_NOT_ENOUGH_MAP_REGISTERS;
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
            "WDFDMATRANSACTION %p cannot complete in a single transfer: "
            "requires %lu map registers, limit is %lu, %!STATUS!",
            GetHandle(), requiredMapRegisters,
            m_AdapterInfo->NumberOfMapRegisters, status);
        return status;
    }

    if (requiredSgListElements > m_DmaEnabler->m_MaxSGElements) {
        status = STATUS_WDF_TOO_FRAGMENTED;
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
            "WDFDMATRANSACTION %p cannot complete in a single transfer: "
            "requires %lu SG elements, device limit is %lu, %!STATUS!",
            GetHandle(), requiredSgListElements,
            m_DmaEnabler->m_MaxSGElements, status);
        return status;
    }

    if (m_DmaEnabler->IsPacketProfile() && requiredSgListElements > 1) {
        status = STATUS_WDF_TOO_FRAGMENTED;
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
            "WDFDMATRANSACTION %p cannot complete in a single transfer: "
            "requires %lu SG elements, packet profile limit is 1, %!STATUS!",
            GetHandle(), requiredSgListElements, status);
        return status;
    }

    //
    // Reallocate SG lists as needed for Scatter/Gather and System
    // profile DMA. Packet mode DMA always uses a 1-element list
    // allocated on the stack, so skip it.
    //
    // It's possible that the preallocated SG list buffer is not large
    // enough to describe this transfer, because it was calculated with
    // the assumption that all transfers will need a SG element for each
    // used map register, for up to the adapter's number of reserved map
    // registers.
    //
    // However, for single-transfer transactions we get the map register
    // number from GetDmaTransferInfo, which might reveal that HAL does
    // not use map registers to map the transfer, and is thus not subject
    // to this bound. Therefore, we may need more SG elements and a larger
    // list buffer.
    //
    if (m_DmaEnabler->m_IsSGListAllocated &&
        requiredSgListSize > GetSgListBufferSize()) {

        //
        // Attempt to allocate a sufficiently large list buffer
        //
        PVOID buffer = ExAllocatePoolWithTag(NonPagedPool,
                                             requiredSgListSize,
                                             pFxDriverGlobals->Tag);
        if (buffer == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            DoTraceLevelMessage(
                GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGDMA,
                "WDFDMAENABLER %p unable to reallocate SG list of size "
                "0x%X (currently 0x%IX) by WDFDMATRANSACTION %p, %!STATUS!",
                m_DmaEnabler->GetHandle(), requiredSgListSize,
                m_DmaEnabler->m_SGListSize, GetHandle(), status);
            return status;
        }

        //
        // Free the old SG list buffer and save the new one
        //
        DoTraceLevelMessage(
            GetDriverGlobals(), TRACE_LEVEL_INFORMATION, TRACINGDMA,
            "WDFDMAENABLER %p reallocated SG list buffer of size "
            "0x%X (was 0x%IX) by WDFDMATRANSACTION %p",
            m_DmaEnabler->GetHandle(), requiredSgListSize,
            m_DmaEnabler->m_SGListSize, GetHandle());

        SetNewSgListBuffer(buffer, requiredSgListSize);
    }

    return status;
}

_Must_inspect_result_
NTSTATUS
FxDmaTransactionBase::Execute(
    __in PVOID          Context
    )
{
    NTSTATUS status;
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

    //
    // Must be in Initialized state.
    //
    if (m_State != FxDmaTransactionStateInitialized) {

        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                    "WDFDMATRANSACTION %p state %!FxDmaTransactionState! "
                    "is invalid", GetHandle(), m_State);

        FxVerifierBugCheck(pFxDriverGlobals,                // globals
                           WDF_DMA_FATAL_ERROR, // specific type
                           (ULONG_PTR) GetObjectHandle(),  // parm 2
                           (ULONG_PTR) m_State);           // parm 3
    }

    //
    // If this was initialized with a request, then reference the 
    // request now.
    //
    if (m_EncodedRequest != NULL) {
        ReferenceRequest();
    }

    //
    // Set state to Transfer.
    // This is necessary because the Execute path complete
    // all the way to DmaCompleted before returning to this point.
    //
    m_State = FxDmaTransactionStateTransfer;

    //
    // Save the caller's context
    //
    m_DmaAcquiredContext = Context;

    ASSERT(m_Transferred == 0);
    ASSERT(m_CurrentFragmentLength == 0);

    status = StartTransfer();
    if (!NT_SUCCESS(status)) {
        m_State = FxDmaTransactionStateTransferFailed;
        m_DmaAcquiredContext = NULL;
        
        if (m_EncodedRequest != NULL) {
            ReleaseButRetainRequest();
        }
    }

    //
    // StartTransfer results in a call to the EvtProgramDma routine
    // where driver could complete and delete the object.  So 
    // don't touch the object beyond this point.
    //

    return status;
}

BOOLEAN
FxDmaTransactionBase::DmaCompleted(
    __in  size_t      TransferredLength,
    __out NTSTATUS  * ReturnStatus,
    __in  FxDmaCompletionType CompletionType
    )
{
    BOOLEAN         hasTransitioned;
    NTSTATUS        status;
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();
    WDFDMATRANSACTION dmaTransaction;

    //
    // In the case of partial completion, we will start a new transfer
    // from with in this function by calling StageTransfer. After that
    // call, we lose ownership of the object.  Since we need the handle 
    // for tracing purposes, we will save the value in a local variable and 
    // use that.
    //
    dmaTransaction = GetHandle();
    
    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                        "Enter WDFDMATRANSACTION %p, length %d",
                        dmaTransaction, (ULONG)TransferredLength);
    }

    //
    // Must be in Transfer state.
    //
    if (m_State != FxDmaTransactionStateTransfer) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                    "WDFDMATRANSACTION %p state %!FxDmaTransactionState! "
                    "is invalid", dmaTransaction, m_State);

        FxVerifierBugCheck(pFxDriverGlobals,                // globals
                            WDF_DMA_FATAL_ERROR, // specific type
                            (ULONG_PTR) dmaTransaction,  // parm 2
                            (ULONG_PTR) m_State);           // parm 3
    }

    if (TransferredLength > m_CurrentFragmentLength) {
        status = STATUS_INVALID_PARAMETER;
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                    "WDFDMATRANSACTION %p Transfered Length %I64d can't be more "
                    "than the length asked to transfer %I64d "
                    "%!STATUS!", dmaTransaction, TransferredLength,
                    m_CurrentFragmentLength, status);
        FxVerifierDbgBreakPoint(pFxDriverGlobals);
        goto End;
    }

    if (CompletionType == FxDmaCompletionTypePartial ||
        CompletionType == FxDmaCompletionTypeAbort) {
        //
        // Tally this DMA tranferred byte count into the accumulator.
        //
        m_Transferred += TransferredLength;

        //
        // Adjust the remaining length to account for the partial transfer.
        //
        m_Remaining += (m_CurrentFragmentLength - TransferredLength);

        //
        // Update CurrentDmaLength to reflect actual transfer because
        // we need to FlushAdapterBuffers based on this value in
        // TransferCompleted for packet based transfer.
        //
        m_CurrentFragmentLength = TransferredLength;

    } else {
        //
        // Tally this DMA tranferred byte count into the accumulator.
        //
        m_Transferred += m_CurrentFragmentLength;
    }

    ASSERT(m_Transferred <= m_TransactionLength);

    //
    // Inform the derived object that transfer is completed so it
    // can release resources specific to last transfer.
    //
    status = TransferCompleted();
    if (!NT_SUCCESS(status)) {
        goto End;
    }

    if (m_RequireSingleTransfer && m_Remaining > 0) {
        status = STATUS_WDF_TOO_MANY_TRANSFERS;
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                            "WDFDMATRANSACTION %p cannot complete in a "
                            "single transfer, %Iu remaining bytes out of "
                            "%Iu total %!STATUS!",
                            GetHandle(),
                            m_Remaining,
                            m_TransactionLength,
                            status);
        goto End;
    }

    //
    // If remaining DmaTransaction length is zero or if the driver wants
    // this to be the last transfer then free the map registers and
    // change the state to completed.
    //
    if (m_Remaining == 0 || CompletionType == FxDmaCompletionTypeAbort) {
        status = STATUS_SUCCESS;
        goto End;
    }

    //
    // Stage the next packet for this DmaTransaction...
    //
    status = StageTransfer();

    if (NT_SUCCESS(status)) {
        //
        // StageTransfer results in a call to the EvtProgramDma routine
        // where driver could complete and delete the object.  So 
        // don't touch the object beyond this point.
        //
        status = STATUS_MORE_PROCESSING_REQUIRED;
    }
    else {
        // 
        // The error will be returned to the caller of 
        // WdfDmaTransactionDmaComplete*()
        //
    }

End:

    if (status != STATUS_MORE_PROCESSING_REQUIRED) {
        //
        // Failed or succeeded. Either way free
        // map registers and release the device.
        //
        if (NT_SUCCESS(status)) {
            m_State = FxDmaTransactionStateTransferCompleted;
        } else {
            m_State = FxDmaTransactionStateTransferFailed;
        }

        if (pFxDriverGlobals->FxVerifierOn) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                "WDFDMATRANSACTION %p completed with status %!STATUS! - "
                                "releasing DMA resources", 
                                GetHandle(),
                                status);
        }

        ReleaseResources(FALSE);

        if (m_EncodedRequest != NULL) {
            ReleaseButRetainRequest();
        }

        m_CurrentFragmentLength = 0;

        hasTransitioned = TRUE;
    } else {
        hasTransitioned = FALSE;
    }

    *ReturnStatus = status;

    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                        "Exit WDFDMATRANSACTION %p "
                        "Transitioned(%!BOOLEAN!)",
                        dmaTransaction, hasTransitioned);
    }

    return hasTransitioned;
}

VOID
FxDmaTransactionBase::ReleaseForReuse(
    __in BOOLEAN ForceRelease
    )
{
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

    if (ForceRelease == FALSE)
    {
        if (m_State == FxDmaTransactionStateReleased) {

            //
            // Double release is probably due to cancel during early in transaction
            // initialization.  DC2 on very slow machines shows this behavior.
            // The double release case is rare and benign.
            //
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_WARNING, TRACINGDMA,
                                "WDFDMATRANSACTION %p is already released, "
                                "%!STATUS!", GetHandle(), STATUS_SUCCESS);

            return;  // already released.
        }
        
        //
        // Must not be in transfer state.
        //
        if (m_State == FxDmaTransactionStateTransfer) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                        "WDFDMATRANSACTION %p state %!FxDmaTransactionState! "
                        "is invalid (release transaction)", GetHandle(), m_State);

            if (pFxDriverGlobals->IsVerificationEnabled(1, 11, OkForDownLevel)) {
                FxVerifierBugCheck(pFxDriverGlobals,               // globals
                                   WDF_DMA_FATAL_ERROR,            // type
                                   (ULONG_PTR) GetObjectHandle(),  // parm 2
                                   (ULONG_PTR) m_State);           // parm 3
            }
        }
    }

    m_State = FxDmaTransactionStateReleased;

    ReleaseResources(ForceRelease);

    //
    // Except DMA enabler field and adapter info everything else should be 
    // cleared.  Adapter info is cleared by ReleaseResources above.
    //
    Reset();
    
    if (m_EncodedRequest != NULL) {
        ClearRequest();
    }
}

VOID
FxDmaTransactionBase::SetImmediateExecution(
    __in BOOLEAN Value
    )
{
    if (m_State != FxDmaTransactionStateCreated &&
        m_State != FxDmaTransactionStateInitialized && 
        m_State != FxDmaTransactionStateReleased) {
        DoTraceLevelMessage(
            GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGDMA,
            "Must set immediate execution flag for WDFDMATRANSACTION "
            "%p before calling AllocateResources or Execute (current "
            "state is %!FxDmaTransactionState!)", 
            GetHandle(),
            m_State
            );
        FxVerifierDbgBreakPoint(GetDriverGlobals());
    }

    if (Value) {
        m_Flags |= DMA_SYNCHRONOUS_CALLBACK;
    }
    else {
        m_Flags &= ~DMA_SYNCHRONOUS_CALLBACK;
    }
}

BOOLEAN
FxDmaTransactionBase::CancelResourceAllocation(
    VOID
    )
{
    if ((m_State == FxDmaTransactionStateCreated) || 
        (m_State == FxDmaTransactionStateReleased) || 
        (m_State == FxDmaTransactionStateDeleted)) {

        DoTraceLevelMessage(
            GetDriverGlobals(),
            TRACE_LEVEL_ERROR,
            TRACINGDMA,
            "WDFDMATRANSACTION %p cannot be cancelled in state "
            "%!FxDmaTransactionState!",
            GetHandle(),
            m_State
            );

        FxVerifierBugCheck(GetDriverGlobals(),
                           WDF_DMA_FATAL_ERROR,
                           (ULONG_PTR) GetObjectHandle(),
                           (ULONG_PTR) m_State);
        // unreachable code
    }

    PDMA_OPERATIONS dmaOperations = 
        m_AdapterInfo->AdapterObject->DmaOperations;

    BOOLEAN result;
   
    result = dmaOperations->CancelAdapterChannel(
                            m_AdapterInfo->AdapterObject,
                            m_DmaEnabler->m_FDO,
                            GetTransferContext()
                            );

    if (result) {
        m_State = FxDmaTransactionStateTransferFailed;

        if (m_EncodedRequest != NULL) {
            ReleaseButRetainRequest();
        }
    }

    return result;
}

VOID
FxDmaTransactionBase::_ComputeNextTransferAddress(
    __in PMDL CurrentMdl,
    __in size_t CurrentOffset,
    __in ULONG Transferred,
    __deref_out PMDL  *NextMdl,
    __out size_t *NextOffset
    )
/*++

Routine Description:

    This function computes the next mdl and offset given the current MDL,
    offset and bytes transfered.

Arguments:

    CurrentMdl - Mdl where the transfer currently took place.

    CurrentVa - Current virtual address in the buffer

    Transfered - Bytes transfered or to be transfered

    NextMdl - Mdl where the next transfer will take place

    NextVA - Offset within NextMdl where the transfer will start

--*/
{
    size_t transfered, mdlSize;
    PMDL mdl;

    mdlSize = MmGetMdlByteCount(CurrentMdl) - CurrentOffset;

    if (Transferred < mdlSize) {
        //
        // We are still in the first MDL
        //
        *NextMdl = CurrentMdl;
        *NextOffset = CurrentOffset + Transferred;
        return;
    }

    //
    // We have transfered the content of the first MDL.
    // Move to the next one.
    //
    transfered = Transferred - mdlSize;
    mdl = CurrentMdl->Next;
    ASSERT(mdl != NULL);

    while (transfered >= MmGetMdlByteCount(mdl)) {
        //
        // We have transfered the content of this MDL.
        // Move to the next one.
        //
        transfered -= MmGetMdlByteCount(mdl);
        mdl = mdl->Next;
        ASSERT(mdl != NULL);
    }

    //
    // This is the mdl where the last transfer occured.
    //
    *NextMdl = mdl;
    *NextOffset = transfered;

    return;
}

_Must_inspect_result_
NTSTATUS
FxDmaTransactionBase::_CalculateRequiredMapRegisters(
    __in PMDL Mdl,
    __in size_t CurrentOffset,
    __in ULONG Length,
    __in ULONG AvailableMapRegisters,
    __out_opt PULONG PossibleTransferLength,
    __out PULONG MapRegistersRequired
    )
/*++

Routine Description:

    Used on Windows 2000 to compute number of map registered required
    for this transfer. This is derived from HalCalculateScatterGatherListSize.

Arguments:

    Mdl - Pointer to the MDL that describes the pages of memory that are being
        read or written.

    CurrentVa - Current virtual address in the buffer described by the MDL
        that the transfer is being done to or from.

    Length - Supplies the length of the transfer.

    AvailableMapRegisters - Map registers available to do the transfer

    PossibleTransferLength - Length that can transfered for the

    MapRegistersRequired - Map registers required to the entire transfer

Return Value:

    NTSTATUS

Notes:

--*/
{
    PMDL tempMdl;
    ULONG requiredMapRegisters;
    ULONG transferLength;
    ULONG mdlLength;
    ULONG pageOffset;
    ULONG possTransferLength;

    //
    // Calculate the number of required map registers.
    //
    tempMdl = Mdl;
    transferLength = (ULONG) MmGetMdlByteCount(tempMdl) - (ULONG) CurrentOffset;
    mdlLength = transferLength;

    pageOffset = BYTE_OFFSET(GetStartVaFromOffset(tempMdl, CurrentOffset));
    requiredMapRegisters = 0;
    possTransferLength = 0;

    //
    // The virtual address should fit in the first MDL.
    //

    ASSERT(CurrentOffset <= tempMdl->ByteCount);

    //
    // Loop through chained MDLs, accumulating the required
    // number of map registers.
    //

    while (transferLength < Length && tempMdl->Next != NULL) {

        //
        // With pageOffset and length, calculate number of pages spanned by
        // the buffer.
        //
        requiredMapRegisters += (pageOffset + mdlLength + PAGE_SIZE - 1) >>
                                    PAGE_SHIFT;

        if (requiredMapRegisters <= AvailableMapRegisters) {
            possTransferLength = transferLength;
        }

        tempMdl = tempMdl->Next;
        pageOffset = tempMdl->ByteOffset;
        mdlLength = tempMdl->ByteCount;
        transferLength += mdlLength;
    }

    if ((transferLength + PAGE_SIZE) < (Length + pageOffset )) {
        ASSERT(transferLength >= Length);
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Calculate the last number of map registers based on the requested
    // length not the length of the last MDL.
    //

    ASSERT( transferLength <= mdlLength + Length );

    requiredMapRegisters += (pageOffset + Length + mdlLength - transferLength +
                             PAGE_SIZE - 1) >> PAGE_SHIFT;

    if (requiredMapRegisters <= AvailableMapRegisters) {
        possTransferLength += (Length + mdlLength - transferLength);
    }

    if (PossibleTransferLength != NULL) {
        *PossibleTransferLength = possTransferLength;
    }

    ASSERT(*PossibleTransferLength);

    *MapRegistersRequired = requiredMapRegisters;

    return STATUS_SUCCESS;
}

VOID
FxDmaTransactionBase::GetTransferInfo(
    __out_opt ULONG *MapRegisterCount,
    __out_opt ULONG *ScatterGatherElementCount
    )
{
    if (m_State != FxDmaTransactionStateInitialized) {
        PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                    "WDFDMATRANSACTION %p state %!FxDmaTransactionState! "
                    "is invalid", GetHandle(), m_State);

        FxVerifierBugCheck(pFxDriverGlobals,               // globals
                           WDF_DMA_FATAL_ERROR,            // specific type
                           (ULONG_PTR) GetObjectHandle(),  // parm 2
                           (ULONG_PTR) m_State);           // parm 3
    }

    DMA_TRANSFER_INFO info = {0};






    if (m_DmaEnabler->UsesDmaV3()) {

        //
        // Ask the HAL for information about the MDL and how many resources
        // it will require to transfer.
        //
        m_AdapterInfo->AdapterObject->DmaOperations->GetDmaTransferInfo(
            m_AdapterInfo->AdapterObject,
            m_StartMdl,
            m_StartOffset,
            (ULONG) this->m_TransactionLength,
            this->m_DmaDirection == WDF_DMA_DIRECTION::WdfDmaDirectionWriteToDevice,
            &info
            );
    } else {
        size_t offset = m_StartOffset;
        size_t length = m_TransactionLength;

        //
        // Walk through the MDL chain and make a worst-case computation of 
        // the number of scatter gather entries and map registers the
        // transaction would require.
        //
        for(PMDL mdl = m_StartMdl; 
            mdl != NULL && length != 0; 
            mdl = mdl->Next) {

            size_t byteCount = MmGetMdlByteCount(mdl);
            if (byteCount <= offset) {
                offset -= byteCount;
            } else {
                ULONG_PTR startVa = (ULONG_PTR) MmGetMdlVirtualAddress(mdl);

                startVa += offset;
                byteCount -= offset;

                info.V1.MapRegisterCount +=
                    (ULONG) ADDRESS_AND_SIZE_TO_SPAN_PAGES(
                        startVa, 
                        min(byteCount, length)
                        );

                length -= min(byteCount, length);
            }
        }

        info.V1.ScatterGatherElementCount = info.V1.MapRegisterCount;
    }

    if (ARGUMENT_PRESENT(MapRegisterCount)) {
        *MapRegisterCount = info.V1.MapRegisterCount;
    }

    if (ARGUMENT_PRESENT(ScatterGatherElementCount)) {
        *ScatterGatherElementCount = info.V1.ScatterGatherElementCount;
    }

    return;
}

