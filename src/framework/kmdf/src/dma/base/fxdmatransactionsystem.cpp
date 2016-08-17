/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDmaTransactionSystem.cpp

Abstract:

    WDF DMA Transaction Object for System DMA

Environment:

    Kernel mode only.

Notes:


Revision History:

--*/

#include "FxDmaPCH.hpp"

extern "C" {
#include "FxDmaTransactionSystem.tmh"
}

FxDmaSystemTransaction::FxDmaSystemTransaction(
    __in PFX_DRIVER_GLOBALS FxDriverGlobals,
    __in USHORT ExtraSize,
    __in FxDmaEnabler *DmaEnabler
    ) :
    FxDmaPacketTransaction(FxDriverGlobals, sizeof(FxDmaSystemTransaction), ExtraSize, DmaEnabler)
{
    return;
}

_Must_inspect_result_
NTSTATUS
FxDmaSystemTransaction::_Create(
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
                FxDmaSystemTransaction(FxDriverGlobals, DmaEnabler->GetTransferContextSize(), DmaEnabler);

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

VOID
FxDmaSystemTransaction::SetNewSgListBuffer(
    _In_ PVOID Buffer,
    _In_ ULONG Size
    )
{
    //
    // Free existing buffer
    //
    ExFreePool(m_DmaEnabler->m_SGList.SystemProfile.List);

    m_DmaEnabler->m_SGList.SystemProfile.List = (PSCATTER_GATHER_LIST)Buffer;
    m_DmaEnabler->m_SGListSize = Size;
}

BOOLEAN
FxDmaSystemTransaction::PreMapTransfer(
    VOID
    )
{
    PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();
    BOOLEAN result = TRUE;

    if (m_ConfigureChannelFunction.Method != NULL) {
        //
        // Invoke the callback.  If it returns false then the driver has 
        // completed the transaction in the callback and we must abort 
        // processing.
        //

        if (pFxDriverGlobals->FxVerifierOn) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                "Invoking ConfigureChannel callback %p (context "
                                "%p) for WDFDMATRANSACTION %p.",
                                m_ConfigureChannelFunction.Method,
                                m_ConfigureChannelContext,
                                GetHandle());
        }

        result = m_ConfigureChannelFunction.Invoke(
                    GetHandle(),
                    m_DmaEnabler->m_DeviceBase->GetHandle(),
                    m_ConfigureChannelContext,
                    m_CurrentFragmentMdl,
                    m_CurrentFragmentOffset,
                    m_CurrentFragmentLength
                    );
    }

    return result;
}


PDMA_COMPLETION_ROUTINE
FxDmaSystemTransaction::GetTransferCompletionRoutine(
    VOID
    )
{
    if (m_TransferCompleteFunction.Method == NULL) {
        return NULL;
    }
    else {
        return _SystemDmaCompletion;
    }
}

VOID
FxDmaSystemTransaction::CallEvtDmaCompleted(
    __in DMA_COMPLETION_STATUS Status
    )
{
    //
    // Call the TransferComplete callback to indicate that the 
    // transfer was aborted.
    //
    m_TransferCompleteFunction.Invoke(
        GetHandle(),
        m_DmaEnabler->m_Device->GetHandle(),
        m_TransferCompleteContext,
        m_DmaDirection,
        Status
        );
}

VOID
FxDmaSystemTransaction::_SystemDmaCompletion(
    __in PDMA_ADAPTER          /* DmaAdapter */,
    __in PDEVICE_OBJECT        /* DeviceObject */,
    __in PVOID                 CompletionContext,
    __in DMA_COMPLETION_STATUS Status
    )
{
    FxDmaSystemTransaction* transaction = (FxDmaSystemTransaction*) CompletionContext;
    PFX_DRIVER_GLOBALS pFxDriverGlobals = transaction->GetDriverGlobals();
    KIRQL oldIrql;
    BOOLEAN completionDeferred;

    //
    // Lock the transfer state so that a staging or cancelling thread 
    // cannot change it.
    //
    transaction->LockTransferState(&oldIrql);

    ASSERTMSG("Completion state was already set", 
              (transaction->m_TransferState.CompletionStatus == 
               UNDEFINED_DMA_COMPLETION_STATUS));
    ASSERTMSG("Deferred completion is already pending", 
              (transaction->m_TransferState.RerunCompletion == FALSE));

    //
    // If a staging is in progress then defer the completion.
    //
    if (transaction->m_TransferState.CurrentStagingThread != NULL) {
        transaction->m_TransferState.CompletionStatus = Status;
        transaction->m_TransferState.RerunCompletion = TRUE;
        completionDeferred = TRUE;
    }
    else {
        completionDeferred = FALSE;
    }

    transaction->UnlockTransferState(oldIrql);

    //
    // Process the old state.
    //
    if (completionDeferred == TRUE) {
        //
        // The staging thread has not moved past EvtProgramDma.  The staging thread 
        // will detect the state change and call the completion routine.
        //
        // Nothing to do in this case.
        //
        if (pFxDriverGlobals->FxVerifierOn) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                "Deferring DmaCompleted callback for WDFDMATRANSACTION %p"
                                "(status %x)",
                                transaction->GetHandle(),
                                Status);
        }
    }
    else {
        //
        // Completion occurred while the transfer was running or
        // being cancelled.  Call the completion routine.
        //
        // Note: a cancel when in programming state leaves the 
        //       state as programming.  that we're not in programming
        //       means we don't need to worry about racing with 
        //       EvtProgramDma.
        //

        if (pFxDriverGlobals->FxVerifierOn) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                "Invoking DmaCompleted callback %p (context %p) "
                                "for WDFDMATRANSACTION %p (status %x)",
                                transaction->m_TransferCompleteFunction.Method,
                                transaction->m_TransferCompleteContext,
                                transaction->GetHandle(),
                                Status);
        }
        transaction->CallEvtDmaCompleted(Status);
    }
}

VOID
FxDmaSystemTransaction::StopTransfer(
    VOID
    )
{
    //
    // Mark the transfer cancelled so we have a record of it even if 
    // a racing call to FlushAdapterBuffers clears the TC.
    //
    m_IsCancelled = TRUE;

    //
    // Cancel the system DMA transfer.  This arranges for one of two things 
    // to happen:
    //  * the next call to MapTransfer will fail
    //  * the DMA completion routine will run
    //
    if (CancelMappedTransfer() == FALSE) {

        //
        // The cancel failed.  Someone has already stopped this transfer.
        // That's illegal.
        //
        PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();

        DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDMA,
                            "WDFDMATRANSACTION %p has already been stopped",
                            GetHandle());

        if (pFxDriverGlobals->IsVerificationEnabled(1, 11, OkForDownLevel)) {
            FxVerifierBugCheck(pFxDriverGlobals,               // globals
                               WDF_DMA_FATAL_ERROR,            // type
                               (ULONG_PTR) GetObjectHandle(),  // parm 2
                               (ULONG_PTR) m_State);           // parm 3
        }
    }
}

