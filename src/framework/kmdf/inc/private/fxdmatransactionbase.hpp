//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

extern "C" {
#include "FxDmaTransactionBase.hpp.tmh"
}

#include "FxDmaTransactionCallbacks.hpp"

//
// This type is used to allocate scatter-gather list of 1 element on the stack.
//
typedef __declspec(align(MEMORY_ALLOCATION_ALIGNMENT))UCHAR UCHAR_MEMORY_ALIGNED;

// begin_wpp enum

//
// FxDmaTransactionStateCreated when the object is created.
// FxDmaTransactionStateInitialized when object is initialized using with
//    Mdl/VA/Length.
// FxDmaTransactionStateReserved when driver calls AllocateResources until
//    the adapter control routine returns
// FxDmaTransactionStateTransfer is called when the driver call Execute
//    to start the DMA transfer.
// FxDmaTransactionStateTransferCompleted is when transfer is completed or
//    aborted
// FxDmaTransactionStateTransferFailed is set if the framework is not able
//    to start the transfer due to error.
// FxDmaTransactionStateReleased is set when the object is reinitailized for reuse
// FxDmaTransactionStateDeleted is set in the Dipose due to WdfObjectDelete
//
enum FxDmaTransactionState {
    FxDmaTransactionStateInvalid = 0,
    FxDmaTransactionStateCreated,
    FxDmaTransactionStateReserved,
    FxDmaTransactionStateInitialized,
    FxDmaTransactionStateTransfer,
    FxDmaTransactionStateTransferCompleted,
    FxDmaTransactionStateTransferFailed,
    FxDmaTransactionStateReleased,
    FxDmaTransactionStateDeleted,
};

//
// FxDmaCompletionTypeFull is used when the driver calls WdfDmaTransactionDmaComplete
//      to indicate that last framgement has been transmitted fully and to initiate
//      the transfer of next fragment.
// FxDmaCompletionTypePartial is used when the driver completes the transfer and
//      specifies a amount of bytes it has transfered, and to initiate the next transfer
//      from the untransmitted portion of the buffer.
// FxDmaCompletionTypeAbort i used when the driver calls DmaCompleteFinal to indicate
//      that's the final transfer and not initiate anymore transfers for the remaining
//      data.
//
enum FxDmaCompletionType {
    FxDmaCompletionTypeFull = 1,
    FxDmaCompletionTypePartial,
    FxDmaCompletionTypeAbort,
};

// end_wpp

//
// This tag is used to track whether the request pointer in the transaction
// has a reference taken on it.  Since the pointer is guaranteed to be 
// 4-byte aligned this can be set and cleared without destroying the pointer.
//
#define FX_STRONG_REF_TAG               0x1

//
// Simple set of macros to encode and decode tagged pointers.
//
#define FX_ENCODE_POINTER(T,p,tag)      ((T*) ((ULONG_PTR) p | (ULONG_PTR) tag))
#define FX_DECODE_POINTER(T,p,tag)      ((T*) ((ULONG_PTR) p & ~(ULONG_PTR) tag))
#define FX_IS_POINTER_ENCODED(p,tag)    ((((ULONG_PTR) p & (ULONG_PTR) tag) == tag) ? TRUE : FALSE)

//
// An uninitialized value for Dma completion status
//

#define UNDEFINED_DMA_COMPLETION_STATUS ((DMA_COMPLETION_STATUS) -1)

class FxDmaTransactionBase : public FxNonPagedObject {

    friend class FxDmaEnabler;

public:

    FxDmaTransactionBase(
        __in PFX_DRIVER_GLOBALS FxDriverGlobals,
        __in USHORT ObjectSize,
        __in USHORT ExtraSize,
        __in FxDmaEnabler *DmaEnabler
        );

    static
    VOID
    _ComputeNextTransferAddress(
        __in PMDL CurrentMdl,
        __in size_t CurrentOffset,
        __in ULONG Transferred,
        __deref_out PMDL *NextMdl,
        __out size_t *NextOffset
        );

    _Must_inspect_result_
    static
    NTSTATUS
    _CalculateRequiredMapRegisters(
        __in PMDL Mdl,
        __in size_t CurrentOffset,
        __in ULONG Length,
        __in ULONG AvailableMapRegisters,
        __out_opt PULONG PossibleTransferLength,
        __out PULONG MapRegistersRequired
        );

    virtual
    BOOLEAN
    Dispose(
        VOID
        );

