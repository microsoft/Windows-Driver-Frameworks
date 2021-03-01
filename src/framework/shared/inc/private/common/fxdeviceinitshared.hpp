/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDeviceInitShared.hpp

Abstract:

    This header is split from FxDeviceInit.hpp to have definitions needed
    in shared code.

Author:


Environment:


Revision History:

--*/

#ifndef __FXDEVICEINITSHARED_HPP__
#define __FXDEVICEINITSHARED_HPP__

struct PdoInit {

    PdoInit(
        VOID
        )
    {
        //
        // Cannot RtlZeroMemory the whole structure because the data member
        // FxCollectionInternal has its own constructor
        //
        RtlZeroMemory(&EventCallbacks, sizeof(EventCallbacks));
        Raw = FALSE;
        Static = FALSE;
        Parent = NULL;
        DeviceID = NULL;
        InstanceID = NULL;
        ContainerID = NULL;
        DefaultLocale = 0x0;
        DescriptionEntry = NULL;
        ForwardRequestToParent = FALSE;
        NoPowerDependencyOnParent = FALSE;
        DeviceText.Next = NULL;
        LastDeviceTextEntry = &DeviceText.Next;
    }

    WDF_PDO_EVENT_CALLBACKS EventCallbacks;

    CfxDevice* Parent;

    FxString* DeviceID;

    FxString* InstanceID;

    FxCollectionInternal HardwareIDs;

    FxCollectionInternal CompatibleIDs;

    FxString* ContainerID;

    SINGLE_LIST_ENTRY DeviceText;
    PSINGLE_LIST_ENTRY* LastDeviceTextEntry;

    FxDeviceDescriptionEntry* DescriptionEntry;

    LCID DefaultLocale;

    BOOLEAN Raw;

    BOOLEAN Static;

    BOOLEAN ForwardRequestToParent;

    BOOLEAN NoPowerDependencyOnParent;
};

#endif //__FXDEVICEINITSHARED_HPP__
