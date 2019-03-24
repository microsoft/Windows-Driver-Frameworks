/*++

Copyright (c) Microsoft Corporation

Module Name:

    fxldr.h

Abstract:
    This module contains private interfaces to the WDF loader.  This interface
    is used by
    a) the stub code in the client driver
    b) the framework
    c) the framework loader
--*/
#ifndef __FXLDR_H__
#define __FXLDR_H__

//
// Use __FXLDRUM_H__ (defined in fxldrum.h) to tell who (KMDF/UMDF) is building
//
#ifndef __FXLDRUM_H__

#include <initguid.h>
#include <wdfldr.h>

#ifdef __cplusplus
extern "C" {
#endif

#endif // __FXLDRUM_H__

#define WDF_COMPONENT_NAME(a) L#a

//
// Bind info structures are aligned on memory allocation boundaries on 64-bit
// platforms, pointer boundaries on 32-bit.  The alignment __declspec didn't
// fix this for Itanium builds, so we must accomodate this.
//
// This "marker type" allows us to declare a leading boundary that will always
// have the proper alignment.  It may "waste" 4 to 8 bytes (0-4 to align the
// marker, and 4 more because the alignment isn't necessary, making the marker
// bigger than needed) on 32-bit, but that's not a large price to pay.
//

typedef struct DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT) _MARKER_TYPE {
    UCHAR Pad[MEMORY_ALLOCATION_ALIGNMENT];
} MARKER_TYPE;

typedef struct _LIBRARY_MODULE *PLIBRARY_MODULE;
typedef struct _WDF_LIBRARY_INFO *PWDF_LIBRARY_INFO;

typedef
VOID
(*WDFFUNC)(
    VOID
    );

//
// Version container
//
typedef struct _WDF_VERSION {
    WDF_MAJOR_VERSION  Major;
    WDF_MINOR_VERSION  Minor;
    WDF_BUILD_NUMBER   Build;
} WDF_VERSION;

//
// WDF bind information structure.
//
typedef struct _WDF_BIND_INFO {
    ULONG              Size;
    PWCHAR             Component;
    WDF_VERSION        Version;
    ULONG              FuncCount;
    __field_bcount(FuncCount*sizeof(WDFFUNC)) WDFFUNC* FuncTable;

#ifndef __FXLDRUM_H__
    PLIBRARY_MODULE    Module;     // Mgmt and diagnostic use only
#else
    PVOID              Module;
#endif
} WDF_BIND_INFO, * PWDF_BIND_INFO;

#if (!defined(WDF_STUB_VERSION)) || (WDF_STUB_VERSION >= 25)

//
// Bind info v2. Allow client driver to run on old version of framework
//
typedef struct _WDF_BIND_INFO2 {
    //
    // Embed the original V1 bind info here. Note initalize V1.Size = sizeof(V2)
    //
    WDF_BIND_INFO       V1;

    //
    // The following fields are all pointers to global variables, which will be
    // used later by the client driver to, e.g. check whether the DDI/Structure
    // is available on this platform.
    //

    //
    // The 'minimum required framework version' to load the client driver.
    // In other words, if the driver does load, we know that
    //
    //       MinimumVersionRequired <= framework version
    //
    // There are actually two sub-category of minor version numbers:
    //
    // 1) The 'target framework version', which controls the version of the
    //    header/lib that the client driver was compiled against and is allowed
    //    to use at runtime
    //
    // 2) The 'minimum required framework version'. The client driver won't be
    //    able to load on any framework that is older than this version
    //
    // A driver that is built against an old framework and uses the old v1 bind
    // info has only one version number at BindInfo.Version.Minor. It represents
    // both the 'target' and the 'minimum required' versions.
    //
    // A driver that is built against a newer framework and uses the v2 bind
    // info has both:
    //
    // 1) BindInfoV2.V1.Version.Minor       for the 'target' version
    // 2) BindInfoV2.MinimumVersionRequired for the 'minimum required' version
    //
    // Note that it is Ok for a driver that uses the v2 bind info but specifies
    // a identical 'target' and 'minimum required' version. In such a case the
    // behavior is identical to the one of using v1 bind info.
    //
    WDF_MINOR_VERSION * MinimumVersionRequired;

    //
    // True if BindInfoV2.V1.Version.Minor > Framework Version
    // False otherwise.
    //
    BOOLEAN           * ClientVersionHigherThanFramework;

    //
    // The count of function/DDI provided by the framework that the client
    // driver is allowed to use.
    //
    // Note there is already a V1.FuncCount. However we still need create a new
    // field because the client driver has no access to the bind info structure;
    // instead, the client expects a global variable to hold the total count.
    // Thus we introduce a ptr to the said global, and keep the two in sync.
    //
    //     - V2.V1.FuncCount
    //     - (* V2.FuncCountPtr)
    //
    ULONG             * FuncCountPtr;

    //
    // The count of public structures provided by the framework that the client
    // driver is allowed to use
    //
    ULONG             * StructCountPtr;

    //
    // An array of sizeof(struct) for all public structures provided by the
    // framework that the client driver is allowed to use
    //
    WDF_STRUCT_INFO   * StructTable;

} WDF_BIND_INFO2, * PWDF_BIND_INFO2;

