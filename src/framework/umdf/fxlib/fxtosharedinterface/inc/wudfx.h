//
// Copyright (c) Microsoft. All rights reserved.






#pragma once

//
// Forward defines
//

struct IFxMessageDispatch;
struct IWudfIrp;
struct IWudfIoIrp;
struct IWudfFile;
struct IWDFObject;
struct IObjectCleanup;
struct IWudfDeviceStack;
struct IWudfTargetCallbackDeviceChange;
struct IWudfIoDispatcher;
struct IWudfRemoteDispatcher;
struct IWudfDevice2;

#include <initguid.h>
#include <devpkey.h>

#include "TempTypedefinitions.h"

EXTERN_C const IID IID_IUnknown;

struct IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface
    (
        IN      REFIID,
        OUT     PVOID *
    ) = 0;

    virtual ULONG STDMETHODCALLTYPE AddRef
    (
    ) = 0;

    virtual ULONG STDMETHODCALLTYPE Release
    (
    ) = 0;
};

struct IWudfFile : public IUnknown
{
public:
    virtual LPCWSTR STDMETHODCALLTYPE GetName( void) = 0;
    
    virtual PVOID STDMETHODCALLTYPE GetCreateContext( void) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsDriverCreated( void) = 0;
    
    virtual DWORD STDMETHODCALLTYPE GetInitiatorProcessId( void) = 0;
    
    virtual const WCHAR *STDMETHODCALLTYPE GetCountedName( 
        /* [annotation][out] */ 
        __out  DWORD *pdwCountedNameLength) = 0;
    
};

EXTERN_C const IID IID_IWudfFile2;

struct IWudfFile2 : public IWudfFile
{
public:
    virtual HANDLE STDMETHODCALLTYPE GetWeakRefHandle( void) = 0;
    
};

EXTERN_C const IID IID_IWudfIoDispatcher;

struct IWudfIoDispatcher : public IUnknown
{
public:
    virtual void STDMETHODCALLTYPE Dispatch( 
        /* [annotation][in] */ 
        __in  IWudfIoIrp *pIrp,
        /* [annotation][in] */ 
        __in_opt  void *pCtx) = 0;
    
};

EXTERN_C const IID IID_IWudfRemoteDispatcher;

struct IWudfRemoteDispatcher : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE BindToHandle( 
        /* [annotation][in] */ 
        __in  HANDLE hFile) = 0;
    
    virtual void STDMETHODCALLTYPE CloseHandle( void) = 0;
    
    virtual HANDLE STDMETHODCALLTYPE GetHandle( void) = 0;
    
};

EXTERN_C const IID IID_IWudfTargetCallbackDeviceChange;

struct IWudfTargetCallbackDeviceChange : public IUnknown
{
public:
    virtual BOOL STDMETHODCALLTYPE OnQueryRemove( 
        /* [annotation][in] */ 
        __in  WUDF_TARGET_CONTEXT RegistrationID) = 0;
    
    virtual void STDMETHODCALLTYPE OnRemoveCanceled( 
        /* [annotation][in] */ 
        __in  WUDF_TARGET_CONTEXT RegistrationID) = 0;
    
    virtual void STDMETHODCALLTYPE OnRemoveComplete( 
        /* [annotation][in] */ 
        __in  WUDF_TARGET_CONTEXT RegistrationID) = 0;
    
    virtual void STDMETHODCALLTYPE OnCustomEvent( 
        /* [annotation][in] */ 
        __in  WUDF_TARGET_CONTEXT RegistrationID,
        /* [annotation][in] */ 
        __in  REFGUID Event,
        /* [annotation][in] */ 
        __in_bcount_opt(cbDataSize)  BYTE *pbData,
        /* [annotation][in] */ 
        __in  DWORD cbDataSize,
        /* [annotation][in] */ 
        __in  DWORD NameBufferOffset) = 0;
    
};

struct IWudfDevice : public IUnknown
//
// *** Note: 
// IWudfDevice must not be updated. This is to ensure backwards compatibility 
// with an older OS binary of UMDF 1.0 framework. Any additions to 
// IWudfDevice must be done in IWudfDevice2 interface defined later in this file.
//
{
public:
    virtual ULONG STDMETHODCALLTYPE GetDriverID( void) = 0;
    
    virtual ULONG STDMETHODCALLTYPE GetStackSize( void) = 0;
    
    virtual IFxMessageDispatch *STDMETHODCALLTYPE GetMessageDispatch( void) = 0;
    
    virtual IWudfDevice *STDMETHODCALLTYPE GetAttachedDevice( void) = 0;
    
    virtual IWudfDeviceStack *STDMETHODCALLTYPE GetDeviceStackInterface( void) = 0;
    
    virtual ULONG STDMETHODCALLTYPE GetDeviceObjectWdmFlags( void) = 0;
    
};

struct IWudfDevice2 : public IWudfDevice
{
public:
    virtual void STDMETHODCALLTYPE SetContext( 
        /* [annotation][in] */ 
        __in  void *DeviceContext) = 0;
    
    virtual void *STDMETHODCALLTYPE GetContext( void) = 0;
    
    virtual ULONG STDMETHODCALLTYPE GetStackSize2( void) = 0;

    virtual void STDMETHODCALLTYPE SetStackSize2( _In_ ULONG StackSize) = 0;
};

