/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDriverUm.cpp

Abstract:

    This is the main driver framework.

Author:



Environment:

    User mode only

Revision History:



--*/

#include "coreprivshared.hpp"
#include "fxiotarget.hpp"
#include "fxldrum.h"

// Tracing support
extern "C" {
#include "FxDriverUm.tmh"
}


_Must_inspect_result_
NTSTATUS
FxDriver::AddDevice (
    _In_  PDRIVER_OBJECT_UM         DriverObject,
    _In_  PVOID                     Context,
    _In_  PWUDF_DRIVER_LOAD_CONTEXT DriverLoadContext,
    _In_  LPCWSTR                   KernelDeviceName,
    _In_opt_ HKEY                   PdoKey,
    _In_  LPCWSTR                   ServiceName,
    _In_  LPCWSTR                   DevInstanceID,
    _In_  ULONG                     DriverID
    )
{
    FxDriver *pDriver;

    //
    // Context parameter is CWudfDriverGlobals in legacy UMDF. Not used in
    // UMDF 2.0
    //
    UNREFERENCED_PARAMETER(Context);

    pDriver = FxDriver::GetFxDriver(DriverObject);



    if (pDriver != NULL) {
        if(DriverLoadContext->DriverType != WUDF_DRIVER_TYPE_COMPANION) {
            return pDriver->AddDevice(DriverLoadContext->DeviceStack,
                                      KernelDeviceName,
                                      PdoKey,
                                      ServiceName,
                                      DevInstanceID,
                                      DriverID
                                      );
        }
        else {
            return pDriver->AddCompanion(DriverLoadContext->Companion);
        }
    }

    return STATUS_UNSUCCESSFUL;
}

_Must_inspect_result_
NTSTATUS
FxDriver::AddCompanion(
    _In_  IWudfCompanion *  Companion
    )
{
    NTSTATUS status;
    WDFDEVICE_INIT init(this);
    FxCompanion* pCompanion;

    init.InitType = FxDeviceInitTypeCompanion;
    init.Companion = Companion;

    DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_INFORMATION, TRACINGPNP,
                        "Invoke EvtCompanionAdd for %s",
                        GetDriverGlobals()->Public.DriverName);

    //
    // Invoke driver's AddDevice callback
    //
    // Driver is expected to call WdfCompanionCreate in this call back. Failure
    // to do so will result in below call returning an error
    //
    status = InvokeDeviceAdd(&init);

    //
    // Retrieve the created FxCompanion by driver calling
    // WdfCompanionCreate in DriverDeviceAdd callback
    //
    pCompanion = (FxCompanion*) init.CreatedDevice;

    DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_INFORMATION, TRACINGPNP,
                        "EvtCompanionAdd created WDFCOMPANION %p returned %!STATUS!",
                        (pCompanion == NULL ? NULL : pCompanion->GetHandle()), status);

    if (pCompanion == NULL) {
        ASSERT(status != STATUS_SUCCESS);
    }
    else if (NT_SUCCESS(status)) {
        pCompanion->FinishInitializing();
    }
    else {
        //
        // Created a companion, but returned error.
        //
        pCompanion->DeleteCompanionFromFailedCreate();
        pCompanion = NULL;
    }

    return status;
}

_Must_inspect_result_
NTSTATUS
FxDriver::InvokeDeviceAdd(
    _In_  PWDFDEVICE_INIT Init
    )
/*
    Calls the clients EvtDriverAddDevice callback
    and returns a failure if the client fails to
    create a WDFDEVICE/WDFCOMPANION for umdf
    drivers and companions respectively.
*/
{
    NTSTATUS status;

    BOOLEAN isCompanion = Init->IsCompanionInit();
    PCSTR deviceTypeName = (isCompanion ? "WDFCOMPANION" : "WDFDEVICE");

    //
    // Invoke driver's AddDevice callback
    //
    status = m_DriverDeviceAdd.Invoke(GetHandle(), Init);

    //
    // Caller returned w/out creating a device, we are done.  Returning
    // STATUS_SUCCESS w/out creating a device and attaching to the stack is OK,
    // especially for filter drivers which selectively attach to devices.
    //
    if (Init->CreatedDevice == NULL) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_WARNING, TRACINGPNP,
                            "Driver did not create a %s in "
                            "EvtDriverAddDevice, status %!STATUS!",
                            deviceTypeName, status);
























        if (NT_SUCCESS(status))
        {
            DoTraceLevelMessage(
                GetDriverGlobals(), TRACE_LEVEL_ERROR, TRACINGPNP,
                "Driver returned %!STATUS! without creating a %s, "
                "converting to %!STATUS!", status, deviceTypeName, STATUS_INVALID_DEVICE_STATE);
            status = STATUS_INVALID_DEVICE_STATE;
        }



    }

    return status;
}


