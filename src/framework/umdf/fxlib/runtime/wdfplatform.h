//
// Copyright (c) Microsoft. All rights reserved.





#pragma once

typedef PVOID HDEVINFO;
typedef PVOID PSP_DEVINFO_DATA;

typedef struct WUDF_PROBLEM WUDF_PROBLEM, *PWUDF_PROBLEM;

typedef struct IUMDFTimer IUMDFTimer;
typedef struct IUMDFEvent IUMDFEvent;
typedef struct IUMDFLock IUMDFLock;
typedef struct IUMDFSemaphore IUMDFSemaphore;
typedef struct IUMDFThread IUMDFThread;
typedef struct IUMDFCompletionPort IUMDFCompletionPort;
typedef struct IUMDFTraceSession IUMDFTraceSession;
typedef struct IUMDFStackTracker IUMDFStackTracker;

typedef UINT64 WUDF_TARGET_CONTEXT;

#define WUDF_TARGET_CONTEXT_INVALID 0
typedef UINT64 WUDF_INTERFACE_CONTEXT;

#define WUDF_INTERFACE_CONTEXT_INVALID 0
struct _WDF_ACTIVATION_FRAME
    {
    struct _WDF_ACTIVATION_FRAME **ListHead;
    struct _WDF_ACTIVATION_FRAME *NextFrame;
    void *Caller;
    } ;
typedef struct _WDF_ACTIVATION_FRAME WDF_ACTIVATION_FRAME;

typedef struct _WDF_ACTIVATION_FRAME *PWDF_ACTIVATION_FRAME;

typedef /* [public] */ struct __MIDL___MIDL_itf_wdfplatform_0000_0000_0001
{
    PVOID DllBase;
    PVOID EntryPoint;
    DWORD SizeOfImage;
    HMODULE ModuleHandle;
    BOOL IsWudfComponent;
    PWSTR FullModuleName;
    PWSTR BaseModuleName;
} 	WDF_MODULE_INFO;

typedef struct __MIDL___MIDL_itf_wdfplatform_0000_0000_0001 *PWDF_MODULE_INFO;
typedef const struct __MIDL___MIDL_itf_wdfplatform_0000_0000_0001 *PCWDF_MODULE_INFO;

typedef 
enum WDF_MODULE_INFO_CHANGE_TYPE
    {
        ModuleLoaded	= 0,
        ModuleUnloaded	= ( ModuleLoaded + 1 ) 
    } 	WDF_MODULE_INFO_CHANGE_TYPE;

typedef 
enum WDF_REGISTRY_READ_RESULT
    {
        ValuePresent	= 0,
        ValueNotPresent	= ( ValuePresent + 1 ) ,
        ValueInvalid	= ( ValueNotPresent + 1 ) 
    } 	WDF_REGISTRY_READ_RESULT;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_wdfplatform_0000_0000_0002
    {
    GUID TraceGuid;
    ULONG Flags;
    ULONG Level;
    } 	WDF_ACTIVITY_LOG_TRACE_GUID;

typedef struct __MIDL___MIDL_itf_wdfplatform_0000_0000_0002 *PWDF_ACTIVITY_LOG_TRACE_GUID;

typedef const struct __MIDL___MIDL_itf_wdfplatform_0000_0000_0002 *PCWDF_ACTIVITY_LOG_TRACE_GUID;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_wdfplatform_0000_0000_0003
    {
    BOOL bTrackObjects;
    BOOL bTrackRefCounts;
    ULONG nMaxRefCountChangesToTrack;
    ULONG nMaxStackDepthToTrack;
    } 	WDF_OBJECT_TRACKING_SETTINGS;

typedef struct __MIDL___MIDL_itf_wdfplatform_0000_0000_0003 *PWDF_OBJECT_TRACKING_SETTINGS;

typedef /* [public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_wdfplatform_0000_0000_0004
    {
    PVOID Reserved;
    } 	WDF_MODULE_LIST_HANDLE;

typedef /* [public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_wdfplatform_0000_0000_0005
    {
    PVOID Reserved;
    } 	WDF_VERSION_DATA_HANDLE;

typedef struct WUDF_PLATFORM_THREAD_DATA
    {
    IUMDFThread *Thread;
    PWDF_ACTIVATION_FRAME TrackerList;
    HANDLE OverlappedEvent;
    } 	WUDF_PLATFORM_THREAD_DATA;

typedef struct WUDF_PLATFORM_THREAD_DATA *PWUDF_PLATFORM_THREAD_DATA;

typedef ULONG ( *WudfPlatformThreadRoutine )( 
    /* [annotation][in] */ 
    __in  PVOID StartContext);

