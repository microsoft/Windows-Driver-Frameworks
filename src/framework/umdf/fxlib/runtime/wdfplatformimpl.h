//
//    Copyright (C) Microsoft.  All rights reserved.
//





#pragma once

#include <tlhelp32.h>

#pragma intrinsic(_ReturnAddress)

//
// Activation Frame functions and classes:
//

#ifdef __cplusplus
extern "C" {
#endif

__forceinline
VOID
WdfActivationListPush(
    __inout_opt PWDF_ACTIVATION_FRAME *ListHead,
    __in PWDF_ACTIVATION_FRAME Frame,
    __in PVOID Caller
    )
/*++
 
  Routine Description:

    This function formats an activation frame and inserts it into the 
    provided list.

  Arguments:

    ListHead - the address of the list-head pointer.  The contents of the 
               list-head pointer will be modified to point to the supplied
               Frame.

               If NULL then the list is not updated.  This allows the caller
               to avoid the test.

    Frame - The frame to be initialized and made the head of the list.

    Caller - The caller value to be added to the frame.  This should be 
             the return address of the caller of the function instantiating 
             the activation frame.

  Return Value:

    none.

--*/
{
    Frame->ListHead = ListHead;
    Frame->Caller = Caller;
    if(ListHead != NULL) {
        Frame->NextFrame = *ListHead;
        *ListHead = Frame;
    }
}

__forceinline
VOID
WdfActivationListPop(
    __in PWDF_ACTIVATION_FRAME Frame
    )
/*++

  Routine Description:

    This function removes the top entry from the activation frame list 
    (if any) and points the list at the next frame.

  Arguments:

    Frame - the frame to be removed.

  Return Value:

    none.

--*/
{
    if(Frame->ListHead != NULL) {
        *(Frame->ListHead) = Frame->NextFrame;
        Frame->ListHead = NULL;
    }
}

__forceinline
PWDF_ACTIVATION_FRAME
WdfActivationFrameGetNext(
    __in PWDF_ACTIVATION_FRAME CurrentFrame
    )
/*++

  Routine Description:

    This function returns the next activation frame, representing an earlier
    caller in the activation stack (ie. the call stack).

  Arguments:

    CurrentFrame - the current frame pointer.  The next frame in the list will
                   be returned.

  Return Value:

    the next frame in the list, or NULL if CurrentFrame is the last frame 
    in the list.

--*/
{
    return CurrentFrame->NextFrame;
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

typedef struct WDF_ACTIVATION WDF_ACTIVATION, *PWDF_ACTIVATION;

struct WDF_ACTIVATION : WDF_ACTIVATION_FRAME {

public:

    static
    __forceinline
    PWDF_ACTIVATION
    GetHead(
        __in PWDF_ACTIVATION_FRAME* ListHead
       )
    /*++

      Routine Description:

        Given an instance of the base class, this method returns a pointer
        to this class.

      Arguments:

        ListHead - The list head.

      Return Value:

        A pointer to an instance of WDF_ACTIVATION.

    --*/
    {
        return (PWDF_ACTIVATION) (*ListHead);
    }

    __forceinline
    WDF_ACTIVATION(
        __in PWDF_ACTIVATION_FRAME* ListHead, 
        __in PVOID Caller = _ReturnAddress()
        )
    /*++

      Routine Description:

        The constructor for WDF_ACTIVATION.  This initializes the base class 
        with the provided parameters and inserts it into the specified 
        activation list.

      Arguments:

        ListHead - the head on which to insert this activation frame.

        Caller - the caller value to insert into the frame.

      Return Value:

        none.

    --*/
    {
        WdfActivationListPush(ListHead, this, Caller);
    }

    __forceinline
    VOID
    Unwind(
        VOID
        )
    /*++

      Routine Description:

        Used to unwind an activation frame from the stack.  This is invoked
        naturally by the destructor, but can also be invoked in a finally
        handler in order to unwind around exception handling.

      Arguments:

        None

      Return Value:

        None
    
    --*/
    {
        WdfActivationListPop(this);
    }

    __forceinline
    ~WDF_ACTIVATION(
        VOID
        )
    {
        Unwind();
    }

    __forceinline
    PWDF_ACTIVATION
    Next(
        VOID
        )
    {
        return (PWDF_ACTIVATION) WdfActivationFrameGetNext(this);
    }

};

#endif
