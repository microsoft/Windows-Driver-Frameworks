/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

_WdfVersionBuild_

Module Name: WdfFuncEnum.h

Abstract:
    Generated an enum of all WDF API functions

Environment:
    kernel mode only

    Warning: manual changes to this file will be lost.
--*/

#ifndef _WDFFUNCENUM_H_
#define _WDFFUNCENUM_H_

extern PWDF_DRIVER_GLOBALS WdfDriverGlobals;


//
//
// The first version of the framework that added the support
// for client version to be higher than the framework version
//
#define WDF_FIRST_VERSION_SUPPORTING_CLIENT_VERSION_HIGHER_THAN_FRAMEWORK 25

//
// The function count from the first framework which added the support.
// Any function with index less than the count is always available
//
#define WDF_ALWAYS_AVAILABLE_FUNCTION_COUNT  268

//
// Validate UMDF_MINIMUM_VERSION_REQUIRED falls into the correct range
//
#if defined(UMDF_MINIMUM_VERSION_REQUIRED)

    #if UMDF_MINIMUM_VERSION_REQUIRED < WDF_FIRST_VERSION_SUPPORTING_CLIENT_VERSION_HIGHER_THAN_FRAMEWORK
    #error UMDF_MINIMUM_VERSION_REQUIRED < 25
    #endif

    #if UMDF_MINIMUM_VERSION_REQUIRED > UMDF_VERSION_MINOR
    #error UMDF_MINIMUM_VERSION_REQUIRED > UMDF_VERSION_MINOR
    #endif

#endif


//
// All functions/structures are always available in following two cases:
//
//  1) Building framework itself (which defines WDF_EVERYTHING_ALWAYS_AVAILABLE)
//
//  2) Traditional client drivers who are not aware of the new feature
//     "client version can be higher than framework's" and thus implies
//     UMDF_VERSION_MINOR == UMDF_MINIMUM_VERSION_REQUIRED
//
#if defined(UMDF_MINIMUM_VERSION_REQUIRED) && (UMDF_VERSION_MINOR == UMDF_MINIMUM_VERSION_REQUIRED)

    #if !defined(WDF_EVERYTHING_ALWAYS_AVAILABLE)
    #define WDF_EVERYTHING_ALWAYS_AVAILABLE
    #endif

#endif


//
// private:
//
// TRUE - the client driver's target version is higher than the current framework version.
//     IS_FUNCTION_AVAILABLE() will compare function index agains function count to determine
//     whether the function is usable on the platform. The same for IS_FIELD_AVAILABLE().
//
// FALSE - the client driver is running on a new framework. All functions/Structures
//     from the target framework are always available
//
extern BOOLEAN WdfClientVersionHigherThanFramework;

//
// private:
//
// Only valid when WdfClientVersionHigherThanFramework is TRUE.
//
// Hold the count of usable functions from the framework. Used by IS_FUNCTION_AVAILABLE
// to determine whether a function is available on the platform.
//
extern ULONG   WdfFunctionCount;

//
// private:
//
// Only valid when WdfClientVersionHigherThanFramework is TRUE.
//
// Hold the count of usable structures from the framework. Used by IS_FIELD_AVAILABLE
// to determine whether a field in a structure is available on the platform.
//
extern ULONG   WdfStructureCount;
extern WDF_STRUCT_INFO WdfStructures;


#ifndef WDF_STUB

__declspec(selectany)
ULONG WdfMinimumVersionRequired =
    #if defined(UMDF_MINIMUM_VERSION_REQUIRED)
        UMDF_MINIMUM_VERSION_REQUIRED
    #elif defined(UMDF_VERSION_MINOR)
        UMDF_VERSION_MINOR
    #else
        (ULONG)(-1)
    #endif
    ;

#endif