typedef HRESULT ( *WudfPlatformOnThreadStart )( 
    /* [annotation][in] */ 
    __in  IUMDFThread *Thread,
    /* [annotation][in] */ 
    __in  PVOID Context);

typedef HRESULT ( *WudfPlatformOnThreadStop )( 
    /* [annotation][in] */ 
    __in  IUMDFThread *Thread,
    /* [annotation][in] */ 
    __in  PVOID Context);

typedef void ( *WudfModuleInfoChangeCallback )( 
    /* [annotation][unique][in] */ 
    __in_opt  PVOID ChangeCallbackContext,
    /* [annotation][in] */ 
    __in  PCWDF_MODULE_INFO ChangedInfo,
    /* [annotation][in] */ 
    __in  WDF_MODULE_INFO_CHANGE_TYPE ChangeType);



typedef 
enum WudfBreakPointType
    {
        WudfPlatformUserBreakin	= 0x1,
        WudfPlatformKernelBreakin	= 0x2,
        WudfPlatformUserOrKernelBreakin	= ( WudfPlatformUserBreakin | WudfPlatformKernelBreakin ) 
    } 	WudfBreakPointType;

typedef 
enum WudfDebuggerType
    {
        WudfPlatformUserDebugger	= 0,
        WudfPlatformUserOrKernelDebugger	= ( WudfPlatformUserDebugger + 1 ) 
    } 	WudfDebuggerType;

