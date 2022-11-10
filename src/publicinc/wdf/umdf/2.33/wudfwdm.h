/*++ BUILD Version: 0010    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    wudfwdm.h

Abstract:

    This module defines the WDM types, constants, and functions available
    to User Mode Driver Framework (UMDF) device drivers.

Revision History:

--*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DEVICE_OBJECT           *PDEVICE_OBJECT;
typedef struct _DRIVER_OBJECT           *PDRIVER_OBJECT;
typedef struct _FILE_OBJECT             *PFILE_OBJECT;
typedef struct _INTERFACE               *PINTERFACE;
typedef struct _ACCESS_STATE            *PACCESS_STATE;
typedef struct _IO_STATUS_BLOCK         *PIO_STATUS_BLOCK;
typedef struct _KTHREAD                 *PKTHREAD, *PRKTHREAD;
#define PMDL                            PVOID
#define PIRP                            PVOID
typedef enum _DEVICE_RELATION_TYPE      DEVICE_RELATION_TYPE, *PDEVICE_RELATION_TYPE;

#define PAGED_CODE()
#define PAGED_CODE_LOCKED()


#ifndef _MANAGED
#if defined(_M_IX86)
#define FASTCALL __fastcall
#else
#define FASTCALL
#endif
#else
#define FASTCALL NTAPI
#endif


//
// NTSTATUS
//

typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
/*lint -save -e624 */  // Don't complain about different typedefs.
typedef NTSTATUS *PNTSTATUS;
/*lint -restore */  // Resume checking for different typedefs.

#if _WIN32_WINNT >= 0x0600
typedef CONST NTSTATUS *PCNTSTATUS;
#endif // _WIN32_WINNT >= 0x0600

//
//  Status values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-------------------------+-------------------------------+
//  |Sev|C|       Facility          |               Code            |
//  +---+-+-------------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

//
// Generic test for information on any status value.
//

#ifdef _PREFAST_
#define NT_INFORMATION(Status) (((NTSTATUS)(Status)) >= (long)0x40000000)
#else
#define NT_INFORMATION(Status) ((((ULONG)(Status)) >> 30) == 1)
#endif

//
// Generic test for warning on any status value.
//

#ifdef _PREFAST_
#define NT_WARNING(Status) (((NTSTATUS)(Status) < (long)0xc0000000))
#else
#define NT_WARNING(Status) ((((ULONG)(Status)) >> 30) == 2)
#endif

//
// Generic test for error on any status value.
//

#ifdef _PREFAST_
#define NT_ERROR(Status) (((NTSTATUS)(Status)) >= (unsigned long)0xc0000000)
#else
#define NT_ERROR(Status) ((((ULONG)(Status)) >> 30) == 3)
#endif




//
// Physical address.
//

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;


//
// Event type
//

typedef enum _EVENT_TYPE {
    NotificationEvent,
    SynchronizationEvent
} EVENT_TYPE;


//
// Pointer to an Asciiz string
//

typedef _Null_terminated_ CHAR *PSZ;
typedef _Null_terminated_ CONST char *PCSZ;


//
// Counted String
//

typedef USHORT RTL_STRING_LENGTH_TYPE;

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength), length_is(Length) ]
#endif // MIDL_PASS
    _Field_size_bytes_part_opt_(MaximumLength, Length) PCHAR Buffer;
} STRING;
typedef STRING *PSTRING;
typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;




typedef STRING CANSI_STRING;
typedef PSTRING PCANSI_STRING;

typedef STRING UTF8_STRING;
typedef PSTRING PUTF8_STRING;

//
// Unicode strings are counted 16-bit character strings. If they are
// NULL terminated, Length does not include trailing NULL.
//

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    _Field_size_bytes_part_opt_(MaximumLength, Length) PWCH   Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;