    _Must_inspect_result_
    NTSTATUS
    Initialize(
        __in PFN_WDF_PROGRAM_DMA     ProgramDmaFunction,
        __in WDF_DMA_DIRECTION       DmaDirection,
        __in PMDL                    Mdl,
        __in size_t                  Offset,
        __in ULONG                   Length
        );

    _Must_inspect_result_
    virtual
    NTSTATUS
    InitializeResources(
        VOID
        )=0;

    _Must_inspect_result_
    NTSTATUS
    Execute(
        __in PVOID   Context
        );

    _Must_inspect_result_
    virtual
    NTSTATUS
    StartTransfer(
        VOID
        )=0;

    _Must_inspect_result_
    virtual
    NTSTATUS
    StageTransfer(
        VOID
        )=0;

    BOOLEAN
    DmaCompleted(
        __in  size_t      TransferredLength,
        __out NTSTATUS  * ReturnStatus,
        __in  FxDmaCompletionType CompletionType
        );

    _Must_inspect_result_
    virtual
    NTSTATUS
    TransferCompleted(
        VOID
        )=0;

    VOID
    ReleaseForReuse(
        __in BOOLEAN ForceRelease
        );

    virtual
    VOID
    ReleaseResources(
        __in BOOLEAN ForceRelease
        )=0;

    VOID
    GetTransferInfo(
        __out_opt ULONG *MapRegisterCount,
        __out_opt ULONG *ScatterGatherElementCount
        );

    __forceinline
    size_t
    GetBytesTransferred(
        VOID
        )
    {
        return m_Transferred;
    }

    __forceinline
    FxDmaEnabler *
    GetDmaEnabler(
        VOID
        )
    {
        return m_DmaEnabler;
    }

    __forceinline
    FxRequest *
    GetRequest(
        VOID
        )
    {
        //
        // Strip out the strong reference tag if it's set
        //
        return FX_DECODE_POINTER(FxRequest, 
                                 m_EncodedRequest, 
                                 FX_STRONG_REF_TAG);
    }

    __forceinline
    BOOLEAN
    IsRequestReferenced(
        VOID
        )
    {
        return FX_IS_POINTER_ENCODED(m_EncodedRequest, FX_STRONG_REF_TAG);
    }

    __forceinline
    VOID
    SetRequest(
        __in FxRequest* Request
        )
    {
        ASSERT(m_EncodedRequest == NULL);

        //
        // Make sure the pointer doesn't have the strong ref flag set already
        //
        ASSERT(FX_IS_POINTER_ENCODED(Request, FX_STRONG_REF_TAG) == FALSE);

        m_EncodedRequest = Request;
    }

    __forceinline
    VOID
    ReferenceRequest(
        VOID
        )
    {
        ASSERT(m_EncodedRequest != NULL);
        ASSERT(IsRequestReferenced() == false);

        //
        // Take a reference on the irp to catch completion of request
        // when there is a pending DMA transaction.
        // While there is no need to take a reference on request itself,
        // I'm keeping it to avoid regression as we are so close to
        // shipping this.
        //
        m_EncodedRequest->AddIrpReference();

        //
        // Increment reference to this Request.
        // See complementary section in WdfDmaTransactionDelete
        // and WdfDmaTransactionRelease.
        //
        m_EncodedRequest->ADDREF( this );

        m_EncodedRequest = FX_ENCODE_POINTER(FxRequest, 
                                             m_EncodedRequest, 
                                             FX_STRONG_REF_TAG);
    }

    __forceinline
    VOID
    ReleaseButRetainRequest(
        VOID
        )
    {
        ASSERT(m_EncodedRequest != NULL);
        ASSERT(IsRequestReferenced());

        //
        // Clear the referenced bit on the encoded request.
        //
        m_EncodedRequest = FX_DECODE_POINTER(FxRequest, 
                                             m_EncodedRequest,
                                             FX_STRONG_REF_TAG);

        //
        // Release this reference to the Irp and FxRequest.
        //
        m_EncodedRequest->ReleaseIrpReference();

        m_EncodedRequest->RELEASE( this );
    }

    __forceinline
    VOID
    ClearRequest(
        VOID
        )
    {
        if (IsRequestReferenced()) {
            ReleaseButRetainRequest();
        }
        m_EncodedRequest = NULL;
    }

    __forceinline
    size_t
    GetMaximumFragmentLength(
        VOID
        )
    {
        return m_MaxFragmentLength;
    }

    __forceinline
    VOID
    SetMaximumFragmentLength(
        size_t  MaximumFragmentLength
        )
    {
        if (MaximumFragmentLength < m_AdapterInfo->MaximumFragmentLength) {
            m_MaxFragmentLength = MaximumFragmentLength;
        }
    }