//
// BOOLEAN
// WDF_IS_FUNCTION_AVAILABLE(
//     FunctionName
//     );
//
#define WDF_IS_FUNCTION_AVAILABLE(FunctionName)                                \
(                                                                              \
    (FunctionName ## TableIndex < WDF_ALWAYS_AVAILABLE_FUNCTION_COUNT)         \
    ||                                                                         \
    (!WdfClientVersionHigherThanFramework)                                     \
    ||                                                                         \
    (FunctionName ## TableIndex < WdfFunctionCount)                            \
)

//
// BOOLEAN
// WDF_IS_STRUCTURE_AVAILABLE(
//     StructName
//     );
//
#define WDF_IS_STRUCTURE_AVAILABLE(StructName)                                 \
(                                                                              \
    (!WdfClientVersionHigherThanFramework)                                     \
    ||                                                                         \
    (INDEX_ ## StructName < WdfStructureCount)                                 \
)

//
// BOOLEAN
// WDF_IS_FIELD_AVAILABLE(
//     StructName,
//     FieldName
//     );
//
#define WDF_IS_FIELD_AVAILABLE(StructName, FieldName)                          \
(                                                                              \
    (!WdfClientVersionHigherThanFramework)                                     \
    ||                                                                         \
    (                                                                          \
        (INDEX_ ## StructName < WdfStructureCount)                             \
        &&                                                                     \
        (FIELD_OFFSET(StructName, FieldName) < WdfStructures[INDEX_ ## StructName])\
    )                                                                          \
)

//
// ULONG
// WDF_STRUCTURE_SIZE(
//     StructName
//     );
//
// NOTE: if it returns (-1), the structure is not available on the
//       current framework (and should not be used).
//
#if defined(WDF_EVERYTHING_ALWAYS_AVAILABLE)

#define WDF_STRUCTURE_SIZE(StructName)  (ULONG)sizeof(StructName)

#else

#define WDF_STRUCTURE_SIZE(StructName)                                         \
(ULONG)                                                                        \
(                                                                              \
    WdfClientVersionHigherThanFramework                                        \
        ? (                                                                    \
            (INDEX_ ## StructName < WdfStructureCount)                         \
            ? WdfStructures[INDEX_ ## StructName]                              \
            : (size_t)(-1)                                                     \
          )                                                                    \
        : sizeof(StructName)                                                   \
)

#endif // WDF_STRUCTURE_SIZE

typedef enum _WDFFUNCENUM {

    WdfCollectionCreateTableIndex = 0,
    WdfCollectionGetCountTableIndex = 1,
    WdfCollectionAddTableIndex = 2,
    WdfCollectionRemoveTableIndex = 3,
    WdfCollectionRemoveItemTableIndex = 4,
    WdfCollectionGetItemTableIndex = 5,
    WdfCollectionGetFirstItemTableIndex = 6,
    WdfCollectionGetLastItemTableIndex = 7,
    WdfCxDeviceInitAllocateTableIndex = 8,
    WdfCxDeviceInitSetRequestAttributesTableIndex = 9,
    WdfCxDeviceInitSetFileObjectConfigTableIndex = 10,
    WdfCxVerifierKeBugCheckTableIndex = 11,
    WdfDeviceGetDeviceStateTableIndex = 12,
    WdfDeviceSetDeviceStateTableIndex = 13,
    WdfDeviceGetDriverTableIndex = 14,
    WdfDeviceGetIoTargetTableIndex = 15,
    WdfDeviceAssignS0IdleSettingsTableIndex = 16,
    WdfDeviceAssignSxWakeSettingsTableIndex = 17,
    WdfDeviceOpenRegistryKeyTableIndex = 18,
    WdfDeviceInitSetPnpPowerEventCallbacksTableIndex = 19,
    WdfDeviceInitSetPowerPolicyEventCallbacksTableIndex = 20,
    WdfDeviceInitSetPowerPolicyOwnershipTableIndex = 21,
    WdfDeviceInitSetIoTypeTableIndex = 22,
    WdfDeviceInitSetFileObjectConfigTableIndex = 23,
    WdfDeviceInitSetRequestAttributesTableIndex = 24,
    WdfDeviceCreateTableIndex = 25,
    WdfDeviceSetStaticStopRemoveTableIndex = 26,
    WdfDeviceCreateDeviceInterfaceTableIndex = 27,
    WdfDeviceSetDeviceInterfaceStateTableIndex = 28,
    WdfDeviceRetrieveDeviceInterfaceStringTableIndex = 29,
    WdfDeviceCreateSymbolicLinkTableIndex = 30,
    WdfDeviceQueryPropertyTableIndex = 31,
    WdfDeviceAllocAndQueryPropertyTableIndex = 32,
    WdfDeviceSetPnpCapabilitiesTableIndex = 33,
    WdfDeviceSetPowerCapabilitiesTableIndex = 34,
    WdfDeviceSetFailedTableIndex = 35,
    WdfDeviceStopIdleNoTrackTableIndex = 36,
    WdfDeviceResumeIdleNoTrackTableIndex = 37,
    WdfDeviceGetFileObjectTableIndex = 38,
    WdfDeviceGetDefaultQueueTableIndex = 39,
    WdfDeviceConfigureRequestDispatchingTableIndex = 40,
    WdfDeviceGetSystemPowerActionTableIndex = 41,
    WdfDeviceInitSetReleaseHardwareOrderOnFailureTableIndex = 42,
    WdfDeviceInitSetIoTypeExTableIndex = 43,
    WdfDevicePostEventTableIndex = 44,
    WdfDeviceMapIoSpaceTableIndex = 45,
    WdfDeviceUnmapIoSpaceTableIndex = 46,
    WdfDeviceGetHardwareRegisterMappedAddressTableIndex = 47,
    WdfDeviceReadFromHardwareTableIndex = 48,
    WdfDeviceWriteToHardwareTableIndex = 49,
    WdfDeviceAssignInterfacePropertyTableIndex = 50,
    WdfDeviceAllocAndQueryInterfacePropertyTableIndex = 51,
    WdfDeviceQueryInterfacePropertyTableIndex = 52,
    WdfDeviceGetDeviceStackIoTypeTableIndex = 53,
    WdfDeviceQueryPropertyExTableIndex = 54,
    WdfDeviceAllocAndQueryPropertyExTableIndex = 55,
    WdfDeviceAssignPropertyTableIndex = 56,
    WdfDriverCreateTableIndex = 57,
    WdfDriverGetRegistryPathTableIndex = 58,
    WdfDriverOpenParametersRegistryKeyTableIndex = 59,
    WdfDriverRetrieveVersionStringTableIndex = 60,
    WdfDriverIsVersionAvailableTableIndex = 61,
    WdfFdoInitOpenRegistryKeyTableIndex = 62,
    WdfFdoInitQueryPropertyTableIndex = 63,
    WdfFdoInitAllocAndQueryPropertyTableIndex = 64,
    WdfFdoInitQueryPropertyExTableIndex = 65,
    WdfFdoInitAllocAndQueryPropertyExTableIndex = 66,
    WdfFdoInitSetFilterTableIndex = 67,
    WdfFileObjectGetFileNameTableIndex = 68,
    WdfFileObjectGetDeviceTableIndex = 69,
    WdfFileObjectGetInitiatorProcessIdTableIndex = 70,
    WdfFileObjectGetRelatedFileObjectTableIndex = 71,
    WdfInterruptCreateTableIndex = 72,
    WdfInterruptQueueDpcForIsrTableIndex = 73,
    WdfInterruptQueueWorkItemForIsrTableIndex = 74,
    WdfInterruptSynchronizeTableIndex = 75,
    WdfInterruptAcquireLockTableIndex = 76,
    WdfInterruptReleaseLockTableIndex = 77,
    WdfInterruptEnableTableIndex = 78,
    WdfInterruptDisableTableIndex = 79,
    WdfInterruptGetInfoTableIndex = 80,
    WdfInterruptSetPolicyTableIndex = 81,
    WdfInterruptSetExtendedPolicyTableIndex = 82,
    WdfInterruptGetDeviceTableIndex = 83,
    WdfInterruptTryToAcquireLockTableIndex = 84,
    WdfIoQueueCreateTableIndex = 85,
    WdfIoQueueGetStateTableIndex = 86,
    WdfIoQueueStartTableIndex = 87,
    WdfIoQueueStopTableIndex = 88,
    WdfIoQueueStopSynchronouslyTableIndex = 89,
    WdfIoQueueGetDeviceTableIndex = 90,
    WdfIoQueueRetrieveNextRequestTableIndex = 91,
    WdfIoQueueRetrieveRequestByFileObjectTableIndex = 92,
    WdfIoQueueFindRequestTableIndex = 93,
    WdfIoQueueRetrieveFoundRequestTableIndex = 94,
    WdfIoQueueDrainSynchronouslyTableIndex = 95,
    WdfIoQueueDrainTableIndex = 96,
    WdfIoQueuePurgeSynchronouslyTableIndex = 97,
    WdfIoQueuePurgeTableIndex = 98,
    WdfIoQueueReadyNotifyTableIndex = 99,
    WdfIoQueueStopAndPurgeTableIndex = 100,
    WdfIoQueueStopAndPurgeSynchronouslyTableIndex = 101,
    WdfIoTargetCreateTableIndex = 102,
    WdfIoTargetOpenTableIndex = 103,
    WdfIoTargetCloseForQueryRemoveTableIndex = 104,
    WdfIoTargetCloseTableIndex = 105,
    WdfIoTargetStartTableIndex = 106,
    WdfIoTargetStopTableIndex = 107,
    WdfIoTargetPurgeTableIndex = 108,
    WdfIoTargetGetStateTableIndex = 109,
    WdfIoTargetGetDeviceTableIndex = 110,
    WdfIoTargetSendReadSynchronouslyTableIndex = 111,
    WdfIoTargetFormatRequestForReadTableIndex = 112,
    WdfIoTargetSendWriteSynchronouslyTableIndex = 113,
    WdfIoTargetFormatRequestForWriteTableIndex = 114,
    WdfIoTargetSendIoctlSynchronouslyTableIndex = 115,
    WdfIoTargetFormatRequestForIoctlTableIndex = 116,
    WdfMemoryCreateTableIndex = 117,
    WdfMemoryCreatePreallocatedTableIndex = 118,
    WdfMemoryGetBufferTableIndex = 119,
    WdfMemoryAssignBufferTableIndex = 120,
    WdfMemoryCopyToBufferTableIndex = 121,
    WdfMemoryCopyFromBufferTableIndex = 122,
    WdfObjectGetTypedContextWorkerTableIndex = 123,
    WdfObjectAllocateContextTableIndex = 124,
    WdfObjectContextGetObjectTableIndex = 125,
    WdfObjectReferenceActualTableIndex = 126,
    WdfObjectDereferenceActualTableIndex = 127,
    WdfObjectCreateTableIndex = 128,
    WdfObjectDeleteTableIndex = 129,
    WdfObjectQueryTableIndex = 130,
    WdfRegistryOpenKeyTableIndex = 131,
    WdfRegistryCreateKeyTableIndex = 132,
    WdfRegistryCloseTableIndex = 133,
    WdfRegistryRemoveKeyTableIndex = 134,
    WdfRegistryRemoveValueTableIndex = 135,
    WdfRegistryQueryValueTableIndex = 136,
    WdfRegistryQueryMemoryTableIndex = 137,
    WdfRegistryQueryMultiStringTableIndex = 138,
    WdfRegistryQueryUnicodeStringTableIndex = 139,
    WdfRegistryQueryStringTableIndex = 140,
    WdfRegistryQueryULongTableIndex = 141,
    WdfRegistryAssignValueTableIndex = 142,
    WdfRegistryAssignMemoryTableIndex = 143,
    WdfRegistryAssignMultiStringTableIndex = 144,
    WdfRegistryAssignUnicodeStringTableIndex = 145,
    WdfRegistryAssignStringTableIndex = 146,
    WdfRegistryAssignULongTableIndex = 147,
    WdfRequestCreateTableIndex = 148,
    WdfRequestReuseTableIndex = 149,
    WdfRequestChangeTargetTableIndex = 150,
    WdfRequestFormatRequestUsingCurrentTypeTableIndex = 151,
    WdfRequestSendTableIndex = 152,
    WdfRequestGetStatusTableIndex = 153,
    WdfRequestMarkCancelableTableIndex = 154,
    WdfRequestMarkCancelableExTableIndex = 155,
    WdfRequestUnmarkCancelableTableIndex = 156,
    WdfRequestIsCanceledTableIndex = 157,
    WdfRequestCancelSentRequestTableIndex = 158,
    WdfRequestIsFrom32BitProcessTableIndex = 159,
    WdfRequestSetCompletionRoutineTableIndex = 160,
    WdfRequestGetCompletionParamsTableIndex = 161,
    WdfRequestAllocateTimerTableIndex = 162,
    WdfRequestCompleteTableIndex = 163,
    WdfRequestCompleteWithInformationTableIndex = 164,
    WdfRequestGetParametersTableIndex = 165,
    WdfRequestRetrieveInputMemoryTableIndex = 166,
    WdfRequestRetrieveOutputMemoryTableIndex = 167,
    WdfRequestRetrieveInputBufferTableIndex = 168,
    WdfRequestRetrieveOutputBufferTableIndex = 169,
    WdfRequestSetInformationTableIndex = 170,
    WdfRequestGetInformationTableIndex = 171,
    WdfRequestGetFileObjectTableIndex = 172,
    WdfRequestGetRequestorModeTableIndex = 173,
    WdfRequestForwardToIoQueueTableIndex = 174,
    WdfRequestGetIoQueueTableIndex = 175,
    WdfRequestRequeueTableIndex = 176,
    WdfRequestStopAcknowledgeTableIndex = 177,
    WdfRequestImpersonateTableIndex = 178,
    WdfRequestGetRequestorProcessIdTableIndex = 179,
    WdfRequestIsFromUserModeDriverTableIndex = 180,
    WdfRequestSetUserModeDriverInitiatedIoTableIndex = 181,
    WdfRequestGetUserModeDriverInitiatedIoTableIndex = 182,
    WdfRequestSetActivityIdTableIndex = 183,
    WdfRequestRetrieveActivityIdTableIndex = 184,
    WdfRequestGetEffectiveIoTypeTableIndex = 185,
    WdfCmResourceListGetCountTableIndex = 186,
    WdfCmResourceListGetDescriptorTableIndex = 187,
    WdfStringCreateTableIndex = 188,
    WdfStringGetUnicodeStringTableIndex = 189,
    WdfObjectAcquireLockTableIndex = 190,
    WdfObjectReleaseLockTableIndex = 191,
    WdfWaitLockCreateTableIndex = 192,
    WdfWaitLockAcquireTableIndex = 193,
    WdfWaitLockReleaseTableIndex = 194,
    WdfSpinLockCreateTableIndex = 195,
    WdfSpinLockAcquireTableIndex = 196,
    WdfSpinLockReleaseTableIndex = 197,
    WdfTimerCreateTableIndex = 198,
    WdfTimerStartTableIndex = 199,
    WdfTimerStopTableIndex = 200,
    WdfTimerGetParentObjectTableIndex = 201,
    WdfUsbTargetDeviceCreateTableIndex = 202,
    WdfUsbTargetDeviceCreateWithParametersTableIndex = 203,
    WdfUsbTargetDeviceRetrieveInformationTableIndex = 204,
    WdfUsbTargetDeviceGetDeviceDescriptorTableIndex = 205,
    WdfUsbTargetDeviceRetrieveConfigDescriptorTableIndex = 206,
    WdfUsbTargetDeviceQueryStringTableIndex = 207,
    WdfUsbTargetDeviceAllocAndQueryStringTableIndex = 208,
    WdfUsbTargetDeviceFormatRequestForStringTableIndex = 209,
    WdfUsbTargetDeviceGetNumInterfacesTableIndex = 210,
    WdfUsbTargetDeviceSelectConfigTableIndex = 211,
    WdfUsbTargetDeviceSendControlTransferSynchronouslyTableIndex = 212,
    WdfUsbTargetDeviceFormatRequestForControlTransferTableIndex = 213,
    WdfUsbTargetDeviceResetPortSynchronouslyTableIndex = 214,
    WdfUsbTargetDeviceQueryUsbCapabilityTableIndex = 215,
    WdfUsbTargetPipeGetInformationTableIndex = 216,
    WdfUsbTargetPipeIsInEndpointTableIndex = 217,
    WdfUsbTargetPipeIsOutEndpointTableIndex = 218,
    WdfUsbTargetPipeGetTypeTableIndex = 219,
    WdfUsbTargetPipeSetNoMaximumPacketSizeCheckTableIndex = 220,
    WdfUsbTargetPipeWriteSynchronouslyTableIndex = 221,
    WdfUsbTargetPipeFormatRequestForWriteTableIndex = 222,
    WdfUsbTargetPipeReadSynchronouslyTableIndex = 223,
    WdfUsbTargetPipeFormatRequestForReadTableIndex = 224,
    WdfUsbTargetPipeConfigContinuousReaderTableIndex = 225,
    WdfUsbTargetPipeAbortSynchronouslyTableIndex = 226,
    WdfUsbTargetPipeFormatRequestForAbortTableIndex = 227,
    WdfUsbTargetPipeResetSynchronouslyTableIndex = 228,
    WdfUsbTargetPipeFormatRequestForResetTableIndex = 229,
    WdfUsbInterfaceGetInterfaceNumberTableIndex = 230,
    WdfUsbInterfaceGetNumEndpointsTableIndex = 231,
    WdfUsbInterfaceGetDescriptorTableIndex = 232,
    WdfUsbInterfaceGetNumSettingsTableIndex = 233,
    WdfUsbInterfaceSelectSettingTableIndex = 234,
    WdfUsbInterfaceGetEndpointInformationTableIndex = 235,
    WdfUsbTargetDeviceGetInterfaceTableIndex = 236,
    WdfUsbInterfaceGetConfiguredSettingIndexTableIndex = 237,
    WdfUsbInterfaceGetNumConfiguredPipesTableIndex = 238,
    WdfUsbInterfaceGetConfiguredPipeTableIndex = 239,
    WdfVerifierDbgBreakPointTableIndex = 240,
    WdfVerifierKeBugCheckTableIndex = 241,
    WdfGetTriageInfoTableIndex = 242,
    WdfWorkItemCreateTableIndex = 243,
    WdfWorkItemEnqueueTableIndex = 244,
    WdfWorkItemGetParentObjectTableIndex = 245,
    WdfWorkItemFlushTableIndex = 246,
    WdfRegistryWdmGetHandleTableIndex = 247,
    WdfDeviceStopIdleActualTableIndex = 248,
    WdfDeviceResumeIdleActualTableIndex = 249,
    WdfDeviceInitEnableHidInterfaceTableIndex = 250,
    WdfDeviceHidNotifyPresenceTableIndex = 251,
    WdfDeviceGetSelfIoTargetTableIndex = 252,
    WdfDeviceInitAllowSelfIoTargetTableIndex = 253,
    WdfIoTargetSelfAssignDefaultIoQueueTableIndex = 254,
    WdfDeviceOpenDevicemapKeyTableIndex = 255,
    WdfIoTargetWdmGetTargetFileHandleTableIndex = 256,
    WdfDeviceWdmDispatchIrpTableIndex = 257,
    WdfDeviceWdmDispatchIrpToIoQueueTableIndex = 258,
    WdfDeviceConfigureWdmIrpDispatchCallbackTableIndex = 259,
    WdfCxDeviceInitSetPnpPowerEventCallbacksTableIndex = 260,
    WdfCompanionCreateTableIndex = 261,
    WdfCompanionWdmGetSecureDeviceHandleTableIndex = 262,
    WdfCompanionCreateTaskQueueTableIndex = 263,
    WdfDeviceInitSetCompanionEventCallbacksTableIndex = 264,
    WdfDriverOpenPersistentStateRegistryKeyTableIndex = 265,
    WdfDriverErrorReportApiMissingTableIndex = 266,
    WdfDeviceRetrieveDeviceDirectoryStringTableIndex = 267,
    WdfDriverRetrieveDriverDataDirectoryStringTableIndex = 268,
    WdfCxDeviceInitAllocateContextTableIndex = 269,
    WdfCxDeviceInitGetTypedContextWorkerTableIndex = 270,
    WdfCxDeviceInitSetPowerPolicyEventCallbacksTableIndex = 271,
    WdfDeviceSetDeviceInterfaceStateExTableIndex = 272,
    WdfFunctionTableNumEntries = 273,
} WDFFUNCENUM;

typedef enum _WDFSTRUCTENUM {

    INDEX_WDF_CHILD_ADDRESS_DESCRIPTION_HEADER         = 0,
    INDEX_WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER  = 1,
    INDEX_WDF_CHILD_LIST_CONFIG                        = 2,
    INDEX_WDF_CHILD_LIST_ITERATOR                      = 3,
    INDEX_WDF_CHILD_RETRIEVE_INFO                      = 4,
    INDEX_WDF_CLASS_BIND_INFO                          = 5,
    INDEX_WDF_CLASS_BIND_INFO2                         = 6,
    INDEX_WDF_CLASS_LIBRARY_INFO                       = 7,
    INDEX_WDF_CLASS_VERSION                            = 8,
    INDEX_WDF_COMPANION_EVENT_CALLBACKS                = 9,
    INDEX_WDF_CUSTOM_TYPE_CONTEXT                      = 10,
    INDEX_WDF_DEVICE_INTERFACE_PROPERTY_DATA           = 11,
    INDEX_WDF_DEVICE_PNP_CAPABILITIES                  = 12,
    INDEX_WDF_DEVICE_PNP_NOTIFICATION_DATA             = 13,
    INDEX_WDF_DEVICE_POWER_CAPABILITIES                = 14,
    INDEX_WDF_DEVICE_POWER_NOTIFICATION_DATA           = 15,
    INDEX_WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS        = 16,
    INDEX_WDF_DEVICE_POWER_POLICY_NOTIFICATION_DATA    = 17,
    INDEX_WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS        = 18,
    INDEX_WDF_DEVICE_PROPERTY_DATA                     = 19,
    INDEX_WDF_DEVICE_STATE                             = 20,
    INDEX_WDF_DRIVER_CONFIG                            = 21,
    INDEX_WDF_DRIVER_GLOBALS                           = 22,
    INDEX_WDF_DRIVER_VERSION_AVAILABLE_PARAMS          = 23,
    INDEX_WDF_FDO_EVENT_CALLBACKS                      = 24,
    INDEX_WDF_FILEOBJECT_CONFIG                        = 25,
    INDEX_WDF_INTERRUPT_CONFIG                         = 26,
    INDEX_WDF_INTERRUPT_EXTENDED_POLICY                = 27,
    INDEX_WDF_INTERRUPT_INFO                           = 28,
    INDEX_WDF_IO_FORWARD_PROGRESS_RESERVED_POLICY_SETTINGS = 29,
    INDEX_WDF_IO_QUEUE_CONFIG                          = 30,
    INDEX_WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY         = 31,
    INDEX_WDF_IO_TARGET_OPEN_PARAMS                    = 32,
    INDEX_WDF_IO_TYPE_CONFIG                           = 33,
    INDEX_WDF_MEMORY_DESCRIPTOR                        = 34,
    INDEX_WDF_OBJECT_ATTRIBUTES                        = 35,
    INDEX_WDF_OBJECT_CONTEXT_TYPE_INFO                 = 36,
    INDEX_WDF_PDO_EVENT_CALLBACKS                      = 37,
    INDEX_WDF_PNPPOWER_EVENT_CALLBACKS                 = 38,
    INDEX_WDF_POWER_FRAMEWORK_SETTINGS                 = 39,
    INDEX_WDF_POWER_POLICY_EVENT_CALLBACKS             = 40,
    INDEX_WDF_POWER_ROUTINE_TIMED_OUT_DATA             = 41,
    INDEX_WDF_QUERY_INTERFACE_CONFIG                   = 42,
    INDEX_WDF_QUEUE_FATAL_ERROR_DATA                   = 43,
    INDEX_WDF_REMOVE_LOCK_OPTIONS                      = 44,
    INDEX_WDF_REQUEST_COMPLETION_PARAMS                = 45,
    INDEX_WDF_REQUEST_FATAL_ERROR_INFORMATION_LENGTH_MISMATCH_DATA = 46,
    INDEX_WDF_REQUEST_FORWARD_OPTIONS                  = 47,
    INDEX_WDF_REQUEST_PARAMETERS                       = 48,
    INDEX_WDF_REQUEST_REUSE_PARAMS                     = 49,
    INDEX_WDF_REQUEST_SEND_OPTIONS                     = 50,
    INDEX_WDF_TASK_QUEUE_CONFIG                        = 51,
    INDEX_WDF_TIMER_CONFIG                             = 52,
    INDEX_WDF_TRIAGE_INFO                              = 53,
    INDEX_WDF_USB_CONTINUOUS_READER_CONFIG             = 54,
    INDEX_WDF_USB_DEVICE_CREATE_CONFIG                 = 55,
    INDEX_WDF_USB_DEVICE_INFORMATION                   = 56,
    INDEX_WDF_USB_DEVICE_SELECT_CONFIG_PARAMS          = 57,
    INDEX_WDF_USB_INTERFACE_SELECT_SETTING_PARAMS      = 58,
    INDEX_WDF_USB_INTERFACE_SETTING_PAIR               = 59,
    INDEX_WDF_USB_PIPE_INFORMATION                     = 60,
    INDEX_WDF_USB_REQUEST_COMPLETION_PARAMS            = 61,
    INDEX_WDF_WMI_INSTANCE_CONFIG                      = 62,
    INDEX_WDF_WMI_PROVIDER_CONFIG                      = 63,
    INDEX_WDF_WORKITEM_CONFIG                          = 64,
    INDEX_WDFCONTEXT_TRIAGE_INFO                       = 65,
    INDEX_WDFCONTEXTTYPE_TRIAGE_INFO                   = 66,
    INDEX_WDFCX_FILEOBJECT_CONFIG                      = 67,
    INDEX_WDFCX_PNPPOWER_EVENT_CALLBACKS               = 68,
    INDEX_WDFDEVICE_TRIAGE_INFO                        = 69,
    INDEX_WDFFWDPROGRESS_TRIAGE_INFO                   = 70,
    INDEX_WDFIRP_TRIAGE_INFO                           = 71,
    INDEX_WDFIRPQUEUE_TRIAGE_INFO                      = 72,
    INDEX_WDFMEMORY_OFFSET                             = 73,
    INDEX_WDFOBJECT_TRIAGE_INFO                        = 74,
    INDEX_WDFQUEUE_TRIAGE_INFO                         = 75,
    INDEX_WDFREQUEST_TRIAGE_INFO                       = 76,
    INDEX_WDFCX_POWER_POLICY_EVENT_CALLBACKS           = 77,
    WDF_STRUCTURE_TABLE_NUM_ENTRIES                    = 78,
} WDFSTRUCTENUM;

#define Wdf_STRUCTURE_TABLE_NUM_ENTRIES WDF_STRUCTURE_TABLE_NUM_ENTRIES

#endif // _WDFFUNCENUM_H_