_Must_inspect_result_
NTSTATUS
FxDriver::AddDevice(
    _In_  IWudfDeviceStack *        DevStack,
    _In_  LPCWSTR                   KernelDeviceName,
    _In_opt_ HKEY                   PdoKey,
    _In_  LPCWSTR                   ServiceName,
    _In_  LPCWSTR                   DevInstanceID,
    _In_  ULONG                     DriverID
    )
{
    WDFDEVICE_INIT init(this);
    FxDevice* pDevice;
    NTSTATUS status;
    HRESULT hr = S_OK;
    LONG lRetVal = -1;

    DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                        "Enter AddDevice DevStack %p", DevStack);

    //FX_VERIFY(INTERNAL, CHECK_NOT_NULL(DevStack));
    //FX_VERIFY(INTERNAL, CHECK_NOT_NULL(KernelDeviceName));
    //FX_VERIFY(INTERNAL, CHECK_HANDLE(PdoKey));
    //FX_VERIFY(INTERNAL, CHECK_NOT_NULL(ServiceName));
    //FX_VERIFY(INTERNAL, CHECK_NOT_NULL(DevInstanceID));

    pDevice = NULL;
    init.CreatedOnStack = TRUE;
    init.InitType = FxDeviceInitTypeFdo;
    init.Fdo.PhysicalDevice = NULL;

    //
    // Capture the input parameters
    //
    init.DevStack = DevStack;
    init.DriverID = DriverID;

    lRetVal = RegOpenKeyEx(
        PdoKey,
        NULL,
        0,
        KEY_READ,
        &init.PdoKey
        );

    if (ERROR_SUCCESS != lRetVal) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                            "Registry key open failed for the PDO key, "
                            "winerror %!WINERROR!", lRetVal);

        hr = HRESULT_FROM_WIN32(lRetVal);
        return FxDevice::NtStatusFromHr(DevStack, hr);
    }

    size_t len = 0;
    hr = StringCchLengthW(ServiceName, STRSAFE_MAX_CCH, &len);
    if (FAILED(hr)) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                            "Registry path string too long or badly formed "
                            "path. Invalid configuration HRESULT %!hresult!",
                            hr);
        return FxDevice::NtStatusFromHr(DevStack, hr);
    }

    len += 1;    // Add one for the string termination character
    init.ConfigRegistryPath = new WCHAR[len];
    if (NULL == init.ConfigRegistryPath) {
        hr = E_OUTOFMEMORY;
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                            "Failed to allocate memory for Config path"
                            " HRESULT %!hresult!", hr);

        return FxDevice::NtStatusFromHr(DevStack, hr);
    }

    hr = StringCchCopyW(init.ConfigRegistryPath, len, ServiceName);
    if (FAILED(hr)) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                            "Failed to copy the configuration path status "
                            "%!hresult!", hr);
        return FxDevice::NtStatusFromHr(DevStack, hr);
    }

    //
    // Capture the PDO device instance ID.
    //
    len = 0;
    hr = StringCchLengthW(DevInstanceID, STRSAFE_MAX_CCH, &len);
    if (FAILED(hr)) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                            "Device Instance ID string too long or badly formed"
                            " path. Invalid configuration %!hresult!", hr);
        return FxDevice::NtStatusFromHr(DevStack, hr);
    }

    len += 1; // Add one for the string termination character
    init.DevInstanceID = new WCHAR[len];
    if (NULL == init.DevInstanceID) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                            "Failed to allocate memory for DevInstanceID "
                            "%!hresult!", hr);
        hr = E_OUTOFMEMORY;
        return FxDevice::NtStatusFromHr(DevStack, hr);
    }

    hr = StringCchCopyW(init.DevInstanceID, len, DevInstanceID);
    if (FAILED(hr)) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                            "Unable to copy DevInstanceID %!hresult!", hr);
        return FxDevice::NtStatusFromHr(DevStack, hr);
    }

    //
    // Capture Kernel device name.
    //
    len = 0;
    hr = StringCchLengthW(KernelDeviceName, STRSAFE_MAX_CCH, &len);
    if (FAILED(hr)) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                            "Unable to determine KernelDeviceName length"
                            "%!hresult!", hr);
        return FxDevice::NtStatusFromHr(DevStack, hr);
    }

    len += 1; // Add one for string termination character.
    init.KernelDeviceName = new WCHAR[len];
    if (init.KernelDeviceName == NULL) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                            "Failed to allocate memory for KernelDeviceName "
                            "%!hresult!", hr);
        hr = E_OUTOFMEMORY;
        return FxDevice::NtStatusFromHr(DevStack, hr);
    }

    hr = StringCchCopyW(init.KernelDeviceName, len, KernelDeviceName);
    if (FAILED(hr)) {
        DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                            "Unable to copy kernel device name KernelDeviceName"
                            " %!hresult!", hr);
        return FxDevice::NtStatusFromHr(DevStack, hr);
    }

    //
    // Invoke driver's AddDevice callback
    //
    status = InvokeDeviceAdd(&init);

    //
    // Retrieve the created FxDevice by driver calling
    // WdfDeviceCreate in DriverDeviceAdd callback
    //
    pDevice = (FxDevice*) init.CreatedDevice;

    //
    // This can happen if the driver did not create
    // WDFDEVICE or if this is a filter driver
    //
    if (pDevice == NULL) {
        ASSERT(status != STATUS_SUCCESS);
        return status;
    }

    if (NT_SUCCESS(status)) {
        //
        // Make sure that DO_DEVICE_INITIALIZING is cleared.
        // FxDevice::FdoInitialize does not do this b/c the driver writer may
        // want the bit set until sometime after WdfDeviceCreate returns
        //
        pDevice->FinishInitializing();
    }
    else {
        //
        // Created a device, but returned error.
        //
        ASSERT(pDevice->IsPnp());
        ASSERT(pDevice->m_CurrentPnpState == WdfDevStatePnpInit);

        status = pDevice->DeleteDeviceFromFailedCreate(status, TRUE);
        pDevice = NULL;
    }

    DoTraceLevelMessage(GetDriverGlobals(), TRACE_LEVEL_VERBOSE, TRACINGPNP,
                        "Exit, status %!STATUS!", status);

    return status;
}

