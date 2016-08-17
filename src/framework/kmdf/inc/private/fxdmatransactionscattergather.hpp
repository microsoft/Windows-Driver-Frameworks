//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

class FxDmaScatterGatherTransaction : public FxDmaTransactionBase {

public:

    FxDmaScatterGatherTransaction(
        __in PFX_DRIVER_GLOBALS FxDriverGlobals,
        __in USHORT ExtraSize,
        __in FxDmaEnabler *DmaEnabler
        );

    virtual
    BOOLEAN
    Dispose(
        VOID
        );

    _Must_inspect_result_
    virtual
    NTSTATUS
    InitializeResources(
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

protected:

    //
    // Scatter-Gather list provided by the system.
    //
    PSCATTER_GATHER_LIST         m_SGList;

    //
    // Preallocated memory for the scatter-gather list.
    //
    PVOID                        m_SGListBuffer;

    //
    // Size in bytes of m_SGListBuffer.
    //
    ULONG                        m_SgListBufferSize;

    //
    // Whether m_SGListBuffer was allocated from the enabler's
    // lookaside list, or directly from the system memory pool.
    //
    BOOLEAN                      m_IsBufferFromLookaside;

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
        return m_SgListBufferSize;
    }

    virtual
    ULONG
    GetNumberOfAvailableMapRegisters(
        VOID
        )
    {
        return m_AdapterInfo->NumberOfMapRegisters;
    }

private:

    VOID
    FreeSgListBuffer(
        VOID
        );

    static 
    VOID
    _AdapterListControl(
        __in  DEVICE_OBJECT         * DeviceObject,
        __in  IRP                   * Irp,
        __in  SCATTER_GATHER_LIST   * SgList,
        __in  VOID                  * Context
        );

    _Must_inspect_result_
    NTSTATUS
    GetScatterGatherList (
        __in PMDL  Mdl,
        __in size_t CurrentOffset,
        __in ULONG  Length,
        __in PDRIVER_LIST_CONTROL  ExecutionRoutine,
        __in PVOID  Context
        )
    {
        NTSTATUS status;
        KIRQL irql;

        KeRaiseIrql(DISPATCH_LEVEL, &irql);

        if (m_DmaEnabler->UsesDmaV3())
        {
            PDMA_OPERATIONS dmaOperations = 
                m_AdapterInfo->AdapterObject->DmaOperations;

            status = dmaOperations->GetScatterGatherListEx(
                                               m_AdapterInfo->AdapterObject,
                                               m_DmaEnabler->m_FDO,
                                               GetTransferContext(),
                                               Mdl,
                                               CurrentOffset,
                                               Length,
                                               m_Flags,
                                               ExecutionRoutine,
                                               Context,
                                               (BOOLEAN) m_DmaDirection, 
                                               NULL,
                                               NULL, 
                                               NULL
                                               );
        }
        else
        {
            status = m_AdapterInfo->AdapterObject->DmaOperations->
                        GetScatterGatherList(m_AdapterInfo->AdapterObject,
                                             m_DmaEnabler->m_FDO,
                                             Mdl,
                                             GetStartVaFromOffset(Mdl, CurrentOffset),
                                             Length,
                                             ExecutionRoutine,
                                             Context,
                                             (BOOLEAN) m_DmaDirection);
        }

        KeLowerIrql(irql);

        return status;
    }

    VOID
    PutScatterGatherList(
        __in PSCATTER_GATHER_LIST  ScatterGather
        )
    {
        KIRQL irql;

        KeRaiseIrql(DISPATCH_LEVEL, &irql);

        m_AdapterInfo->AdapterObject->DmaOperations->
                PutScatterGatherList(m_AdapterInfo->AdapterObject,
                                     ScatterGather,
                                     (BOOLEAN) m_DmaDirection);
        KeLowerIrql(irql);

        return;
    }

    _Must_inspect_result_
    NTSTATUS
    BuildScatterGatherList(
        __in PMDL  Mdl,
        __in size_t CurrentOffset,
        __in ULONG  Length,
        __in PDRIVER_LIST_CONTROL  ExecutionRoutine,
        __in PVOID  Context,
        __in PVOID  ScatterGatherBuffer,
        __in ULONG  ScatterGatherBufferLength
        )
    {
        NTSTATUS status;
        KIRQL irql;

        KeRaiseIrql(DISPATCH_LEVEL, &irql);

        if (m_DmaEnabler->UsesDmaV3()) {

            PDMA_OPERATIONS dmaOperations = 
                m_AdapterInfo->AdapterObject->DmaOperations;
            ULONG flags = 0;

            if (GetDriverGlobals()->IsVersionGreaterThanOrEqualTo(1,15)) {
                //
                // Though the correct behavior is to pass the m_Flags to the 
                // BuildScatterGatherListEx function, the code was not doing it
                // for versions <= 1.13. To reduce any chance of regression,
                // the m_Flags is honored for drivers that are 1.15
                // or newer.
                //
                flags = m_Flags;
            }

            status = dmaOperations->BuildScatterGatherListEx(
                                                 m_AdapterInfo->AdapterObject,
                                                 m_DmaEnabler->m_FDO,
                                                 GetTransferContext(),
                                                 Mdl,
                                                 CurrentOffset,
                                                 Length,
                                                 flags,
                                                 ExecutionRoutine,
                                                 Context,
                                                 (BOOLEAN) m_DmaDirection,
                                                 ScatterGatherBuffer,
                                                 ScatterGatherBufferLength,
                                                 NULL,
                                                 NULL,
                                                 NULL
                                                 );
        }
        else {

            status = m_AdapterInfo->AdapterObject->DmaOperations->
                        BuildScatterGatherList(m_AdapterInfo->AdapterObject,
                                               m_DmaEnabler->m_FDO,
                                               Mdl,
                                               GetStartVaFromOffset(Mdl, CurrentOffset),
                                               Length,
                                               ExecutionRoutine,
                                               Context,
                                               (BOOLEAN) m_DmaDirection,
                                               ScatterGatherBuffer,
                                               ScatterGatherBufferLength);
        }


        KeLowerIrql(irql);

        return status;
    }
};