#endif

NTSTATUS
WdfBindClientHelper(
    _Inout_ PWDF_BIND_INFO       BindInfo,
    _In_ WDF_MAJOR_VERSION       FxMajorVersion,
    _In_ WDF_MINOR_VERSION       FxMinorVersion
    );

typedef
_Must_inspect_result_
NTSTATUS
(*PFNLIBRARYCOMMISSION)(
    VOID
    );

typedef
_Must_inspect_result_
NTSTATUS
(*PFNLIBRARYDECOMMISSION)(
    VOID
    );

typedef
_Must_inspect_result_
NTSTATUS
(*PFNLIBRARYREGISTERCLIENT)(
    __in PWDF_BIND_INFO             Info,
    __deref_out   PWDF_COMPONENT_GLOBALS   * ComponentGlobals,
    __deref_inout PVOID                    * Context
    );

typedef
_Must_inspect_result_
NTSTATUS
(*PFNLIBRARYUNREGISTERCLIENT)(
    __in PWDF_BIND_INFO             Info,
    __in PWDF_COMPONENT_GLOBALS     DriverGlobals
    );

#define WDF_REGISTRY_DBGPRINT_ON   L"DbgPrintOn"

typedef struct _WDF_LIBRARY_INFO {
    ULONG                             Size;
    PFNLIBRARYCOMMISSION              LibraryCommission;
    PFNLIBRARYDECOMMISSION            LibraryDecommission;
    PFNLIBRARYREGISTERCLIENT          LibraryRegisterClient;
    PFNLIBRARYUNREGISTERCLIENT        LibraryUnregisterClient;
    WDF_VERSION                       Version;
} WDF_LIBRARY_INFO, *PWDF_LIBRARY_INFO;

//
// Client Driver information structure. This is used by loader when
// registering client with library to provide some additional info to
// library that is not already present in WDF_BIND_INFO (also passed to library
// during client registration)
//
typedef struct _CLIENT_INFO {
    //
    // Size of this structure
    //
    ULONG              Size;

    //
    // registry service path of client driver
    //
    PUNICODE_STRING    RegistryPath;

} CLIENT_INFO, *PCLIENT_INFO;

#ifndef __FXLDRUM_H__

typedef
_Must_inspect_result_
NTSTATUS
(*PWDF_REGISTER_LIBRARY)(
    __in  PWDF_LIBRARY_INFO   LibraryInfo,
    __in  PUNICODE_STRING     ServicePath,
    __in  PCUNICODE_STRING    LibraryDeviceName
    ) ;

typedef
_Must_inspect_result_
NTSTATUS
(*PWDF_VERSION_BIND)(
    __in  PDRIVER_OBJECT           DriverObject,
    __in  PUNICODE_STRING          RegistryPath,
    __in  PWDF_BIND_INFO           Info,
    __out PWDF_COMPONENT_GLOBALS * Globals
    ) ;

typedef
NTSTATUS
(*PWDF_VERSION_UNBIND)(
    __in PUNICODE_STRING         RegistryPath,
    __in PWDF_BIND_INFO          Info,
    __in PWDF_COMPONENT_GLOBALS  Globals
    );