_Must_inspect_result_
NTSTATUS
FxDriver::AllocateDriverObjectExtensionAndStoreFxDriver(
    VOID
    )
{
    //
    // No allocation needed for user-mode, just store FxDriver in driver object.
    //
    m_DriverObject.GetObject()->FxDriver = this;

    return STATUS_SUCCESS;
}

FxDriver*
FxDriver::GetFxDriver(
    __in MdDriverObject DriverObject
    )
{
    return DriverObject->FxDriver;
}

VOID
FxDriver::ClearDriverObjectFxDriver(
    VOID
    )
{
    PDRIVER_OBJECT_UM pDriverObj = m_DriverObject.GetObject();

    if (pDriverObj != NULL) {
        pDriverObj->FxDriver = NULL;
    }
}

NTSTATUS
FxDriver::OpenDriverKey(
    _In_ UMINT::WDF_PROPERTY_STORE_ROOT_CLASS ServiceKeyType
    )
/*++
Routine Description:
    This routine will call into the WudfHost to make an elevation call and
    obtain a handle to the driver's key. This can be the parameters key or the
    persistent state key.

Arguments:
    ServiceKeyType : WdfPropertyStoreRootDriverParametersKey or
                     WdfPropertyStoreRootDriverPersistentStateKey

Returns:
    NTSTATUS

--*/
{
    HRESULT hr;
    NTSTATUS status;

    PFX_DRIVER_GLOBALS FxDriverGlobals = GetDriverGlobals();
    PDRIVER_OBJECT_UM pDrvObj = GetDriverObject();
    IWudfDeviceStack* pDevStack = (IWudfDeviceStack*)pDrvObj->DriverLoadContext->DeviceStack;

    UMINT::WDF_PROPERTY_STORE_ROOT rootSpecifier;
    UMINT::WDF_PROPERTY_STORE_RETRIEVE_FLAGS flags;
    CANSI_STRING serviceNameA;
    DECLARE_UNICODE_STRING_SIZE(serviceNameW, WDF_DRIVER_GLOBALS_NAME_LEN);
    HKEY hKey;

    if (ServiceKeyType != UMINT::WdfPropertyStoreRootDriverParametersKey &&
        ServiceKeyType != UMINT::WdfPropertyStoreRootDriverPersistentStateKey) {
        return STATUS_INVALID_PARAMETER;
    }

    RtlInitAnsiString(&serviceNameA, FxDriverGlobals->Public.DriverName);
    status = RtlAnsiStringToUnicodeString(&serviceNameW,
                                          &serviceNameA,
                                          FALSE);
    if (NT_SUCCESS(status)) {
        rootSpecifier.LengthCb = sizeof(UMINT::WDF_PROPERTY_STORE_ROOT);
        rootSpecifier.RootClass = ServiceKeyType;
        rootSpecifier.Qualifier.ParametersKey.ServiceName = serviceNameW.Buffer;

        flags = UMINT::WdfPropertyStoreCreateIfMissing;

        hr = pDevStack->CreateRegistryEntry(&rootSpecifier,
                                            flags,
                                            GENERIC_ALL & ~(GENERIC_WRITE | KEY_CREATE_SUB_KEY | WRITE_DAC),
                                            NULL,
                                            &hKey,
                                            NULL);
        status = FxDevice::NtStatusFromHr(pDevStack, hr);
        if (NT_SUCCESS(status)) {
            if (ServiceKeyType == UMINT::WdfPropertyStoreRootDriverParametersKey) {
                m_DriverParametersKey = hKey;
            }
            else {
                m_DriverPersistentStateKey = hKey;
            }
        }
    }

    return status;
}