#define DECLARE_CONST_UNICODE_STRING(_var, _string) \
const WCHAR _var ## _buffer[] = _string; \
__pragma(warning(push)) \
__pragma(warning(disable:4221)) __pragma(warning(disable:4204)) \
const UNICODE_STRING _var = { sizeof(_string) - sizeof(WCHAR), sizeof(_string), (PWCH) _var ## _buffer } \
__pragma(warning(pop))

#define DECLARE_GLOBAL_CONST_UNICODE_STRING(_var, _str) \
extern const __declspec(selectany) UNICODE_STRING _var = RTL_CONSTANT_STRING(_str)

#define DECLARE_UNICODE_STRING_SIZE(_var, _size) \
WCHAR _var ## _buffer[_size]; \
__pragma(warning(push)) \
__pragma(warning(disable:4221)) __pragma(warning(disable:4204)) \
UNICODE_STRING _var = { 0, (_size) * sizeof(WCHAR) , _var ## _buffer } \
__pragma(warning(pop))


//
// Valid values for the Attributes field
//

#define OBJ_INHERIT                         0x00000002L
#define OBJ_PERMANENT                       0x00000010L
#define OBJ_EXCLUSIVE                       0x00000020L
#define OBJ_CASE_INSENSITIVE                0x00000040L
#define OBJ_OPENIF                          0x00000080L
#define OBJ_OPENLINK                        0x00000100L
#define OBJ_KERNEL_HANDLE                   0x00000200L
#define OBJ_FORCE_ACCESS_CHECK              0x00000400L
#define OBJ_IGNORE_IMPERSONATED_DEVICEMAP   0x00000800L
#define OBJ_DONT_REPARSE                    0x00001000L
#define OBJ_VALID_ATTRIBUTES                0x00001FF2L


typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;
typedef CONST OBJECT_ATTRIBUTES *PCOBJECT_ATTRIBUTES;

//++
//
// VOID
// InitializeObjectAttributes(
//     _Out_ POBJECT_ATTRIBUTES p,
//     _In_ PUNICODE_STRING n,
//     _In_ ULONG a,
//     _In_ HANDLE r,
//     _In_ PSECURITY_DESCRIPTOR s
//     )
//
//--

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

//
// Determine if an argument is present by testing the value of the pointer
// to the argument value.
//

#define ARGUMENT_PRESENT(ArgumentPointer)    (\
    (CHAR *)((ULONG_PTR)(ArgumentPointer)) != (CHAR *)(NULL) )



//
// Interrupt Request Level (IRQL)
//

typedef UCHAR KIRQL;

typedef KIRQL *PKIRQL;






//
// Interrupt modes.
//

typedef enum _KINTERRUPT_MODE {
    LevelSensitive,
    Latched
} KINTERRUPT_MODE;



typedef enum _MEMORY_CACHING_TYPE_ORIG {
    MmFrameBufferCached = 2
} MEMORY_CACHING_TYPE_ORIG;

typedef enum _MEMORY_CACHING_TYPE {
    MmNonCached = FALSE,
    MmCached = TRUE,
    MmWriteCombined = MmFrameBufferCached,
    MmHardwareCoherentCached,
    MmNonCachedUnordered,       // IA64
    MmUSWCCached,
    MmMaximumCacheType,
    MmNotMapped = -1
} MEMORY_CACHING_TYPE;


//
// Define the major function codes for IRPs.
//


#define IRP_MJ_CREATE                   0x00
#define IRP_MJ_CREATE_NAMED_PIPE        0x01
#define IRP_MJ_CLOSE                    0x02
#define IRP_MJ_READ                     0x03
#define IRP_MJ_WRITE                    0x04
#define IRP_MJ_QUERY_INFORMATION        0x05
#define IRP_MJ_SET_INFORMATION          0x06
#define IRP_MJ_QUERY_EA                 0x07
#define IRP_MJ_SET_EA                   0x08
#define IRP_MJ_FLUSH_BUFFERS            0x09
#define IRP_MJ_QUERY_VOLUME_INFORMATION 0x0a
#define IRP_MJ_SET_VOLUME_INFORMATION   0x0b
#define IRP_MJ_DIRECTORY_CONTROL        0x0c
#define IRP_MJ_FILE_SYSTEM_CONTROL      0x0d
#define IRP_MJ_DEVICE_CONTROL           0x0e
#define IRP_MJ_INTERNAL_DEVICE_CONTROL  0x0f
#define IRP_MJ_SHUTDOWN                 0x10
#define IRP_MJ_LOCK_CONTROL             0x11
#define IRP_MJ_CLEANUP                  0x12
#define IRP_MJ_CREATE_MAILSLOT          0x13
#define IRP_MJ_QUERY_SECURITY           0x14
#define IRP_MJ_SET_SECURITY             0x15
#define IRP_MJ_POWER                    0x16
#define IRP_MJ_SYSTEM_CONTROL           0x17
#define IRP_MJ_DEVICE_CHANGE            0x18
#define IRP_MJ_QUERY_QUOTA              0x19
#define IRP_MJ_SET_QUOTA                0x1a
#define IRP_MJ_PNP                      0x1b
#define IRP_MJ_PNP_POWER                IRP_MJ_PNP      // Obsolete....
#define IRP_MJ_MAXIMUM_FUNCTION         0x1b

//
// Make the Scsi major code the same as internal device control.
//

#define IRP_MJ_SCSI                     IRP_MJ_INTERNAL_DEVICE_CONTROL

//
// Define the minor function codes for IRPs.  The lower 128 codes, from 0x00 to
// 0x7f are reserved to Microsoft.  The upper 128 codes, from 0x80 to 0xff, are
// reserved to customers of Microsoft.
//


#if defined(_WIN64)
#define POINTER_ALIGNMENT DECLSPEC_ALIGN(8)
#else
#define POINTER_ALIGNMENT
#endif

//
// Define driver initialization routine type.
//
_Function_class_(DRIVER_INITIALIZE)
_IRQL_requires_same_
_IRQL_requires_(PASSIVE_LEVEL)
typedef
NTSTATUS
DRIVER_INITIALIZE (
    _In_ struct _DRIVER_OBJECT *DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    );

typedef DRIVER_INITIALIZE *PDRIVER_INITIALIZE;

//
// Define the I/O system's security context type for use by file system's
// when checking access to volumes, files, and directories.
//

typedef struct _IO_SECURITY_CONTEXT {
    PSECURITY_QUALITY_OF_SERVICE SecurityQos;
    PACCESS_STATE AccessState;
    ACCESS_MASK DesiredAccess;
    ULONG FullCreateOptions;
} IO_SECURITY_CONTEXT, *PIO_SECURITY_CONTEXT;

//
// \Callback\PowerState values
//

#define PO_CB_SYSTEM_POWER_POLICY       0
#define PO_CB_AC_STATUS                 1
#define PO_CB_BUTTON_COLLISION          2 // deprecated
#define PO_CB_SYSTEM_STATE_LOCK         3
#define PO_CB_LID_SWITCH_STATE          4
#define PO_CB_PROCESSOR_POWER_POLICY    5 // deprecated




//
// Runtime Power Management Framework
//

#define PO_FX_VERSION_V1 0x00000001
#define PO_FX_VERSION_V2 0x00000002
#define PO_FX_VERSION_V3 0x00000003
#define PO_FX_VERSION PO_FX_VERSION_V1

DECLARE_HANDLE(POHANDLE);

typedef
_Function_class_(PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK (
    _In_ PVOID Context,
    _In_ ULONG Component
    );

typedef PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK
    *PPO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK;

typedef
_Function_class_(PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK (
    _In_ PVOID Context,
    _In_ ULONG Component
    );

typedef PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK
    *PPO_FX_COMPONENT_IDLE_CONDITION_CALLBACK;

typedef
_Function_class_(PO_FX_COMPONENT_IDLE_STATE_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PO_FX_COMPONENT_IDLE_STATE_CALLBACK (
    _In_ PVOID Context,
    _In_ ULONG Component,
    _In_ ULONG State
    );

typedef PO_FX_COMPONENT_IDLE_STATE_CALLBACK
    *PPO_FX_COMPONENT_IDLE_STATE_CALLBACK;

typedef
_Function_class_(PO_FX_DEVICE_POWER_REQUIRED_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PO_FX_DEVICE_POWER_REQUIRED_CALLBACK (
    _In_ PVOID Context
    );

typedef PO_FX_DEVICE_POWER_REQUIRED_CALLBACK
    *PPO_FX_DEVICE_POWER_REQUIRED_CALLBACK;

typedef
_Function_class_(PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK (
    _In_ PVOID Context
    );

typedef PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK
    *PPO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK;

typedef
_Function_class_(PO_FX_POWER_CONTROL_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PO_FX_POWER_CONTROL_CALLBACK (
    _In_ PVOID DeviceContext,
    _In_ LPCGUID PowerControlCode,
    _In_reads_bytes_opt_(InBufferSize) PVOID InBuffer,
    _In_ SIZE_T InBufferSize,
    _Out_writes_bytes_opt_(OutBufferSize) PVOID OutBuffer,
    _In_ SIZE_T OutBufferSize,
    _Out_opt_ PSIZE_T BytesReturned
    );

typedef PO_FX_POWER_CONTROL_CALLBACK *PPO_FX_POWER_CONTROL_CALLBACK;

typedef
_Function_class_(PO_FX_COMPONENT_CRITICAL_TRANSITION_CALLBACK)
_IRQL_requires_max_(HIGH_LEVEL)
VOID
PO_FX_COMPONENT_CRITICAL_TRANSITION_CALLBACK (
    _In_ PVOID Context,
    _In_ ULONG Component,
    _In_ BOOLEAN Active
    );

typedef PO_FX_COMPONENT_CRITICAL_TRANSITION_CALLBACK
    *PPO_FX_COMPONENT_CRITICAL_TRANSITION_CALLBACK;

typedef struct _PO_FX_COMPONENT_IDLE_STATE {
    ULONGLONG TransitionLatency;
    ULONGLONG ResidencyRequirement;
    ULONG NominalPower;
} PO_FX_COMPONENT_IDLE_STATE, *PPO_FX_COMPONENT_IDLE_STATE;

typedef struct _PO_FX_COMPONENT_V1 {
    GUID Id;
    ULONG IdleStateCount;
    ULONG DeepestWakeableIdleState;
    _Field_size_full_(IdleStateCount) PPO_FX_COMPONENT_IDLE_STATE IdleStates;
} PO_FX_COMPONENT_V1, *PPO_FX_COMPONENT_V1;

typedef struct _PO_FX_DEVICE_V1 {
    ULONG Version;
    ULONG ComponentCount;
    PPO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK ComponentActiveConditionCallback;
    PPO_FX_COMPONENT_IDLE_CONDITION_CALLBACK ComponentIdleConditionCallback;
    PPO_FX_COMPONENT_IDLE_STATE_CALLBACK ComponentIdleStateCallback;
    PPO_FX_DEVICE_POWER_REQUIRED_CALLBACK DevicePowerRequiredCallback;
    PPO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK DevicePowerNotRequiredCallback;
    PPO_FX_POWER_CONTROL_CALLBACK PowerControlCallback;
    PVOID DeviceContext;
    _Field_size_full_(ComponentCount) PO_FX_COMPONENT_V1 Components[ANYSIZE_ARRAY];
} PO_FX_DEVICE_V1, *PPO_FX_DEVICE_V1;

#define PO_FX_COMPONENT_FLAG_F0_ON_DX 0x0000000000000001
#define PO_FX_COMPONENT_FLAG_NO_DEBOUNCE 0x0000000000000002

typedef struct _PO_FX_COMPONENT_V2 {
    GUID Id;
    ULONGLONG Flags;
    ULONG DeepestWakeableIdleState;
    ULONG IdleStateCount;
    _Field_size_full_(IdleStateCount) PPO_FX_COMPONENT_IDLE_STATE IdleStates;
    ULONG ProviderCount;
    _Field_size_full_(ProviderCount) PULONG Providers;
} PO_FX_COMPONENT_V2, *PPO_FX_COMPONENT_V2;

typedef struct _PO_FX_DEVICE_V2 {
    ULONG Version;
    ULONGLONG Flags;
    PPO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK ComponentActiveConditionCallback;
    PPO_FX_COMPONENT_IDLE_CONDITION_CALLBACK ComponentIdleConditionCallback;
    PPO_FX_COMPONENT_IDLE_STATE_CALLBACK ComponentIdleStateCallback;
    PPO_FX_DEVICE_POWER_REQUIRED_CALLBACK DevicePowerRequiredCallback;
    PPO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK DevicePowerNotRequiredCallback;
    PPO_FX_POWER_CONTROL_CALLBACK PowerControlCallback;
    PVOID DeviceContext;
    ULONG ComponentCount;
    _Field_size_full_(ComponentCount) PO_FX_COMPONENT_V2 Components[ANYSIZE_ARRAY];
} PO_FX_DEVICE_V2, *PPO_FX_DEVICE_V2;

//
// PoFx Version 3 type definitions.
//

//
// PO_FX_DEVICE_FLAG_DIRECT_CHILDREN_OPTIONAL - When set allows all direct
//     child devices of this device to optionally support Directed Fx.
//     If not provided, then all direct children must support Directed Fx
//     for this device to fully support Directed Fx.
//
// PO_FX_DEVICE_FLAG_POWER_CHILDREN_OPTIONAL - When set allows all power
//     child devices of this device to optionally support Directed Fx.
//     If not provided, then all power children must support Directed Fx
//     for this device to fully support Directed Fx.
//
// PO_FX_DEVICE_FLAG_DFX_CHILDREN_OPTIONAL - Sets both PO_FX_DEVICE_FLAG_DFX_DIRECT_CHILDREN_OPTIONAL
//     and PO_FX_DEVICE_FLAG_DFX_POWER_CHILDREN_OPTIONAL.
//
// PO_FX_DEVICE_FLAG_DISABLE_FAST_RESUME - When set this enforces that the
//     device completes its D0-IRP before the system will complete the S0-IRP
//     when resuming from a system state transition.
//
// PO_FX_DEVICE_FLAG_ENABLE_FAST_RESUME - When set this enforces that the device
//     does not wait for the D0-IRP to be completed before the system allows
//     the S0-IRP to be completed when resuming from a system state transition.
//

#define PO_FX_DEVICE_FLAG_RESERVED_1                   (0x0000000000000001ull)
#define PO_FX_DEVICE_FLAG_DFX_DIRECT_CHILDREN_OPTIONAL (0x0000000000000002ull)
#define PO_FX_DEVICE_FLAG_DFX_POWER_CHILDREN_OPTIONAL  (0x0000000000000004ull)
#define PO_FX_DEVICE_FLAG_DFX_CHILDREN_OPTIONAL \
            (PO_FX_DEVICE_FLAG_DFX_DIRECT_CHILDREN_OPTIONAL | \
             PO_FX_DEVICE_FLAG_DFX_POWER_CHILDREN_OPTIONAL)

#define PO_FX_DEVICE_FLAG_DISABLE_FAST_RESUME          (0x0000000000000008ull)
#define PO_FX_DEVICE_FLAG_ENABLE_FAST_RESUME           (0x0000000000000010ull)
#define PO_FX_DEVICE_FLAG_NO_FAULT_CALLBACKS           (0x0000000000000020ull)

#define PO_FX_DIRECTED_FX_DEFAULT_IDLE_TIMEOUT    (0ul)
#define PO_FX_DIRECTED_FX_IMMEDIATE_IDLE_TIMEOUT  ((ULONG)-1)
#define PO_FX_DIRECTED_FX_MAX_IDLE_TIMEOUT        (10ul * 60)

#define PO_FX_DEVICE_FLAG_D0_ONLY              (0xFFEE000000000000ull)


typedef
_Function_class_(PO_FX_DIRECTED_POWER_UP_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
VOID
PO_FX_DIRECTED_POWER_UP_CALLBACK (
    _In_ PVOID Context,
    _In_ ULONG Flags
    );

typedef PO_FX_DIRECTED_POWER_UP_CALLBACK *PPO_FX_DIRECTED_POWER_UP_CALLBACK;

typedef
_Function_class_(PO_FX_DIRECTED_POWER_DOWN_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
VOID
PO_FX_DIRECTED_POWER_DOWN_CALLBACK (
    _In_ PVOID Context,
    _In_ ULONG Flags
    );

typedef PO_FX_DIRECTED_POWER_DOWN_CALLBACK *PPO_FX_DIRECTED_POWER_DOWN_CALLBACK;

typedef struct _PO_FX_DEVICE_V3 {
    ULONG Version;
    ULONGLONG Flags;
    PPO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK ComponentActiveConditionCallback;
    PPO_FX_COMPONENT_IDLE_CONDITION_CALLBACK ComponentIdleConditionCallback;
    PPO_FX_COMPONENT_IDLE_STATE_CALLBACK ComponentIdleStateCallback;
    PPO_FX_DEVICE_POWER_REQUIRED_CALLBACK DevicePowerRequiredCallback;
    PPO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK DevicePowerNotRequiredCallback;
    PPO_FX_POWER_CONTROL_CALLBACK PowerControlCallback;
    PPO_FX_DIRECTED_POWER_UP_CALLBACK DirectedPowerUpCallback;
    PPO_FX_DIRECTED_POWER_DOWN_CALLBACK DirectedPowerDownCallback;
    ULONG DirectedFxTimeoutInSeconds;
    PVOID DeviceContext;
    ULONG ComponentCount;
    _Field_size_full_(ComponentCount) PO_FX_COMPONENT_V2 Components[ANYSIZE_ARRAY];
} PO_FX_DEVICE_V3, *PPO_FX_DEVICE_V3;

#if (PO_FX_VERSION == PO_FX_VERSION_V1)
typedef PO_FX_COMPONENT_V1 PO_FX_COMPONENT, *PPO_FX_COMPONENT;
typedef PO_FX_DEVICE_V1 PO_FX_DEVICE, *PPO_FX_DEVICE;
#elif (PO_FX_VERSION == PO_FX_VERSION_V2)
typedef PO_FX_COMPONENT_V2 PO_FX_COMPONENT, *PPO_FX_COMPONENT;
typedef PO_FX_DEVICE_V2 PO_FX_DEVICE, *PPO_FX_DEVICE;
#elif (PO_FX_VERSION == PO_FX_VERSION_V3)
typedef PO_FX_COMPONENT_V2 PO_FX_COMPONENT, *PPO_FX_COMPONENT;
typedef PO_FX_DEVICE_V3 PO_FX_DEVICE, *PPO_FX_DEVICE;
#else
#error PO_FX_VERSION undefined!
#endif

typedef enum _PO_FX_PERF_STATE_UNIT {
    PoFxPerfStateUnitOther,
    PoFxPerfStateUnitFrequency,
    PoFxPerfStateUnitBandwidth,
    PoFxPerfStateUnitMaximum
} PO_FX_PERF_STATE_UNIT, *PPO_FX_PERF_STATE_UNIT;

typedef enum _PO_FX_PERF_STATE_TYPE {
    PoFxPerfStateTypeDiscrete,
    PoFxPerfStateTypeRange,
    PoFxPerfStateTypeMaximum
} PO_FX_PERF_STATE_TYPE, *PPO_FX_PERF_STATE_TYPE;

typedef struct _PO_FX_PERF_STATE {
    ULONGLONG Value;
    PVOID Context;
} PO_FX_PERF_STATE, *PPO_FX_PERF_STATE;

typedef struct _PO_FX_COMPONENT_PERF_SET {
    UNICODE_STRING Name;
    ULONGLONG Flags;
    PO_FX_PERF_STATE_UNIT Unit;
    PO_FX_PERF_STATE_TYPE Type;
    union {
        struct {
            ULONG Count;
            _Field_size_full_(Count) PPO_FX_PERF_STATE States;
        } Discrete;
        struct {
            ULONGLONG Minimum;
            ULONGLONG Maximum;
        } Range;
    };
} PO_FX_COMPONENT_PERF_SET, *PPO_FX_COMPONENT_PERF_SET;

typedef struct _PO_FX_COMPONENT_PERF_INFO {
    ULONG PerfStateSetsCount;
    PO_FX_COMPONENT_PERF_SET PerfStateSets[ANYSIZE_ARRAY];
} PO_FX_COMPONENT_PERF_INFO, *PPO_FX_COMPONENT_PERF_INFO;

typedef struct _PO_FX_PERF_STATE_CHANGE {
    ULONG Set;
    union {
        ULONG StateIndex;
        ULONGLONG StateValue;
    };
} PO_FX_PERF_STATE_CHANGE, *PPO_FX_PERF_STATE_CHANGE;


//
// Pool Allocation routines (in pool.c)
//
typedef _Enum_is_bitflag_ enum _POOL_TYPE {
    NonPagedPool,
    NonPagedPoolExecute = NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed = NonPagedPool + 2,
    DontUseThisType,
    NonPagedPoolCacheAligned = NonPagedPool + 4,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS = NonPagedPool + 6,
    MaxPoolType,

    //
    // Define base types for NonPaged (versus Paged) pool, for use in cracking
    // the underlying pool type.
    //

    NonPagedPoolBase = 0,
    NonPagedPoolBaseMustSucceed = NonPagedPoolBase + 2,
    NonPagedPoolBaseCacheAligned = NonPagedPoolBase + 4,
    NonPagedPoolBaseCacheAlignedMustS = NonPagedPoolBase + 6,

    //
    // Note these per session types are carefully chosen so that the appropriate
    // masking still applies as well as MaxPoolType above.
    //

    NonPagedPoolSession = 32,
    PagedPoolSession = NonPagedPoolSession + 1,
    NonPagedPoolMustSucceedSession = PagedPoolSession + 1,
    DontUseThisTypeSession = NonPagedPoolMustSucceedSession + 1,
    NonPagedPoolCacheAlignedSession = DontUseThisTypeSession + 1,
    PagedPoolCacheAlignedSession = NonPagedPoolCacheAlignedSession + 1,
    NonPagedPoolCacheAlignedMustSSession = PagedPoolCacheAlignedSession + 1,

    NonPagedPoolNx = 512,
    NonPagedPoolNxCacheAligned = NonPagedPoolNx + 4,
    NonPagedPoolSessionNx = NonPagedPoolNx + 32,

} _Enum_is_bitflag_ POOL_TYPE;

#define POOL_COLD_ALLOCATION 256     // Note this cannot encode into the header.

#define POOL_NX_ALLOCATION   512     // Note this cannot encode into the header.

#define POOL_ZERO_ALLOCATION      1024


//++
//
// ULONG_PTR
// ROUND_TO_PAGES (
//     _In_ ULONG_PTR Size
//     )
//
// Routine Description:
//
//     The ROUND_TO_PAGES macro takes a size in bytes and rounds it up to a
//     multiple of the page size.
//
//     NOTE: This macro fails for values 0xFFFFFFFF - (PAGE_SIZE - 1).
//
// Arguments:
//
//     Size - Size in bytes to round up to a page multiple.
//
// Return Value:
//
//     Returns the size rounded up to a multiple of the page size.
//
//--

#define ROUND_TO_PAGES(Size)  (((ULONG_PTR)(Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

//++
//
// ULONG
// BYTES_TO_PAGES (
//     _In_ ULONG Size
//     )
//
// Routine Description:
//
//     The BYTES_TO_PAGES macro takes the size in bytes and calculates the
//     number of pages required to contain the bytes.
//
// Arguments:
//
//     Size - Size in bytes.
//
// Return Value:
//
//     Returns the number of pages required to contain the specified size.
//
//--

#define BYTES_TO_PAGES(Size)  (((Size) >> PAGE_SHIFT) + \
                               (((Size) & (PAGE_SIZE - 1)) != 0))

//++
//
// ULONG
// BYTE_OFFSET (
//     _In_ PVOID Va
//     )
//
// Routine Description:
//
//     The BYTE_OFFSET macro takes a virtual address and returns the byte offset
//     of that address within the page.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns the byte offset portion of the virtual address.
//
//--

#define BYTE_OFFSET(Va) ((ULONG)((LONG_PTR)(Va) & (PAGE_SIZE - 1)))

//++
//
// PVOID
// PAGE_ALIGN (
//     _In_ PVOID Va
//     )
//
// Routine Description:
//
//     The PAGE_ALIGN macro takes a virtual address and returns a page-aligned
//     virtual address for that page.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns the page aligned virtual address.
//
//--

#define PAGE_ALIGN(Va) ((PVOID)((ULONG_PTR)(Va) & ~(PAGE_SIZE - 1)))

//++
//
// SIZE_T
// ADDRESS_AND_SIZE_TO_SPAN_PAGES (
//     _In_ PVOID Va,
//     _In_ SIZE_T Size
//     )
//
// Routine Description:
//
//     The ADDRESS_AND_SIZE_TO_SPAN_PAGES macro takes a virtual address and
//     size and returns the number of pages spanned by the size.
//
// Arguments:
//
//     Va - Virtual address.
//
//     Size - Size in bytes.
//
// Return Value:
//
//     Returns the number of pages spanned by the size.
//
//--

#define ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size) \
    ((BYTE_OFFSET (Va) + ((SIZE_T) (Size)) + (PAGE_SIZE - 1)) >> PAGE_SHIFT)


//
// Define PnP Device Property for IoGetDeviceProperty
//

#ifdef _PREFAST_
#define __string_type 0x1000
#define __guid_type 0x2000
#define __multiString_type 0x4000
#else
#define __string_type 0
#define __guid_type 0
#define __multiString_type 0
#endif

typedef enum {
    DevicePropertyDeviceDescription = 0x0 | __string_type,
    DevicePropertyHardwareID = 0x1 | __multiString_type,
    DevicePropertyCompatibleIDs = 0x2 | __multiString_type,
    DevicePropertyBootConfiguration = 0x3,
    DevicePropertyBootConfigurationTranslated = 0x4,
    DevicePropertyClassName = 0x5 | __string_type,
    DevicePropertyClassGuid = 0x6 | __string_type,
    DevicePropertyDriverKeyName = 0x7 | __string_type,
    DevicePropertyManufacturer = 0x8 | __string_type,
    DevicePropertyFriendlyName = 0x9 | __string_type,
    DevicePropertyLocationInformation = 0xa | __string_type,
    DevicePropertyPhysicalDeviceObjectName = 0xb | __string_type,
    DevicePropertyBusTypeGuid = 0xc | __guid_type,
    DevicePropertyLegacyBusType = 0xd,
    DevicePropertyBusNumber = 0xe,
    DevicePropertyEnumeratorName = 0xf | __string_type,
    DevicePropertyAddress = 0x10,
    DevicePropertyUINumber = 0x11,
    DevicePropertyInstallState = 0x12,
    DevicePropertyRemovalPolicy = 0x13,
    DevicePropertyResourceRequirements = 0x14,
    DevicePropertyAllocatedResources = 0x15,
    DevicePropertyContainerID = 0x16 | __string_type
} DEVICE_REGISTRY_PROPERTY;

//
// The following definitions are used in IoOpenDeviceRegistryKey
//

#define PLUGPLAY_REGKEY_DEVICE  1
#define PLUGPLAY_REGKEY_DRIVER  2
#define PLUGPLAY_REGKEY_CURRENT_HWPROFILE 4


#if (NTDDI_VERSION >= NTDDI_VISTA)

//
// Custom device properties...
//

#include <devpropdef.h>

//
// Definitions of property flags.
//

#define PLUGPLAY_PROPERTY_PERSISTENT  0x00000001

#endif

//
// Processor modes.
//

typedef CCHAR KPROCESSOR_MODE;

typedef enum _MODE {
    KernelMode,
    UserMode,
    MaximumMode
} MODE;



#if defined(_X86_)

//
// i386 Specific portions of Mm component.
//
// Define the page size for the Intel 386 as 4096 (0x1000).
//

#define PAGE_SIZE 0x1000

//
// Define the number of trailing zeros in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 12L

#elif defined(_AMD64_)

//
// AMD64 Specific portions of Mm component.
//
// Define the page size for the AMD64 as 4096 (0x1000).
//

#define PAGE_SIZE 0x1000

//
// Define the number of trailing zeros in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 12L

#elif defined(_ARM64_)

//
// ARM Specific portions of Mm component.
//
// Define the page size for the ARM64 as 4096 (0x1000).
//

#define PAGE_SIZE 0x1000

//
// Define the number of trailing zeros in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 12L

#elif defined(_ARM_)

//
// ARM Specific portions of Mm component.
//
// Define the page size for the ARM as 4096 (0x1000).
//

#define PAGE_SIZE 0x1000

//
// Define the number of trailing zeros in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 12L

#endif





#if !defined(NTKERNELAPI)

#if defined(_NTHALLIB_)

#define NTKERNELAPI

#else

#define NTKERNELAPI DECLSPEC_IMPORT     // wdm ntndis ntifs

#endif

#endif




//
// Priority increment definitions.  The comment for each definition gives
// the names of the system services that use the definition when satisfying
// a wait.
//

//
// Priority increment used when satisfying a wait on an executive event
// (NtPulseEvent and NtSetEvent)
//

#define EVENT_INCREMENT                 1



//
// Priority increment when no I/O has been done.  This is used by device
// and file system drivers when completing an IRP (IoCompleteRequest).
//

#define IO_NO_INCREMENT                 0


//
// Priority increment for completing CD-ROM I/O.  This is used by CD-ROM device
// and file system drivers when completing an IRP (IoCompleteRequest)
//

#define IO_CD_ROM_INCREMENT             1

//
// Priority increment for completing disk I/O.  This is used by disk device
// and file system drivers when completing an IRP (IoCompleteRequest)
//

#define IO_DISK_INCREMENT               1



//
// Priority increment for completing keyboard I/O.  This is used by keyboard
// device drivers when completing an IRP (IoCompleteRequest)
//

#define IO_KEYBOARD_INCREMENT           6


//
// Priority increment for completing mailslot I/O.  This is used by the mail-
// slot file system driver when completing an IRP (IoCompleteRequest).
//

#define IO_MAILSLOT_INCREMENT           2


//
// Priority increment for completing mouse I/O.  This is used by mouse device
// drivers when completing an IRP (IoCompleteRequest)
//

#define IO_MOUSE_INCREMENT              6


//
// Priority increment for completing named pipe I/O.  This is used by the
// named pipe file system driver when completing an IRP (IoCompleteRequest).
//

#define IO_NAMED_PIPE_INCREMENT         2

//
// Priority increment for completing network I/O.  This is used by network
// device and network file system drivers when completing an IRP
// (IoCompleteRequest).
//

#define IO_NETWORK_INCREMENT            2


//
// Priority increment for completing parallel I/O.  This is used by parallel
// device drivers when completing an IRP (IoCompleteRequest)
//

#define IO_PARALLEL_INCREMENT           1

//
// Priority increment for completing serial I/O.  This is used by serial device
// drivers when completing an IRP (IoCompleteRequest)
//

#define IO_SERIAL_INCREMENT             2

//
// Priority increment for completing sound I/O.  This is used by sound device
// drivers when completing an IRP (IoCompleteRequest)
//

#define IO_SOUND_INCREMENT              8

//
// Priority increment for completing video I/O.  This is used by video device
// drivers when completing an IRP (IoCompleteRequest)
//

#define IO_VIDEO_INCREMENT              1



//
// Priority increment used when satisfying a wait on an executive semaphore
// (NtReleaseSemaphore)
//

#define SEMAPHORE_INCREMENT             1



// begin_halextenv



#if defined(_AMD64_) && !defined(_ARM64EC_) && !defined(DSF_DRIVER)

//
// I/O space read and write macros.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space.
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT space.
//

#ifdef __cplusplus
extern "C" {
#endif

__forceinline
UCHAR
READ_REGISTER_UCHAR (
    _In_ _Notliteral_ volatile UCHAR *Register
    )
{
    _ReadWriteBarrier();
    return *Register;
}

__forceinline
USHORT
READ_REGISTER_USHORT (
    _In_ _Notliteral_ volatile USHORT *Register
    )
{
    _ReadWriteBarrier();
    return *Register;
}

__forceinline
ULONG
READ_REGISTER_ULONG (
    _In_ _Notliteral_ volatile ULONG *Register
    )
{
    _ReadWriteBarrier();
    return *Register;
}

__forceinline
ULONG64
READ_REGISTER_ULONG64 (
    _In_ _Notliteral_ volatile ULONG64 *Register
    )
{
    _ReadWriteBarrier();
    return *Register;
}

__forceinline
VOID
READ_REGISTER_BUFFER_UCHAR (
    _In_reads_(Count) _Notliteral_ volatile UCHAR *Register,
    _Out_writes_all_(Count) PUCHAR Buffer,
    _In_ ULONG Count
    )
{
    _ReadWriteBarrier();
    __movsb(Buffer, (PUCHAR)Register, Count);
    return;
}

__forceinline
VOID
READ_REGISTER_BUFFER_USHORT (
    _In_reads_(Count) _Notliteral_ volatile USHORT *Register,
    _Out_writes_all_(Count) PUSHORT Buffer,
    _In_ ULONG Count
    )
{
    _ReadWriteBarrier();
    __movsw(Buffer, (PUSHORT)Register, Count);
    return;
}

__forceinline
VOID
READ_REGISTER_BUFFER_ULONG (
    _In_reads_(Count) _Notliteral_ volatile ULONG *Register,
    _Out_writes_all_(Count) PULONG Buffer,
    _In_ ULONG Count
    )
{
    _ReadWriteBarrier();
    __movsd(Buffer, (PULONG)Register, Count);
    return;
}

__forceinline
VOID
READ_REGISTER_BUFFER_ULONG64 (
    _In_reads_(Count) _Notliteral_ volatile ULONG64 *Register,
    _Out_writes_all_(Count) PULONG64 Buffer,
    _In_ ULONG Count
    )
{
    _ReadWriteBarrier();
    __movsq(Buffer, (PULONG64)Register, Count);
    return;
}

__forceinline
VOID
WRITE_REGISTER_UCHAR (
    _In_ _Notliteral_ volatile UCHAR *Register,
    _In_ UCHAR Value
    )
{

    *Register = Value;
    FastFence();
    return;
}

__forceinline
VOID
WRITE_REGISTER_USHORT (
    _In_ _Notliteral_ volatile USHORT *Register,
    _In_ USHORT Value
    )
{

    *Register = Value;
    FastFence();
    return;
}

__forceinline
VOID
WRITE_REGISTER_ULONG (
    _In_ _Notliteral_ volatile ULONG *Register,
    _In_ ULONG Value
    )
{

    *Register = Value;
    FastFence();
    return;
}

__forceinline
VOID
WRITE_REGISTER_ULONG64 (
    _In_ _Notliteral_ volatile ULONG64 *Register,
    _In_ ULONG64 Value
    )
{

    *Register = Value;
    FastFence();
    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_UCHAR (
    _Out_writes_(Count) _Notliteral_ volatile UCHAR *Register,
    _In_reads_(Count) PUCHAR Buffer,
    _In_ ULONG Count
    )
{

    __movsb((PUCHAR)Register, Buffer, Count);
    FastFence();
    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_USHORT (
    _Out_writes_(Count) _Notliteral_ volatile USHORT *Register,
    _In_reads_(Count) PUSHORT Buffer,
    _In_ ULONG Count
    )
{

    __movsw((PUSHORT)Register, Buffer, Count);
    FastFence();
    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_ULONG (
    _Out_writes_(Count) _Notliteral_ volatile ULONG *Register,
    _In_reads_(Count) PULONG Buffer,
    _In_ ULONG Count
    )
{

    __movsd((PULONG)Register, Buffer, Count);
    FastFence();
    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_ULONG64 (
    _Out_writes_(Count) _Notliteral_ volatile ULONG64 *Register,
    _In_reads_(Count) PULONG64 Buffer,
    _In_ ULONG Count
    )
{

    __movsq((PULONG64)Register, Buffer, Count);
    FastFence();
    return;
}

// end_halextenv
// begin_halextenv

#ifdef __cplusplus
}
#endif

#endif

// end_halextenv



// begin_halextenv

#if defined(_ARM64_) || defined(_ARM64EC_)

//
// I/O space read and write macros.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space.
//
//  N.B. This implementation assumes that the memory mapped registers
//       have been mapped using the OS concept of uncached memory
//       which is implemented using the ARMv7 strongly ordered memory
//       type.  In addition, the register access is bracketed by a
//       compiler barrier to ensure that the compiler does not
//       re-order the I/O accesses with other accesses and a data
//       synchronization barrier to ensure that any side effects of
//       the access have started (but not necessarily completed).
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT
//  space.  The ARM architecture doesn't have a seperate I/O space.
//  These operations bugcheck so as to identify incorrect code.
//

#ifdef __cplusplus
extern "C" {
#endif

//
// The line of code used in the following macro may seem just like a strange
// no-op. In fact, it does prevent the compiler from generating register
// writeback load/store instructions into the hardware register. Without it, the
// compiler will generate these instructions very aggressively, for
// post-incrementing the pointer, when there is a loop. While there isn't,
// presently, anything in AARCH explicitly advising against using register
// writeback l/s instructions into hardware registers, these do present a
// problem: They are too complex to be fully described by the HSR.ISS syndrome
// field, in case of a Data Abort Exception. This means a hypervisor performing
// hardware emulation would have to fetch and decode the faulting instruction in
// order to emulate it. Since this comes with some security implications, some
// hypervisors, like Linux KVM, have opted to simply restrict guests from using
// said instructions. While this puts a significant limitation on the
// virtualization platform, it is also a somewhat understandable decision.
// N.B. Linux (as a guest) never uses these instructions for hardware access.
//

#define ARM64_PREVENT_REGISTER_WRITEBACK(_type, _variable) \
    _variable = (volatile _type *)ReadPointerNoFence((PVOID const volatile *)&##_variable);

__forceinline
UCHAR
READ_REGISTER_NOFENCE_UCHAR (
    _In_ _Notliteral_ volatile UCHAR *Register
    )
{

    ARM64_PREVENT_REGISTER_WRITEBACK(UCHAR, Register);
    return ReadUCharNoFence(Register);
}

__forceinline
USHORT
READ_REGISTER_NOFENCE_USHORT (
    _In_ _Notliteral_ volatile USHORT *Register
    )
{

    ARM64_PREVENT_REGISTER_WRITEBACK(USHORT, Register);
    return ReadUShortNoFence(Register);
}

__forceinline
ULONG
READ_REGISTER_NOFENCE_ULONG (
    _In_ _Notliteral_ volatile ULONG *Register
    )
{

    ARM64_PREVENT_REGISTER_WRITEBACK(ULONG, Register);
    return ReadULongNoFence(Register);
}

__forceinline
ULONG64
READ_REGISTER_NOFENCE_ULONG64 (
    _In_ _Notliteral_ volatile ULONG64 *Register
    )
{

    ARM64_PREVENT_REGISTER_WRITEBACK(ULONG64, Register);
    return ReadULong64NoFence(Register);
}

__forceinline
VOID
READ_REGISTER_NOFENCE_BUFFER_UCHAR (
    _In_reads_(Count) _Notliteral_ volatile UCHAR *Register,
    _Out_writes_all_(Count) PUCHAR Buffer,
    _In_ ULONG Count
    )
{

    volatile UCHAR *registerBuffer =  Register;
    PUCHAR readBuffer = Buffer;
    ULONG readCount;

    for (readCount = Count; readCount--; readBuffer++, registerBuffer++) {
        ARM64_PREVENT_REGISTER_WRITEBACK(UCHAR, registerBuffer);
        *readBuffer = ReadUCharNoFence(registerBuffer);
    }


    return;
}

__forceinline
VOID
READ_REGISTER_NOFENCE_BUFFER_USHORT (
    _In_reads_(Count) _Notliteral_ volatile USHORT *Register,
    _Out_writes_all_(Count) PUSHORT Buffer,
    _In_ ULONG Count
    )
{
    volatile USHORT *registerBuffer =  Register;
    PUSHORT readBuffer = Buffer;
    ULONG readCount;

    for (readCount = Count; readCount--; readBuffer++, registerBuffer++) {
        ARM64_PREVENT_REGISTER_WRITEBACK(USHORT, registerBuffer);
        *readBuffer = ReadUShortNoFence(registerBuffer);
    }

    return;
}

__forceinline
VOID
READ_REGISTER_NOFENCE_BUFFER_ULONG (
    _In_reads_(Count) _Notliteral_ volatile ULONG *Register,
    _Out_writes_all_(Count) PULONG Buffer,
    _In_ ULONG Count
    )
{
    volatile ULONG *registerBuffer =  Register;
    PULONG readBuffer = Buffer;
    ULONG readCount;

    for (readCount = Count; readCount--; readBuffer++, registerBuffer++) {
        ARM64_PREVENT_REGISTER_WRITEBACK(ULONG, registerBuffer);
        *readBuffer = ReadULongNoFence(registerBuffer);
    }
    return;
}

__forceinline
VOID
READ_REGISTER_NOFENCE_BUFFER_ULONG64 (
    _In_reads_(Count) _Notliteral_ volatile ULONG64 *Register,
    _Out_writes_all_(Count) PULONG64 Buffer,
    _In_ ULONG Count
    )
{
    volatile ULONG64 *registerBuffer =  Register;
    PULONG64 readBuffer = Buffer;
    ULONG readCount;

    for (readCount = Count; readCount--; readBuffer++, registerBuffer++) {
        ARM64_PREVENT_REGISTER_WRITEBACK(ULONG64, registerBuffer);
        *readBuffer = ReadULong64NoFence(registerBuffer);
    }
    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_UCHAR (
    _In_ _Notliteral_ volatile UCHAR *Register,
    _In_ UCHAR Value
    )
{

    ARM64_PREVENT_REGISTER_WRITEBACK(UCHAR, Register);
    WriteUCharNoFence(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_USHORT (
    _In_ _Notliteral_ volatile USHORT *Register,
    _In_ USHORT Value
    )
{

    ARM64_PREVENT_REGISTER_WRITEBACK(USHORT, Register);
    WriteUShortNoFence(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_ULONG (
    _In_ _Notliteral_ volatile ULONG *Register,
    _In_ ULONG Value
    )
{

    ARM64_PREVENT_REGISTER_WRITEBACK(ULONG, Register);
    WriteULongNoFence(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_ULONG64 (
    _In_ _Notliteral_ volatile ULONG64 *Register,
    _In_ ULONG64 Value
    )
{

    ARM64_PREVENT_REGISTER_WRITEBACK(ULONG64, Register);
    WriteULong64NoFence(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_BUFFER_UCHAR (
    _Out_writes_(Count) _Notliteral_ volatile UCHAR *Register,
    _In_reads_(Count) PUCHAR Buffer,
    _In_ ULONG Count
    )
{

    volatile UCHAR *registerBuffer = Register;
    PUCHAR writeBuffer = Buffer;
    ULONG writeCount;

    for (writeCount = Count; writeCount--; writeBuffer++, registerBuffer++) {
        ARM64_PREVENT_REGISTER_WRITEBACK(UCHAR, registerBuffer);
        WriteUCharNoFence(registerBuffer, *writeBuffer);
    }

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_BUFFER_USHORT (
    _Out_writes_(Count) _Notliteral_ volatile USHORT *Register,
    _In_reads_(Count) PUSHORT Buffer,
    _In_ ULONG Count
    )
{

    volatile USHORT *registerBuffer = Register;
    PUSHORT writeBuffer = Buffer;
    ULONG writeCount;

    for (writeCount = Count; writeCount--; writeBuffer++, registerBuffer++) {
        ARM64_PREVENT_REGISTER_WRITEBACK(USHORT, registerBuffer);
        WriteUShortNoFence(registerBuffer, *writeBuffer);
    }

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_BUFFER_ULONG (
    _Out_writes_(Count) _Notliteral_ volatile ULONG *Register,
    _In_reads_(Count) PULONG Buffer,
    _In_ ULONG Count
    )
{

    volatile ULONG *registerBuffer = Register;
    PULONG writeBuffer = Buffer;
    ULONG writeCount;

    for (writeCount = Count; writeCount--; writeBuffer++, registerBuffer++) {
        ARM64_PREVENT_REGISTER_WRITEBACK(ULONG, registerBuffer);
        WriteULongNoFence(registerBuffer, *writeBuffer);
    }

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_BUFFER_ULONG64 (
    _Out_writes_(Count) _Notliteral_ volatile ULONG64 *Register,
    _In_reads_(Count) PULONG64 Buffer,
    _In_ ULONG Count
    )
{

    volatile ULONG64 *registerBuffer = Register;
    PULONG64 writeBuffer = Buffer;
    ULONG writeCount;

    for (writeCount = Count; writeCount--; writeBuffer++, registerBuffer++) {
        ARM64_PREVENT_REGISTER_WRITEBACK(ULONG64, registerBuffer);
        WriteULong64NoFence(registerBuffer, *writeBuffer);
    }

    return;
}

__forceinline
VOID
REGISTER_FENCE (
    VOID
    )
{

    _DataSynchronizationBarrier();
}

__forceinline
UCHAR
READ_REGISTER_UCHAR (
    _In_ _Notliteral_ volatile UCHAR *Register
    )
{
    UCHAR Value;

    _DataSynchronizationBarrier();
    Value = READ_REGISTER_NOFENCE_UCHAR(Register);

    return Value;
}

__forceinline
USHORT
READ_REGISTER_USHORT (
    _In_ _Notliteral_ volatile USHORT *Register
    )
{
    USHORT Value;

    _DataSynchronizationBarrier();
    Value = READ_REGISTER_NOFENCE_USHORT(Register);

    return Value;
}

__forceinline
ULONG
READ_REGISTER_ULONG (
    _In_ _Notliteral_ volatile ULONG *Register
    )
{
    ULONG Value;

    _DataSynchronizationBarrier();
    Value = READ_REGISTER_NOFENCE_ULONG(Register);

    return Value;
}

__forceinline
ULONG64
READ_REGISTER_ULONG64 (
    _In_ _Notliteral_ volatile ULONG64 *Register
    )
{
    ULONG64 Value;

    _DataSynchronizationBarrier();
    Value = READ_REGISTER_NOFENCE_ULONG64(Register);

    return Value;
}

__forceinline
VOID
READ_REGISTER_BUFFER_UCHAR (
    _In_reads_(Count) _Notliteral_ volatile UCHAR *Register,
    _Out_writes_all_(Count) PUCHAR Buffer,
    _In_ ULONG Count
    )
{
    _DataSynchronizationBarrier();
    READ_REGISTER_NOFENCE_BUFFER_UCHAR(Register, Buffer, Count);

    return;
}

__forceinline
VOID
READ_REGISTER_BUFFER_USHORT (
    _In_reads_(Count) _Notliteral_ volatile USHORT *Register,
    _Out_writes_all_(Count) PUSHORT Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    READ_REGISTER_NOFENCE_BUFFER_USHORT(Register, Buffer, Count);

    return;
}

__forceinline
VOID
READ_REGISTER_BUFFER_ULONG (
    _In_reads_(Count) _Notliteral_ volatile ULONG *Register,
    _Out_writes_all_(Count) PULONG Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    READ_REGISTER_NOFENCE_BUFFER_ULONG(Register, Buffer, Count);

    return;
}

__forceinline
VOID
READ_REGISTER_BUFFER_ULONG64 (
    _In_reads_(Count) _Notliteral_ volatile ULONG64 *Register,
    _Out_writes_all_(Count) PULONG64 Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    READ_REGISTER_NOFENCE_BUFFER_ULONG64(Register, Buffer, Count);

    return;
}

__forceinline
VOID
WRITE_REGISTER_UCHAR (
    _In_ _Notliteral_ volatile UCHAR *Register,
    _In_ UCHAR Value
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_UCHAR(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_USHORT (
    _In_ _Notliteral_ volatile USHORT *Register,
    _In_ USHORT Value
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_USHORT(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_ULONG (
    _In_ _Notliteral_ volatile ULONG *Register,
    _In_ ULONG Value
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_ULONG(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_ULONG64 (
    _In_ _Notliteral_ volatile ULONG64 *Register,
    _In_ ULONG64 Value
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_ULONG64(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_UCHAR (
    _Out_writes_(Count) _Notliteral_ volatile UCHAR *Register,
    _In_reads_(Count) PUCHAR Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_BUFFER_UCHAR(Register, Buffer, Count);

    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_USHORT (
    _Out_writes_(Count) _Notliteral_ volatile USHORT *Register,
    _In_reads_(Count) PUSHORT Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_BUFFER_USHORT(Register, Buffer, Count);

    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_ULONG (
    _Out_writes_(Count) _Notliteral_ volatile ULONG *Register,
    _In_reads_(Count) PULONG Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_BUFFER_ULONG(Register, Buffer, Count);

    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_ULONG64 (
    _Out_writes_(Count) _Notliteral_ volatile ULONG64 *Register,
    _In_reads_(Count) PULONG64 Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_BUFFER_ULONG64(Register, Buffer, Count);

    return;
}

// end_halextenv


// begin_halextenv

#ifdef __cplusplus
}
#endif

#endif

// end_halextenv


// begin_halextenv

#if defined(_ARM_)

//
// I/O space read and write macros.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space.
//
//  N.B. This implementation assumes that the memory mapped registers
//       have been mapped using the OS concept of uncached memory
//       which is implemented using the ARMv7 strongly ordered memory
//       type.  In addition, the register access is bracketed by a
//       compiler barrier to ensure that the compiler does not
//       re-order the I/O accesses with other accesses and a data
//       synchronization barrier to ensure that any side effects of
//       the access have started (but not necessarily completed).
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT
//  space.  The ARM architecture doesn't have a separate I/O space.
//  These operations bugcheck so as to identify incorrect code.
//

#ifdef __cplusplus
extern "C" {
#endif

__forceinline
UCHAR
READ_REGISTER_NOFENCE_UCHAR (
    _In_ _Notliteral_ volatile UCHAR *Register
    )
{

    return ReadUCharNoFence(Register);
}

__forceinline
USHORT
READ_REGISTER_NOFENCE_USHORT (
    _In_ _Notliteral_ volatile USHORT *Register
    )
{

    return ReadUShortNoFence(Register);
}

__forceinline
ULONG
READ_REGISTER_NOFENCE_ULONG (
    _In_ _Notliteral_ volatile ULONG *Register
    )
{

    return ReadULongNoFence(Register);
}

__forceinline
VOID
READ_REGISTER_NOFENCE_BUFFER_UCHAR (
    _In_reads_(Count) _Notliteral_ volatile UCHAR *Register,
    _Out_writes_all_(Count) PUCHAR Buffer,
    _In_ ULONG Count
    )
{

    volatile UCHAR *registerBuffer =  Register;
    PUCHAR readBuffer = Buffer;
    ULONG readCount;

    for (readCount = Count; readCount--; readBuffer++, registerBuffer++) {
        *readBuffer = ReadUCharNoFence(registerBuffer);
    }


    return;
}

__forceinline
VOID
READ_REGISTER_NOFENCE_BUFFER_USHORT (
    _In_reads_(Count) _Notliteral_ volatile USHORT *Register,
    _Out_writes_all_(Count) PUSHORT Buffer,
    _In_ ULONG Count
    )
{
    volatile USHORT *registerBuffer =  Register;
    PUSHORT readBuffer = Buffer;
    ULONG readCount;

    for (readCount = Count; readCount--; readBuffer++, registerBuffer++) {
        *readBuffer = ReadUShortNoFence(registerBuffer);
    }

    return;
}

__forceinline
VOID
READ_REGISTER_NOFENCE_BUFFER_ULONG (
    _In_reads_(Count) _Notliteral_ volatile ULONG *Register,
    _Out_writes_all_(Count) PULONG Buffer,
    _In_ ULONG Count
    )
{
    volatile ULONG *registerBuffer =  Register;
    PULONG readBuffer = Buffer;
    ULONG readCount;

    for (readCount = Count; readCount--; readBuffer++, registerBuffer++) {
        *readBuffer = ReadULongNoFence(registerBuffer);
    }
    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_UCHAR (
    _In_ _Notliteral_ volatile UCHAR *Register,
    _In_ UCHAR Value
    )
{

    WriteUCharNoFence(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_USHORT (
    _In_ _Notliteral_ volatile USHORT *Register,
    _In_ USHORT Value
    )
{

    WriteUShortNoFence(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_ULONG (
    _In_ _Notliteral_ volatile ULONG *Register,
    _In_ ULONG Value
    )
{

    WriteULongNoFence(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_BUFFER_UCHAR (
    _Out_writes_(Count) _Notliteral_ volatile UCHAR *Register,
    _In_reads_(Count) PUCHAR Buffer,
    _In_ ULONG Count
    )
{

    volatile UCHAR *registerBuffer = Register;
    PUCHAR writeBuffer = Buffer;
    ULONG writeCount;

    for (writeCount = Count; writeCount--; writeBuffer++, registerBuffer++) {
        WriteUCharNoFence(registerBuffer, *writeBuffer);
    }

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_BUFFER_USHORT (
    _Out_writes_(Count) _Notliteral_ volatile USHORT *Register,
    _In_reads_(Count) PUSHORT Buffer,
    _In_ ULONG Count
    )
{

    volatile USHORT *registerBuffer = Register;
    PUSHORT writeBuffer = Buffer;
    ULONG writeCount;

    for (writeCount = Count; writeCount--; writeBuffer++, registerBuffer++) {
        WriteUShortNoFence(registerBuffer, *writeBuffer);
    }

    return;
}

__forceinline
VOID
WRITE_REGISTER_NOFENCE_BUFFER_ULONG (
    _Out_writes_(Count) _Notliteral_ volatile ULONG *Register,
    _In_reads_(Count) PULONG Buffer,
    _In_ ULONG Count
    )
{

    volatile ULONG *registerBuffer = Register;
    PULONG writeBuffer = Buffer;
    ULONG writeCount;

    for (writeCount = Count; writeCount--; writeBuffer++, registerBuffer++) {
        WriteULongNoFence(registerBuffer, *writeBuffer);
    }

    return;
}

__forceinline
VOID
REGISTER_FENCE (
    VOID
    )
{

    _DataSynchronizationBarrier();
}

__forceinline
UCHAR
READ_REGISTER_UCHAR (
    _In_ _Notliteral_ volatile UCHAR *Register
    )
{
    UCHAR Value;

    _DataSynchronizationBarrier();
    Value = READ_REGISTER_NOFENCE_UCHAR(Register);

    return Value;
}

__forceinline
USHORT
READ_REGISTER_USHORT (
    _In_ _Notliteral_ volatile USHORT *Register
    )
{
    USHORT Value;

    _DataSynchronizationBarrier();
    Value = READ_REGISTER_NOFENCE_USHORT(Register);

    return Value;
}

__forceinline
ULONG
READ_REGISTER_ULONG (
    _In_ _Notliteral_ volatile ULONG *Register
    )
{
    ULONG Value;

    _DataSynchronizationBarrier();
    Value = READ_REGISTER_NOFENCE_ULONG(Register);

    return Value;
}

__forceinline
VOID
READ_REGISTER_BUFFER_UCHAR (
    _In_reads_(Count) _Notliteral_ volatile UCHAR *Register,
    _Out_writes_all_(Count) PUCHAR Buffer,
    _In_ ULONG Count
    )
{
    _DataSynchronizationBarrier();
    READ_REGISTER_NOFENCE_BUFFER_UCHAR(Register, Buffer, Count);

    return;
}

__forceinline
VOID
READ_REGISTER_BUFFER_USHORT (
    _In_reads_(Count) _Notliteral_ volatile USHORT *Register,
    _Out_writes_all_(Count) PUSHORT Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    READ_REGISTER_NOFENCE_BUFFER_USHORT(Register, Buffer, Count);

    return;
}

__forceinline
VOID
READ_REGISTER_BUFFER_ULONG (
    _In_reads_(Count) _Notliteral_ volatile ULONG *Register,
    _Out_writes_all_(Count) PULONG Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    READ_REGISTER_NOFENCE_BUFFER_ULONG(Register, Buffer, Count);

    return;
}

__forceinline
VOID
WRITE_REGISTER_UCHAR (
    _In_ _Notliteral_ volatile UCHAR *Register,
    _In_ UCHAR Value
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_UCHAR(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_USHORT (
    _In_ _Notliteral_ volatile USHORT *Register,
    _In_ USHORT Value
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_USHORT(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_ULONG (
    _In_ _Notliteral_ volatile ULONG *Register,
    _In_ ULONG Value
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_ULONG(Register, Value);

    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_UCHAR (
    _Out_writes_(Count) _Notliteral_ volatile UCHAR *Register,
    _In_reads_(Count) PUCHAR Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_BUFFER_UCHAR(Register, Buffer, Count);

    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_USHORT (
    _Out_writes_(Count) _Notliteral_ volatile USHORT *Register,
    _In_reads_(Count) PUSHORT Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_BUFFER_USHORT(Register, Buffer, Count);

    return;
}

__forceinline
VOID
WRITE_REGISTER_BUFFER_ULONG (
    _Out_writes_(Count) _Notliteral_ volatile ULONG *Register,
    _In_reads_(Count) PULONG Buffer,
    _In_ ULONG Count
    )
{

    _DataSynchronizationBarrier();
    WRITE_REGISTER_NOFENCE_BUFFER_ULONG(Register, Buffer, Count);

    return;
}

// end_halextenv
// begin_halextenv

#ifdef __cplusplus
}
#endif

#endif

// end_halextenv
#if defined(_X86_) 

// begin_halextenv

//
// I/O space read and write macros.
//
//  These have to be actual functions on the 386, because we need
//  to use assembler, but cannot return a value if we inline it.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space.
//  (Use x86 move instructions, with LOCK prefix to force correct behavior
//   w.r.t. caches and write buffers.)
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT space.
//  (Use x86 in/out instructions.)
//

NTKERNELAPI
UCHAR
NTAPI
READ_REGISTER_UCHAR(
    _In_ _Notliteral_ volatile UCHAR *Register
    );

NTKERNELAPI
USHORT
NTAPI
READ_REGISTER_USHORT(
    _In_ _Notliteral_ volatile USHORT *Register
    );

NTKERNELAPI
ULONG
NTAPI
READ_REGISTER_ULONG(
    _In_ _Notliteral_ volatile ULONG *Register
    );

NTKERNELAPI
VOID
NTAPI
READ_REGISTER_BUFFER_UCHAR(
    _In_ _Notliteral_ volatile UCHAR *Register,
    _Out_writes_all_(Count) PUCHAR  Buffer,
    _In_ ULONG   Count
    );

NTKERNELAPI
VOID
NTAPI
READ_REGISTER_BUFFER_USHORT(
    _In_ _Notliteral_ volatile USHORT *Register,
    _Out_writes_all_(Count) PUSHORT Buffer,
    _In_ ULONG   Count
    );

NTKERNELAPI
VOID
NTAPI
READ_REGISTER_BUFFER_ULONG(
    _In_ _Notliteral_ volatile ULONG *Register,
    _Out_writes_all_(Count) PULONG  Buffer,
    _In_ ULONG   Count
    );


NTKERNELAPI
VOID
NTAPI
WRITE_REGISTER_UCHAR(
    _In_ _Notliteral_ volatile UCHAR *Register,
    _In_ UCHAR   Value
    );

NTKERNELAPI
VOID
NTAPI
WRITE_REGISTER_USHORT(
    _In_ _Notliteral_ volatile USHORT *Register,
    _In_ USHORT  Value
    );

NTKERNELAPI
VOID
NTAPI
WRITE_REGISTER_ULONG(
    _In_ _Notliteral_ volatile ULONG *Register,
    _In_ ULONG   Value
    );

NTKERNELAPI
VOID
NTAPI
WRITE_REGISTER_BUFFER_UCHAR(
    _In_ _Notliteral_ volatile UCHAR *Register,
    _In_reads_(Count) PUCHAR  Buffer,
    _In_ ULONG   Count
    );

NTKERNELAPI
VOID
NTAPI
WRITE_REGISTER_BUFFER_USHORT(
    _In_ _Notliteral_ volatile USHORT *Register,
    _In_reads_(Count) PUSHORT Buffer,
    _In_ ULONG   Count
    );

NTKERNELAPI
VOID
NTAPI
WRITE_REGISTER_BUFFER_ULONG(
    _In_ _Notliteral_ volatile ULONG *Register,
    _In_reads_(Count) PULONG  Buffer,
    _In_ ULONG   Count
    );

// end_halextenv
#endif //_X86_  


#if _MSC_VER >= 1610

//++
//VOID
//RtlFailFast (
//    _In_ ULONG Code
//    );
//
// Routine Description:
//
//    This routine brings down the caller immediately in the event that
//    critical corruption has been detected.  No exception handlers are
//    invoked.
//
//    The routine may be used in libraries shared with user mode and
//    kernel mode.  In user mode, the process is terminated, whereas in
//    kernel mode, a KERNEL_SECURITY_CHECK_FAILURE bug check is raised.
//
// Arguments
//
//    Code - Supplies the reason code describing what type of corruption
//           was detected.
//
// Return Value:
//
//    None.  There is no return from this routine.
//
//--

DECLSPEC_NORETURN
FORCEINLINE
VOID
RtlFailFast(
    _In_ ULONG Code
    )

{

    __fastfail(Code);
}

#endif // _MSC_VER

//
// The __fastfail intrinsic is only available for compilation to native code.
// Pure and safe managed code may not reference it in compilation (though such
// code should not invoke the LIST_ENTRY forceinlines anyway).
//

#if !defined(_MSC_FULL_VER) || (_MSC_FULL_VER < 161030716) || defined(_M_CEE_PURE) || defined(_M_CEE_SAFE)

#if !defined(NO_KERNEL_LIST_ENTRY_CHECKS)
#define NO_KERNEL_LIST_ENTRY_CHECKS
#endif

#endif

//
//  Doubly-linked list manipulation routines.
//

//
//  VOID
//  InitializeListHead32(
//      PLIST_ENTRY32 ListHead
//      );
//

#define InitializeListHead32(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = PtrToUlong((ListHead)))

#define RTL_STATIC_LIST_HEAD(x) LIST_ENTRY x = { &x, &x }

FORCEINLINE
VOID
InitializeListHead(
    _Out_ PLIST_ENTRY ListHead
    )

{

    ListHead->Flink = ListHead->Blink = ListHead;
    return;
}

_Must_inspect_result_
BOOLEAN
CFORCEINLINE
IsListEmpty(
    _In_ const LIST_ENTRY * ListHead
    )

{

    return (BOOLEAN)(ListHead->Flink == ListHead);
}

FORCEINLINE
BOOLEAN
RemoveEntryListUnsafe(
    _In_ PLIST_ENTRY Entry
    )

{

    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (BOOLEAN)(Flink == Blink);
}

#if defined(NO_KERNEL_LIST_ENTRY_CHECKS)

FORCEINLINE
BOOLEAN
RemoveEntryList(
    _In_ PLIST_ENTRY Entry
    )

{

    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (BOOLEAN)(Flink == Blink);
}

FORCEINLINE
PLIST_ENTRY
RemoveHeadList(
    _Inout_ PLIST_ENTRY ListHead
    )

{

    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;
    return Entry;
}



FORCEINLINE
PLIST_ENTRY
RemoveTailList(
    _Inout_ PLIST_ENTRY ListHead
    )

{

    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;
    return Entry;
}


FORCEINLINE
VOID
InsertTailList(
    _Inout_ PLIST_ENTRY ListHead,
    _Inout_ __drv_aliasesMem PLIST_ENTRY Entry
    )
{

    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
    return;
}


FORCEINLINE
VOID
InsertHeadList(
    _Inout_ PLIST_ENTRY ListHead,
    _Inout_ __drv_aliasesMem PLIST_ENTRY Entry
    )
{

    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
    return;
}

FORCEINLINE
VOID
AppendTailList(
    _Inout_ PLIST_ENTRY ListHead,
    _Inout_ PLIST_ENTRY ListToAppend
    )
{

    PLIST_ENTRY ListEnd = ListHead->Blink;

    ListHead->Blink->Flink = ListToAppend;
    ListHead->Blink = ListToAppend->Blink;
    ListToAppend->Blink->Flink = ListHead;
    ListToAppend->Blink = ListEnd;
    return;
}

#else // NO_KERNEL_LIST_ENTRY_CHECKS

//++
//VOID
//FatalListEntryError (
//    _In_ PVOID p1,
//    _In_ PVOID p2,
//    _In_ PVOID p3
//    );
//
// Routine Description:
//
//    This routine reports a fatal list entry error.  It is implemented here as a
//    wrapper around RtlFailFast so that alternative reporting mechanisms (such
//    as simply logging and trying to continue) can be easily switched in.
//
// Arguments:
//
//    p1 - Supplies the first failure parameter.
//
//    p2 - Supplies the second failure parameter.
//
//    p3 - Supplies the third failure parameter.
//
//Return Value:
//
//    None.
//--

FORCEINLINE
VOID
FatalListEntryError(
    _In_ PVOID p1,
    _In_ PVOID p2,
    _In_ PVOID p3
    )

{

    UNREFERENCED_PARAMETER(p1);
    UNREFERENCED_PARAMETER(p2);
    UNREFERENCED_PARAMETER(p3);

    RtlFailFast(FAST_FAIL_CORRUPT_LIST_ENTRY);
}

FORCEINLINE
VOID
RtlpCheckListEntry(
    _In_ PLIST_ENTRY Entry
    )

{

    if ((((Entry->Flink)->Blink) != Entry) || (((Entry->Blink)->Flink) != Entry)) {
        FatalListEntryError((PVOID)(Entry),
                            (PVOID)((Entry->Flink)->Blink),
                            (PVOID)((Entry->Blink)->Flink));
    }
}


FORCEINLINE
BOOLEAN
RemoveEntryList(
    _In_ PLIST_ENTRY Entry
    )

{

    PLIST_ENTRY PrevEntry;
    PLIST_ENTRY NextEntry;

    NextEntry = Entry->Flink;
    PrevEntry = Entry->Blink;
    if ((NextEntry->Blink != Entry) || (PrevEntry->Flink != Entry)) {
        FatalListEntryError((PVOID)PrevEntry,
                            (PVOID)Entry,
                            (PVOID)NextEntry);
    }

    PrevEntry->Flink = NextEntry;
    NextEntry->Blink = PrevEntry;
    return (BOOLEAN)(PrevEntry == NextEntry);
}

FORCEINLINE
PLIST_ENTRY
RemoveHeadList(
    _Inout_ PLIST_ENTRY ListHead
    )

{

    PLIST_ENTRY Entry;
    PLIST_ENTRY NextEntry;

    Entry = ListHead->Flink;

#if DBG

    RtlpCheckListEntry(ListHead);

#endif

    NextEntry = Entry->Flink;
    if ((Entry->Blink != ListHead) || (NextEntry->Blink != Entry)) {
        FatalListEntryError((PVOID)ListHead,
                            (PVOID)Entry,
                            (PVOID)NextEntry);
    }

    ListHead->Flink = NextEntry;
    NextEntry->Blink = ListHead;

    return Entry;
}

FORCEINLINE
PLIST_ENTRY
RemoveTailList(
    _Inout_ PLIST_ENTRY ListHead
    )
{

    PLIST_ENTRY Entry;
    PLIST_ENTRY PrevEntry;

    Entry = ListHead->Blink;

#if DBG

    RtlpCheckListEntry(ListHead);

#endif

    PrevEntry = Entry->Blink;
    if ((Entry->Flink != ListHead) || (PrevEntry->Flink != Entry)) {
        FatalListEntryError((PVOID)PrevEntry,
                            (PVOID)Entry,
                            (PVOID)ListHead);
    }

    ListHead->Blink = PrevEntry;
    PrevEntry->Flink = ListHead;
    return Entry;
}


FORCEINLINE
VOID
InsertTailList(
    _Inout_ PLIST_ENTRY ListHead,
    _Out_ __drv_aliasesMem PLIST_ENTRY Entry
    )
{

    PLIST_ENTRY PrevEntry;

#if DBG

    RtlpCheckListEntry(ListHead);

#endif

    PrevEntry = ListHead->Blink;
    if (PrevEntry->Flink != ListHead) {
        FatalListEntryError((PVOID)PrevEntry,
                            (PVOID)ListHead,
                            (PVOID)PrevEntry->Flink);
    }

    Entry->Flink = ListHead;
    Entry->Blink = PrevEntry;
    PrevEntry->Flink = Entry;
    ListHead->Blink = Entry;
    return;
}


FORCEINLINE
VOID
InsertHeadList(
    _Inout_ PLIST_ENTRY ListHead,
    _Out_ __drv_aliasesMem PLIST_ENTRY Entry
    )

{

    PLIST_ENTRY NextEntry;

#if DBG

    RtlpCheckListEntry(ListHead);

#endif

    NextEntry = ListHead->Flink;
    if (NextEntry->Blink != ListHead) {
        FatalListEntryError((PVOID)ListHead,
                            (PVOID)NextEntry,
                            (PVOID)NextEntry->Blink);
    }

    Entry->Flink = NextEntry;
    Entry->Blink = ListHead;
    NextEntry->Blink = Entry;
    ListHead->Flink = Entry;
    return;
}

FORCEINLINE
VOID
AppendTailList(
    _Inout_ PLIST_ENTRY ListHead,
    _Inout_ PLIST_ENTRY ListToAppend
    )
{
    PLIST_ENTRY ListEnd = ListHead->Blink;

    RtlpCheckListEntry(ListHead);
    RtlpCheckListEntry(ListToAppend);
    ListHead->Blink->Flink = ListToAppend;
    ListHead->Blink = ListToAppend->Blink;
    ListToAppend->Blink->Flink = ListHead;
    ListToAppend->Blink = ListEnd;
    return;
}

#endif // NO_KERNEL_LIST_ENTRY_CHECKS



FORCEINLINE
PSINGLE_LIST_ENTRY
PopEntryList(
    _Inout_ PSINGLE_LIST_ENTRY ListHead
    )
{

    PSINGLE_LIST_ENTRY FirstEntry;

    FirstEntry = ListHead->Next;
    if (FirstEntry != NULL) {
        ListHead->Next = FirstEntry->Next;
    }

    return FirstEntry;
}


FORCEINLINE
VOID
PushEntryList(
    _Inout_ PSINGLE_LIST_ENTRY ListHead,
    _Inout_ __drv_aliasesMem PSINGLE_LIST_ENTRY Entry
    )

{

    Entry->Next = ListHead->Next;
    ListHead->Next = Entry;
    return;
}


#if (NTDDI_VERSION >= NTDDI_WIN2K)
_IRQL_requires_max_(PASSIVE_LEVEL)
_At_(String->MaximumLength, _Const_)
NTSYSAPI
NTSTATUS
NTAPI
RtlIntegerToUnicodeString (
    _In_ ULONG Value,
    _In_opt_ ULONG Base,
    _Inout_ PUNICODE_STRING String
    );
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
_IRQL_requires_max_(PASSIVE_LEVEL)
_At_(String->MaximumLength, _Const_)
NTSYSAPI
NTSTATUS
NTAPI
RtlInt64ToUnicodeString (
    _In_ ULONGLONG Value,
    _In_opt_ ULONG Base,
    _Inout_ PUNICODE_STRING String
    );
#endif

#ifdef _WIN64
#define RtlIntPtrToUnicodeString(Value, Base, String) RtlInt64ToUnicodeString(Value, Base, String)
#else
#define RtlIntPtrToUnicodeString(Value, Base, String) RtlIntegerToUnicodeString(Value, Base, String)
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToInteger (
    _In_ PCUNICODE_STRING String,
    _In_opt_ ULONG Base,
    _Out_ PULONG Value
    );
#endif

#if !defined(BLDR_KERNEL_RUNTIME)
NTSTATUS
RtlUnicodeStringToInt64 (
    _In_ PCUNICODE_STRING String,
    _In_opt_ ULONG Base,
    _Out_ PLONG64 Number,
    _Out_opt_ PWSTR *EndPointer
    );
#endif


//@[comment("MVI_tracked")]
_IRQL_requires_max_(DISPATCH_LEVEL)
_At_(DestinationString->Buffer, _Post_equal_to_(SourceString))
_At_(DestinationString->Length, _Post_equal_to_(_String_length_(SourceString) * sizeof(WCHAR)))
_At_(DestinationString->MaximumLength, _Post_equal_to_((_String_length_(SourceString)+1) * sizeof(WCHAR)))
NTSYSAPI
VOID
NTAPI
RtlInitUnicodeString(
    _Out_ PUNICODE_STRING DestinationString,
    _In_opt_z_ __drv_aliasesMem PCWSTR SourceString
    );



_At_(UnicodeString->Buffer, _Post_equal_to_(Buffer))
_At_(UnicodeString->Length, _Post_equal_to_(0))
_At_(UnicodeString->MaximumLength, _Post_equal_to_(BufferSize))
FORCEINLINE
VOID
RtlInitEmptyUnicodeString(
    _Out_ PUNICODE_STRING UnicodeString,
    _Writable_bytes_(BufferSize)
    _When_(BufferSize != 0, _Notnull_)
    __drv_aliasesMem PWCHAR Buffer,
    _In_ USHORT BufferSize
    )
{
    memset(UnicodeString, 0, sizeof(*UnicodeString));
    UnicodeString->MaximumLength = BufferSize;
    UnicodeString->Buffer = Buffer;
}


#if (NTDDI_VERSION >= NTDDI_WIN2K)
_IRQL_requires_max_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSYSAPI
LONG
NTAPI
RtlCompareUnicodeStrings(
    _In_reads_(String1Length) PCWCH String1,
    _In_ SIZE_T String1Length,
    _In_reads_(String2Length) PCWCH String2,
    _In_ SIZE_T String2Length,
    _In_ BOOLEAN CaseInSensitive
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSYSAPI
LONG
NTAPI
RtlCompareUnicodeString(
    _In_ PCUNICODE_STRING String1,
    _In_ PCUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
    );
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
//@[comment("MVI_tracked")]
_IRQL_requires_max_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSYSAPI
BOOLEAN
NTAPI
RtlEqualUnicodeString(
    _In_ PCUNICODE_STRING String1,
    _In_ PCUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
    );
#endif


#if (NTDDI_VERSION >= NTDDI_WIN2K)
_Unchanged_(DestinationString->Buffer)
_Unchanged_(DestinationString->MaximumLength)
_At_(DestinationString->Length,
    _When_(SourceString->Length > DestinationString->MaximumLength,
        _Post_equal_to_(DestinationString->MaximumLength))
    _When_(SourceString->Length <= DestinationString->MaximumLength,
        _Post_equal_to_(SourceString->Length)))
NTSYSAPI
VOID
NTAPI
RtlCopyUnicodeString(
    _Inout_ PUNICODE_STRING DestinationString,
    _In_opt_ PCUNICODE_STRING SourceString
    );
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
_Success_(1)
_Unchanged_(Destination->MaximumLength)
_Unchanged_(Destination->Buffer)
_When_(_Old_(Destination->Length) + Source->Length <= Destination->MaximumLength,
    _At_(Destination->Length,
         _Post_equal_to_(_Old_(Destination->Length) + Source->Length))
    _At_(return, _Out_range_(==, 0)))
_When_(_Old_(Destination->Length) + Source->Length > Destination->MaximumLength,
    _Unchanged_(Destination->Length)
    _At_(return, _Out_range_(<, 0)))
NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeStringToString (
    _Inout_ PUNICODE_STRING Destination,
    _In_ PCUNICODE_STRING Source
    );
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
_Success_(1)
_Unchanged_(Destination->MaximumLength)
_Unchanged_(Destination->Buffer)
_When_(_Old_(Destination->Length) + _String_length_(Source) * sizeof(WCHAR) <= Destination->MaximumLength,
    _At_(Destination->Length,
         _Post_equal_to_(_Old_(Destination->Length) + _String_length_(Source) * sizeof(WCHAR)))
    _At_(return, _Out_range_(==, 0)))
_When_(_Old_(Destination->Length) + _String_length_(Source) * sizeof(WCHAR) > Destination->MaximumLength,
    _Unchanged_(Destination->Length)
    _At_(return, _Out_range_(<, 0)))
NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeToString (
    _Inout_ PUNICODE_STRING Destination,
    _In_opt_z_ PCWSTR Source
    );
#endif



_IRQL_requires_max_(PASSIVE_LEVEL)
NTSYSAPI
VOID
NTAPI
RtlFreeUnicodeString(
    _Inout_ _At_(UnicodeString->Buffer, _Frees_ptr_opt_)
        PUNICODE_STRING UnicodeString
    );


#if (NTDDI_VERSION >= NTDDI_WIN2K)

#if (_MSC_FULL_VER >= 150030729) && !defined(IMPORT_NATIVE_DBG_BREAK)

#define DbgBreakPoint __debugbreak

#else

__analysis_noreturn
VOID
NTAPI
DbgBreakPoint(
    VOID
    );

#endif

#endif

#if DBG 
#define KdPrintEx(_x_) DbgPrintEx _x_ 
#define KdBreakPoint() DbgBreakPoint() 
#else 
#define KdPrintEx(_x_) 
#define KdBreakPoint() 
#endif 

#if (NTDDI_VERSION >= NTDDI_WINXP)
NTSYSAPI
ULONG
__cdecl
DbgPrintEx (
    _In_ ULONG ComponentId,
    _In_ ULONG Level,
    _In_z_ _Printf_format_string_ PCSTR Format,
    ...
    );
#endif

#if (NTDDI_VERSION >= NTDDI_WINXP)
#include <dpfilter.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

//
// Assert exception.
//

#if !defined(_DBGRAISEASSERTIONFAILURE_) && !defined(RC_INVOKED) && !defined(MIDL_PASS)

#define _DBGRAISEASSERTIONFAILURE_

#if defined(_PREFAST_)

__analysis_noreturn
FORCEINLINE
VOID
DbgRaiseAssertionFailure (
    VOID
    );

#endif

#if defined(_AMD64_) && !defined(_ARM64EC_)

#if defined(_M_AMD64)

VOID
__int2c (
    VOID
    );

#pragma intrinsic(__int2c)

#if !defined(_PREFAST_)

#define DbgRaiseAssertionFailure() __int2c()

#endif // !defined(_PREFAST_)

#endif // defined(_M_AMD64) && !defined(_ARM64EC_)

#elif defined(_X86_) && !defined(_M_HYBRID_X86_ARM64)

#if defined(_M_IX86) && !defined(_M_HYBRID_X86_ARM64)

#if _MSC_FULL_VER >= 140030222

VOID
__int2c (
    VOID
    );

#pragma intrinsic(__int2c)

#if !defined(_PREFAST_)

#define DbgRaiseAssertionFailure() __int2c()

#endif // !defined(_PREFAST_)

#else // _MSC_FULL_VER >= 140030222

#pragma warning( push )
#pragma warning( disable : 4793 )

#if !defined(_PREFAST_)

__analysis_noreturn
FORCEINLINE
VOID
DbgRaiseAssertionFailure (
    VOID
    )

{
    __asm int 0x2c
}

#endif // !defined(_PREFAST_)

#pragma warning( pop )

#endif // _MSC_FULL_VER >= 140030222

#endif // defined(_M_IX86)

#elif defined(_IA64_)

#if defined(_M_IA64)

#pragma prefast( push )
#pragma prefast( disable: 28301 )

void
__break(
    _In_ int StIIM
    );

#pragma prefast( pop )

#pragma intrinsic (__break)

#define BREAK_DEBUG_BASE    0x080000
#define ASSERT_BREAKPOINT         (BREAK_DEBUG_BASE+3)  // Cause a STATUS_ASSERTION_FAILURE exception to be raised.

#if !defined(_PREFAST_)

#define DbgRaiseAssertionFailure() __break(ASSERT_BREAKPOINT)

#endif // !defined(_PREFAST_)

#endif // defined(_M_IA64)

#elif defined(_ARM64_) || defined(_CHPE_X86_ARM64_) || defined(_ARM64EC_)

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)

#pragma prefast( push )
#pragma prefast( disable: 28301 )

void
__break(
    _In_ int Code
    );

#pragma prefast( pop )

#pragma intrinsic (__break)

#if !defined(_PREFAST_)

#define DbgRaiseAssertionFailure() __break(0xf001)

#endif // !defined(_PREFAST_)

#endif // defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)

#elif defined(_ARM_)

#if defined(_M_ARM)

VOID
__emit(
    const unsigned __int32 opcode
    );

#pragma intrinsic(__emit)

#if !defined(_PREFAST_)

#define DbgRaiseAssertionFailure() __emit(0xdefc)     // THUMB_ASSERT

#endif // !defined(_PREFAST_)

#endif // defined(_M_ARM)

#endif // _AMD64_, _X86_, _IA64_, _ARM64_, _ARM_
#endif // !defined(_DBGRAISEASSERTIONFAILURE_) && !defined(RC_INVOKED) && !defined(MIDL_PASS)

#ifdef __cplusplus
}
#endif





#if _MSC_VER >= 1300

#if defined(_PREFAST_)
// _Analysis_assume_ will never result in any code generation for _exp,
// so using it will not have runtime impact, even if _exp has side effects.
#define NT_ANALYSIS_ASSUME(_exp) _Analysis_assume_(_exp)
#else // _PREFAST_

// NT_ANALYSIS_ASSUME ensures that _exp is parsed in non-analysis compile.
// On DBG, it's guaranteed to be parsed as part of the normal compile, but with
// non-DBG, use __noop to ensure _exp is parseable but without code generation.
#if DBG
#define NT_ANALYSIS_ASSUME(_exp) ((void) 0)
#else // DBG
#define NT_ANALYSIS_ASSUME(_exp) __noop(_exp)
#endif // DBG

#endif // _PREFAST_

// NT_ASSERT_ACTION is the actual assertion action, i.e. raising runtime
// assertion failure. It should be used only by the macro of NT_ASSERT
// and NT_FRE_ASSERT below.
#define NT_ASSERT_ACTION(_exp) \
    ((!(_exp)) ? \
        (__annotation(L"Debug", L"AssertFail", L## #_exp), \
         DbgRaiseAssertionFailure(), FALSE) : \
        TRUE)

#define NT_ASSERTMSG_ACTION(_msg, _exp) \
    ((!(_exp)) ? \
        (__annotation(L"Debug", L"AssertFail", L##_msg), \
         DbgRaiseAssertionFailure(), FALSE) : \
        TRUE)

#define NT_ASSERTMSGW_ACTION(_msg, _exp) \
    ((!(_exp)) ? \
        (__annotation(L"Debug", L"AssertFail", _msg), \
         DbgRaiseAssertionFailure(), FALSE) : \
        TRUE)

#if DBG

#define NT_ASSERT_ASSUME(_exp) \
    (NT_ANALYSIS_ASSUME(_exp), NT_ASSERT_ACTION(_exp))

#define NT_ASSERTMSG_ASSUME(_msg, _exp) \
    (NT_ANALYSIS_ASSUME(_exp), NT_ASSERTMSG_ACTION(_msg, _exp))

#define NT_ASSERTMSGW_ASSUME(_msg, _exp) \
    (NT_ANALYSIS_ASSUME(_exp), NT_ASSERTMSGW_ACTION(_msg, _exp))

// For DBG builds, NT_ASSERT_ASSUME and NT_ASSERT_NOASSUME have the same
// behavior. The behavior only differs for non-DBG.
#define NT_ASSERT_NOASSUME     NT_ASSERT_ASSUME
#define NT_ASSERTMSG_NOASSUME  NT_ASSERTMSG_ASSUME
#define NT_ASSERTMSGW_NOASSUME NT_ASSERTMSGW_ASSUME

#define NT_VERIFY     NT_ASSERT
#define NT_VERIFYMSG  NT_ASSERTMSG
#define NT_VERIFYMSGW NT_ASSERTMSGW

#else // DBG

#define NT_ASSERT_ASSUME(_exp)           (NT_ANALYSIS_ASSUME(_exp), 0)
#define NT_ASSERTMSG_ASSUME(_msg, _exp)  (NT_ANALYSIS_ASSUME(_exp), 0)
#define NT_ASSERTMSGW_ASSUME(_msg, _exp) (NT_ANALYSIS_ASSUME(_exp), 0)

#define NT_ASSERT_NOASSUME(_exp)           ((void) 0)
#define NT_ASSERTMSG_NOASSUME(_msg, _exp)  ((void) 0)
#define NT_ASSERTMSGW_NOASSUME(_msg, _exp) ((void) 0)

#define NT_VERIFY(_exp)           (NT_ANALYSIS_ASSUME(_exp), ((_exp) ? TRUE : FALSE))
#define NT_VERIFYMSG(_msg, _exp ) (NT_ANALYSIS_ASSUME(_exp), ((_exp) ? TRUE : FALSE))
#define NT_VERIFYMSGW(_msg, _exp) (NT_ANALYSIS_ASSUME(_exp), ((_exp) ? TRUE : FALSE))

#endif // DBG

// NT_FRE_ASSERT always takes the assertion action whether DBG or non-DBG.
#define NT_FRE_ASSERT(_exp)           (NT_ANALYSIS_ASSUME(_exp), NT_ASSERT_ACTION(_exp))
#define NT_FRE_ASSERTMSG(_msg, _exp)  (NT_ANALYSIS_ASSUME(_exp), NT_ASSERTMSG_ACTION(_msg, _exp))
#define NT_FRE_ASSERTMSGW(_msg, _exp) (NT_ANALYSIS_ASSUME(_exp), NT_ASSERTMSGW_ACTION(_msg, _exp))

#ifdef NT_ASSERT_ALWAYS_ASSUMES

#define NT_ASSERT     NT_ASSERT_ASSUME
#define NT_ASSERTMSG  NT_ASSERTMSG_ASSUME
#define NT_ASSERTMSGW NT_ASSERTMSGW_ASSUME

#else // NT_ASSERT_ALWAYS_ASSUMES

#define NT_ASSERT     NT_ASSERT_NOASSUME
#define NT_ASSERTMSG  NT_ASSERTMSG_NOASSUME
#define NT_ASSERTMSGW NT_ASSERTMSGW_NOASSUME

#endif // NT_ASSERT_ALWAYS_ASSUMES

#endif // _MSC_VER >= 1300


#define USBD_CLIENT_CONTRACT_VERSION_INVALID 0xFFFFFFFF
#define USBD_CLIENT_CONTRACT_VERSION_602 0x602

#define USBD_INTERFACE_VERSION_600 0x600
#define USBD_INTERFACE_VERSION_602 0x602
#define USBD_INTERFACE_VERSION_603 0x603


#if !defined (NT_INCLUDED)

//
// Define the create disposition values
//

#define FILE_SUPERSEDE                  0x00000000
#define FILE_OPEN                       0x00000001
#define FILE_CREATE                     0x00000002
#define FILE_OPEN_IF                    0x00000003
#define FILE_OVERWRITE                  0x00000004
#define FILE_OVERWRITE_IF               0x00000005
#define FILE_MAXIMUM_DISPOSITION        0x00000005

//
// Define the create/open option flags
//

#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080

#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_REMOTE_INSTANCE               0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800

#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000

#if (NTDDI_VERSION >= NTDDI_WIN7)
#define FILE_OPEN_REQUIRING_OPLOCK              0x00010000
#define FILE_DISALLOW_EXCLUSIVE                 0x00020000
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */
#if (NTDDI_VERSION >= NTDDI_WIN8)
#define FILE_SESSION_AWARE                      0x00040000
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

//
//  CreateOptions flag to pass in call to CreateFile to allow the write through xro.sys
//

#define FILE_RESERVE_OPFILTER                   0x00100000
#define FILE_OPEN_REPARSE_POINT                 0x00200000
#define FILE_OPEN_NO_RECALL                     0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY          0x00800000

//
// Define the base asynchronous I/O argument types
//

typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

//
// Define 64 bit version of IO_STATUS_BLOCK to simplify WOW support when kernel
// and user mode communicates using shared memory (like IoRing).
//

#if defined(_WIN64)

typedef IO_STATUS_BLOCK IO_STATUS_BLOCK64;

#define Iosb64ToIosb(_iosb, _iosb64) {  \
    (_iosb) = (_iosb64);                \
}

#define IosbToIosb64(_iosb64, _iosb) {  \
    (_iosb64) = (_iosb);                \
}

#else

typedef struct _IO_STATUS_BLOCK64 {
    union {
        NTSTATUS Status;
        PVOID64 Pointer;
    } DUMMYUNIONNAME;

    ULONG64 Information;
} IO_STATUS_BLOCK64;

#define Iosb64ToIosb(_iosb, _iosb64) {                      \
    (_iosb).Pointer = Ptr64ToPtr( (_iosb64).Pointer );      \
    (_iosb).Information = (ULONG_PTR)(_iosb64).Information; \
}

#define IosbToIosb64(_iosb64, _iosb) {                      \
    (_iosb64).Pointer = PtrToPtr64( (_iosb).Pointer );      \
    (_iosb64).Information = (ULONG64)(_iosb).Information;   \
}

#endif

typedef IO_STATUS_BLOCK64 *PIO_STATUS_BLOCK64;


//
// Define the I/O bus interface types.
//

typedef enum _INTERFACE_TYPE {
    InterfaceTypeUndefined = -1,
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMEBus,
    NuBus,
    PCMCIABus,
    CBus,
    MPIBus,
    MPSABus,
    ProcessorInternal,
    InternalPowerBus,
    PNPISABus,
    PNPBus,
    Vmcs,
    ACPIBus,
    MaximumInterfaceType
}INTERFACE_TYPE, *PINTERFACE_TYPE;


//
// Resource List definitions
//



//
// Defines the Type in the RESOURCE_DESCRIPTOR
//
// NOTE:  For all CM_RESOURCE_TYPE values, there must be a
// corresponding ResType value in the 32-bit ConfigMgr headerfile
// (cfgmgr32.h).  Values in the range [0x6,0x80) use the same values
// as their ConfigMgr counterparts.  CM_RESOURCE_TYPE values with
// the high bit set (i.e., in the range [0x80,0xFF]), are
// non-arbitrated resources.  These correspond to the same values
// in cfgmgr32.h that have their high bit set (however, since
// cfgmgr32.h uses 16 bits for ResType values, these values are in
// the range [0x8000,0x807F).  Note that ConfigMgr ResType values
// cannot be in the range [0x8080,0xFFFF), because they would not
// be able to map into CM_RESOURCE_TYPE values.  (0xFFFF itself is
// a special value, because it maps to CmResourceTypeDeviceSpecific.)
//

typedef int CM_RESOURCE_TYPE;

// CmResourceTypeNull is reserved

#define CmResourceTypeNull                0   // ResType_All or ResType_None (0x0000)
#define CmResourceTypePort                1   // ResType_IO (0x0002)
#define CmResourceTypeInterrupt           2   // ResType_IRQ (0x0004)
#define CmResourceTypeMemory              3   // ResType_Mem (0x0001)
#define CmResourceTypeDma                 4   // ResType_DMA (0x0003)
#define CmResourceTypeDeviceSpecific      5   // ResType_ClassSpecific (0xFFFF)
#define CmResourceTypeBusNumber           6   // ResType_BusNumber (0x0006)
#define CmResourceTypeMemoryLarge         7   // ResType_MemLarge (0x0007)
#define CmResourceTypeNonArbitrated     128   // Not arbitrated if 0x80 bit set
#define CmResourceTypeConfigData        128   // ResType_Reserved (0x8000)
#define CmResourceTypeDevicePrivate     129   // ResType_DevicePrivate (0x8001)
#define CmResourceTypePcCardConfig      130   // ResType_PcCardConfig (0x8002)
#define CmResourceTypeMfCardConfig      131   // ResType_MfCardConfig (0x8003)
#define CmResourceTypeConnection        132   // ResType_Connection (0x8004)



//
// Defines the ShareDisposition in the RESOURCE_DESCRIPTOR
//

typedef enum _CM_SHARE_DISPOSITION {
    CmResourceShareUndetermined = 0,    // Reserved
    CmResourceShareDeviceExclusive,
    CmResourceShareDriverExclusive,
    CmResourceShareShared
} CM_SHARE_DISPOSITION;



//
// Define the bit masks for Flags when type is CmResourceTypeInterrupt
//

#define CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE           0x00
#define CM_RESOURCE_INTERRUPT_LATCHED                   0x01
#define CM_RESOURCE_INTERRUPT_MESSAGE                   0x02
#define CM_RESOURCE_INTERRUPT_POLICY_INCLUDED           0x04
#define CM_RESOURCE_INTERRUPT_SECONDARY_INTERRUPT       0x10
#define CM_RESOURCE_INTERRUPT_WAKE_HINT                 0x20

//
// A bitmask defining the bits in a resource or requirements descriptor
// flags field that corresponds to the latch mode or a level triggered
// interrupt.
//

#define CM_RESOURCE_INTERRUPT_LEVEL_LATCHED_BITS 0x0001

//
// Define the token value used for an interrupt vector to mean that the vector
// is message signaled.  This value is used in the MaximumVector field.
//

#define CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN   ((ULONG)-2)



//
// Define the bit masks for Flags when type is CmResourceTypeMemory
// or CmResourceTypeMemoryLarge
//

#define CM_RESOURCE_MEMORY_READ_WRITE                       0x0000
#define CM_RESOURCE_MEMORY_READ_ONLY                        0x0001
#define CM_RESOURCE_MEMORY_WRITE_ONLY                       0x0002
#define CM_RESOURCE_MEMORY_WRITEABILITY_MASK                0x0003
#define CM_RESOURCE_MEMORY_PREFETCHABLE                     0x0004

#define CM_RESOURCE_MEMORY_COMBINEDWRITE                    0x0008
#define CM_RESOURCE_MEMORY_24                               0x0010
#define CM_RESOURCE_MEMORY_CACHEABLE                        0x0020
#define CM_RESOURCE_MEMORY_WINDOW_DECODE                    0x0040
#define CM_RESOURCE_MEMORY_BAR                              0x0080

#define CM_RESOURCE_MEMORY_COMPAT_FOR_INACCESSIBLE_RANGE    0x0100



//
// Define the bit masks exclusive to type CmResourceTypeMemoryLarge.
//

#define CM_RESOURCE_MEMORY_LARGE                            0x0E00
#define CM_RESOURCE_MEMORY_LARGE_40                         0x0200
#define CM_RESOURCE_MEMORY_LARGE_48                         0x0400
#define CM_RESOURCE_MEMORY_LARGE_64                         0x0800

//
// Define limits for large memory resources
//

#define CM_RESOURCE_MEMORY_LARGE_40_MAXLEN          0x000000FFFFFFFF00
#define CM_RESOURCE_MEMORY_LARGE_48_MAXLEN          0x0000FFFFFFFF0000
#define CM_RESOURCE_MEMORY_LARGE_64_MAXLEN          0xFFFFFFFF00000000

//
// Define the bit masks for Flags when type is CmResourceTypePort
//

#define CM_RESOURCE_PORT_MEMORY                             0x0000
#define CM_RESOURCE_PORT_IO                                 0x0001
#define CM_RESOURCE_PORT_10_BIT_DECODE                      0x0004
#define CM_RESOURCE_PORT_12_BIT_DECODE                      0x0008
#define CM_RESOURCE_PORT_16_BIT_DECODE                      0x0010
#define CM_RESOURCE_PORT_POSITIVE_DECODE                    0x0020
#define CM_RESOURCE_PORT_PASSIVE_DECODE                     0x0040
#define CM_RESOURCE_PORT_WINDOW_DECODE                      0x0080
#define CM_RESOURCE_PORT_BAR                                0x0100

//
// Define the bit masks for Flags when type is CmResourceTypeDma
//

#define CM_RESOURCE_DMA_8                   0x0000
#define CM_RESOURCE_DMA_16                  0x0001
#define CM_RESOURCE_DMA_32                  0x0002
#define CM_RESOURCE_DMA_8_AND_16            0x0004
#define CM_RESOURCE_DMA_BUS_MASTER          0x0008
#define CM_RESOURCE_DMA_TYPE_A              0x0010
#define CM_RESOURCE_DMA_TYPE_B              0x0020
#define CM_RESOURCE_DMA_TYPE_F              0x0040
#define CM_RESOURCE_DMA_V3                  0x0080

//
// Define the different types of DMA transfer width values.
//

#define DMAV3_TRANFER_WIDTH_8               0x00
#define DMAV3_TRANFER_WIDTH_16              0x01
#define DMAV3_TRANFER_WIDTH_32              0x02
#define DMAV3_TRANFER_WIDTH_64              0x03
#define DMAV3_TRANFER_WIDTH_128             0x04
#define DMAV3_TRANFER_WIDTH_256             0x05

//
// Define the Class and Type values for CmResourceTypeConnection
//

#define CM_RESOURCE_CONNECTION_CLASS_GPIO                  0x01
#define CM_RESOURCE_CONNECTION_CLASS_SERIAL                0x02
#define CM_RESOURCE_CONNECTION_CLASS_FUNCTION_CONFIG       0x03


#define CM_RESOURCE_CONNECTION_TYPE_GPIO_IO                0x02
#define CM_RESOURCE_CONNECTION_TYPE_SERIAL_I2C             0x01
#define CM_RESOURCE_CONNECTION_TYPE_SERIAL_SPI             0x02
#define CM_RESOURCE_CONNECTION_TYPE_SERIAL_UART            0x03
#define CM_RESOURCE_CONNECTION_TYPE_FUNCTION_CONFIG        0x01



//
// This structure defines one type of resource used by a driver.
//
// There can only be *1* DeviceSpecificData block. It must be located at
// the end of all resource descriptors in a full descriptor block.
//

//
// Make sure alignment is made properly by compiler; otherwise move
// flags back to the top of the structure (common to all members of the
// union).
//



#include "pshpack4.h"
typedef struct _CM_PARTIAL_RESOURCE_DESCRIPTOR {
    UCHAR Type;
    UCHAR ShareDisposition;
    USHORT Flags;
    union {

        //
        // Range of resources, inclusive.  These are physical, bus relative.
        // It is known that Port and Memory below have the exact same layout
        // as Generic.
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Generic;

        //
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Port;

        //
        //

        struct {
#if defined(NT_PROCESSOR_GROUPS)
            USHORT Level;
            USHORT Group;
#else
            ULONG Level;
#endif
            ULONG Vector;
            KAFFINITY Affinity;
        } Interrupt;

        //
        // Values for message signaled interrupts are distinct in the
        // raw and translated cases.
        //

        struct {
            union {
               struct {
#if defined(NT_PROCESSOR_GROUPS)
                   USHORT Group;
#else
                   USHORT Reserved;
#endif
                   USHORT MessageCount;
                   ULONG Vector;
                   KAFFINITY Affinity;
               } Raw;

               struct {
#if defined(NT_PROCESSOR_GROUPS)
                   USHORT Level;
                   USHORT Group;
#else
                   ULONG Level;
#endif
                   ULONG Vector;
                   KAFFINITY Affinity;
               } Translated;
            } DUMMYUNIONNAME;
        } MessageInterrupt;

        //
        // Range of memory addresses, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        //

        struct {
            PHYSICAL_ADDRESS Start;    // 64 bit physical addresses.
            ULONG Length;
        } Memory;

        //
        // Physical DMA channel.
        //

        struct {
            ULONG Channel;
            ULONG Port;
            ULONG Reserved1;
        } Dma;

        struct {
            ULONG Channel;
            ULONG RequestLine;
            UCHAR TransferWidth;
            UCHAR Reserved1;
            UCHAR Reserved2;
            UCHAR Reserved3;
        } DmaV3;

        //
        // Device driver private data, usually used to help it figure
        // what the resource assignments decisions that were made.
        //

        struct {
            ULONG Data[3];
        } DevicePrivate;

        //
        // Bus Number information.
        //

        struct {
            ULONG Start;
            ULONG Length;
            ULONG Reserved;
        } BusNumber;

        //
        // Device Specific information defined by the driver.
        // The DataSize field indicates the size of the data in bytes. The
        // data is located immediately after the DeviceSpecificData field in
        // the structure.
        //

        struct {
            ULONG DataSize;
            ULONG Reserved1;
            ULONG Reserved2;
        } DeviceSpecificData;

        // The following structures provide support for memory-mapped
        // IO resources greater than MAXULONG
        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length40;
        } Memory40;

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length48;
        } Memory48;

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length64;
        } Memory64;

        struct {
            UCHAR Class;
            UCHAR Type;
            UCHAR Reserved1;
            UCHAR Reserved2;
            ULONG IdLowPart;
            ULONG IdHighPart;
        } Connection;

    } u;
} CM_PARTIAL_RESOURCE_DESCRIPTOR, *PCM_PARTIAL_RESOURCE_DESCRIPTOR;
#include "poppack.h"



//
// A Partial Resource List is what can be found in the ARC firmware
// or will be generated by ntdetect.com.
// The configuration manager will transform this structure into a Full
// resource descriptor when it is about to store it in the regsitry.
//
// Note: There must a be a convention to the order of fields of same type,
// (defined on a device by device basis) so that the fields can make sense
// to a driver (i.e. when multiple memory ranges are necessary).
//

typedef struct _CM_PARTIAL_RESOURCE_LIST {
    USHORT Version;
    USHORT Revision;
    ULONG Count;
    CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[1];
} CM_PARTIAL_RESOURCE_LIST, *PCM_PARTIAL_RESOURCE_LIST;

//
// A Full Resource Descriptor is what can be found in the registry.
// This is what will be returned to a driver when it queries the registry
// to get device information; it will be stored under a key in the hardware
// description tree.
//
// Note: There must a be a convention to the order of fields of same type,
// (defined on a device by device basis) so that the fields can make sense
// to a driver (i.e. when multiple memory ranges are necessary).
//

typedef struct _CM_FULL_RESOURCE_DESCRIPTOR {
    INTERFACE_TYPE InterfaceType; // unused for WDM
    ULONG BusNumber; // unused for WDM
    CM_PARTIAL_RESOURCE_LIST PartialResourceList;
} CM_FULL_RESOURCE_DESCRIPTOR, *PCM_FULL_RESOURCE_DESCRIPTOR;

//
// The Resource list is what will be stored by the drivers into the
// resource map via the IO API.
//

typedef struct _CM_RESOURCE_LIST {
    ULONG Count;
    CM_FULL_RESOURCE_DESCRIPTOR List[1];
} CM_RESOURCE_LIST, *PCM_RESOURCE_LIST;


#endif // !defined (NT_INCLUDED)

#ifdef __cplusplus
}       // extern "C"
#endif