#define WDF_LIBRARY_COMMISSION          LibraryCommission
#define WDF_LIBRARY_DECOMMISSION        LibraryDecommission
#define WDF_LIBRARY_REGISTER_CLIENT     LibraryRegisterClient
#define WDF_LIBRARY_UNREGISTER_CLIENT   LibraryUnregisterClient

// {49215DFF-F5AC-4901-8588-AB3D540F6021}
DEFINE_GUID(GUID_WDF_LOADER_INTERFACE_STANDARD, \
             0x49215dff, 0xf5ac, 0x4901, 0x85, 0x88, 0xab, 0x3d, 0x54, 0xf, 0x60, 0x21);

typedef struct _WDF_LOADER_INTERFACE {
    WDF_INTERFACE_HEADER        Header;
    PWDF_REGISTER_LIBRARY       RegisterLibrary;
    PWDF_VERSION_BIND           VersionBind;
    PWDF_VERSION_UNBIND         VersionUnbind;
    PWDF_LDR_DIAGNOSTICS_VALUE_BY_NAME_AS_ULONG DiagnosticsValueByNameAsULONG;
} WDF_LOADER_INTERFACE,  *PWDF_LOADER_INTERFACE;

VOID
__inline
WDF_LOADER_INTERFACE_INIT(
    PWDF_LOADER_INTERFACE Interface
    )
{
    RtlZeroMemory(Interface, sizeof(WDF_LOADER_INTERFACE));
    Interface->Header.InterfaceSize = sizeof(WDF_LOADER_INTERFACE);
    Interface->Header.InterfaceType = &GUID_WDF_LOADER_INTERFACE_STANDARD;
}

//-----------------------------------------------------------------------------
// WDFLDR.SYS exported function prototype definitions
//-----------------------------------------------------------------------------
_Must_inspect_result_
NTSTATUS
WdfVersionBind(
    __in    PDRIVER_OBJECT DriverObject,
    __in    PUNICODE_STRING RegistryPath,
    __inout PWDF_BIND_INFO BindInfo,
    __out   PWDF_COMPONENT_GLOBALS* ComponentGlobals
    );

NTSTATUS
WdfVersionUnbind(
    __in PUNICODE_STRING RegistryPath,
    __in PWDF_BIND_INFO BindInfo,
    __in PWDF_COMPONENT_GLOBALS ComponentGlobals
    );

_Must_inspect_result_
NTSTATUS
WdfRegisterLibrary(
    __in PWDF_LIBRARY_INFO LibraryInfo,
    __in PUNICODE_STRING ServicePath,
    __in PCUNICODE_STRING LibraryDeviceName
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, WdfVersionBind)
#pragma alloc_text (PAGE, WdfVersionUnbind)
#pragma alloc_text (PAGE, WdfRegisterLibrary)
#pragma alloc_text (PAGE, WdfRegisterClassLibrary)
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __FXLDRUM_H__

//
// Event name: WdfCensusEvtLinkClientToCx
//
// Source:      WdfLdr / WudfHost (UM loader)
// Description: Written when a client is binding to a class extension.
//              WdfVersionBindClass which is called from the client's stub,
//              will load/reference the Cx and add it to the fx library's
//              list of clients. The client driver's class extension list is
//              also updated at that time, which is when this event is written.
//
// Frequency: Everytime a client driver binds to a class extension.
//
//
#define WDF_CENSUS_EVT_WRITE_LINK_CLIENT_TO_CX(TraceHandle, CxImageName, ClientImageName, ClientVerMajor, ClientVerMinor, ClientVerBuild)        \
            TraceLoggingWrite(TraceHandle,                                     \
                "WdfCensusEvtLinkClientToCx",                                  \
                WDF_TELEMETRY_EVT_KEYWORDS,                                    \
                TraceLoggingWideString(CxImageName,     "CxImageName"),        \
                TraceLoggingWideString(ClientImageName, "ClientImageName"  ),  \
                TraceLoggingUInt32(ClientVerMajor,      "ClientVersionMajor"), \
                TraceLoggingUInt32(ClientVerMinor,      "ClientVersionMinor"), \
                TraceLoggingUInt32(ClientVerBuild,      "ClientVersionBuild")  \
                );

#endif __FXLDR_H__