struct IUMDFPlatform : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE Initialize( 
        /* [annotation][in] */ 
        __in  WdfComponentType Component,
        /* [annotation][in] */ 
        __in  PCWSTR RootDirectoryPath,
        /* [annotation][in] */ 
        __in  BOOL AddContextToProcessThreads,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR Identifier,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR GlobalTraceSessionName,
        /* [annotation][unique][in] */ 
        __in_opt  LPCGUID HostLifetimeId) = 0;
    
    virtual PCWSTR STDMETHODCALLTYPE GetVersion( 
        /* [annotation][unique][out] */ 
        __out_opt  ULONG *VersionStringCch) = 0;
    
    virtual LPCGUID STDMETHODCALLTYPE GetLifetimeId( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CreateThread( 
        /* [annotation][in] */ 
        __in  WudfPlatformThreadRoutine StartAddress,
        /* [annotation][in] */ 
        __in  PVOID Parameter,
        /* [annotation][out] */ 
        __out  IUMDFThread **Thread) = 0;
    
    virtual void STDMETHODCALLTYPE SetThreadCallbacks( 
        /* [annotation][in] */ 
        __in  WudfPlatformOnThreadStart StartCallback,
        /* [annotation][in] */ 
        __in  WudfPlatformOnThreadStop StopCallback,
        /* [annotation][in] */ 
        __in  PVOID Context) = 0;
    
    virtual BOOL STDMETHODCALLTYPE AllocateLock( 
        /* [annotation][out] */ 
        __out  IUMDFLock **Lock) = 0;
    
    virtual BOOL STDMETHODCALLTYPE AllocateEvent( 
        /* [annotation][in] */ 
        __in  EVENT_TYPE EventType,
        /* [annotation][out] */ 
        __out  IUMDFEvent **Event) = 0;
    
    virtual BOOL STDMETHODCALLTYPE AllocateSemaphore( 
        /* [annotation][out] */ 
        __out  IUMDFSemaphore **Semaphore) = 0;
    
    virtual BOOL STDMETHODCALLTYPE CreateTimer( 
        /* [annotation][out] */ 
        __out  IUMDFTimer **Timer) = 0;
    
    virtual LARGE_INTEGER STDMETHODCALLTYPE ConvertSecondsToTimerUnit( 
        /* [annotation][in] */ 
        __in  ULONG NumSeconds) = 0;
    
    virtual LONG STDMETHODCALLTYPE ConvertSecondsToTimerPeriodUnit( 
        /* [annotation][in] */ 
        __in  ULONG NumSeconds) = 0;
    
    virtual BOOLEAN STDMETHODCALLTYPE CreateIoCompletionPort( 
        /* [annotation][out] */ 
        __out  IUMDFCompletionPort **Port) = 0;
    
    virtual PVOID STDMETHODCALLTYPE AllocateMemory( 
        /* [annotation][in] */ 
        __in  POOL_TYPE PoolType,
        /* [annotation][in] */ 
        __in  SIZE_T Size,
        /* [annotation][in] */ 
        __in  ULONG Tag) = 0;
    
    virtual void STDMETHODCALLTYPE FreeMemory( 
        /* [annotation][in] */ 
        __in  PVOID AllocationHeader) = 0;
    
    virtual NTSTATUS STDMETHODCALLTYPE AllocateGUIDString( 
        /* [annotation][unique][string][in] */ 
        __in_opt  PCWSTR GuidStringPrefix1,
        /* [annotation][unique][string][in] */ 
        __in_opt  PCWSTR GuidStringPrefix2,
        /* [annotation][unique][out] */ 
        __out_opt  PWSTR *GuidString,
        /* [annotation][unique][out] */ 
        __out_opt  GUID *Guid) = 0;
    
    virtual PSECURITY_DESCRIPTOR STDMETHODCALLTYPE AllocateSecurityDescriptor( 
        /* [annotation][in] */ 
        __in_ecount_opt(NumServerSIDs)  PSID *ServerSIDs,
        /* [annotation][in] */ 
        __in_opt  ULONG NumServerSIDs) = 0;
    
    virtual PSID STDMETHODCALLTYPE AllocateWellKnownSid( 
        /* [annotation][in] */ 
        __in  ULONG SidType) = 0;
    
    virtual PSID STDMETHODCALLTYPE AllocateServiceSid( 
        /* [annotation][in] */ 
        __in  PCWSTR ServiceName) = 0;
    
    virtual PSID STDMETHODCALLTYPE AllocateMySid( void) = 0;
    
    virtual void STDMETHODCALLTYPE FreeSid( 
        /* [annotation][in] */ 
        __in  PSID Sid) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CreateHostTokenAndSid( 
        /* [annotation][in] */ 
        __in  PCWSTR AccountName,
        /* [annotation][in] */ 
        __in  PCWSTR Authority,
        /* [annotation][out] */ 
        __out  HANDLE *HostToken,
        /* [annotation][out] */ 
        __out  PSID *HostSid) = 0;
    
    virtual void STDMETHODCALLTYPE FreeHostTokenAndSid( 
        /* [annotation][unique][in] */ 
        __in_opt  HANDLE HostToken,
        /* [annotation][unique][in] */ 
        __in_opt  PSID HostSid) = 0;
    
    virtual WDF_REGISTRY_READ_RESULT STDMETHODCALLTYPE ReadRegistryDword( 
        /* [annotation][in] */ 
        __in  HKEY Key,
        /* [annotation][in] */ 
        __in  PCWSTR ValueName,
        /* [annotation][out] */ 
        __out  DWORD *Value) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE WriteRegistryDword( 
        /* [annotation][in] */ 
        __in  HKEY Key,
        /* [annotation][in] */ 
        __in  PCWSTR ValueName,
        /* [annotation][in] */ 
        __in  DWORD Value) = 0;
    
    virtual PWDF_ACTIVATION_FRAME *STDMETHODCALLTYPE GetActivationListHead( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CaptureLoadedModuleList( 
        /* [annotation][out] */ 
        __out  WDF_MODULE_LIST_HANDLE *ModuleList) = 0;
    
    virtual PCWDF_MODULE_INFO STDMETHODCALLTYPE FindLoadedModule( 
        /* [annotation][in] */ 
        __in  WDF_MODULE_LIST_HANDLE ModuleList,
        /* [annotation][in] */ 
        __in  PVOID Address) = 0;
    
    virtual PCWDF_MODULE_INFO STDMETHODCALLTYPE FindDriverModule( 
        /* [annotation][in] */ 
        __in  WDF_MODULE_LIST_HANDLE ModuleList,
        /* [annotation][unique][in] */ 
        __in_opt  PWDF_ACTIVATION_FRAME ActivationList,
        /* [annotation][unique][out] */ 
        __out_opt  PWDF_ACTIVATION_FRAME *MatchingFrame) = 0;
    
    virtual void STDMETHODCALLTYPE CompareLoadedModuleLists( 
        /* [annotation][in] */ 
        __in  WDF_MODULE_LIST_HANDLE OldModuleList,
        /* [annotation][in] */ 
        __in  WDF_MODULE_LIST_HANDLE NewModuleList,
        /* [annotation][in] */ 
        __in  WudfModuleInfoChangeCallback ChangeCallback,
        /* [annotation][unique][in] */ 
        __in_opt  PVOID ChangeCallbackContext) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetModuleVersionData( 
        /* [annotation][in] */ 
        __in  PCWSTR FullModulePath,
        /* [annotation][out] */ 
        __out  WDF_VERSION_DATA_HANDLE *VersionData) = 0;
    
    virtual VS_FIXEDFILEINFO *STDMETHODCALLTYPE QueryVersionFixedFileInfo( 
        /* [annotation][in] */ 
        __in  WDF_VERSION_DATA_HANDLE VersionData) = 0;
    
    virtual PCWSTR STDMETHODCALLTYPE QueryVersionString( 
        /* [annotation][in] */ 
        __in  WDF_VERSION_DATA_HANDLE VersionData,
        /* [annotation][in] */ 
        __in  PCWSTR StringName,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR NotFoundValue) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE QueryVersionValue( 
        /* [annotation][in] */ 
        __in  WDF_VERSION_DATA_HANDLE VersionData,
        /* [annotation][in] */ 
        __in  PCWSTR SubBlock,
        /* [annotation][out] */ 
        __out  PVOID *Buffer,
        /* [annotation][out] */ 
        __out  ULONG *BufferLength) = 0;
    
    virtual void STDMETHODCALLTYPE ReleaseModuleVersionData( 
        /* [annotation][in] */ 
        __in  WDF_VERSION_DATA_HANDLE VersionData) = 0;
    
    virtual void STDMETHODCALLTYPE ReleaseLoadedModuleList( 
        /* [annotation][in] */ 
        __in  WDF_MODULE_LIST_HANDLE ModuleList) = 0;
    
    virtual PCWSTR STDMETHODCALLTYPE GetLogDirectory( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CreateActivityTrace( 
        /* [annotation][in] */ 
        __in  PCWSTR SessionName,
        /* [annotation][in] */ 
        __in  PCWSTR TraceFilePrefix,
        /* [annotation][in] */ 
        __in  BOOLEAN AddUniqueSuffix,
        /* [annotation][out] */ 
        __out  IUMDFTraceSession **TraceSession) = 0;
    
    virtual void STDMETHODCALLTYPE UmdfDriverStop( 
        /* [annotation][in] */ 
        __in  WdfDriverStopType Type,
        /* [annotation][in] */ 
        __in  ULONGLONG ErrorNumber,
        /* [annotation][in] */ 
        __in  PCWSTR Location,
        /* [annotation][in] */ 
        __in  PCSTR Message,
        /* [annotation][in] */ 
        __in  PCONTEXT ContextRecord) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsUserDebuggerPresent( void) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsKernelDebuggerPresent( void) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsAnyDebuggerPresent( void) = 0;
    
    virtual void STDMETHODCALLTYPE DebugBreakPoint( 
        /* [annotation][in] */ 
        __in  WudfBreakPointType Type) = 0;
    
    virtual void STDMETHODCALLTYPE WaitForDebugger( 
        /* [annotation][in] */ 
        __in  ULONG TimeoutSeconds,
        /* [annotation][in] */ 
        __in  WudfDebuggerType Type) = 0;
    
    virtual DWORD STDMETHODCALLTYPE GetDeviceRegistryPropertyString( 
        /* [annotation][in] */ 
        __in  HDEVINFO DeviceInfoSet,
        /* [annotation][in] */ 
        __in  PSP_DEVINFO_DATA DeviceInfoData,
        /* [annotation][in] */ 
        __in  DWORD Property,
        /* [annotation][out] */ 
        __out_ecount(*PropertyBufferCch)  PWSTR *PropertyBuffer,
        /* [annotation][out] */ 
        __out_opt  DWORD *PropertyBufferCch) = 0;
    
    virtual BOOL STDMETHODCALLTYPE ReportHostProblem( 
        /* [annotation][in] */ 
        __in  HANDLE HostProcess,
        /* [annotation][in] */ 
        __in  PWUDF_PROBLEM Problem,
        /* [annotation][in] */ 
        __in  BOOLEAN HostInDebugMode,
        /* [annotation][in] */ 
        __in  PCWSTR LogDirectory,
        /* [annotation][in] */ 
        __in  PCWSTR InstanceId,
        /* [annotation][in] */ 
        __in  LPCGUID HostLifetimeId,
        /* [annotation][in] */ 
        __in_opt  PCWSTR HardwareId) = 0;
    
    virtual void STDMETHODCALLTYPE SetCrashReportHardwareId( 
        /* [annotation][in] */ 
        __in_opt  PCWSTR CrashReportHardwareId) = 0;
    
    virtual void STDMETHODCALLTYPE RemoveCrashReportHardwareId( 
        /* [annotation][in] */ 
        __in_opt  PCWSTR CrashReportHardwareId) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE ConfigureObjectTracking( 
        /* [annotation][in] */ 
        __in  PWDF_OBJECT_TRACKING_SETTINGS Settings) = 0;
    
    virtual void STDMETHODCALLTYPE GetObjectTrackingSettings( 
        /* [annotation][out] */ 
        __out  WDF_OBJECT_TRACKING_SETTINGS *Settings) = 0;
    
    virtual IUMDFStackTracker *STDMETHODCALLTYPE GetStackTracker( void) = 0;
    
    virtual BOOL STDMETHODCALLTYPE CancelIoEx( 
        /* [annotation][in] */ 
        __in  PCWSTR ControlDeviceName,
        /* [annotation][in] */ 
        __in  HANDLE hFile,
        /* [annotation][in] */ 
        __in  LPOVERLAPPED lpOverlapped) = 0;
    
    virtual BOOL STDMETHODCALLTYPE CancelSynchronousIo( 
        /* [annotation][in] */ 
        __in  PCWSTR ControlDeviceName,
        /* [annotation][in] */ 
        __in  HANDLE hThread) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE DuplicateString( 
        /* [annotation][in] */ 
        __in  ULONG Tag,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR Source,
        /* [annotation][out] */ 
        __out_ecount(*CchReturned)  PWSTR *Destination,
        /* [annotation][unique][out] */ 
        __out_opt  SIZE_T *CchReturned) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE ConcatenateStrings( 
        /* [annotation][in] */ 
        __in  ULONG Tag,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR String1,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR String2,
        /* [annotation][out] */ 
        __out_ecount(*CchReturned)  PWSTR *Destination,
        /* [annotation][unique][out] */ 
        __out_opt  SIZE_T *CchReturned) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CanonicalizePath( 
        /* [annotation][in] */ 
        __in  PCWSTR Path,
        /* [annotation][out] */ 
        __out  PWSTR *CanonicalizedPath,
        /* [annotation][unique][out] */ 
        __out_opt  SIZE_T *CanonicalizedPathCch) = 0;
    
    virtual DWORD STDMETHODCALLTYPE GetTlsIndex( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetDebugMode( 
        /* [annotation][in] */ 
        __in  BOOLEAN Value) = 0;
    
};

//
// We're currently using this file instead of the Wdfplatform.h generated from the .idl. We do the same in
// multiple spots. This is worth revisiting to see why we're using snapshots (see wudfx_i.c for this pattern).
//
struct IUMDFPlatform2 : public IUMDFPlatform
{
public:
    
   virtual void STDMETHODCALLTYPE UmdfDriverStopWithActiveDriver( 
        /* [annotation][in] */ 
        __in  WdfDriverStopType Type,
        /* [annotation][in] */ 
        __in  ULONGLONG ErrorNumber,
        /* [annotation][in] */ 
        __in  PCWSTR Location,
        /* [annotation][in] */ 
        __in  PCSTR Message,
        /* [annotation][in] */ 
        __in  PCONTEXT ContextRecord,
        /* [annotation][in] */
        __in PSTR ActiveDriver) = 0;

};

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

MIDL_DEFINE_GUID(IID, IID_IUMDFPlatform2, 0x601c2a5b, 0x0ce5, 0x432b, 0xb7, 0x64, 0xeb, 0xcf, 0xcf, 0x66, 0x43, 0xb3);



