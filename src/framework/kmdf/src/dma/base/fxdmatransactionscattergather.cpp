/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDmaTransactionScatterGather.cpp

Abstract:

    WDF DMA Transaction Object for Scatter-Gather DMA

Environment:

    Kernel mode only.

Notes:


Revision History:

--*/

#include "FxDmaPCH.hpp"

extern "C" {
#include "FxDmaTransactionScatterGather.tmh"
}

FxDmaScatterGatherTransaction::FxDmaScatterGatherTransaction(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in USHORT ExtraSize,
    __in FxDmaEnabler *DmaEnabler
    ) :
    FxDmaTransactionBase(FxDriverGlobals, 
                         sizeof(FxDmaScatterGatherTransaction),
                         ExtraSize,
                         DmaEnabler)
{
    m_SGList = NULL;
    m_SGListBuffer = NULL;
    m_SgListBufferSize = 0;
    m_IsBufferFromLookaside = FALSE;
}

_Must_inspect_result_
NTSTATUS
FxDmaScatterGatherTransaction::_Create(
    __in  PFX_DRIVER_GLOBALS      FxDriverGlobals,
    __in  PWDF_OBJECT_ATTRIBUTES  Attributes,
    __in  FxDmaEnabler*           DmaEnabler,
    __out WDFDMATRANSACTION*      Transaction
    )
{
    FxDmaScatterGatherTransaction* pTransaction;
    WDFOBJECT hTransaction;
    NTSTATUS status;

    pTransaction = new (FxDriverGlobals, Attributes, DmaEnabler->GetTransferContextSize())
                FxDmaScatterGatherTransaction(FxDriverGlobals, 
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

    if (NT_SUCCESS(status) && DmaEnabler->m_IsSGListAllocated) {

        //
        // Allocate buffer for SGList from lookaside list.
        //
        pTransaction->m_SGListBuffer = FxAllocateFromNPagedLookasideList(
                &DmaEnabler->m_SGList.ScatterGatherProfile.Lookaside);
        if (pTransaction->m_SGListBuffer == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            DoTraceLevelMessage(FxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                                "Unable to allocate memory for SG List, "
                                "WDFDMATRANSACTION %p, %!STATUS! ",
                                pTransaction->GetHandle(), status);
        }
        else {
            //
            // Take a reference on the enabler to ensure that it remains valid 
            // if the transaction's disposal is deferred.
            //
            DmaEnabler->ADDREF(pTransaction);            
            pTransaction->m_SgListBufferSize = (ULONG)DmaEnabler->m_SGListSize;
            pTransaction->m_IsBufferFromLookaside = TRUE;
        }
    }

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

VOID
FxDmaScatterGatherTransaction::SetNewSgListBuffer(
    _In_ PVOID Buffer,
    _In_ ULONG Size
    )
{
    FreeSgListBuffer();
    
    m_SGListBuffer = Buffer;
    m_SgListBufferSize = Size;
}

BOOLEAN
FxDmaScatterGatherTransaction::Dispose(
    VOID
    )
{
    BOOLEAN ret;

    ret = __super::Dispose();

    //
    // Free Lookaside Buffer which held SGList
    //
    if (m_SGListBuffer != NULL) {
        FreeSgListBuffer();

        //
        // Release enabler reference taken in _Create
        //
        m_DmaEnabler->RELEASE(this);
    }

    return ret;
}

VOID
FxDmaScatterGatherTransaction::FreeSgListBuffer(
    VOID
    )
{
    //
    // The original SG list buffer was allocated from the enabler's
    // fixed-size lookaside list.
    //
    if (m_IsBufferFromLookaside) {
        FxFreeToNPagedLookasideList(
            &m_DmaEnabler->m_SGList.ScatterGatherProfile.Lookaside,
            m_SGListBuffer);
        
        m_IsBufferFromLookaside = FALSE;
    }
    else {
        ExFreePool(m_SGListBuffer);
    }
    
    m_SGListBuffer = NULL;
    m_SgListBufferSize = 0;
}

_Must_inspect_result_
NTSTATUS
FxDmaScatterGatherTransaction::InitializeResources(
    VOID
    )
{
    NTSTATUS status;
    PMDL  nextMdl;
    size_t nextOffset;
    ULONG mapRegistersRequired;
    size_t remLength, transferLength, transferred, possibleLength=0;
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

    if (m_RequireSingleTransfer) {
        //
        // We already validated the resources with ValidateForSingleTransfer
        // in Initialize. Return early, since the number calculated here
        // may conflict with what GetDmaTransferInfo told us earlier.
        //
        return STATUS_SUCCESS;
    }

    status = STATUS_SUCCESS;

    //
    // If the caller has specified a limit on the number of scatter-gather
    // elements each transfer can support then make sure it's within the
    // limit by breaking up the whole transfer into m_MaxFragmentLength and
    // computing the number of map-registers required for each fragment.
    // This check may not be valid if the driver starts to do partial
    // transfers. So driver that do partial transfer with sg-element limit
    // should be capable of handling STATUS_WDF_TOO_FRAGMENTED failures during
    // dma execution.
    //
    remLength = m_TransactionLength;
    transferred = 0;
    nextMdl = m_StartMdl;
    nextOffset = m_StartOffset;
    transferLength = 0;

    while (remLength != 0) {

        _ComputeNextTransferAddress(nextMdl,
                                    nextOffset,
                                    (ULONG) transferLength,
                                    &nextMdl,
                                    &nextOffset);

        transferLength = FxSizeTMin(remLength, m_MaxFragmentLength);

        status = _CalculateRequiredMapRegisters(nextMdl,
                                     nextOffset,
                                     (ULONG) transferLength,
                                     m_AdapterInfo->NumberOfMapRegisters,
                                     (PULONG) &possibleLength,
                                     &mapRegistersRequired
                                     );

        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(
                pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                "CalculateScatterGatherList failed for "
                "WDFDMATRANSACTION %p, %!STATUS!", GetHandle(), status);
            FxVerifierDbgBreakPoint(pFxDriverGlobals);
            return status;
        }

        if (mapRegistersRequired > m_DmaEnabler->m_MaxSGElements) {
            status = STATUS_WDF_TOO_FRAGMENTED;
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                        "WDFDMATRANSACTION %p for MDL %p is more fragmented (%d) "
                        "than the limit (%I64d) specified by the driver, %!STATUS! ",
                        GetHandle(), nextMdl, mapRegistersRequired,
                        m_DmaEnabler->m_MaxSGElements, status);
            return status;
        }

        transferred += transferLength;
        remLength -= transferLength;
    }

    return status;
}

