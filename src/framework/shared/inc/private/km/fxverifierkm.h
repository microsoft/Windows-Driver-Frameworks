/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxVerifierKm.cpp

Abstract:

    Verifier code specific to KMDF

Environment:

    kernel mode

Revision History:



--*/

#ifndef _FXVERIFIERKM_H_
#define _FXVERIFIERKM_H_

extern "C" {

BOOLEAN
VfIsRuleClassEnabled (
    _In_ ULONG RuleClassID
    );

VOID
VfCheckNxPoolType (
    _In_ POOL_TYPE PoolType,
    _In_ PVOID CallingAddress,
    _In_ ULONG PoolTag
    );

} // extern "C"

FORCEINLINE
VOID
FxVerifierCheckNxPoolType(
    _In_ PFX_DRIVER_GLOBALS FxDriverGlobals,
    _In_ POOL_TYPE PoolType,
    _In_ ULONG PoolTag
    )

/*++

Routine Description:

    This function performs the "no execute" pool type check for KMDF
    client drivers.

    N.B. It is important to keep this function inlined to make sure
         _ReturnAddress() produces the correct calling function.

Arguments:

    FxDriverGlobals - Supplies a pointer to the WDF driver object
        globals.

    PoolType - Supplies the pool type.

    PoolTag - Supplies an optional pool tag.

Return Value:

    None.

--*/

{
    if (FxDriverGlobals->FxVerifierOn) {

        //
        // Forward the call to Driver Verifier. This will provide a consistent
        // behavior across all verified drivers.
        //

        VfCheckNxPoolType(PoolType, _ReturnAddress(), PoolTag);
    }
}

#endif // _FXVERIFIERKM_H_
