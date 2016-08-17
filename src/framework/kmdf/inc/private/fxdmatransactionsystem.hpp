//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

extern "C" {
#include "FxDmaTransactionSystem.hpp.tmh"
}

class FxDmaSystemTransaction: public FxDmaPacketTransaction {

    friend FxDmaPacketTransaction;

public:

    FxDmaSystemTransaction(
        __in PFX_DRIVER_GLOBALS FxDriverGlobals,
        __in USHORT ExtraSize,
        __in FxDmaEnabler *DmaEnabler
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
    SetConfigureChannelCallback(
        __in_opt PFN_WDF_DMA_TRANSACTION_CONFIGURE_DMA_CHANNEL Callback,
        __in_opt PVOID Context
        )
    {
        m_ConfigureChannelFunction.Method = Callback;
        m_ConfigureChannelContext = Context;
    }

    VOID
    SetTransferCompleteCallback(
        __in_opt PFN_WDF_DMA_TRANSACTION_DMA_TRANSFER_COMPLETE Callback,
        __in_opt PVOID Context
        )
    {
        m_TransferCompleteFunction.Method = Callback;
        m_TransferCompleteContext = Context;
    }

    VOID
    StopTransfer(
        VOID
        );

protected:
    
    //
    // Callback and context for configure channel callback
    //
    FxDmaTransactionConfigureChannel m_ConfigureChannelFunction;
    PVOID                            m_ConfigureChannelContext;

    //
    // Callback and context for DMA completion callback
    //
    FxDmaTransactionTransferComplete m_TransferCompleteFunction;
    PVOID                            m_TransferCompleteContext;

    IO_ALLOCATION_ACTION
    GetAdapterControlReturnValue(
        VOID
        )
    {
        return KeepObject;
    }

    VOID
    FreeMapRegistersAndAdapter(
        VOID
        )
    {
        PFX_DRIVER_GLOBALS pFxDriverGlobals = GetDriverGlobals();
        KIRQL irql;

        KeRaiseIrql(DISPATCH_LEVEL, &irql);

        if (pFxDriverGlobals->FxVerifierOn) {
            DoTraceLevelMessage(pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDMA,
                                "Freeing adapter channel for WDFDMATRANSACTION %p",
                                GetHandle());
        }

        m_AdapterInfo->AdapterObject->DmaOperations->
                    FreeAdapterChannel(m_AdapterInfo->AdapterObject);
        KeLowerIrql(irql);

        return;
    }

    BOOLEAN
    CancelMappedTransfer(
        VOID
        )
    {
        NTSTATUS status;

        ASSERT(m_DmaEnabler->UsesDmaV3());

        //
        // Cancel the transfer.  if it's not yet mapped this will mark the 
        // TC so that mapping will fail.  If it's running this will invoke the 
        // DMA completion routine.
        //
        status = 
            m_AdapterInfo->AdapterObject->DmaOperations->CancelMappedTransfer(
                m_AdapterInfo->AdapterObject,
                GetTransferContext()
                );

        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGDMA,
                            "Stopping WDFDMATRANSACTION %p returned status %!STATUS!",
                            GetHandle(),
                            status);

        return NT_SUCCESS(status);
    }

    VOID
    Reuse(
        VOID
        )
    {
        __super::Reuse();
        m_ConfigureChannelFunction.Method = NULL;
        m_ConfigureChannelContext = NULL;

        m_TransferCompleteFunction.Method = NULL;
        m_TransferCompleteContext = NULL;
    }

    VOID
    CallEvtDmaCompleted(
        __in DMA_COMPLETION_STATUS Status
        );

    virtual
    BOOLEAN
    PreMapTransfer(
        VOID
        );

    virtual
    PDMA_COMPLETION_ROUTINE
    GetTransferCompletionRoutine(
        VOID
        );

    static DMA_COMPLETION_ROUTINE _SystemDmaCompletion;

    virtual
    VOID
    SetNewSgListBuffer(
        _In_ PVOID Buffer,
        _In_ ULONG Size
        );

    virtual
    ULONG
    GetSgListBufferSize(
        VOID
        )
    {
        return (ULONG)m_DmaEnabler->m_SGListSize;
    }
};