    __forceinline
    size_t
    GetCurrentFragmentLength(
        VOID
        )
    {
        return m_CurrentFragmentLength;
    }

    BOOLEAN
    GetRequireSingleTransfer(
        VOID
        )
    {
        return m_RequireSingleTransfer;
    }

    VOID
    SetRequireSingleTransfer(
        _In_ BOOLEAN RequireSingleTransfer
        )
    {
        m_RequireSingleTransfer = RequireSingleTransfer;
    }

    __forceinline
    WDFDMATRANSACTION
    GetHandle(
        VOID
        )
    {
        return (WDFDMATRANSACTION) GetObjectHandle();
    }

    PVOID
    GetTransferContext(
        VOID
        )
    {
        return m_TransferContext;
    }

    VOID
    SetImmediateExecution(
        __in BOOLEAN Value
        );

    BOOLEAN
    CancelResourceAllocation(
        VOID
        );

    FxDmaTransactionState 
    GetTransactionState(
        VOID
        )
    {
        return m_State;
    }

protected:

    FxDmaTransactionState         m_State;

    WDF_DMA_DIRECTION             m_DmaDirection;

    FxDmaEnabler*                 m_DmaEnabler;

    //
    // Depending on the direction of the transfer, this one
    // points to either m_ReadAdapterInfo or m_WriteAdapterInfo
    // structure of the DMA enabler.
    //
    FxDmaDescription*             m_AdapterInfo;

    //
    // Request associated with this transfer.  Encoding uses the 
    // FX_[EN|DE]CODE_POINTER macros with the FX_STRONG_REF_TAG
    // to indicate whether the reference has been taken or not
    //
    FxRequest*                    m_EncodedRequest;

    //
    // Callback and context for ProgramDma function
    // 
    // The callback is overloaded to also hold the callback for 
    // Packet & System transfer's Reserve callback (to save space.)
    // This is possible because the driver may not call execute 
    // and reserve in parallel on the same transaction.  Disambiguate
    // using the state of the transaction.
    //
    FxDmaTransactionProgramOrReserveDma    m_DmaAcquiredFunction;
    PVOID                                  m_DmaAcquiredContext;

    //
    // The DMA transfer context (when using V3 DMA)
    //
    PVOID                         m_TransferContext;

    //
    // This is the first MDL of the transaction.
    //
    PMDL                          m_StartMdl;

    //
    // This is the MDL where the current transfer is being executed.
    // If the data spans multiple MDL then this would be different
    // from the startMDL when we stage large transfers and also
    // if the driver performs partial transfers.
    //
    PMDL                          m_CurrentFragmentMdl;

    //
    // Starting offset in the first m_StartMdl. This might be same as
    // m_StartMdl->StartVA.
    //
    size_t                        m_StartOffset;

    //
    // Points to address where the next transfer will begin.
    //
    size_t                        m_CurrentFragmentOffset;

    //
    // This is maximum length of transfer that can be made. This is
    // computed based on the available map registers and driver
    // configuration.
    //
    size_t                        m_MaxFragmentLength;

    //
    // Length of the whole transaction.
    //
    size_t                        m_TransactionLength;

    //
    // Number of bytes pending to be transfered.
    //
    size_t                        m_Remaining;

    //
    // Total number of bytes transfered.
    //
    size_t                        m_Transferred;

    //
    // Number of bytes transfered in the last transaction.
    //
    size_t                        m_CurrentFragmentLength;

    //
    // Whether WDF may divide this transaction into multiple DMA transfers.
    //
    BOOLEAN                       m_RequireSingleTransfer;

    //
    // DMA flags for passing to GetScatterGatherListEx & 
    // AllocateAdapterChannelEx
    //
    ULONG m_Flags;

    static
    PVOID
    GetStartVaFromOffset(
        __in PMDL   Mdl,
        __in size_t Offset
        )
    {
        return ((PUCHAR) MmGetMdlVirtualAddress(Mdl)) + Offset;
    }

    virtual
    VOID
    Reuse(
        VOID
        )
    {
        return;
    }

    virtual
    VOID
    SetNewSgListBuffer(
        _In_ PVOID Buffer,
        _In_ ULONG Size
        )=0;

    virtual
    ULONG
    GetSgListBufferSize(
        VOID
        )=0;

    virtual
    ULONG
    GetNumberOfAvailableMapRegisters(
        VOID
        )=0;

private:

    VOID
    Reset(
        VOID
        );

    _Must_inspect_result_
    NTSTATUS
    PrepareForSingleTransfer(
        VOID
        );

};