NTSTATUS
FxDriver::InitFxRegKey(
    _In_  ACCESS_MASK                          DesiredAccess,
    _In_  UMINT::WDF_PROPERTY_STORE_ROOT_CLASS ServiceKeyType,
    _In_  FxRegKey*                            FrameworkRegKey
    )
/*++
Routine Description:
    This a helper routine to support WdfDriverOpenParametersRegKey and
    WdfDriverOpenPersistentKey APIs. It will set the framework reg key object's
    underlying reg key handle based on the desired access requested and the
    service key being opened.

Arguments:
    DesiredAccess -  Requested access to key
    ServiceKeyType - WdfPropertyStoreRootDriverParametersKey or
                     WdfPropertyStoreRootDriverPersistentStateKey
    FrameworkRegKey - Fx object corresponding to the reg key. Upon success
                      this object's reg key handle is set to the requested
                      driver key

Returns:
    NTSTATUS

--*/
{
    PFX_DRIVER_GLOBALS pFxDriverGlobals;
    HKEY existingKeyHandle;
    NTSTATUS status;
    HKEY newKeyHandle = NULL;
    LONG result;

    if (ServiceKeyType != UMINT::WdfPropertyStoreRootDriverParametersKey &&
        ServiceKeyType != UMINT::WdfPropertyStoreRootDriverPersistentStateKey) {
        ASSERT(FALSE);
        return STATUS_INVALID_PARAMETER;
    }

    //
    // First obtain the existing key handle. Depending on the DesiredAccess,
    // we will later open another handle or set this handle.
    //
    if (ServiceKeyType == UMINT::WdfPropertyStoreRootDriverParametersKey) {
        existingKeyHandle = GetDriverParametersKey();
    }
    else {
        existingKeyHandle = GetDriverPersistentStateKey();
    }

    pFxDriverGlobals = GetDriverGlobals();
    if (DesiredAccess & (GENERIC_WRITE | KEY_CREATE_SUB_KEY | WRITE_DAC)) {
        //
        // These access rights are not allowed. This restriction is
        // imposed by the host process and the reflector driver.
        //
        // Even though the maximum-permissions handle is already opened,
        // we fail so that the caller knows not to assume it has the
        // GENERIC_WRITE, KEY_CREATE_SUB_KEY, or WRITE_DAC permissions.
        //
        status = STATUS_ACCESS_DENIED;
        DoTraceLevelMessage(
                pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDRIVER,
                "Could not open '%s' driver service key %x with access "
                "rights 0x%x, %!STATUS!",
                pFxDriverGlobals->Public.DriverName,
                ServiceKeyType,
                DesiredAccess, status);
    } else if ((DesiredAccess & ~(KEY_READ | GENERIC_READ)) == 0) {
        //
        // If caller requested read-only access, open a new handle
        // to the parameters key, no reason to give more privileges
        // than needed.
        //
        result = RegOpenKeyEx(existingKeyHandle,
                              L"",
                              0,
                              DesiredAccess,
                              &newKeyHandle);
        status = WinErrorToNtStatus(result);
        if (!NT_SUCCESS(status)) {
            DoTraceLevelMessage(
                pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDRIVER,
                "Could not open driver service key type %x for %s Error %!STATUS!",
                ServiceKeyType, pFxDriverGlobals->Public.DriverName, status);
        }
    } else {
        //
        // If caller requested write access, give it the pre-opened handle,
        // since we do not have permission to open this key with write access
        // rights from user mode.
        //
        newKeyHandle = existingKeyHandle;

        //
        // Mark the registry key handle such that it won't be closed
        // when this FxRegKey is deleted. We might need the handle again
        // for future calls to WdfDriverOpenParametersRegistryKey.
        //
        FrameworkRegKey->SetCanCloseHandle(FALSE);
        status = STATUS_SUCCESS;
    }

    if (NT_SUCCESS(status)) {
        FrameworkRegKey->SetHandle((HANDLE)newKeyHandle);
    }

    return status;
}

