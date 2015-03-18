/*++

Copyright (c) Microsoft Corporation

ModuleName:

    MxWorkItem.h

Abstract:

    Mode agnostic definition of WorkItem functions
    
    See MxWorkItemKm.h and MxWorkItemUm.h for mode
    specific implementations

Author:



Revision History:



--*/

#pragma once

class MxWorkItem
{

protected:
    MdWorkItem m_WorkItem;    

public:
    
    FORCEINLINE
    MxWorkItem(
        );

    //
    // This is used only by the UM implementation
    //
    
#if ((FX_CORE_MODE)==(FX_CORE_USER_MODE))
    static
    VOID
    CALLBACK
    _WorkerThunk (
        _Inout_ PTP_CALLBACK_INSTANCE Instance,
        _Inout_opt_ PVOID Parameter,
        _Inout_ PTP_WAIT Wait,
        _In_ TP_WAIT_RESULT WaitResult
        );

    VOID
    WaitForCallbacksToComplete(
        VOID
        );

#endif

    _Must_inspect_result_
    FORCEINLINE
    NTSTATUS
    Allocate(
        __in MdDeviceObject DeviceObject,
        __in_opt PVOID ThreadPoolEnv = NULL
        );

    FORCEINLINE
    VOID
    Enqueue(
        __in PMX_WORKITEM_ROUTINE Callback,
        __in PVOID Context
        );

    FORCEINLINE
    MdWorkItem
    GetWorkItem(
        );

    static
    FORCEINLINE
    VOID
    _Free(
        __in MdWorkItem Item
        );

    FORCEINLINE
    VOID
    Free(
        );

    FORCEINLINE
    ~MxWorkItem(
        )
    {
    }

};

//
// MxAutoWorkItem adds value to MxWorkItem by automatically freeing the 
// associated MdWorkItem when it goes out of scope
//
struct MxAutoWorkItem : public MxWorkItem {
public:
    
    MxAutoWorkItem(
        )
    {
    }

    FORCEINLINE
    ~MxAutoWorkItem(
        );
};