VOID
FxDmaScatterGatherTransaction::ReleaseResources(
    __in BOOLEAN /* ForceRelease */
    )
{
    if (m_SGList != NULL) {
        PutScatterGatherList(m_SGList);
        m_SGList = NULL;
    }
    m_AdapterInfo = NULL;
}

_Must_inspect_result_
NTSTATUS
FxDmaScatterGatherTransaction::StartTransfer(
    VOID
    )
{
    ASSERT(m_CurrentFragmentMdl == m_StartMdl);
    ASSERT(m_CurrentFragmentOffset == m_StartOffset);
    ASSERT(m_CurrentFragmentLength == 0);
    ASSERT(m_Transferred == 0);

    return StageTransfer();
}

_Must_inspect_result_
NTSTATUS
FxDmaScatterGatherTransaction::StageTransfer(
    VOID
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG mapRegistersRequired;
    WDFDMATRANSACTION dmaTransaction;
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

    //
    // Use an invalid value to make the function fail if the var is not 
    // updated correctly below.
    //
    mapRegistersRequired = 0xFFFFFFFF;
    
    //
    // Client driver could complete and delete the object in
    // EvtProgramDmaFunction. So, save the handle because we need it
    // for tracing after we invoke the callback.
    //
    dmaTransaction = GetHandle();

    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                        "Enter WDFDMATRANSACTION %p ", GetHandle());
    }

    //
    // Given the first MDL and the bytes transfered, find the next MDL
    // and byteoffset within that MDL.
    //
    _ComputeNextTransferAddress(m_CurrentFragmentMdl,
                                m_CurrentFragmentOffset,
                                (ULONG) m_CurrentFragmentLength,
                                &m_CurrentFragmentMdl,
                                &m_CurrentFragmentOffset);

    //
    // Get the next possible transfer size.
    //
    m_CurrentFragmentLength = FxSizeTMin(m_Remaining, m_MaxFragmentLength);

    //
    // If this is a single-transfer transaction, then this is the first transfer
    // we're about to make. It was already validated in Initialize and doesn't
    // need any adjustments here.
    //
    if (m_RequireSingleTransfer == FALSE) {
        //
        // Fix m_CurrentFragmentLength to meet the map registers limit. This is done
        // in case the MDL is a chained MDL for an highly fragmented buffer.
        //
        status = _CalculateRequiredMapRegisters(m_CurrentFragmentMdl,
                                       m_CurrentFragmentOffset,
                                       (ULONG) m_CurrentFragmentLength,
                                       m_AdapterInfo->NumberOfMapRegisters,
                                       (PULONG)&m_CurrentFragmentLength,
                                       &mapRegistersRequired);
        //
        // We have already validated the entire transfer during initialize
        // to see each transfer meets the sglimit. So this call shouldn't fail.
        // But, if the driver does partial transfer and changes the fragment
        // boundaries then it's possible for the sg-elements to vary. So, check
        // one more time to see if we are within the bounds before building
        // the sglist and calling into the driver.
        //
        ASSERT(NT_SUCCESS(status));

        if (mapRegistersRequired > m_DmaEnabler->m_MaxSGElements) {
            status = STATUS_WDF_TOO_FRAGMENTED;
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                        "WDFDMATRANSACTION %p for MDL %p is more fragmented (%d) "
                        "than the limit (%I64d) specified by the driver, %!STATUS! ",
                        dmaTransaction, m_CurrentFragmentMdl, mapRegistersRequired,
                        m_DmaEnabler->m_MaxSGElements, status);
            return status;
        }
    }

    m_Remaining -= m_CurrentFragmentLength;

    if (m_DmaEnabler->m_IsSGListAllocated) {

        ASSERT(m_SGListBuffer != NULL);
        status = BuildScatterGatherList(m_CurrentFragmentMdl,
                                        m_CurrentFragmentOffset,
                                        (ULONG) m_CurrentFragmentLength,
#pragma prefast(suppress: __WARNING_CLASS_MISMATCH_NONE, "This warning requires a wrapper class for the DRIVER_LIST_CONTROL type.")
                                        _AdapterListControl,
                                        this,
                                        m_SGListBuffer,
                                        (ULONG) m_SgListBufferSize);

    } else {

        status = GetScatterGatherList(m_CurrentFragmentMdl,
                                      m_CurrentFragmentOffset,
                                      (ULONG) m_CurrentFragmentLength,
#pragma prefast(suppress: __WARNING_CLASS_MISMATCH_NONE, "This warning requires a wrapper class for the DRIVER_LIST_CONTROL type.")
                                      _AdapterListControl,
                                      this);
    }

    if (!NT_SUCCESS(status)) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                            "Build or GetScatterGatherList failed for "
                            "WDFDMATRANSACTION %p, %!STATUS!",
                            dmaTransaction, status);
        //
        // Readjust remaining bytes transfered.
        //
        m_Remaining += m_CurrentFragmentLength;
        return status;
    }

    //
    // Before GetScatterGatherList returns, _AdapterListControl can get called
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