NTSTATUS
FxDriver::GetDriverDataDirectory(
    _In_ FxString *String
    )
/*++
Routine Description:

    Retrieve the full path name to the persistent driver data directory.
    The directory was pre-created during driver initialization.

Arguments:
    FxString - A framework string object that receives the directory name

Returns:
    NTSTATUS

--*/
{
    NTSTATUS            status;
    PFX_DRIVER_GLOBALS  pFxDriverGlobals;

    pFxDriverGlobals = GetDriverGlobals();

    if (m_DriverDataDirectory == NULL) {
        status = STATUS_UNSUCCESSFUL;
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDRIVER,
            "DriverDataDirectory was not properly initialized for WDFDRIVER %p",
            GetHandle());
        return status;
    }

    status = String->Assign(m_DriverDataDirectory->GetUnicodeString());
    return status;
}

NTSTATUS
FxDriver::InitDriverDataDirectory(
    VOID
    )
/*++
Routine Description:
    Initialize the persistent driver data directory.

Arguments:

Returns:
    NTSTATUS

--*/
{
    NTSTATUS            status;
    HRESULT             hr;
    PCWSTR              dirPath = NULL;
    IWudfDeviceStack2*  pDevStack;
    PFX_DRIVER_GLOBALS  pFxDriverGlobals;
    UNICODE_STRING      serviceName;
    PWUDF_DRIVER_LOAD_CONTEXT pLoadContext;

    pFxDriverGlobals = GetDriverGlobals();

    //
    // We need a IWudfDeviceStack2 to talk to redirector. The pointer is only
    // valid during driver initialization.
    //
    pLoadContext = GetDriverObject()->DriverLoadContext;
    if (pLoadContext == NULL) {
        status = STATUS_UNSUCCESSFUL;
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDRIVER,
            "Unexpected NULL DriverLoadContext: '%s'",
            pFxDriverGlobals->Public.DriverName
            );
        goto exit;
    }

    if (pFxDriverGlobals->IsCompanion()) {
        //
        // Driver companion has no access to Win32 file API, thus there is no
        // use to driver data directory. Return success to let loading continue.
        //
        status = STATUS_SUCCESS;
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_VERBOSE, TRACINGDRIVER,
            "Driver Companion has no access to file system anyway: '%s'",
            pFxDriverGlobals->Public.DriverName
            );
        goto exit;
    }

    pDevStack = (IWudfDeviceStack2*)pLoadContext->DeviceStack;

    status = GetDriverServiceName(&serviceName);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Talk to redirector to get the directory path
    //
    hr = pDevStack->GetDriverDataDirectory(serviceName.Buffer,
                                           &dirPath);
    if (FAILED(hr)) {
        status = FxDevice::NtStatusFromHr(pDevStack, hr);
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDRIVER,
            "InitDriverDataDirectory for WDFDRIVER %p failed %!STATUS!",
            GetHandle(), status);
        goto exit;
    }

    m_DriverDataDirectory = new(pFxDriverGlobals, WDF_NO_OBJECT_ATTRIBUTES)
                            FxString(pFxDriverGlobals);
    if (m_DriverDataDirectory == NULL) {
        status = STATUS_NO_MEMORY;
        DoTraceLevelMessage(
            pFxDriverGlobals, TRACE_LEVEL_ERROR, TRACINGDRIVER,
            "Out of memory");
        goto exit;
    }

    status = m_DriverDataDirectory->Assign(dirPath);

exit:
    if (dirPath != NULL) {
        HeapFree(GetProcessHeap(), 0, (PVOID)dirPath);
    }

    return status;
}