struct IWudfDeviceStack : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE CreateDevice( 
        /* [annotation][in] */ 
        __in  ULONG ulDriverID,
        /* [annotation][in] */ 
        __in  IFxMessageDispatch *pFxDevice,
        /* [annotation][out] */ 
        __out  IWudfDevice **ppWudfDevice) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE AttachDevice( 
        /* [annotation][in] */ 
        __in  IWudfDevice *pWudfDevice,
        /* [annotation][out] */ 
        __out  IWudfDevice **ppIAttachedWudfDevice) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE DetachDevice( 
        /* [annotation][in] */ 
        __in  IWudfDevice *pWudfDevice) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CreateWdfFile( 
        /* [annotation][in] */ 
        __in  IWudfDevice *pIWudfDevice,
        /* [annotation][unique][in] */ 
        __in_opt  IWudfDevice *pTargetIWudfDevice,
        /* [annotation][in] */ 
        __in  PCWSTR pcwszFileName,
        /* [annotation][out] */ 
        __out  IWudfFile **ppWudfFile) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CreateWdfFileForOpenHandle( 
        /* [annotation][in] */ 
        __in  IWudfDevice *pIWudfDevice,
        /* [annotation][unique][in] */ 
        __in_opt  IWudfDevice *pTargetIWudfDevice,
        /* [annotation][in] */ 
        __in  HANDLE hOpened,
        /* [annotation][out] */ 
        __out  IWudfFile **ppWudfFile) = 0;
    
    virtual void STDMETHODCALLTYPE CloseFile( 
        /* [annotation][in] */ 
        __in  IWudfFile *pWudfFile) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE AllocateIoIrp( 
        /* [annotation][in] */ 
        __in  IWudfDevice *pIWudfDevice,
        /* [annotation][in] */ 
        __in  ULONG ulStackSize,
        /* [annotation][out] */ 
        __out  IWudfIoIrp **ppIoIrp) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CreateDeviceInterface( 
        /* [annotation][in] */ 
        __in  LPCGUID pDeviceInterfaceGuid,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR pReferenceString) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetDeviceInterfaceState( 
        /* [annotation][in] */ 
        __in  LPCGUID pDeviceInterfaceGuid,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR pReferenceString,
        /* [annotation][in] */ 
        __in  BOOL Enable) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CreateSymbolicLink( 
        /* [annotation][unique][in] */ 
        __in  PCWSTR pSymbolicLink,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR pReferenceString) = 0;
    
    virtual void STDMETHODCALLTYPE RegisterMultiTransportDispatcher( 
        /* [annotation][in] */ 
        __in  IFxMessageDispatch *pDevice) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE RegisterRemoteInterfaceNotification( 
        /* [annotation][in] */ 
        __in  IFxMessageDispatch *pDevice,
        /* [annotation][in] */ 
        __in  LPCGUID pDeviceInterfaceGuid,
        /* [annotation][in] */ 
        __in  BOOL IncludeExistingInterfaces) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE PostEvent( 
        /* [annotation][in] */ 
        __in  REFGUID EventGuid,
        /* [annotation][in] */ 
        __in  WDF_EVENT_TYPE WdfEventType,
        /* [annotation][size_is][in] */ 
        __in_bcount(cbDataSize)  BYTE *pbData,
        /* [annotation][in] */ 
        __in  DWORD cbDataSize) = 0;
    
    virtual void STDMETHODCALLTYPE InvalidateDeviceState( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetPowerState( 
        /* [annotation][in] */ 
        _At_((DEVICE_POWER_STATE)NewState, _In_)  ULONG NewState,
        /* [annotation][unique][out] */ 
        _At_((PDEVICE_POWER_STATE)OldState, _Out_)  ULONG *OldState) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE RequestPowerIrp( 
        /* [annotation][in] */ 
        __in  UCHAR MinorFunction,
        /* [annotation][in] */ 
        __in  POWER_STATE PowerState,
        /* [annotation][in] */ 
        __in  PREQUEST_POWER_COMPLETE CompletionFunction,
        /* [annotation][in] */ 
        __in  PVOID Context) = 0;
    
    virtual WUDF_DISPATCHER_TYPE STDMETHODCALLTYPE GetDispatcherType( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SaveIdleState( 
        /* [annotation][in] */ 
        __in  BOOL Enabled) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE RetrieveIdleState( 
        /* [annotation][out] */ 
        __out  BOOL *Enabled) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CreateRegistryEntry( 
        /* [annotation][in] */ 
        __in  PWDF_PROPERTY_STORE_ROOT RootSpecifier,
        /* [annotation][in] */ 
        __in  WDF_PROPERTY_STORE_RETRIEVE_FLAGS Flags,
        /* [annotation][in] */ 
        __in  REGSAM DesiredAccess,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR SubKeyPath,
        /* [annotation][out] */ 
        __out  HKEY *hKey,
        /* [annotation][unique][out] */ 
        __out_opt  WDF_PROPERTY_STORE_DISPOSITION *pDisposition) = 0;
    
    virtual void STDMETHODCALLTYPE GetDeviceStackPreferredTransferMode( 
        /* [annotation][out] */ 
        __out  WDF_DEVICE_IO_BUFFER_RETRIEVAL *RetrievalMode,
        /* [annotation][out] */ 
        __out  WDF_DEVICE_IO_TYPE *RWPreference,
        /* [annotation][out] */ 
        __out  WDF_DEVICE_IO_TYPE *IoctlPreference) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetStackCapabilities( 
        /* [annotation][out] */ 
        __out  PSTACK_DEVICE_CAPABILITIES Capabilities) = 0;
    
    virtual void STDMETHODCALLTYPE SetD3ColdSupport( 
        /* [annotation][in] */ 
        __in  BOOLEAN UseD3Cold) = 0;
    
    virtual NTSTATUS STDMETHODCALLTYPE InitializeUsbSS( void) = 0;
    
    virtual void STDMETHODCALLTYPE SubmitUsbIdleNotification( 
        /* [annotation][in] */ 
        __in  PUSB_IDLE_CALLBACK_INFO UsbIdleCallbackInfo,
        /* [annotation][in] */ 
        __in  PWUDF_IO_COMPLETION_ROUTINE UsbSSCompletionRoutine,
        /* [annotation][in] */ 
        __in  PVOID UsbSSCompletionContext) = 0;
    
    virtual void STDMETHODCALLTYPE CancelUsbSS( void) = 0;
    
    virtual void STDMETHODCALLTYPE SignalUsbSSCallbackProcessingComplete( void) = 0;
    
    virtual NTSTATUS STDMETHODCALLTYPE UpdateIdleWakeWmiInstance( 
        /* [annotation][in] */ 
        __in  WmiIdleWakeInstanceUpdate UpdateType) = 0;
    
    virtual HKEY STDMETHODCALLTYPE GetDeviceRegistryKey( void) = 0;
    
    virtual IWudfDevice *STDMETHODCALLTYPE GetPPO( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetPPO( 
        /* [annotation][in] */ 
        __in  IWudfDevice *pDevice) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE RegisterTargetDeviceNotification( 
        /* [annotation][in] */ 
        __in  IWudfTargetCallbackDeviceChange *pCallback,
        /* [annotation][in] */ 
        __in  HANDLE hTarget,
        /* [annotation][out] */ 
        __out  WUDF_TARGET_CONTEXT *pRegistrationID) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE UnregisterTargetDeviceNotification( 
        /* [annotation][in] */ 
        __in  WUDF_TARGET_CONTEXT RegistrationID) = 0;
    
    virtual WUDF_INTERFACE_CONTEXT STDMETHODCALLTYPE GetNextRemoteInterfaceID( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE CreateRemoteDispatcher( 
        /* [annotation][out] */ 
        __out  IWudfIoDispatcher **ppIoDispatcher,
        /* [annotation][out] */ 
        __out  IWudfRemoteDispatcher **ppRemoteDispatcher) = 0;
    
    virtual PUMDF_VERSION_DATA STDMETHODCALLTYPE GetMinDriverVersion( void) = 0;
    
    virtual BOOL STDMETHODCALLTYPE ShouldPreserveIrpCompletionStatusCompatibility( void) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsNullFileObjectSupportEnabled( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE MapIoSpace( 
        /* [annotation][in] */ 
        __in  PHYSICAL_ADDRESS PhysicalAddress,
        /* [annotation][in] */ 
        __in  SIZE_T SizeInBytes,
        /* [annotation][in] */ 
        __in  MEMORY_CACHING_TYPE CacheType,
        /* [annotation][out] */ 
        __out  PVOID *MappedSystemAddress,
        /* [annotation][out] */ 
        __out_opt  PVOID *MappedUsermodeAddress) = 0;
    
    virtual void STDMETHODCALLTYPE UnmapIoSpace( 
        /* [annotation][in] */ 
        __in  PVOID MappedBaseAddress,
        /* [annotation][in] */ 
        __in  SIZE_T SizeInBytes) = 0;
    
    virtual void STDMETHODCALLTYPE ReadFromHardware( 
        /* [annotation][in] */ 
        __in  WDF_DEVICE_HWACCESS_TARGET_TYPE Type,
        /* [annotation][in] */ 
        __in  WDF_DEVICE_HWACCESS_TARGET_SIZE Size,
        /* [annotation][in] */ 
        __in  PVOID Register,
        /* [annotation][out] */ 
        __out_opt  SIZE_T *Value,
        /* [annotation][out] */ 
        __out_ecount_full_opt(Count)  PVOID Buffer,
        /* [annotation][in] */ 
        __in_opt  ULONG Count) = 0;
    
    virtual void STDMETHODCALLTYPE WriteToHardware( 
        /* [annotation][in] */ 
        __in  WDF_DEVICE_HWACCESS_TARGET_TYPE Type,
        /* [annotation][in] */ 
        __in  WDF_DEVICE_HWACCESS_TARGET_SIZE Size,
        /* [annotation][in] */ 
        __in  PVOID Register,
        /* [annotation][in] */ 
        __in  SIZE_T Value,
        /* [annotation][in] */ 
        __in_ecount_opt(Count)  PVOID Buffer,
        /* [annotation][in] */ 
        __in_opt  ULONG Count) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE InitializeInterrupt( 
        /* [annotation][in] */ 
        __in  WUDF_INTERRUPT_CONTEXT WudfInterruptContext,
        /* [annotation][in] */ 
        __in  HANDLE Event,
        /* [annotation][in] */ 
        __in  CM_SHARE_DISPOSITION ShareVector,
        /* [annotation][out] */ 
        __out  RD_INTERRUPT_CONTEXT *RdInterruptContext) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE ControlInterrupt( 
        /* [annotation][in] */ 
        __in  RD_INTERRUPT_CONTEXT RdInterruptContext,
        /* [annotation][in] */ 
        __in  InterruptControlType ControlType) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE AssignInterruptResources( 
        /* [annotation][in] */ 
        __in  RD_INTERRUPT_CONTEXT RdInterruptContext,
        /* [annotation][in] */ 
        __in  PCM_PARTIAL_RESOURCE_DESCRIPTOR RawResource,
        /* [annotation][in] */ 
        __in  PCM_PARTIAL_RESOURCE_DESCRIPTOR TransResource,
        /* [annotation][in] */ 
        __in  PWDF_INTERRUPT_INFO InterruptInfo,
        /* [annotation][in] */ 
        __in  BOOLEAN PassiveHandling) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE AcknowledgeInterrupt( 
        /* [annotation][in] */ 
        __in  RD_INTERRUPT_CONTEXT RdInterruptContext,
        /* [annotation][in] */ 
        __in  BOOLEAN Claimed) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetInterruptPolicy( 
        /* [annotation][in] */ 
        __in  RD_INTERRUPT_CONTEXT RdInterruptContext,
        /* [annotation][in] */ 
        __in  WDF_INTERRUPT_POLICY Policy,
        /* [annotation][in] */ 
        __in  WDF_INTERRUPT_PRIORITY Priority,
        /* [annotation][in] */ 
        __in  PGROUP_AFFINITY TargetProcessorSet) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetUnifiedPropertyData( 
        /* [annotation][in] */ 
        _In_  PWDF_PROPERTY_STORE_ROOT RootSpecifier,
        /* [annotation][in] */ 
        _In_  const DEVPROPKEY *PropertyKey,
        /* [annotation][in] */ 
        _In_  LCID Lcid,
        /* [annotation][in] */ 
        _In_  ULONG Flags,
        /* [annotation][in] */ 
        _In_  DEVPROPTYPE PropertyType,
        /* [annotation][in] */ 
        _In_  ULONG PropertyDataSize,
        /* [annotation][size_is][unique][in] */ 
        _In_opt_bytecount_(PropertyDataSize)  PVOID PropertyData) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetUnifiedPropertyData( 
        /* [annotation][in] */ 
        _In_  PWDF_PROPERTY_STORE_ROOT RootSpecifier,
        /* [annotation][in] */ 
        _In_  const DEVPROPKEY *PropertyKey,
        /* [annotation][in] */ 
        _In_  LCID Lcid,
        /* [annotation][in] */ 
        _In_  ULONG Flags,
        /* [annotation][in] */ 
        _In_  ULONG PropertyDataSize,
        /* [annotation][out] */ 
        _Out_  DEVPROPTYPE *PropertyType,
        /* [annotation][out] */ 
        _Out_  ULONG *PropertyDataRequiredSize,
        /* [annotation][size_is][unique][out] */ 
        _Out_opt_bytecap_(PropertyDataSize)  PVOID PropertyData) = 0;
    
    virtual void STDMETHODCALLTYPE SetHidInterfaceSupport( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE HidNotifyPresence( 
        /* [annotation][in] */ 
        __in  BOOLEAN IsPresent) = 0;
    
    virtual void STDMETHODCALLTYPE GetPdoProperties( 
        /* [annotation][out] */ 
        _Out_  LPCWSTR *HardwareIds,
        /* [annotation][out] */ 
        _Out_  LPCWSTR *SetupClass,
        /* [annotation][out] */ 
        _Out_  LPCWSTR *BusEnum,
        /* [annotation][out] */ 
        _Out_  LPCWSTR *Manufacturer) = 0;
    
};

EXTERN_C const IID IID_IWudfDeviceStack2;

//MIDL_INTERFACE("f71f77d3-9b3c-4037-a10f-44a4a22e4d79")
struct IWudfDeviceStack2 : public IWudfDeviceStack
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetInterfaceSymbolicLink( 
        /* [annotation][in] */ 
        __in  LPCGUID pDeviceInterfaceGuid,
        /* [annotation][unique][in] */ 
        __in_opt  PCWSTR pReferenceString,
        /* [annotation][out] */ 
        _Out_  PCWSTR *pSymbolicLink) = 0;

    virtual HRESULT STDMETHODCALLTYPE PoFxRegisterDevice( void ) = 0;

    virtual void STDMETHODCALLTYPE PoFxUnregisterDevice( void ) = 0;     

    virtual void STDMETHODCALLTYPE PoFxStartDevicePowerManagement( void ) = 0;     

    virtual void STDMETHODCALLTYPE PoFxIdleComponent( void ) = 0;     

    virtual void STDMETHODCALLTYPE PoFxActivateComponent( void ) = 0;  

    virtual void STDMETHODCALLTYPE PoFxCompleteDevicePowerNotRequired( void ) = 0;     
    
    virtual void STDMETHODCALLTYPE PoFxReportDevicePoweredOn( void ) = 0;  
    
    virtual void STDMETHODCALLTYPE PoFxSetDeviceIdleTimeout( 
        /* [annotation][in] */ 
       __in  ULONGLONG IdleTimeout) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE AllocateIfrMemory( 
        /* [annotation][in] */ 
        _In_  LPCWSTR ServiceName, 
        /* [annotation][in] */ 
        _In_  ULONG PageCount,
        /* [annotation][in] */ 
        _In_  BOOL IsDriverIfr,
        /* [annotation][in] */
        _In_  BOOL StoreInMinidump,
        /* [annotation][out] */ 
        _Out_  PVOID *Buffer,
        /* [annotation][out] */ 
        _Out_  ULONG *BufferLengthCb ) = 0;

    virtual HRESULT STDMETHODCALLTYPE UpdateProcessKeepAliveCount( 
        /* [annotation][in] */ 
        _In_  IWudfFile *FileObject,
        /* [annotation][in] */ 
        _In_  BOOL Increment) = 0;

    virtual HRESULT STDMETHODCALLTYPE ReenumerateSelf( void) = 0;

};

EXTERN_C const IID IID_IWudfIrpCompletionCallback;

// MIDL_INTERFACE("2fccd471-a14b-4190-a743-2ca1f86d637c")
struct IWudfIrpCompletionCallback : public IUnknown
{
public:
    virtual WUDF_COMPLETION_CALLBACK_STATUS STDMETHODCALLTYPE OnCompletion(
        /* [in] */
        __in  IFxMessageDispatch *pDevice,
        /* [in] */
        __in  IWudfIrp *pCompletedIrp,
        /* [in] */
        __in  PVOID pCtx) = 0;

};

EXTERN_C const IID IID_IWudfIrpCancelCallback;

// MIDL_INTERFACE("090B69BA-7606-4485-B919-BAD9FE2F506A")
struct IWudfIrpCancelCallback : public IUnknown
{
public:
    virtual void STDMETHODCALLTYPE OnCancel(
        /* [in] */
        __in  IWudfIoIrp *pCanceledIrp) = 0;

};

EXTERN_C const IID IID_IWudfIrp;

struct IWudfIrp : public IUnknown
//
// *** Note: 
// IWudfIrp must not be updated. This is to ensure backwards compatibility 
// with an older OS binary of UMDF 1.0 framework. 
// In case the IWudfIrp needs to be extended one option is to create a 
// IWudfIrpAddon interface that doesnt derive from anything. And then have 
// CWudfIoIrp and CWudfPnpIrp implement IWudfIrpAddon in addition to 
// what they already implement.
//
{
public:
    virtual void STDMETHODCALLTYPE CompleteRequest( void) = 0;
    
    virtual void STDMETHODCALLTYPE Complete( 
        /* [annotation][in] */ 
        __in  HRESULT hr,
        /* [annotation][in] */ 
        __in  ULONG_PTR Information) = 0;
    
    virtual void STDMETHODCALLTYPE Forward( void) = 0;
    
    virtual void STDMETHODCALLTYPE ForwardEx( 
        /* [annotation][unique][in] */ 
        __in_opt  IWudfDevice *pIWudfDevice) = 0;
    
    virtual void STDMETHODCALLTYPE SetCompletionRoutine( 
        /* [annotation][in] */ 
        __in  PWUDF_IO_COMPLETION_ROUTINE pWudfIoCompletionRoutine,
        /* [annotation][in] */ 
        __in_opt  PVOID pCtx) = 0;
    
    virtual void STDMETHODCALLTYPE CopyCurrentIrpStackLocationToNext( void) = 0;
    
    virtual void STDMETHODCALLTYPE SkipCurrentIrpStackLocation( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetCsqContext( 
        /* [annotation][in] */ 
        __in  PVOID pCtx) = 0;
    
    virtual void *STDMETHODCALLTYPE GetCsqContext( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetNextIrpStackLocation( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetStatus( 
        /* [annotation][in] */ 
        __in  NTSTATUS status) = 0;
    
    virtual NTSTATUS STDMETHODCALLTYPE GetStatus( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetStatusHr( 
        /* [annotation][in] */ 
        __in  HRESULT hrStatus) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetStatusHr( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetInformation( 
        /* [annotation][in] */ 
        __in  ULONG_PTR Information) = 0;
    
    virtual ULONG_PTR STDMETHODCALLTYPE GetInformation( void) = 0;
    
    virtual void STDMETHODCALLTYPE MarkIrpPending( void) = 0;
    
    virtual boolean STDMETHODCALLTYPE PendingReturned( void) = 0;
    
    virtual void STDMETHODCALLTYPE PropagatePendingReturned( void) = 0;
    
    virtual ULONG STDMETHODCALLTYPE GetRequestorProcessId( void) = 0;
    
    virtual PWUDF_DRIVER_CANCEL STDMETHODCALLTYPE SetCancelRoutine( 
        /* [annotation][unique][in] */ 
        __in_opt  PWUDF_DRIVER_CANCEL pNewCancelRoutine) = 0;
    
    virtual ULONGLONG STDMETHODCALLTYPE GetId( void) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsCanceled( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE ImpersonateRequestorProcess( 
        /* [annotation][in] */ 
        __in  SECURITY_IMPERSONATION_LEVEL RequestedLevel) = 0;
    
    virtual void STDMETHODCALLTYPE RevertImpersonation( void) = 0;
    
    virtual BOOL STDMETHODCALLTYPE Cancel( void) = 0;
    
    virtual PVOID STDMETHODCALLTYPE GetContext( 
        /* [annotation][in] */ 
        __in  ULONG Index) = 0;
    
    virtual void STDMETHODCALLTYPE SetContext( 
        /* [annotation][in] */ 
        __in  ULONG Index,
        /* [annotation][unique][in] */ 
        __in_opt  PVOID Value) = 0;
    
    virtual PLIST_ENTRY STDMETHODCALLTYPE GetListEntry( void) = 0;
    
    virtual void STDMETHODCALLTYPE ClearNextStackLocation( void) = 0;
    
};

EXTERN_C const IID IID_IWudfPnpIrp;

//MIDL_INTERFACE("52AE00B3-4067-40cb-A679-F18199921BF7")
struct IWudfPnpIrp : public IWudfIrp
//
// *** Note: 
// IWudfPnpIrp must not be updated. This is to ensure backwards compatibility 
// with an older OS binary of UMDF 1.0 framework. Any additions to 
// IWudfPnpIrp must be done in a new IWudfPnpIrp2 interface. For an example 
// see IWudfIoIrp2.
//
{
public:
    virtual UCHAR STDMETHODCALLTYPE GetMinorFunction( void) = 0;
    
    virtual UCHAR STDMETHODCALLTYPE GetMajorFunction( void) = 0;
    
    virtual POWER_STATE_TYPE STDMETHODCALLTYPE GetPowerType( void) = 0;
    
    virtual POWER_STATE STDMETHODCALLTYPE GetPowerState( void) = 0;
    
    virtual POWER_ACTION STDMETHODCALLTYPE GetPowerAction( void) = 0;
    
    virtual PDEVICE_CAPABILITIES STDMETHODCALLTYPE GetDeviceCapabilities( void) = 0;
    
    virtual SYSTEM_POWER_STATE STDMETHODCALLTYPE GetPowerStateSystemState( void) = 0;
    
    virtual DEVICE_POWER_STATE STDMETHODCALLTYPE GetPowerStateDeviceState( void) = 0;
    
    virtual ULONG STDMETHODCALLTYPE GetSystemPowerStateContext( void) = 0;
    
    virtual PCM_RESOURCE_LIST STDMETHODCALLTYPE GetParameterAllocatedResources( void) = 0;
    
    virtual PCM_RESOURCE_LIST STDMETHODCALLTYPE GetParameterAllocatedResourcesTranslated( void) = 0;
    
};

EXTERN_C const IID IID_IWudfIoIrp;
 
//    MIDL_INTERFACE("a04b21cf-8a23-45a9-9bf4-28ec6c3a2a1e")
struct IWudfIoIrp : public IWudfIrp
//
// *** Note: 
// IWudfIoIrp must not be updated. This is to ensure backwards compatibility 
// with an older OS binary of UMDF 1.0 framework. Any additions to 
// IWudfIoIrp must be done in IWudfIoIrp2 interface defined later in this file.
//
{
public:
    virtual void STDMETHODCALLTYPE Deallocate( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetFileForNextIrpStackLocation( 
        /* [annotation][in] */ 
        __in_opt  IWudfFile *pHostFile) = 0;
    
    virtual void STDMETHODCALLTYPE SetFrameworkFileObjectContext( 
        /* [annotation][in] */ 
        __in  IWudfDevice *pIWudfDevice,
        /* [annotation][in] */ 
        __in  IUnknown *FrameworkContext) = 0;
    
    virtual WDF_REQUEST_TYPE STDMETHODCALLTYPE GetType( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetTypeForNextStackLocation( 
        WDF_REQUEST_TYPE type) = 0;
    
    virtual IWudfFile *STDMETHODCALLTYPE GetFile( void) = 0;
    
    virtual void STDMETHODCALLTYPE GetCreateParameters( 
        /* [annotation][unique][out] */ 
        __out_opt  ULONG *pOptions,
        /* [annotation][unique][out] */ 
        __out_opt  USHORT *pFileAttributes,
        /* [annotation][unique][out] */ 
        __out_opt  USHORT *pShareAccess,
        /* [annotation][unique][out] */ 
        __out_opt  ACCESS_MASK *pDesiredAccess) = 0;
    
    virtual void STDMETHODCALLTYPE GetOtherParameters( 
        /* [annotation][unique][out] */ 
        __out_opt  PVOID *pArgument1,
        /* [annotation][unique][out] */ 
        __out_opt  PVOID *pArgument2,
        /* [annotation][unique][out] */ 
        __out_opt  PVOID *pArgument3,
        /* [annotation][unique][out] */ 
        __out_opt  PVOID *pArgument4) = 0;
    
    virtual void STDMETHODCALLTYPE SetOtherParametersForNextStackLocation( 
        /* [annotation][unique][in] */ 
        __in_opt  PVOID *pArgument1,
        /* [annotation][unique][in] */ 
        __in_opt  PVOID *pArgument2,
        /* [annotation][unique][in] */ 
        __in_opt  PVOID *pArgument3,
        /* [annotation][unique][in] */ 
        __in_opt  PVOID *pArgument4) = 0;
    
    virtual void STDMETHODCALLTYPE GetDeviceIoControlParameters( 
        /* [annotation][unique][out] */ 
        __out_opt  ULONG *pIoControlCode,
        /* [annotation][unique][out] */ 
        __out_opt  ULONG *pInputBufferLength,
        /* [annotation][unique][out] */ 
        __out_opt  ULONG *pOutputBufferLength) = 0;
    
    virtual void STDMETHODCALLTYPE SetDeviceIoControlParametersForNextStackLocation( 
        /* [annotation][in] */ 
        __in  ULONG IoControlCode,
        /* [annotation][in] */ 
        __in  ULONG InputBufferLength,
        /* [annotation][in] */ 
        __in  ULONG OutputBufferLength) = 0;
    
    virtual void STDMETHODCALLTYPE GetReadParameters( 
        /* [annotation][unique][out] */ 
        __out_opt  ULONG *pLength,
        /* [annotation][unique][out] */ 
        __out_opt  LONGLONG *pllOffset,
        /* [annotation][unique][out] */ 
        __out_opt  ULONG *pulKey) = 0;
    
    virtual void STDMETHODCALLTYPE SetReadParametersForNextStackLocation( 
        /* [annotation][in] */ 
        __in  ULONG Length,
        /* [annotation][in] */ 
        __in_opt  LONGLONG *pllOffset,
        /* [annotation][in] */ 
        __in  ULONG Key) = 0;
    
    virtual void STDMETHODCALLTYPE GetWriteParameters( 
        /* [annotation][unique][out] */ 
        __out_opt  ULONG *pLength,
        /* [annotation][unique][out] */ 
        __out_opt  LONGLONG *pllOffset,
        /* [annotation][unique][out] */ 
        __out_opt  ULONG *pulKey) = 0;
    
    virtual void STDMETHODCALLTYPE SetWriteParametersForNextStackLocation( 
        /* [annotation][in] */ 
        __in  ULONG Length,
        /* [annotation][in] */ 
        __in_opt  LONGLONG *pllOffset,
        /* [annotation][in] */ 
        __in  ULONG Key) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE RetrieveBuffers( 
        /* [annotation][out] */ 
        __out_opt  SIZE_T *InputBufferCb,
        /* [annotation][out] */ 
        __deref_opt_out_bcount(*InputBufferCb)  PVOID *InputBuffer,
        /* [annotation][out] */ 
        __out_opt  SIZE_T *OutputBufferCb,
        /* [annotation][out] */ 
        __deref_opt_out_bcount(*OutputBufferCb)  PVOID *OutputBuffer) = 0;
    
    virtual void STDMETHODCALLTYPE SwapCurrentBuffer( 
        /* [annotation][in] */ 
        __in  BOOL Valid,
        /* [annotation][in] */ 
        __in  ULONG NewInputBufferCb,
        /* [annotation][in] */ 
        __in_bcount_opt(NewInputBufferCb)  PVOID NewInputBuffer,
        /* [annotation][in] */ 
        __in  ULONG NewOutputBufferCb,
        /* [annotation][in] */ 
        __in_bcount_opt(NewOutputBufferCb)  PVOID NewOutputBuffer,
        /* [annotation][out] */ 
        __out  WUDFX_IRP_BUFFER_INFO *PreviousBuffer) = 0;
    
    virtual void STDMETHODCALLTYPE RestoreCurrentBuffer( 
        /* [annotation][in] */ 
        __in  PWUDFX_IRP_BUFFER_INFO PreviousBuffer) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsFrom32BitProcess( void) = 0;
    
    virtual void STDMETHODCALLTYPE Reuse( 
        /* [annotation][in] */ 
        __in  HRESULT hrNewStatus) = 0;
    
    virtual void STDMETHODCALLTYPE GetFrameworkRelatedFileObjectContext( 
        /* [annotation][in] */ 
        __in  IWudfDevice *pIWudfDevice,
        /* [annotation][out] */ 
        __out  IUnknown **ppFrameworkContext) = 0;
    
    virtual KPROCESSOR_MODE STDMETHODCALLTYPE GetRequestorMode( void) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsDriverCreated( void) = 0;
    
    virtual WDF_DEVICE_IO_TYPE STDMETHODCALLTYPE GetTransferMode( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetFlagsForNextStackLocation( 
        /* [annotation][in] */ 
        __in  WUDFX_IRP_STACK_FLAGS Flags) = 0;
    
    virtual void STDMETHODCALLTYPE GetQuerySetInformationParameters( 
        /* [annotation][unique][out] */ 
        __out_opt  WDF_FILE_INFORMATION_CLASS *InfoClass,
        /* [annotation][unique][out] */ 
        __out_opt  ULONG *Length) = 0;
    
    virtual void STDMETHODCALLTYPE SetQuerySetInformationParametersForNextStackLocation( 
        /* [annotation][in] */ 
        __in  WDF_FILE_INFORMATION_CLASS InfoClass,
        /* [annotation][in] */ 
        __in  ULONG Length) = 0;
    
    virtual BOOL STDMETHODCALLTYPE GetUserModeDriverInitiatedIo( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetUserModeDriverInitiatedIo( 
        /* [annotation][in] */ 
        __in  BOOL IsUserModeDriverInitiated) = 0;
    
    virtual BOOL STDMETHODCALLTYPE IsActivityIdSet( void) = 0;
    
    virtual LPCGUID STDMETHODCALLTYPE GetActivityId( void) = 0;
    
    virtual void STDMETHODCALLTYPE SetActivityId( 
        /* [annotation][in] */ 
        _In_  LPCGUID ActivityId) = 0;
    
};

EXTERN_C const IID IID_IWudfIoIrp2;
struct IWudfIoIrp2 : public IWudfIoIrp
{
public:
    virtual void STDMETHODCALLTYPE PrepareToForwardToSelf( void) = 0;

};

EXTERN_C const IID IID_IWDFObject;

struct IWDFObject : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE DeleteWdfObject( void) = 0;

    virtual HRESULT STDMETHODCALLTYPE AssignContext(
        /* [unique][in] */
        __in_opt  IObjectCleanup *pCleanupCallback,
        /* [unique][in] */
        __in_opt  void *pContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE RetrieveContext(
        /* [out] */
        __out  void **ppvContext) = 0;

    virtual void STDMETHODCALLTYPE AcquireLock( void) = 0;

    virtual void STDMETHODCALLTYPE ReleaseLock( void) = 0;

};

EXTERN_C const IID IID_IObjectCleanup;

struct IObjectCleanup : public IUnknown
{
public:
    virtual void STDMETHODCALLTYPE OnCleanup(
        /* [in] */
        __in  IWDFObject *pWdfObject) = 0;

};

EXTERN_C const IID IID_IWudfHost;

//MIDL_INTERFACE("2b3a5fa1-727f-4d7e-8956-6832cf3f7445")
struct IWudfHost : public IUnknown
{
public:
    virtual BOOL STDMETHODCALLTYPE IsCurrentThreadImpersonated( void) = 0;
    
    virtual LPCGUID STDMETHODCALLTYPE GetLifetimeId( void) = 0;
    
    virtual WUDF_BOOT_MODE STDMETHODCALLTYPE GetUmdfBootMode( void) = 0;
    
};

EXTERN_C const IID IID_IWudfHost2;

//MIDL_INTERFACE("e52d7af5-7598-4308-985d-06153fc46730")
struct IWudfHost2 : public IWudfHost
{
public:
    virtual IWudfIrp *STDMETHODCALLTYPE GetIrpFromListEntry( 
        /* [annotation][in] */ 
        __in  PLIST_ENTRY ListEntry) = 0;
    
};

EXTERN_C const IID IID_IFxMessageDispatch;

//MIDL_INTERFACE("5A84C307-3EAA-4413-A5DB-72C8BB9C6586")
struct IFxMessageDispatch : public IUnknown
{
public:
    virtual void STDMETHODCALLTYPE DispatchPnP( 
        /* [unique][in] */ IWudfIrp *pUMIrp) = 0;
    
    virtual void STDMETHODCALLTYPE CreateFile( 
        /* [annotation][in] */ 
        __in  IWudfIoIrp *pCreateIrp) = 0;
    
    virtual void STDMETHODCALLTYPE DeviceControl( 
        /* [annotation][in] */ 
        __in  IWudfIoIrp *pIrp,
        /* [annotation][in] */ 
        __in_opt  IUnknown *pFxContext) = 0;
    
    virtual void STDMETHODCALLTYPE ReadFile( 
        /* [annotation][in] */ 
        __in  IWudfIoIrp *pIrp,
        /* [annotation][in] */ 
        __in_opt  IUnknown *pFxContext) = 0;
    
    virtual void STDMETHODCALLTYPE WriteFile( 
        /* [annotation][in] */ 
        __in  IWudfIoIrp *pIrp,
        /* [annotation][in] */ 
        __in_opt  IUnknown *pFxContext) = 0;
    
    virtual void STDMETHODCALLTYPE CleanupFile( 
        /* [annotation][in] */ 
        __in  IWudfIoIrp *pUMIrp,
        /* [annotation][in] */ 
        __in  IUnknown *pFxContext) = 0;
    
    virtual void STDMETHODCALLTYPE CloseFile( 
        /* [annotation][in] */ 
        __in  IWudfIoIrp *pUMIrp,
        /* [annotation][in] */ 
        __in  IUnknown *pFxContext) = 0;
    
    virtual void STDMETHODCALLTYPE GetPreferredTransferMode( 
        /* [annotation][out] */ 
        __out  WDF_DEVICE_IO_BUFFER_RETRIEVAL *RetrievalMode,
        /* [annotation][out] */ 
        __out  WDF_DEVICE_IO_TYPE *RWPreference,
        /* [annotation][out] */ 
        __out  WDF_DEVICE_IO_TYPE *IoctlPreference) = 0;
    
    virtual void STDMETHODCALLTYPE FlushBuffers( 
        /* [annotation][in] */ 
        __in  IWudfIoIrp *pIrp,
        /* [annotation][in] */ 
        __in_opt  IUnknown *pFxContext) = 0;
    
    virtual void STDMETHODCALLTYPE QueryInformationFile( 
        /* [annotation][in] */ 
        __in  IWudfIoIrp *pIrp,
        /* [annotation][in] */ 
        __in_opt  IUnknown *pFxContext) = 0;
    
    virtual void STDMETHODCALLTYPE SetInformationFile( 
        /* [annotation][in] */ 
        __in  IWudfIoIrp *pIrp,
        /* [annotation][in] */ 
        __in_opt  IUnknown *pFxContext) = 0;
    
    virtual NTSTATUS STDMETHODCALLTYPE ProcessWmiPowerQueryOrSetData( 
        /* [annotation][in] */ 
        __in  RdWmiPowerAction Action,
        /* [annotation][out] */ 
        __out  BOOLEAN *QueryResult) = 0;
    
    virtual WUDF_INTERFACE_CONTEXT STDMETHODCALLTYPE RemoteInterfaceArrival( 
        /* [annotation][in] */ 
        __in  LPCGUID pDeviceInterfaceGuid,
        /* [annotation][in] */ 
        __in  PCWSTR pSymbolicLink) = 0;
    
    virtual void STDMETHODCALLTYPE RemoteInterfaceRemoval( 
        /* [annotation][in] */ 
        __in  WUDF_INTERFACE_CONTEXT RemoteInterfaceID) = 0;
    
    virtual BOOL STDMETHODCALLTYPE TransportQueryID( 
        /* [annotation][in] */ 
        __in  DWORD Id,
        /* [annotation][in] */ 
        __in_bcount(cbDataBufferSize)  PVOID DataBuffer,
        /* [annotation][in] */ 
        __in  SIZE_T cbDataBufferSize) = 0;
    
};

EXTERN_C const IID IID_IFxMessageDispatch2;

// MIDL_INTERFACE("3b183190-d4a2-4f96-932d-e9c18e9a5222")
struct IFxMessageDispatch2 : public IFxMessageDispatch
{
public:
    virtual ULONG STDMETHODCALLTYPE GetDirectTransferThreshold( void) = 0;
    
    virtual void STDMETHODCALLTYPE PoFxDevicePowerNotRequired( void) = 0;

    virtual void STDMETHODCALLTYPE PoFxDevicePowerRequired( void) = 0;    
};