VOID
FxDmaScatterGatherTransaction::_AdapterListControl(
    __in  PDEVICE_OBJECT         DeviceObject,
    __in  PIRP                   Irp,            // UNUSED
    __in  PSCATTER_GATHER_LIST   SgList,
    __in  PVOID                  Context
    )
{
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    WDFDMATRANSACTION dmaTransaction;
    FxDmaScatterGatherTransaction * pDmaTransaction;
    
    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(DeviceObject);

    pDmaTransaction = (FxDmaScatterGatherTransaction*) Context;
    pFxDriverGlobals = pDmaTransaction->GetDriverGlobals();
    dmaTransaction = pDmaTransaction->GetHandle();

    if (pFxDriverGlobals->FxVerifierOn) {
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                        "Enter WDFDMATRANSACTION %p",
                        dmaTransaction);
    }

    ASSERT(pDmaTransaction != NULL);
    ASSERT(pDmaTransaction->m_DmaAcquiredFunction.Method.ProgramDma != NULL);

    ASSERT(SgList->NumberOfElements <= pDmaTransaction->m_DmaEnabler->GetMaxSGElements());

    pDmaTransaction->m_SGList = SgList;

    //
    // We ignore the return value. The pattern we want the driver to follow is
    // that if it fails to program DMA transfer, it should call DmaCompletedFinal
    // to abort the transfer.
    //
    (VOID) pDmaTransaction->m_DmaAcquiredFunction.InvokeProgramDma(
                dmaTransaction,
                pDmaTransaction->m_DmaEnabler->m_DeviceBase->GetHandle(),
                pDmaTransaction->m_DmaAcquiredContext,
                pDmaTransaction->m_DmaDirection,
                SgList);

    if (pFxDriverGlobals->FxVerifierOn) {        
        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                        "Exit WDFDMATRANSACTION %p",
                        dmaTransaction);
    }
}

_Must_inspect_result_
NTSTATUS
FxDmaScatterGatherTransaction::TransferCompleted(
    VOID
    )
{
    //
    // All we have to do is release the scatter-gather list.
    //
    if (m_SGList != NULL) {

        PutScatterGatherList(m_SGList);
        m_SGList = NULL;
    }

    return STATUS_SUCCESS;
}

