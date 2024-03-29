/*++

Copyright (c) Microsoft. All rights reserved.

Module Name: FxDynamics.h

Abstract:
    Generated header for WDF APIs

Environment:
    User mode only

    Warning: manual changes to this file will be lost.
--*/

#ifndef _FXDYNAMICS_H_
#define _FXDYNAMICS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _WDFFUNCTIONS {

    PFN_WDFCOLLECTIONCREATE                                   pfnWdfCollectionCreate;
    PFN_WDFCOLLECTIONGETCOUNT                                 pfnWdfCollectionGetCount;
    PFN_WDFCOLLECTIONADD                                      pfnWdfCollectionAdd;
    PFN_WDFCOLLECTIONREMOVE                                   pfnWdfCollectionRemove;
    PFN_WDFCOLLECTIONREMOVEITEM                               pfnWdfCollectionRemoveItem;
    PFN_WDFCOLLECTIONGETITEM                                  pfnWdfCollectionGetItem;
    PFN_WDFCOLLECTIONGETFIRSTITEM                             pfnWdfCollectionGetFirstItem;
    PFN_WDFCOLLECTIONGETLASTITEM                              pfnWdfCollectionGetLastItem;
    PFN_WDFCXDEVICEINITALLOCATE                               pfnWdfCxDeviceInitAllocate;
    PFN_WDFCXDEVICEINITSETREQUESTATTRIBUTES                   pfnWdfCxDeviceInitSetRequestAttributes;
    PFN_WDFCXDEVICEINITSETFILEOBJECTCONFIG                    pfnWdfCxDeviceInitSetFileObjectConfig;
    PFN_WDFCXVERIFIERKEBUGCHECK                               pfnWdfCxVerifierKeBugCheck;
    PFN_WDFDEVICEGETDEVICESTATE                               pfnWdfDeviceGetDeviceState;
    PFN_WDFDEVICESETDEVICESTATE                               pfnWdfDeviceSetDeviceState;
    PFN_WDFDEVICEGETDRIVER                                    pfnWdfDeviceGetDriver;
    PFN_WDFDEVICEGETIOTARGET                                  pfnWdfDeviceGetIoTarget;
    PFN_WDFDEVICEASSIGNS0IDLESETTINGS                         pfnWdfDeviceAssignS0IdleSettings;
    PFN_WDFDEVICEASSIGNSXWAKESETTINGS                         pfnWdfDeviceAssignSxWakeSettings;
    PFN_WDFDEVICEOPENREGISTRYKEY                              pfnWdfDeviceOpenRegistryKey;
    PFN_WDFDEVICEINITSETPNPPOWEREVENTCALLBACKS                pfnWdfDeviceInitSetPnpPowerEventCallbacks;
    PFN_WDFDEVICEINITSETPOWERPOLICYEVENTCALLBACKS             pfnWdfDeviceInitSetPowerPolicyEventCallbacks;
    PFN_WDFDEVICEINITSETPOWERPOLICYOWNERSHIP                  pfnWdfDeviceInitSetPowerPolicyOwnership;
    PFN_WDFDEVICEINITSETIOTYPE                                pfnWdfDeviceInitSetIoType;
    PFN_WDFDEVICEINITSETFILEOBJECTCONFIG                      pfnWdfDeviceInitSetFileObjectConfig;
    PFN_WDFDEVICEINITSETREQUESTATTRIBUTES                     pfnWdfDeviceInitSetRequestAttributes;
    PFN_WDFDEVICECREATE                                       pfnWdfDeviceCreate;
    PFN_WDFDEVICESETSTATICSTOPREMOVE                          pfnWdfDeviceSetStaticStopRemove;
    PFN_WDFDEVICECREATEDEVICEINTERFACE                        pfnWdfDeviceCreateDeviceInterface;
    PFN_WDFDEVICESETDEVICEINTERFACESTATE                      pfnWdfDeviceSetDeviceInterfaceState;
    PFN_WDFDEVICERETRIEVEDEVICEINTERFACESTRING                pfnWdfDeviceRetrieveDeviceInterfaceString;
    PFN_WDFDEVICECREATESYMBOLICLINK                           pfnWdfDeviceCreateSymbolicLink;
    PFN_WDFDEVICEQUERYPROPERTY                                pfnWdfDeviceQueryProperty;
    PFN_WDFDEVICEALLOCANDQUERYPROPERTY                        pfnWdfDeviceAllocAndQueryProperty;
    PFN_WDFDEVICESETPNPCAPABILITIES                           pfnWdfDeviceSetPnpCapabilities;
    PFN_WDFDEVICESETPOWERCAPABILITIES                         pfnWdfDeviceSetPowerCapabilities;
    PFN_WDFDEVICESETFAILED                                    pfnWdfDeviceSetFailed;
    PFN_WDFDEVICESTOPIDLENOTRACK                              pfnWdfDeviceStopIdleNoTrack;
    PFN_WDFDEVICERESUMEIDLENOTRACK                            pfnWdfDeviceResumeIdleNoTrack;
    PFN_WDFDEVICEGETFILEOBJECT                                pfnWdfDeviceGetFileObject;
    PFN_WDFDEVICEGETDEFAULTQUEUE                              pfnWdfDeviceGetDefaultQueue;
    PFN_WDFDEVICECONFIGUREREQUESTDISPATCHING                  pfnWdfDeviceConfigureRequestDispatching;
    PFN_WDFDEVICEGETSYSTEMPOWERACTION                         pfnWdfDeviceGetSystemPowerAction;
    PFN_WDFDEVICEINITSETRELEASEHARDWAREORDERONFAILURE         pfnWdfDeviceInitSetReleaseHardwareOrderOnFailure;
    PFN_WDFDEVICEINITSETIOTYPEEX                              pfnWdfDeviceInitSetIoTypeEx;
    PFN_WDFDEVICEPOSTEVENT                                    pfnWdfDevicePostEvent;
    PFN_WDFDEVICEMAPIOSPACE                                   pfnWdfDeviceMapIoSpace;
    PFN_WDFDEVICEUNMAPIOSPACE                                 pfnWdfDeviceUnmapIoSpace;
    PFN_WDFDEVICEGETHARDWAREREGISTERMAPPEDADDRESS             pfnWdfDeviceGetHardwareRegisterMappedAddress;
    PFN_WDFDEVICEREADFROMHARDWARE                             pfnWdfDeviceReadFromHardware;
    PFN_WDFDEVICEWRITETOHARDWARE                              pfnWdfDeviceWriteToHardware;
    PFN_WDFDEVICEASSIGNINTERFACEPROPERTY                      pfnWdfDeviceAssignInterfaceProperty;
    PFN_WDFDEVICEALLOCANDQUERYINTERFACEPROPERTY               pfnWdfDeviceAllocAndQueryInterfaceProperty;
    PFN_WDFDEVICEQUERYINTERFACEPROPERTY                       pfnWdfDeviceQueryInterfaceProperty;
    PFN_WDFDEVICEGETDEVICESTACKIOTYPE                         pfnWdfDeviceGetDeviceStackIoType;
    PFN_WDFDEVICEQUERYPROPERTYEX                              pfnWdfDeviceQueryPropertyEx;
    PFN_WDFDEVICEALLOCANDQUERYPROPERTYEX                      pfnWdfDeviceAllocAndQueryPropertyEx;
    PFN_WDFDEVICEASSIGNPROPERTY                               pfnWdfDeviceAssignProperty;
    PFN_WDFDRIVERCREATE                                       pfnWdfDriverCreate;
    PFN_WDFDRIVERGETREGISTRYPATH                              pfnWdfDriverGetRegistryPath;
    PFN_WDFDRIVEROPENPARAMETERSREGISTRYKEY                    pfnWdfDriverOpenParametersRegistryKey;
    PFN_WDFDRIVERRETRIEVEVERSIONSTRING                        pfnWdfDriverRetrieveVersionString;
    PFN_WDFDRIVERISVERSIONAVAILABLE                           pfnWdfDriverIsVersionAvailable;
    PFN_WDFFDOINITOPENREGISTRYKEY                             pfnWdfFdoInitOpenRegistryKey;
    PFN_WDFFDOINITQUERYPROPERTY                               pfnWdfFdoInitQueryProperty;
    PFN_WDFFDOINITALLOCANDQUERYPROPERTY                       pfnWdfFdoInitAllocAndQueryProperty;
    PFN_WDFFDOINITQUERYPROPERTYEX                             pfnWdfFdoInitQueryPropertyEx;
    PFN_WDFFDOINITALLOCANDQUERYPROPERTYEX                     pfnWdfFdoInitAllocAndQueryPropertyEx;
    PFN_WDFFDOINITSETFILTER                                   pfnWdfFdoInitSetFilter;
    PFN_WDFFILEOBJECTGETFILENAME                              pfnWdfFileObjectGetFileName;
    PFN_WDFFILEOBJECTGETDEVICE                                pfnWdfFileObjectGetDevice;
    PFN_WDFFILEOBJECTGETINITIATORPROCESSID                    pfnWdfFileObjectGetInitiatorProcessId;
    PFN_WDFFILEOBJECTGETRELATEDFILEOBJECT                     pfnWdfFileObjectGetRelatedFileObject;
    PFN_WDFINTERRUPTCREATE                                    pfnWdfInterruptCreate;
    PFN_WDFINTERRUPTQUEUEDPCFORISR                            pfnWdfInterruptQueueDpcForIsr;
    PFN_WDFINTERRUPTQUEUEWORKITEMFORISR                       pfnWdfInterruptQueueWorkItemForIsr;
    PFN_WDFINTERRUPTSYNCHRONIZE                               pfnWdfInterruptSynchronize;
    PFN_WDFINTERRUPTACQUIRELOCK                               pfnWdfInterruptAcquireLock;
    PFN_WDFINTERRUPTRELEASELOCK                               pfnWdfInterruptReleaseLock;
    PFN_WDFINTERRUPTENABLE                                    pfnWdfInterruptEnable;
    PFN_WDFINTERRUPTDISABLE                                   pfnWdfInterruptDisable;
    PFN_WDFINTERRUPTGETINFO                                   pfnWdfInterruptGetInfo;
    PFN_WDFINTERRUPTSETPOLICY                                 pfnWdfInterruptSetPolicy;
    PFN_WDFINTERRUPTSETEXTENDEDPOLICY                         pfnWdfInterruptSetExtendedPolicy;
    PFN_WDFINTERRUPTGETDEVICE                                 pfnWdfInterruptGetDevice;
    PFN_WDFINTERRUPTTRYTOACQUIRELOCK                          pfnWdfInterruptTryToAcquireLock;
    PFN_WDFIOQUEUECREATE                                      pfnWdfIoQueueCreate;
    PFN_WDFIOQUEUEGETSTATE                                    pfnWdfIoQueueGetState;
    PFN_WDFIOQUEUESTART                                       pfnWdfIoQueueStart;
    PFN_WDFIOQUEUESTOP                                        pfnWdfIoQueueStop;
    PFN_WDFIOQUEUESTOPSYNCHRONOUSLY                           pfnWdfIoQueueStopSynchronously;
    PFN_WDFIOQUEUEGETDEVICE                                   pfnWdfIoQueueGetDevice;
    PFN_WDFIOQUEUERETRIEVENEXTREQUEST                         pfnWdfIoQueueRetrieveNextRequest;
    PFN_WDFIOQUEUERETRIEVEREQUESTBYFILEOBJECT                 pfnWdfIoQueueRetrieveRequestByFileObject;
    PFN_WDFIOQUEUEFINDREQUEST                                 pfnWdfIoQueueFindRequest;
    PFN_WDFIOQUEUERETRIEVEFOUNDREQUEST                        pfnWdfIoQueueRetrieveFoundRequest;
    PFN_WDFIOQUEUEDRAINSYNCHRONOUSLY                          pfnWdfIoQueueDrainSynchronously;
    PFN_WDFIOQUEUEDRAIN                                       pfnWdfIoQueueDrain;
    PFN_WDFIOQUEUEPURGESYNCHRONOUSLY                          pfnWdfIoQueuePurgeSynchronously;
    PFN_WDFIOQUEUEPURGE                                       pfnWdfIoQueuePurge;
    PFN_WDFIOQUEUEREADYNOTIFY                                 pfnWdfIoQueueReadyNotify;
    PFN_WDFIOQUEUESTOPANDPURGE                                pfnWdfIoQueueStopAndPurge;
    PFN_WDFIOQUEUESTOPANDPURGESYNCHRONOUSLY                   pfnWdfIoQueueStopAndPurgeSynchronously;
    PFN_WDFIOTARGETCREATE                                     pfnWdfIoTargetCreate;
    PFN_WDFIOTARGETOPEN                                       pfnWdfIoTargetOpen;
    PFN_WDFIOTARGETCLOSEFORQUERYREMOVE                        pfnWdfIoTargetCloseForQueryRemove;
    PFN_WDFIOTARGETCLOSE                                      pfnWdfIoTargetClose;
    PFN_WDFIOTARGETSTART                                      pfnWdfIoTargetStart;
    PFN_WDFIOTARGETSTOP                                       pfnWdfIoTargetStop;
    PFN_WDFIOTARGETPURGE                                      pfnWdfIoTargetPurge;
    PFN_WDFIOTARGETGETSTATE                                   pfnWdfIoTargetGetState;
    PFN_WDFIOTARGETGETDEVICE                                  pfnWdfIoTargetGetDevice;
    PFN_WDFIOTARGETSENDREADSYNCHRONOUSLY                      pfnWdfIoTargetSendReadSynchronously;
    PFN_WDFIOTARGETFORMATREQUESTFORREAD                       pfnWdfIoTargetFormatRequestForRead;
    PFN_WDFIOTARGETSENDWRITESYNCHRONOUSLY                     pfnWdfIoTargetSendWriteSynchronously;
    PFN_WDFIOTARGETFORMATREQUESTFORWRITE                      pfnWdfIoTargetFormatRequestForWrite;
    PFN_WDFIOTARGETSENDIOCTLSYNCHRONOUSLY                     pfnWdfIoTargetSendIoctlSynchronously;
    PFN_WDFIOTARGETFORMATREQUESTFORIOCTL                      pfnWdfIoTargetFormatRequestForIoctl;
    PFN_WDFMEMORYCREATE                                       pfnWdfMemoryCreate;
    PFN_WDFMEMORYCREATEPREALLOCATED                           pfnWdfMemoryCreatePreallocated;
    PFN_WDFMEMORYGETBUFFER                                    pfnWdfMemoryGetBuffer;
    PFN_WDFMEMORYASSIGNBUFFER                                 pfnWdfMemoryAssignBuffer;
    PFN_WDFMEMORYCOPYTOBUFFER                                 pfnWdfMemoryCopyToBuffer;
    PFN_WDFMEMORYCOPYFROMBUFFER                               pfnWdfMemoryCopyFromBuffer;
    PFN_WDFOBJECTGETTYPEDCONTEXTWORKER                        pfnWdfObjectGetTypedContextWorker;
    PFN_WDFOBJECTALLOCATECONTEXT                              pfnWdfObjectAllocateContext;
    PFN_WDFOBJECTCONTEXTGETOBJECT                             pfnWdfObjectContextGetObject;
    PFN_WDFOBJECTREFERENCEACTUAL                              pfnWdfObjectReferenceActual;
    PFN_WDFOBJECTDEREFERENCEACTUAL                            pfnWdfObjectDereferenceActual;
    PFN_WDFOBJECTCREATE                                       pfnWdfObjectCreate;
    PFN_WDFOBJECTDELETE                                       pfnWdfObjectDelete;
    PFN_WDFOBJECTQUERY                                        pfnWdfObjectQuery;
    PFN_WDFREGISTRYOPENKEY                                    pfnWdfRegistryOpenKey;
    PFN_WDFREGISTRYCREATEKEY                                  pfnWdfRegistryCreateKey;
    PFN_WDFREGISTRYCLOSE                                      pfnWdfRegistryClose;
    PFN_WDFREGISTRYREMOVEKEY                                  pfnWdfRegistryRemoveKey;
    PFN_WDFREGISTRYREMOVEVALUE                                pfnWdfRegistryRemoveValue;
    PFN_WDFREGISTRYQUERYVALUE                                 pfnWdfRegistryQueryValue;
    PFN_WDFREGISTRYQUERYMEMORY                                pfnWdfRegistryQueryMemory;
    PFN_WDFREGISTRYQUERYMULTISTRING                           pfnWdfRegistryQueryMultiString;
    PFN_WDFREGISTRYQUERYUNICODESTRING                         pfnWdfRegistryQueryUnicodeString;
    PFN_WDFREGISTRYQUERYSTRING                                pfnWdfRegistryQueryString;
    PFN_WDFREGISTRYQUERYULONG                                 pfnWdfRegistryQueryULong;
    PFN_WDFREGISTRYASSIGNVALUE                                pfnWdfRegistryAssignValue;
    PFN_WDFREGISTRYASSIGNMEMORY                               pfnWdfRegistryAssignMemory;
    PFN_WDFREGISTRYASSIGNMULTISTRING                          pfnWdfRegistryAssignMultiString;
    PFN_WDFREGISTRYASSIGNUNICODESTRING                        pfnWdfRegistryAssignUnicodeString;
    PFN_WDFREGISTRYASSIGNSTRING                               pfnWdfRegistryAssignString;
    PFN_WDFREGISTRYASSIGNULONG                                pfnWdfRegistryAssignULong;
    PFN_WDFREQUESTCREATE                                      pfnWdfRequestCreate;
    PFN_WDFREQUESTREUSE                                       pfnWdfRequestReuse;
    PFN_WDFREQUESTCHANGETARGET                                pfnWdfRequestChangeTarget;
    PFN_WDFREQUESTFORMATREQUESTUSINGCURRENTTYPE               pfnWdfRequestFormatRequestUsingCurrentType;
    PFN_WDFREQUESTSEND                                        pfnWdfRequestSend;
    PFN_WDFREQUESTGETSTATUS                                   pfnWdfRequestGetStatus;
    PFN_WDFREQUESTMARKCANCELABLE                              pfnWdfRequestMarkCancelable;
    PFN_WDFREQUESTMARKCANCELABLEEX                            pfnWdfRequestMarkCancelableEx;
    PFN_WDFREQUESTUNMARKCANCELABLE                            pfnWdfRequestUnmarkCancelable;
    PFN_WDFREQUESTISCANCELED                                  pfnWdfRequestIsCanceled;
    PFN_WDFREQUESTCANCELSENTREQUEST                           pfnWdfRequestCancelSentRequest;
    PFN_WDFREQUESTISFROM32BITPROCESS                          pfnWdfRequestIsFrom32BitProcess;
    PFN_WDFREQUESTSETCOMPLETIONROUTINE                        pfnWdfRequestSetCompletionRoutine;
    PFN_WDFREQUESTGETCOMPLETIONPARAMS                         pfnWdfRequestGetCompletionParams;
    PFN_WDFREQUESTALLOCATETIMER                               pfnWdfRequestAllocateTimer;
    PFN_WDFREQUESTCOMPLETE                                    pfnWdfRequestComplete;
    PFN_WDFREQUESTCOMPLETEWITHINFORMATION                     pfnWdfRequestCompleteWithInformation;
    PFN_WDFREQUESTGETPARAMETERS                               pfnWdfRequestGetParameters;
    PFN_WDFREQUESTRETRIEVEINPUTMEMORY                         pfnWdfRequestRetrieveInputMemory;
    PFN_WDFREQUESTRETRIEVEOUTPUTMEMORY                        pfnWdfRequestRetrieveOutputMemory;
    PFN_WDFREQUESTRETRIEVEINPUTBUFFER                         pfnWdfRequestRetrieveInputBuffer;
    PFN_WDFREQUESTRETRIEVEOUTPUTBUFFER                        pfnWdfRequestRetrieveOutputBuffer;
    PFN_WDFREQUESTSETINFORMATION                              pfnWdfRequestSetInformation;
    PFN_WDFREQUESTGETINFORMATION                              pfnWdfRequestGetInformation;
    PFN_WDFREQUESTGETFILEOBJECT                               pfnWdfRequestGetFileObject;
    PFN_WDFREQUESTGETREQUESTORMODE                            pfnWdfRequestGetRequestorMode;
    PFN_WDFREQUESTFORWARDTOIOQUEUE                            pfnWdfRequestForwardToIoQueue;
    PFN_WDFREQUESTGETIOQUEUE                                  pfnWdfRequestGetIoQueue;
    PFN_WDFREQUESTREQUEUE                                     pfnWdfRequestRequeue;
    PFN_WDFREQUESTSTOPACKNOWLEDGE                             pfnWdfRequestStopAcknowledge;
    PFN_WDFREQUESTIMPERSONATE                                 pfnWdfRequestImpersonate;
    PFN_WDFREQUESTGETREQUESTORPROCESSID                       pfnWdfRequestGetRequestorProcessId;
    PFN_WDFREQUESTISFROMUSERMODEDRIVER                        pfnWdfRequestIsFromUserModeDriver;
    PFN_WDFREQUESTSETUSERMODEDRIVERINITIATEDIO                pfnWdfRequestSetUserModeDriverInitiatedIo;
    PFN_WDFREQUESTGETUSERMODEDRIVERINITIATEDIO                pfnWdfRequestGetUserModeDriverInitiatedIo;
    PFN_WDFREQUESTSETACTIVITYID                               pfnWdfRequestSetActivityId;
    PFN_WDFREQUESTRETRIEVEACTIVITYID                          pfnWdfRequestRetrieveActivityId;
    PFN_WDFREQUESTGETEFFECTIVEIOTYPE                          pfnWdfRequestGetEffectiveIoType;
    PFN_WDFCMRESOURCELISTGETCOUNT                             pfnWdfCmResourceListGetCount;
    PFN_WDFCMRESOURCELISTGETDESCRIPTOR                        pfnWdfCmResourceListGetDescriptor;
    PFN_WDFSTRINGCREATE                                       pfnWdfStringCreate;
    PFN_WDFSTRINGGETUNICODESTRING                             pfnWdfStringGetUnicodeString;
    PFN_WDFOBJECTACQUIRELOCK                                  pfnWdfObjectAcquireLock;
    PFN_WDFOBJECTRELEASELOCK                                  pfnWdfObjectReleaseLock;
    PFN_WDFWAITLOCKCREATE                                     pfnWdfWaitLockCreate;
    PFN_WDFWAITLOCKACQUIRE                                    pfnWdfWaitLockAcquire;
    PFN_WDFWAITLOCKRELEASE                                    pfnWdfWaitLockRelease;
    PFN_WDFSPINLOCKCREATE                                     pfnWdfSpinLockCreate;
    PFN_WDFSPINLOCKACQUIRE                                    pfnWdfSpinLockAcquire;
    PFN_WDFSPINLOCKRELEASE                                    pfnWdfSpinLockRelease;
    PFN_WDFTIMERCREATE                                        pfnWdfTimerCreate;
    PFN_WDFTIMERSTART                                         pfnWdfTimerStart;
    PFN_WDFTIMERSTOP                                          pfnWdfTimerStop;
    PFN_WDFTIMERGETPARENTOBJECT                               pfnWdfTimerGetParentObject;
    PFN_WDFUSBTARGETDEVICECREATE                              pfnWdfUsbTargetDeviceCreate;
    PFN_WDFUSBTARGETDEVICECREATEWITHPARAMETERS                pfnWdfUsbTargetDeviceCreateWithParameters;
    PFN_WDFUSBTARGETDEVICERETRIEVEINFORMATION                 pfnWdfUsbTargetDeviceRetrieveInformation;
    PFN_WDFUSBTARGETDEVICEGETDEVICEDESCRIPTOR                 pfnWdfUsbTargetDeviceGetDeviceDescriptor;
    PFN_WDFUSBTARGETDEVICERETRIEVECONFIGDESCRIPTOR            pfnWdfUsbTargetDeviceRetrieveConfigDescriptor;
    PFN_WDFUSBTARGETDEVICEQUERYSTRING                         pfnWdfUsbTargetDeviceQueryString;
    PFN_WDFUSBTARGETDEVICEALLOCANDQUERYSTRING                 pfnWdfUsbTargetDeviceAllocAndQueryString;
    PFN_WDFUSBTARGETDEVICEFORMATREQUESTFORSTRING              pfnWdfUsbTargetDeviceFormatRequestForString;
    PFN_WDFUSBTARGETDEVICEGETNUMINTERFACES                    pfnWdfUsbTargetDeviceGetNumInterfaces;
    PFN_WDFUSBTARGETDEVICESELECTCONFIG                        pfnWdfUsbTargetDeviceSelectConfig;
    PFN_WDFUSBTARGETDEVICESENDCONTROLTRANSFERSYNCHRONOUSLY    pfnWdfUsbTargetDeviceSendControlTransferSynchronously;
    PFN_WDFUSBTARGETDEVICEFORMATREQUESTFORCONTROLTRANSFER     pfnWdfUsbTargetDeviceFormatRequestForControlTransfer;
    PFN_WDFUSBTARGETDEVICERESETPORTSYNCHRONOUSLY              pfnWdfUsbTargetDeviceResetPortSynchronously;
    PFN_WDFUSBTARGETDEVICEQUERYUSBCAPABILITY                  pfnWdfUsbTargetDeviceQueryUsbCapability;
    PFN_WDFUSBTARGETPIPEGETINFORMATION                        pfnWdfUsbTargetPipeGetInformation;
    PFN_WDFUSBTARGETPIPEISINENDPOINT                          pfnWdfUsbTargetPipeIsInEndpoint;
    PFN_WDFUSBTARGETPIPEISOUTENDPOINT                         pfnWdfUsbTargetPipeIsOutEndpoint;
    PFN_WDFUSBTARGETPIPEGETTYPE                               pfnWdfUsbTargetPipeGetType;
    PFN_WDFUSBTARGETPIPESETNOMAXIMUMPACKETSIZECHECK           pfnWdfUsbTargetPipeSetNoMaximumPacketSizeCheck;
    PFN_WDFUSBTARGETPIPEWRITESYNCHRONOUSLY                    pfnWdfUsbTargetPipeWriteSynchronously;
    PFN_WDFUSBTARGETPIPEFORMATREQUESTFORWRITE                 pfnWdfUsbTargetPipeFormatRequestForWrite;
    PFN_WDFUSBTARGETPIPEREADSYNCHRONOUSLY                     pfnWdfUsbTargetPipeReadSynchronously;
    PFN_WDFUSBTARGETPIPEFORMATREQUESTFORREAD                  pfnWdfUsbTargetPipeFormatRequestForRead;
    PFN_WDFUSBTARGETPIPECONFIGCONTINUOUSREADER                pfnWdfUsbTargetPipeConfigContinuousReader;
    PFN_WDFUSBTARGETPIPEABORTSYNCHRONOUSLY                    pfnWdfUsbTargetPipeAbortSynchronously;
    PFN_WDFUSBTARGETPIPEFORMATREQUESTFORABORT                 pfnWdfUsbTargetPipeFormatRequestForAbort;
    PFN_WDFUSBTARGETPIPERESETSYNCHRONOUSLY                    pfnWdfUsbTargetPipeResetSynchronously;
    PFN_WDFUSBTARGETPIPEFORMATREQUESTFORRESET                 pfnWdfUsbTargetPipeFormatRequestForReset;
    PFN_WDFUSBINTERFACEGETINTERFACENUMBER                     pfnWdfUsbInterfaceGetInterfaceNumber;
    PFN_WDFUSBINTERFACEGETNUMENDPOINTS                        pfnWdfUsbInterfaceGetNumEndpoints;
    PFN_WDFUSBINTERFACEGETDESCRIPTOR                          pfnWdfUsbInterfaceGetDescriptor;
    PFN_WDFUSBINTERFACEGETNUMSETTINGS                         pfnWdfUsbInterfaceGetNumSettings;
    PFN_WDFUSBINTERFACESELECTSETTING                          pfnWdfUsbInterfaceSelectSetting;
    PFN_WDFUSBINTERFACEGETENDPOINTINFORMATION                 pfnWdfUsbInterfaceGetEndpointInformation;
    PFN_WDFUSBTARGETDEVICEGETINTERFACE                        pfnWdfUsbTargetDeviceGetInterface;
    PFN_WDFUSBINTERFACEGETCONFIGUREDSETTINGINDEX              pfnWdfUsbInterfaceGetConfiguredSettingIndex;
    PFN_WDFUSBINTERFACEGETNUMCONFIGUREDPIPES                  pfnWdfUsbInterfaceGetNumConfiguredPipes;
    PFN_WDFUSBINTERFACEGETCONFIGUREDPIPE                      pfnWdfUsbInterfaceGetConfiguredPipe;
    PFN_WDFVERIFIERDBGBREAKPOINT                              pfnWdfVerifierDbgBreakPoint;
    PFN_WDFVERIFIERKEBUGCHECK                                 pfnWdfVerifierKeBugCheck;
    PFN_WDFGETTRIAGEINFO                                      pfnWdfGetTriageInfo;
    PFN_WDFWORKITEMCREATE                                     pfnWdfWorkItemCreate;
    PFN_WDFWORKITEMENQUEUE                                    pfnWdfWorkItemEnqueue;
    PFN_WDFWORKITEMGETPARENTOBJECT                            pfnWdfWorkItemGetParentObject;
    PFN_WDFWORKITEMFLUSH                                      pfnWdfWorkItemFlush;
    PFN_WDFREGISTRYWDMGETHANDLE                               pfnWdfRegistryWdmGetHandle;
    PFN_WDFDEVICESTOPIDLEACTUAL                               pfnWdfDeviceStopIdleActual;
    PFN_WDFDEVICERESUMEIDLEACTUAL                             pfnWdfDeviceResumeIdleActual;
    PFN_WDFDEVICEINITENABLEHIDINTERFACE                       pfnWdfDeviceInitEnableHidInterface;
    PFN_WDFDEVICEHIDNOTIFYPRESENCE                            pfnWdfDeviceHidNotifyPresence;
    PFN_WDFDEVICEGETSELFIOTARGET                              pfnWdfDeviceGetSelfIoTarget;
    PFN_WDFDEVICEINITALLOWSELFIOTARGET                        pfnWdfDeviceInitAllowSelfIoTarget;
    PFN_WDFIOTARGETSELFASSIGNDEFAULTIOQUEUE                   pfnWdfIoTargetSelfAssignDefaultIoQueue;
    PFN_WDFDEVICEOPENDEVICEMAPKEY                             pfnWdfDeviceOpenDevicemapKey;
    PFN_WDFIOTARGETWDMGETTARGETFILEHANDLE                     pfnWdfIoTargetWdmGetTargetFileHandle;
    PFN_WDFDEVICEWDMDISPATCHIRP                               pfnWdfDeviceWdmDispatchIrp;
    PFN_WDFDEVICEWDMDISPATCHIRPTOIOQUEUE                      pfnWdfDeviceWdmDispatchIrpToIoQueue;
    PFN_WDFDEVICECONFIGUREWDMIRPDISPATCHCALLBACK              pfnWdfDeviceConfigureWdmIrpDispatchCallback;
    PFN_WDFCXDEVICEINITSETPNPPOWEREVENTCALLBACKS              pfnWdfCxDeviceInitSetPnpPowerEventCallbacks;
    PFN_WDFCOMPANIONCREATE                                    pfnWdfCompanionCreate;
    PFN_WDFCOMPANIONWDMGETSECUREDEVICEHANDLE                  pfnWdfCompanionWdmGetSecureDeviceHandle;
    PFN_WDFCOMPANIONCREATETASKQUEUE                           pfnWdfCompanionCreateTaskQueue;
    PFN_WDFDEVICEINITSETCOMPANIONEVENTCALLBACKS               pfnWdfDeviceInitSetCompanionEventCallbacks;
    PFN_WDFDRIVEROPENPERSISTENTSTATEREGISTRYKEY               pfnWdfDriverOpenPersistentStateRegistryKey;
    PFN_WDFDRIVERERRORREPORTAPIMISSING                        pfnWdfDriverErrorReportApiMissing;
    PFN_WDFDEVICERETRIEVEDEVICEDIRECTORYSTRING                pfnWdfDeviceRetrieveDeviceDirectoryString;
    PFN_WDFDRIVERRETRIEVEDRIVERDATADIRECTORYSTRING            pfnWdfDriverRetrieveDriverDataDirectoryString;
    PFN_WDFCXDEVICEINITALLOCATECONTEXT                        pfnWdfCxDeviceInitAllocateContext;
    PFN_WDFCXDEVICEINITGETTYPEDCONTEXTWORKER                  pfnWdfCxDeviceInitGetTypedContextWorker;
    PFN_WDFCXDEVICEINITSETPOWERPOLICYEVENTCALLBACKS           pfnWdfCxDeviceInitSetPowerPolicyEventCallbacks;
    PFN_WDFDEVICESETDEVICEINTERFACESTATEEX                    pfnWdfDeviceSetDeviceInterfaceStateEx;
    PFN_WDFDEVICEWDMASSIGNPOWERFRAMEWORKSETTINGS              pfnWdfDeviceWdmAssignPowerFrameworkSettings;

} WDFFUNCTIONS, *PWDFFUNCTIONS;


typedef struct _WDFSTRUCTURES {

    size_t  SIZE_WDF_CHILD_ADDRESS_DESCRIPTION_HEADER;
    size_t  SIZE_WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
    size_t  SIZE_WDF_CHILD_LIST_CONFIG;
    size_t  SIZE_WDF_CHILD_LIST_ITERATOR;
    size_t  SIZE_WDF_CHILD_RETRIEVE_INFO;
    size_t  SIZE_WDF_CLASS_BIND_INFO;
    size_t  SIZE_WDF_CLASS_BIND_INFO2;
    size_t  SIZE_WDF_CLASS_LIBRARY_INFO;
    size_t  SIZE_WDF_CLASS_VERSION;
    size_t  SIZE_WDF_COMPANION_EVENT_CALLBACKS;
    size_t  SIZE_WDF_CUSTOM_TYPE_CONTEXT;
    size_t  SIZE_WDF_DEVICE_INTERFACE_PROPERTY_DATA;
    size_t  SIZE_WDF_DEVICE_PNP_CAPABILITIES;
    size_t  SIZE_WDF_DEVICE_PNP_NOTIFICATION_DATA;
    size_t  SIZE_WDF_DEVICE_POWER_CAPABILITIES;
    size_t  SIZE_WDF_DEVICE_POWER_NOTIFICATION_DATA;
    size_t  SIZE_WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS;
    size_t  SIZE_WDF_DEVICE_POWER_POLICY_NOTIFICATION_DATA;
    size_t  SIZE_WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS;
    size_t  SIZE_WDF_DEVICE_PROPERTY_DATA;
    size_t  SIZE_WDF_DEVICE_STATE;
    size_t  SIZE_WDF_DRIVER_CONFIG;
    size_t  SIZE_WDF_DRIVER_GLOBALS;
    size_t  SIZE_WDF_DRIVER_VERSION_AVAILABLE_PARAMS;
    size_t  SIZE_WDF_FDO_EVENT_CALLBACKS;
    size_t  SIZE_WDF_FILEOBJECT_CONFIG;
    size_t  SIZE_WDF_INTERRUPT_CONFIG;
    size_t  SIZE_WDF_INTERRUPT_EXTENDED_POLICY;
    size_t  SIZE_WDF_INTERRUPT_INFO;
    size_t  SIZE_WDF_IO_FORWARD_PROGRESS_RESERVED_POLICY_SETTINGS;
    size_t  SIZE_WDF_IO_QUEUE_CONFIG;
    size_t  SIZE_WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY;
    size_t  SIZE_WDF_IO_TARGET_OPEN_PARAMS;
    size_t  SIZE_WDF_IO_TYPE_CONFIG;
    size_t  SIZE_WDF_MEMORY_DESCRIPTOR;
    size_t  SIZE_WDF_OBJECT_ATTRIBUTES;
    size_t  SIZE_WDF_OBJECT_CONTEXT_TYPE_INFO;
    size_t  SIZE_WDF_PDO_EVENT_CALLBACKS;
    size_t  SIZE_WDF_PNPPOWER_EVENT_CALLBACKS;
    size_t  SIZE_WDF_POWER_FRAMEWORK_SETTINGS;
    size_t  SIZE_WDF_POWER_POLICY_EVENT_CALLBACKS;
    size_t  SIZE_WDF_POWER_ROUTINE_TIMED_OUT_DATA;
    size_t  SIZE_WDF_QUERY_INTERFACE_CONFIG;
    size_t  SIZE_WDF_QUEUE_FATAL_ERROR_DATA;
    size_t  SIZE_WDF_REMOVE_LOCK_OPTIONS;
    size_t  SIZE_WDF_REQUEST_COMPLETION_PARAMS;
    size_t  SIZE_WDF_REQUEST_FATAL_ERROR_INFORMATION_LENGTH_MISMATCH_DATA;
    size_t  SIZE_WDF_REQUEST_FORWARD_OPTIONS;
    size_t  SIZE_WDF_REQUEST_PARAMETERS;
    size_t  SIZE_WDF_REQUEST_REUSE_PARAMS;
    size_t  SIZE_WDF_REQUEST_SEND_OPTIONS;
    size_t  SIZE_WDF_TASK_QUEUE_CONFIG;
    size_t  SIZE_WDF_TIMER_CONFIG;
    size_t  SIZE_WDF_TRIAGE_INFO;
    size_t  SIZE_WDF_USB_CONTINUOUS_READER_CONFIG;
    size_t  SIZE_WDF_USB_DEVICE_CREATE_CONFIG;
    size_t  SIZE_WDF_USB_DEVICE_INFORMATION;
    size_t  SIZE_WDF_USB_DEVICE_SELECT_CONFIG_PARAMS;
    size_t  SIZE_WDF_USB_INTERFACE_SELECT_SETTING_PARAMS;
    size_t  SIZE_WDF_USB_INTERFACE_SETTING_PAIR;
    size_t  SIZE_WDF_USB_PIPE_INFORMATION;
    size_t  SIZE_WDF_USB_REQUEST_COMPLETION_PARAMS;
    size_t  SIZE_WDF_WMI_INSTANCE_CONFIG;
    size_t  SIZE_WDF_WMI_PROVIDER_CONFIG;
    size_t  SIZE_WDF_WORKITEM_CONFIG;
    size_t  SIZE_WDFCONTEXT_TRIAGE_INFO;
    size_t  SIZE_WDFCONTEXTTYPE_TRIAGE_INFO;
    size_t  SIZE_WDFCX_FILEOBJECT_CONFIG;
    size_t  SIZE_WDFCX_PNPPOWER_EVENT_CALLBACKS;
    size_t  SIZE_WDFDEVICE_TRIAGE_INFO;
    size_t  SIZE_WDFFWDPROGRESS_TRIAGE_INFO;
    size_t  SIZE_WDFIRP_TRIAGE_INFO;
    size_t  SIZE_WDFIRPQUEUE_TRIAGE_INFO;
    size_t  SIZE_WDFMEMORY_OFFSET;
    size_t  SIZE_WDFOBJECT_TRIAGE_INFO;
    size_t  SIZE_WDFQUEUE_TRIAGE_INFO;
    size_t  SIZE_WDFREQUEST_TRIAGE_INFO;
    size_t  SIZE_WDFCX_POWER_POLICY_EVENT_CALLBACKS;

} WDFSTRUCTURES, *PWDFSTRUCTURES;


typedef struct _WDFVERSION {

    ULONG         Size;
    ULONG         FuncCount;
    WDFFUNCTIONS  Functions;
    ULONG         StructCount;
    WDFSTRUCTURES Structures;

} WDFVERSION, *PWDFVERSION;


_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfCollectionCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES CollectionAttributes,
    _Out_
    WDFCOLLECTION* Collection
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
ULONG
NTAPI
WDFEXPORT(WdfCollectionGetCount)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCOLLECTION Collection
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfCollectionAdd)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCOLLECTION Collection,
    _In_
    WDFOBJECT Object
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfCollectionRemove)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCOLLECTION Collection,
    _In_
    WDFOBJECT Item
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfCollectionRemoveItem)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCOLLECTION Collection,
    _In_
    ULONG Index
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFOBJECT
NTAPI
WDFEXPORT(WdfCollectionGetItem)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCOLLECTION Collection,
    _In_
    ULONG Index
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFOBJECT
NTAPI
WDFEXPORT(WdfCollectionGetFirstItem)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCOLLECTION Collection
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFOBJECT
NTAPI
WDFEXPORT(WdfCollectionGetLastItem)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCOLLECTION Collection
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitSetCompanionEventCallbacks)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_COMPANION_EVENT_CALLBACKS CompanionEventCallbacks
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfCompanionCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _Inout_
    PWDFDEVICE_INIT* DeviceInit,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES DeviceAttributes,
    _Out_
    WDFCOMPANION* Companion
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
HANDLE
NTAPI
WDFEXPORT(WdfCompanionWdmGetSecureDeviceHandle)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCOMPANION Companion
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfCompanionCreateTaskQueue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCOMPANION Companion,
    _In_
    PWDF_TASK_QUEUE_CONFIG Config,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES QueueAttributes,
    _Out_opt_
    WDFTASKQUEUE* Queue
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
PWDFCXDEVICE_INIT
NTAPI
WDFEXPORT(WdfCxDeviceInitAllocate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfCxDeviceInitSetRequestAttributes)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFCXDEVICE_INIT CxDeviceInit,
    _In_
    PWDF_OBJECT_ATTRIBUTES RequestAttributes
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfCxDeviceInitSetFileObjectConfig)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFCXDEVICE_INIT CxDeviceInit,
    _In_
    PWDFCX_FILEOBJECT_CONFIG CxFileObjectConfig,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES FileObjectAttributes
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfCxDeviceInitSetPnpPowerEventCallbacks)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFCXDEVICE_INIT CxDeviceInit,
    _In_
    PWDFCX_PNPPOWER_EVENT_CALLBACKS CxPnpPowerCallbacks
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfCxDeviceInitSetPowerPolicyEventCallbacks)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFCXDEVICE_INIT CxDeviceInit,
    _In_
    PWDFCX_POWER_POLICY_EVENT_CALLBACKS CxPowerPolicyCallbacks
    );

WDFAPI
VOID
NTAPI
WDFEXPORT(WdfCxVerifierKeBugCheck)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    WDFOBJECT Object,
    _In_
    ULONG BugCheckCode,
    _In_
    ULONG_PTR BugCheckParameter1,
    _In_
    ULONG_PTR BugCheckParameter2,
    _In_
    ULONG_PTR BugCheckParameter3,
    _In_
    ULONG_PTR BugCheckParameter4
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfCxDeviceInitAllocateContext)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_OBJECT_ATTRIBUTES ContextAttributes,
    _Outptr_opt_
    PVOID* Context
    );

_IRQL_requires_max_(DISPATCH_LEVEL+1)
WDFAPI
PVOID
NTAPI
WDFEXPORT(WdfCxDeviceInitGetTypedContextWorker)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PCWDF_OBJECT_CONTEXT_TYPE_INFO TypeInfo
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceGetDeviceState)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _Inout_
    PWDF_DEVICE_STATE DeviceState
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceSetDeviceState)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_STATE DeviceState
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceWdmDispatchIrp)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PIRP Irp,
    _In_
    WDFCONTEXT DispatchContext
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceWdmDispatchIrpToIoQueue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PIRP Irp,
    _In_
    WDFQUEUE Queue,
    _In_
    ULONG Flags
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFDRIVER
NTAPI
WDFEXPORT(WdfDeviceGetDriver)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFIOTARGET
NTAPI
WDFEXPORT(WdfDeviceGetIoTarget)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceAssignS0IdleSettings)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_POWER_POLICY_IDLE_SETTINGS Settings
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceAssignSxWakeSettings)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_POWER_POLICY_WAKE_SETTINGS Settings
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceOpenRegistryKey)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    ULONG DeviceInstanceKeyType,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceOpenDevicemapKey)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PCUNICODE_STRING KeyName,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitSetPnpPowerEventCallbacks)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_PNPPOWER_EVENT_CALLBACKS PnpPowerEventCallbacks
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitSetPowerPolicyEventCallbacks)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_POWER_POLICY_EVENT_CALLBACKS PowerPolicyEventCallbacks
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitSetPowerPolicyOwnership)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    BOOLEAN IsPowerPolicyOwner
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitSetIoType)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    WDF_DEVICE_IO_TYPE IoType
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitSetFileObjectConfig)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_FILEOBJECT_CONFIG FileObjectConfig,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES FileObjectAttributes
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitSetRequestAttributes)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_OBJECT_ATTRIBUTES RequestAttributes
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _Inout_
    PWDFDEVICE_INIT* DeviceInit,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES DeviceAttributes,
    _Out_
    WDFDEVICE* Device
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceSetStaticStopRemove)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    BOOLEAN Stoppable
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceCreateDeviceInterface)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    CONST GUID* InterfaceClassGUID,
    _In_opt_
    PCUNICODE_STRING ReferenceString
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceSetDeviceInterfaceState)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    CONST GUID* InterfaceClassGUID,
    _In_opt_
    PCUNICODE_STRING ReferenceString,
    _In_
    BOOLEAN IsInterfaceEnabled
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceSetDeviceInterfaceStateEx)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    CONST GUID* InterfaceClassGUID,
    _In_opt_
    PCUNICODE_STRING ReferenceString,
    _In_
    BOOLEAN IsInterfaceEnabled
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceRetrieveDeviceInterfaceString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    CONST GUID* InterfaceClassGUID,
    _In_opt_
    PCUNICODE_STRING ReferenceString,
    _In_
    WDFSTRING String
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceCreateSymbolicLink)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PCUNICODE_STRING SymbolicLinkName
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceQueryProperty)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    DEVICE_REGISTRY_PROPERTY DeviceProperty,
    _In_
    ULONG BufferLength,
    _Out_writes_bytes_all_(BufferLength)
    PVOID PropertyBuffer,
    _Out_
    PULONG ResultLength
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceAllocAndQueryProperty)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    DEVICE_REGISTRY_PROPERTY DeviceProperty,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES PropertyMemoryAttributes,
    _Out_
    WDFMEMORY* PropertyMemory
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceSetPnpCapabilities)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_PNP_CAPABILITIES PnpCapabilities
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceSetPowerCapabilities)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_POWER_CAPABILITIES PowerCapabilities
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceSetFailed)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    WDF_DEVICE_FAILED_ACTION FailedAction
    );

_Must_inspect_result_
_When_(WaitForD0 == 0, _IRQL_requires_max_(DISPATCH_LEVEL))
_When_(WaitForD0 != 0, _IRQL_requires_max_(PASSIVE_LEVEL))
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceStopIdleNoTrack)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    BOOLEAN WaitForD0
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceResumeIdleNoTrack)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device
    );

_Must_inspect_result_
_When_(WaitForD0 == 0, _IRQL_requires_max_(DISPATCH_LEVEL))
_When_(WaitForD0 != 0, _IRQL_requires_max_(PASSIVE_LEVEL))
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceStopIdleActual)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    BOOLEAN WaitForD0,
    _In_opt_
    PVOID Tag,
    _In_
    LONG Line,
    _In_z_
    PCCH File
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceResumeIdleActual)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_opt_
    PVOID Tag,
    _In_
    LONG Line,
    _In_z_
    PCCH File
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFFILEOBJECT
NTAPI
WDFEXPORT(WdfDeviceGetFileObject)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PFILE_OBJECT FileObject
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFQUEUE
NTAPI
WDFEXPORT(WdfDeviceGetDefaultQueue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceConfigureRequestDispatching)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    WDFQUEUE Queue,
    _In_
    _Strict_type_match_
    WDF_REQUEST_TYPE RequestType
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceConfigureWdmIrpDispatchCallback)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_opt_
    WDFDRIVER Driver,
    _In_
    UCHAR MajorFunction,
    _In_
    PFN_WDFDEVICE_WDM_IRP_DISPATCH EvtDeviceWdmIrpDispatch,
    _In_opt_
    WDFCONTEXT DriverContext
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
POWER_ACTION
NTAPI
WDFEXPORT(WdfDeviceGetSystemPowerAction)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceWdmAssignPowerFrameworkSettings)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_POWER_FRAMEWORK_SETTINGS PowerFrameworkSettings
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitSetReleaseHardwareOrderOnFailure)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    WDF_RELEASE_HARDWARE_ORDER_ON_FAILURE ReleaseHardwareOrderOnFailure
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitSetIoTypeEx)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_IO_TYPE_CONFIG IoTypeConfig
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDevicePostEvent)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    REFGUID EventGuid,
    _In_
    WDF_EVENT_TYPE WdfEventType,
    _In_reads_bytes_(DataSizeCb)
    BYTE* Data,
    _In_
    ULONG DataSizeCb
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceMapIoSpace)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PHYSICAL_ADDRESS PhysicalAddress,
    _In_
    SIZE_T NumberOfBytes,
    _In_
    MEMORY_CACHING_TYPE CacheType,
    _Out_
    PVOID* PseudoBaseAddress
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceUnmapIoSpace)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PVOID PseudoBaseAddress,
    _In_
    SIZE_T NumberOfBytes
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
PVOID
NTAPI
WDFEXPORT(WdfDeviceGetHardwareRegisterMappedAddress)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PVOID PseudoBaseAddress
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
SIZE_T
NTAPI
WDFEXPORT(WdfDeviceReadFromHardware)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    WDF_DEVICE_HWACCESS_TARGET_TYPE Type,
    _In_
    WDF_DEVICE_HWACCESS_TARGET_SIZE Size,
    _In_
    PVOID TargetAddress,
    _Out_writes_all_opt_(Count)
    PVOID Buffer,
    _In_opt_
    ULONG Count
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceWriteToHardware)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    WDF_DEVICE_HWACCESS_TARGET_TYPE Type,
    _In_
    WDF_DEVICE_HWACCESS_TARGET_SIZE Size,
    _In_
    PVOID TargetAddress,
    _In_
    SIZE_T Value,
    _In_reads_opt_(Count)
    PVOID Buffer,
    _In_opt_
    ULONG Count
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceAssignInterfaceProperty)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_INTERFACE_PROPERTY_DATA PropertyData,
    _In_
    DEVPROPTYPE Type,
    _In_
    ULONG BufferLength,
    _In_reads_bytes_opt_(BufferLength)
    PVOID PropertyBuffer
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceAllocAndQueryInterfaceProperty)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_INTERFACE_PROPERTY_DATA PropertyData,
    _In_
    POOL_TYPE PoolType,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES PropertyMemoryAttributes,
    _Out_
    WDFMEMORY* PropertyMemory,
    _Out_
    PDEVPROPTYPE Type
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceQueryInterfaceProperty)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_INTERFACE_PROPERTY_DATA PropertyData,
    _In_
    ULONG BufferLength,
    _Out_writes_bytes_opt_(BufferLength)
    PVOID PropertyBuffer,
    _Out_
    PULONG ResultLength,
    _Out_
    PDEVPROPTYPE Type
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceGetDeviceStackIoType)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _Out_
    WDF_DEVICE_IO_TYPE* ReadWriteIoType,
    _Out_
    WDF_DEVICE_IO_TYPE* IoControlIoType
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceRetrieveDeviceDirectoryString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    WDFSTRING String
    );

_Must_inspect_result_
_IRQL_requires_max_(APC_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceQueryPropertyEx)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_PROPERTY_DATA DeviceProperty,
    _In_
    ULONG BufferLength,
    _Out_
    PVOID PropertyBuffer,
    _Out_
    PULONG RequiredSize,
    _Out_
    PDEVPROPTYPE Type
    );

_Must_inspect_result_
_IRQL_requires_max_(APC_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceAllocAndQueryPropertyEx)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_PROPERTY_DATA DeviceProperty,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES PropertyMemoryAttributes,
    _Out_
    WDFMEMORY* PropertyMemory,
    _Out_
    PDEVPROPTYPE Type
    );

_Must_inspect_result_
_IRQL_requires_max_(APC_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceAssignProperty)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_DEVICE_PROPERTY_DATA DeviceProperty,
    _In_
    DEVPROPTYPE Type,
    _In_
    ULONG Size,
    _In_opt_
    PVOID Data
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFIOTARGET
NTAPI
WDFEXPORT(WdfDeviceGetSelfIoTarget)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitAllowSelfIoTarget)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDriverCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PDRIVER_OBJECT DriverObject,
    _In_
    PCUNICODE_STRING RegistryPath,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES DriverAttributes,
    _In_
    PWDF_DRIVER_CONFIG DriverConfig,
    _Out_opt_
    WDFDRIVER* Driver
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
PWSTR
NTAPI
WDFEXPORT(WdfDriverGetRegistryPath)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDRIVER Driver
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDriverOpenParametersRegistryKey)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDRIVER Driver,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDriverRetrieveVersionString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDRIVER Driver,
    _In_
    WDFSTRING String
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfDriverIsVersionAvailable)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDRIVER Driver,
    _In_
    PWDF_DRIVER_VERSION_AVAILABLE_PARAMS VersionAvailableParams
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL + 1)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDriverErrorReportApiMissing)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDRIVER Driver,
    _In_opt_
    PCWSTR FrameworkExtensionName,
    _In_
    ULONG ApiIndex,
    _In_
    BOOLEAN DoesApiReturnNtstatus
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDriverOpenPersistentStateRegistryKey)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDRIVER Driver,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDriverRetrieveDriverDataDirectoryString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDRIVER Driver,
    _In_
    WDFSTRING String
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfFdoInitOpenRegistryKey)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    ULONG DeviceInstanceKeyType,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfFdoInitQueryProperty)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    DEVICE_REGISTRY_PROPERTY DeviceProperty,
    _In_
    ULONG BufferLength,
    _Out_writes_bytes_all_opt_(BufferLength)
    PVOID PropertyBuffer,
    _Out_
    PULONG ResultLength
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfFdoInitAllocAndQueryProperty)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    DEVICE_REGISTRY_PROPERTY DeviceProperty,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES PropertyMemoryAttributes,
    _Out_
    WDFMEMORY* PropertyMemory
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfFdoInitQueryPropertyEx)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_DEVICE_PROPERTY_DATA DeviceProperty,
    _In_
    ULONG BufferLength,
    _Out_
    PVOID PropertyBuffer,
    _Out_
    PULONG ResultLength,
    _Out_
    PDEVPROPTYPE Type
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfFdoInitAllocAndQueryPropertyEx)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit,
    _In_
    PWDF_DEVICE_PROPERTY_DATA DeviceProperty,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES PropertyMemoryAttributes,
    _Out_
    WDFMEMORY* PropertyMemory,
    _Out_
    PDEVPROPTYPE Type
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfFdoInitSetFilter)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
PUNICODE_STRING
NTAPI
WDFEXPORT(WdfFileObjectGetFileName)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFFILEOBJECT FileObject
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFDEVICE
NTAPI
WDFEXPORT(WdfFileObjectGetDevice)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFFILEOBJECT FileObject
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
ULONG
NTAPI
WDFEXPORT(WdfFileObjectGetInitiatorProcessId)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFFILEOBJECT FileObject
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
WDFFILEOBJECT
NTAPI
WDFEXPORT(WdfFileObjectGetRelatedFileObject)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFFILEOBJECT FileObject
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfDeviceInitEnableHidInterface)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDFDEVICE_INIT DeviceInit
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfDeviceHidNotifyPresence)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    BOOLEAN IsPresent
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfInterruptCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_INTERRUPT_CONFIG Configuration,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _Out_
    WDFINTERRUPT* Interrupt
    );

WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfInterruptQueueDpcForIsr)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFINTERRUPT Interrupt
    );

WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfInterruptQueueWorkItemForIsr)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFINTERRUPT Interrupt
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfInterruptSynchronize)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFINTERRUPT Interrupt,
    _In_
    PFN_WDF_INTERRUPT_SYNCHRONIZE Callback,
    _In_
    WDFCONTEXT Context
    );

_IRQL_requires_max_(DISPATCH_LEVEL + 1)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfInterruptAcquireLock)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    _Requires_lock_not_held_(_Curr_)
    _Acquires_lock_(_Curr_)
    WDFINTERRUPT Interrupt
    );

_IRQL_requires_max_(DISPATCH_LEVEL + 1)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfInterruptReleaseLock)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    _Requires_lock_held_(_Curr_)
    _Releases_lock_(_Curr_)
    WDFINTERRUPT Interrupt
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfInterruptEnable)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFINTERRUPT Interrupt
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfInterruptDisable)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFINTERRUPT Interrupt
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfInterruptGetInfo)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFINTERRUPT Interrupt,
    _Inout_
    PWDF_INTERRUPT_INFO Info
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfInterruptSetPolicy)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFINTERRUPT Interrupt,
    _In_
    WDF_INTERRUPT_POLICY Policy,
    _In_
    WDF_INTERRUPT_PRIORITY Priority,
    _In_
    KAFFINITY TargetProcessorSet
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfInterruptSetExtendedPolicy)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFINTERRUPT Interrupt,
    _In_
    PWDF_INTERRUPT_EXTENDED_POLICY PolicyAndGroup
    );

WDFAPI
WDFDEVICE
NTAPI
WDFEXPORT(WdfInterruptGetDevice)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFINTERRUPT Interrupt
    );

_Must_inspect_result_
_Post_satisfies_(return == 1 || return == 0)
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfInterruptTryToAcquireLock)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    _Requires_lock_not_held_(_Curr_)
    _When_(return!=0, _Acquires_lock_(_Curr_))
    WDFINTERRUPT Interrupt
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoQueueCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_IO_QUEUE_CONFIG Config,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES QueueAttributes,
    _Out_opt_
    WDFQUEUE* Queue
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDF_IO_QUEUE_STATE
NTAPI
WDFEXPORT(WdfIoQueueGetState)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _Out_opt_
    PULONG QueueRequests,
    _Out_opt_
    PULONG DriverRequests
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoQueueStart)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoQueueStop)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE StopComplete,
    _When_(StopComplete != 0, _In_)
    _When_(StopComplete == 0, _In_opt_)
    WDFCONTEXT Context
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoQueueStopSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFDEVICE
NTAPI
WDFEXPORT(WdfIoQueueGetDevice)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoQueueRetrieveNextRequest)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _Out_
    WDFREQUEST* OutRequest
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoQueueRetrieveRequestByFileObject)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _In_
    WDFFILEOBJECT FileObject,
    _Out_
    WDFREQUEST* OutRequest
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoQueueFindRequest)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _In_opt_
    WDFREQUEST FoundRequest,
    _In_opt_
    WDFFILEOBJECT FileObject,
    _Inout_opt_
    PWDF_REQUEST_PARAMETERS Parameters,
    _Out_
    WDFREQUEST* OutRequest
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoQueueRetrieveFoundRequest)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST FoundRequest,
    _Out_
    WDFREQUEST* OutRequest
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoQueueDrainSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoQueueDrain)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE DrainComplete,
    _When_(DrainComplete != 0, _In_)
    _When_(DrainComplete == 0, _In_opt_)
    WDFCONTEXT Context
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoQueuePurgeSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoQueuePurge)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE PurgeComplete,
    _When_(PurgeComplete != 0, _In_)
    _When_(PurgeComplete == 0, _In_opt_)
    WDFCONTEXT Context
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoQueueReadyNotify)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _In_opt_
    PFN_WDF_IO_QUEUE_STATE QueueReady,
    _In_opt_
    WDFCONTEXT Context
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoQueueStopAndPurge)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE StopAndPurgeComplete,
    _When_(StopAndPurgeComplete != 0, _In_)
    _When_(StopAndPurgeComplete == 0, _In_opt_)
    WDFCONTEXT Context
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoQueueStopAndPurgeSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoTargetCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES IoTargetAttributes,
    _Out_
    WDFIOTARGET* IoTarget
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoTargetOpen)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget,
    _In_
    PWDF_IO_TARGET_OPEN_PARAMS OpenParams
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoTargetCloseForQueryRemove)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoTargetClose)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoTargetStart)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget
    );

_When_(Action == 3, _IRQL_requires_max_(DISPATCH_LEVEL))
_When_(Action == 0 || Action == 1 || Action == 2, _IRQL_requires_max_(PASSIVE_LEVEL))
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoTargetStop)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget,
    _In_
    _Strict_type_match_
    WDF_IO_TARGET_SENT_IO_ACTION Action
    );

_When_(Action == 2, _IRQL_requires_max_(DISPATCH_LEVEL))
_When_(Action == 0 || Action == 1, _IRQL_requires_max_(PASSIVE_LEVEL))
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfIoTargetPurge)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget,
    _In_
    _Strict_type_match_
    WDF_IO_TARGET_PURGE_IO_ACTION Action
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDF_IO_TARGET_STATE
NTAPI
WDFEXPORT(WdfIoTargetGetState)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFDEVICE
NTAPI
WDFEXPORT(WdfIoTargetGetDevice)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
HANDLE
NTAPI
WDFEXPORT(WdfIoTargetWdmGetTargetFileHandle)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoTargetSendReadSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget,
    _In_opt_
    WDFREQUEST Request,
    _In_opt_
    PWDF_MEMORY_DESCRIPTOR OutputBuffer,
    _In_opt_
    PLONGLONG DeviceOffset,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    _Out_opt_
    PULONG_PTR BytesRead
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoTargetFormatRequestForRead)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget,
    _In_
    WDFREQUEST Request,
    _In_opt_
    WDFMEMORY OutputBuffer,
    _In_opt_
    PWDFMEMORY_OFFSET OutputBufferOffset,
    _In_opt_
    PLONGLONG DeviceOffset
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoTargetSendWriteSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget,
    _In_opt_
    WDFREQUEST Request,
    _In_opt_
    PWDF_MEMORY_DESCRIPTOR InputBuffer,
    _In_opt_
    PLONGLONG DeviceOffset,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    _Out_opt_
    PULONG_PTR BytesWritten
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoTargetFormatRequestForWrite)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget,
    _In_
    WDFREQUEST Request,
    _In_opt_
    WDFMEMORY InputBuffer,
    _In_opt_
    PWDFMEMORY_OFFSET InputBufferOffset,
    _In_opt_
    PLONGLONG DeviceOffset
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoTargetSendIoctlSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget,
    _In_opt_
    WDFREQUEST Request,
    _In_
    ULONG IoctlCode,
    _In_opt_
    PWDF_MEMORY_DESCRIPTOR InputBuffer,
    _In_opt_
    PWDF_MEMORY_DESCRIPTOR OutputBuffer,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    _Out_opt_
    PULONG_PTR BytesReturned
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoTargetFormatRequestForIoctl)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget,
    _In_
    WDFREQUEST Request,
    _In_
    ULONG IoctlCode,
    _In_opt_
    WDFMEMORY InputBuffer,
    _In_opt_
    PWDFMEMORY_OFFSET InputBufferOffset,
    _In_opt_
    WDFMEMORY OutputBuffer,
    _In_opt_
    PWDFMEMORY_OFFSET OutputBufferOffset
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfIoTargetSelfAssignDefaultIoQueue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFIOTARGET IoTarget,
    _In_
    WDFQUEUE Queue
    );

_Must_inspect_result_
_When_(PoolType == 1 || PoolType == 257, _IRQL_requires_max_(APC_LEVEL))
_When_(PoolType == 0 || PoolType == 256 || PoolType == 512, _IRQL_requires_max_(DISPATCH_LEVEL))
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfMemoryCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    ULONG PoolTag,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize,
    _Out_
    WDFMEMORY* Memory,
    _Outptr_opt_result_bytebuffer_(BufferSize)
    PVOID* Buffer
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfMemoryCreatePreallocated)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _In_ __drv_aliasesMem
    PVOID Buffer,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize,
    _Out_
    WDFMEMORY* Memory
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
PVOID
NTAPI
WDFEXPORT(WdfMemoryGetBuffer)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFMEMORY Memory,
    _Out_opt_
    size_t* BufferSize
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfMemoryAssignBuffer)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFMEMORY Memory,
    _Pre_notnull_ _Pre_writable_byte_size_(BufferSize)
    PVOID Buffer,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfMemoryCopyToBuffer)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFMEMORY SourceMemory,
    _In_
    size_t SourceOffset,
    _Out_writes_bytes_( NumBytesToCopyTo )
    PVOID Buffer,
    _In_
    _When_(NumBytesToCopyTo == 0, __drv_reportError(NumBytesToCopyTo cannot be zero))
    size_t NumBytesToCopyTo
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfMemoryCopyFromBuffer)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFMEMORY DestinationMemory,
    _In_
    size_t DestinationOffset,
    _In_
    PVOID Buffer,
    _In_
    _When_(NumBytesToCopyFrom == 0, __drv_reportError(NumBytesToCopyFrom cannot be zero))
    size_t NumBytesToCopyFrom
    );

_IRQL_requires_max_(DISPATCH_LEVEL+1)
WDFAPI
PVOID
FASTCALL
WDFEXPORT(WdfObjectGetTypedContextWorker)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFOBJECT Handle,
    _In_
    PCWDF_OBJECT_CONTEXT_TYPE_INFO TypeInfo
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfObjectAllocateContext)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFOBJECT Handle,
    _In_
    PWDF_OBJECT_ATTRIBUTES ContextAttributes,
    _Outptr_opt_
    PVOID* Context
    );

_IRQL_requires_max_(DISPATCH_LEVEL+1)
WDFAPI
WDFOBJECT
FASTCALL
WDFEXPORT(WdfObjectContextGetObject)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PVOID ContextPointer
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfObjectReferenceActual)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFOBJECT Handle,
    _In_opt_
    PVOID Tag,
    _In_
    LONG Line,
    _In_z_
    PCCH File
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfObjectDereferenceActual)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFOBJECT Handle,
    _In_opt_
    PVOID Tag,
    _In_
    LONG Line,
    _In_z_
    PCCH File
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfObjectCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _Out_
    WDFOBJECT* Object
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfObjectDelete)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFOBJECT Object
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfObjectQuery)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFOBJECT Object,
    _In_
    CONST GUID* Guid,
    _In_
    ULONG QueryBufferLength,
    _Out_writes_bytes_(QueryBufferLength)
    PVOID QueryBuffer
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryOpenKey)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    WDFKEY ParentKey,
    _In_
    PCUNICODE_STRING KeyName,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryCreateKey)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    WDFKEY ParentKey,
    _In_
    PCUNICODE_STRING KeyName,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_
    ULONG CreateOptions,
    _Out_opt_
    PULONG CreateDisposition,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRegistryClose)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
HANDLE
NTAPI
WDFEXPORT(WdfRegistryWdmGetHandle)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryRemoveKey)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryRemoveValue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryQueryValue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG ValueLength,
    _Out_writes_bytes_opt_( ValueLength)
    PVOID Value,
    _Out_opt_
    PULONG ValueLengthQueried,
    _Out_opt_
    PULONG ValueType
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryQueryMemory)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES MemoryAttributes,
    _Out_
    WDFMEMORY* Memory,
    _Out_opt_
    PULONG ValueType
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryQueryMultiString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES StringsAttributes,
    _In_
    WDFCOLLECTION Collection
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryQueryUnicodeString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _Out_opt_
    PUSHORT ValueByteLength,
    _Inout_opt_
    PUNICODE_STRING Value
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryQueryString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    WDFSTRING String
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryQueryULong)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _Out_
    PULONG Value
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryAssignValue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG ValueType,
    _In_
    ULONG ValueLength,
    _In_reads_( ValueLength)
    PVOID Value
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryAssignMemory)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG ValueType,
    _In_
    WDFMEMORY Memory,
    _In_opt_
    PWDFMEMORY_OFFSET MemoryOffsets
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryAssignMultiString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    WDFCOLLECTION StringsCollection
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryAssignUnicodeString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    PCUNICODE_STRING Value
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryAssignString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    WDFSTRING String
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRegistryAssignULong)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG Value
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES RequestAttributes,
    _In_opt_
    WDFIOTARGET IoTarget,
    _Out_
    WDFREQUEST* Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
ULONG
NTAPI
WDFEXPORT(WdfRequestGetRequestorProcessId)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestReuse)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    PWDF_REQUEST_REUSE_PARAMS ReuseParams
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestChangeTarget)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    WDFIOTARGET IoTarget
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestFormatRequestUsingCurrentType)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
_When_(Options->Flags & WDF_REQUEST_SEND_OPTION_SYNCHRONOUS == 0, _Must_inspect_result_)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfRequestSend)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    WDFIOTARGET Target,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS Options
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestGetStatus)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestMarkCancelable)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    PFN_WDF_REQUEST_CANCEL EvtRequestCancel
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestMarkCancelableEx)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    PFN_WDF_REQUEST_CANCEL EvtRequestCancel
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestUnmarkCancelable)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfRequestIsCanceled)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfRequestCancelSentRequest)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(APC_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfRequestIsFrom32BitProcess)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestSetCompletionRoutine)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_opt_
    PFN_WDF_REQUEST_COMPLETION_ROUTINE CompletionRoutine,
    _In_opt_ __drv_aliasesMem
    WDFCONTEXT CompletionContext
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestGetCompletionParams)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Inout_
    PWDF_REQUEST_COMPLETION_PARAMS Params
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestAllocateTimer)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestComplete)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    NTSTATUS Status
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestCompleteWithInformation)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    NTSTATUS Status,
    _In_
    ULONG_PTR Information
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestGetParameters)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Inout_
    PWDF_REQUEST_PARAMETERS Parameters
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestRetrieveInputMemory)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Out_
    WDFMEMORY* Memory
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestRetrieveOutputMemory)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Out_
    WDFMEMORY* Memory
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestRetrieveInputBuffer)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    size_t MinimumRequiredLength,
    _Outptr_result_bytebuffer_(*Length)
    PVOID* Buffer,
    _Out_opt_
    size_t* Length
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestRetrieveOutputBuffer)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    size_t MinimumRequiredSize,
    _Outptr_result_bytebuffer_(*Length)
    PVOID* Buffer,
    _Out_opt_
    size_t* Length
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestSetInformation)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    ULONG_PTR Information
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
ULONG_PTR
NTAPI
WDFEXPORT(WdfRequestGetInformation)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFFILEOBJECT
NTAPI
WDFEXPORT(WdfRequestGetFileObject)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
KPROCESSOR_MODE
NTAPI
WDFEXPORT(WdfRequestGetRequestorMode)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestForwardToIoQueue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    WDFQUEUE DestinationQueue
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFQUEUE
NTAPI
WDFEXPORT(WdfRequestGetIoQueue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestRequeue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestStopAcknowledge)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    BOOLEAN Requeue
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestImpersonate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
    _In_
    PFN_WDF_REQUEST_IMPERSONATE EvtRequestImpersonate,
    _In_opt_
    PVOID Context
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfRequestIsFromUserModeDriver)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestSetUserModeDriverInitiatedIo)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    BOOLEAN IsUserModeDriverInitiated
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfRequestGetUserModeDriverInitiatedIo)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfRequestSetActivityId)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    LPGUID ActivityId
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfRequestRetrieveActivityId)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Out_
    LPGUID ActivityId
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
WDF_DEVICE_IO_TYPE
NTAPI
WDFEXPORT(WdfRequestGetEffectiveIoType)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
ULONG
NTAPI
WDFEXPORT(WdfCmResourceListGetCount)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCMRESLIST List
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
PCM_PARTIAL_RESOURCE_DESCRIPTOR
NTAPI
WDFEXPORT(WdfCmResourceListGetDescriptor)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFCMRESLIST List,
    _In_
    ULONG Index
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfStringCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PCUNICODE_STRING UnicodeString,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES StringAttributes,
    _Out_
    WDFSTRING* String
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfStringGetUnicodeString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFSTRING String,
    _Out_
    PUNICODE_STRING UnicodeString
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfObjectAcquireLock)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    _Requires_lock_not_held_(_Curr_)
    _Acquires_lock_(_Curr_)
    WDFOBJECT Object
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfObjectReleaseLock)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    _Requires_lock_held_(_Curr_)
    _Releases_lock_(_Curr_)
    WDFOBJECT Object
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfWaitLockCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES LockAttributes,
    _Out_
    WDFWAITLOCK* Lock
    );

_When_(Timeout == NULL, _IRQL_requires_max_(PASSIVE_LEVEL))
_When_(Timeout != NULL && *Timeout == 0, _IRQL_requires_max_(DISPATCH_LEVEL))
_When_(Timeout != NULL && *Timeout != 0, _IRQL_requires_max_(PASSIVE_LEVEL))
_Always_(_When_(Timeout == NULL, _Acquires_lock_(Lock)))
_When_(Timeout != NULL && return == STATUS_SUCCESS, _Acquires_lock_(Lock))
_When_(Timeout != NULL, _Must_inspect_result_)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfWaitLockAcquire)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    _Requires_lock_not_held_(_Curr_)
    WDFWAITLOCK Lock,
    _In_opt_
    PLONGLONG Timeout
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfWaitLockRelease)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    _Requires_lock_held_(_Curr_)
    _Releases_lock_(_Curr_)
    WDFWAITLOCK Lock
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfSpinLockCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES SpinLockAttributes,
    _Out_
    WDFSPINLOCK* SpinLock
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_raises_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfSpinLockAcquire)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    _Requires_lock_not_held_(_Curr_)
    _Acquires_lock_(_Curr_)
    _IRQL_saves_
    WDFSPINLOCK SpinLock
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_min_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfSpinLockRelease)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    _Requires_lock_held_(_Curr_)
    _Releases_lock_(_Curr_)
    _IRQL_restores_
    WDFSPINLOCK SpinLock
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfTimerCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDF_TIMER_CONFIG Config,
    _In_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _Out_
    WDFTIMER* Timer
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfTimerStart)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFTIMER Timer,
    _In_
    LONGLONG DueTime
    );

_When_(Wait == __true, _IRQL_requires_max_(PASSIVE_LEVEL))
_When_(Wait == __false, _IRQL_requires_max_(DISPATCH_LEVEL))
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfTimerStop)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFTIMER Timer,
    _In_
    BOOLEAN Wait
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFOBJECT
NTAPI
WDFEXPORT(WdfTimerGetParentObject)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFTIMER Timer
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _Out_
    WDFUSBDEVICE* UsbDevice
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceCreateWithParameters)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_USB_DEVICE_CREATE_CONFIG Config,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _Out_
    WDFUSBDEVICE* UsbDevice
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceRetrieveInformation)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _Inout_
    PWDF_USB_DEVICE_INFORMATION Information
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfUsbTargetDeviceGetDeviceDescriptor)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _Out_
    PUSB_DEVICE_DESCRIPTOR UsbDeviceDescriptor
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceRetrieveConfigDescriptor)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _Out_writes_bytes_to_opt_(*ConfigDescriptorLength,*ConfigDescriptorLength)
    PVOID ConfigDescriptor,
    _Inout_
    PUSHORT ConfigDescriptorLength
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceQueryString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _In_opt_
    WDFREQUEST Request,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    _Out_writes_opt_(*NumCharacters)
    PUSHORT String,
    _Inout_
    PUSHORT NumCharacters,
    _In_
    UCHAR StringIndex,
    _In_opt_
    USHORT LangID
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceAllocAndQueryString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES StringMemoryAttributes,
    _Out_
    WDFMEMORY* StringMemory,
    _Out_opt_
    PUSHORT NumCharacters,
    _In_
    UCHAR StringIndex,
    _In_opt_
    USHORT LangID
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceFormatRequestForString)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _In_
    WDFREQUEST Request,
    _In_
    WDFMEMORY Memory,
    _In_opt_
    PWDFMEMORY_OFFSET Offset,
    _In_
    UCHAR StringIndex,
    _In_opt_
    USHORT LangID
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
UCHAR
NTAPI
WDFEXPORT(WdfUsbTargetDeviceGetNumInterfaces)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceSelectConfig)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES PipeAttributes,
    _Inout_
    PWDF_USB_DEVICE_SELECT_CONFIG_PARAMS Params
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceSendControlTransferSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _In_opt_
    WDFREQUEST Request,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    _In_
    PWDF_USB_CONTROL_SETUP_PACKET SetupPacket,
    _In_opt_
    PWDF_MEMORY_DESCRIPTOR MemoryDescriptor,
    _Out_opt_
    PULONG BytesTransferred
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceFormatRequestForControlTransfer)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _In_
    WDFREQUEST Request,
    _In_
    PWDF_USB_CONTROL_SETUP_PACKET SetupPacket,
    _In_opt_
    WDFMEMORY TransferMemory,
    _In_opt_
    PWDFMEMORY_OFFSET TransferOffset
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceResetPortSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetDeviceQueryUsbCapability)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _In_
    CONST GUID* CapabilityType,
    _In_
    ULONG CapabilityBufferLength,
    _When_(CapabilityBufferLength == 0, _Out_opt_)
    _When_(CapabilityBufferLength != 0 && ResultLength == NULL, _Out_writes_bytes_(CapabilityBufferLength))
    _When_(CapabilityBufferLength != 0 && ResultLength != NULL, _Out_writes_bytes_to_opt_(CapabilityBufferLength, *ResultLength))
    PVOID CapabilityBuffer,
    _Out_opt_
    _When_(ResultLength != NULL,_Deref_out_range_(<=,CapabilityBufferLength))
    PULONG ResultLength
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfUsbTargetPipeGetInformation)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe,
    _Out_
    PWDF_USB_PIPE_INFORMATION PipeInformation
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfUsbTargetPipeIsInEndpoint)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
NTAPI
WDFEXPORT(WdfUsbTargetPipeIsOutEndpoint)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDF_USB_PIPE_TYPE
NTAPI
WDFEXPORT(WdfUsbTargetPipeGetType)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfUsbTargetPipeSetNoMaximumPacketSizeCheck)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetPipeWriteSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe,
    _In_opt_
    WDFREQUEST Request,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    _In_opt_
    PWDF_MEMORY_DESCRIPTOR MemoryDescriptor,
    _Out_opt_
    PULONG BytesWritten
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetPipeFormatRequestForWrite)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe,
    _In_
    WDFREQUEST Request,
    _In_opt_
    WDFMEMORY WriteMemory,
    _In_opt_
    PWDFMEMORY_OFFSET WriteOffset
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetPipeReadSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe,
    _In_opt_
    WDFREQUEST Request,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    _In_opt_
    PWDF_MEMORY_DESCRIPTOR MemoryDescriptor,
    _Out_opt_
    PULONG BytesRead
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetPipeFormatRequestForRead)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe,
    _In_
    WDFREQUEST Request,
    _In_opt_
    WDFMEMORY ReadMemory,
    _In_opt_
    PWDFMEMORY_OFFSET ReadOffset
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetPipeConfigContinuousReader)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe,
    _In_
    PWDF_USB_CONTINUOUS_READER_CONFIG Config
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetPipeAbortSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe,
    _In_opt_
    WDFREQUEST Request,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS RequestOptions
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetPipeFormatRequestForAbort)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetPipeResetSynchronously)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe,
    _In_opt_
    WDFREQUEST Request,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS RequestOptions
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbTargetPipeFormatRequestForReset)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBPIPE Pipe,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BYTE
NTAPI
WDFEXPORT(WdfUsbInterfaceGetInterfaceNumber)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBINTERFACE UsbInterface
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BYTE
NTAPI
WDFEXPORT(WdfUsbInterfaceGetNumEndpoints)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBINTERFACE UsbInterface,
    _In_
    UCHAR SettingIndex
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfUsbInterfaceGetDescriptor)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBINTERFACE UsbInterface,
    _In_
    UCHAR SettingIndex,
    _Out_
    PUSB_INTERFACE_DESCRIPTOR InterfaceDescriptor
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BYTE
NTAPI
WDFEXPORT(WdfUsbInterfaceGetNumSettings)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBINTERFACE UsbInterface
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfUsbInterfaceSelectSetting)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBINTERFACE UsbInterface,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES PipesAttributes,
    _In_
    PWDF_USB_INTERFACE_SELECT_SETTING_PARAMS Params
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfUsbInterfaceGetEndpointInformation)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBINTERFACE UsbInterface,
    _In_
    UCHAR SettingIndex,
    _In_
    UCHAR EndpointIndex,
    _Inout_
    PWDF_USB_PIPE_INFORMATION EndpointInfo
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFUSBINTERFACE
NTAPI
WDFEXPORT(WdfUsbTargetDeviceGetInterface)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBDEVICE UsbDevice,
    _In_
    UCHAR InterfaceIndex
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BYTE
NTAPI
WDFEXPORT(WdfUsbInterfaceGetConfiguredSettingIndex)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBINTERFACE Interface
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BYTE
NTAPI
WDFEXPORT(WdfUsbInterfaceGetNumConfiguredPipes)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBINTERFACE UsbInterface
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFUSBPIPE
NTAPI
WDFEXPORT(WdfUsbInterfaceGetConfiguredPipe)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFUSBINTERFACE UsbInterface,
    _In_
    UCHAR PipeIndex,
    _Inout_opt_
    PWDF_USB_PIPE_INFORMATION PipeInfo
    );

WDFAPI
VOID
NTAPI
WDFEXPORT(WdfVerifierDbgBreakPoint)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals
    );

WDFAPI
VOID
NTAPI
WDFEXPORT(WdfVerifierKeBugCheck)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    ULONG BugCheckCode,
    _In_
    ULONG_PTR BugCheckParameter1,
    _In_
    ULONG_PTR BugCheckParameter2,
    _In_
    ULONG_PTR BugCheckParameter3,
    _In_
    ULONG_PTR BugCheckParameter4
    );

WDFAPI
PVOID
NTAPI
WDFEXPORT(WdfGetTriageInfo)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
NTAPI
WDFEXPORT(WdfWorkItemCreate)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDF_WORKITEM_CONFIG Config,
    _In_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _Out_
    WDFWORKITEM* WorkItem
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfWorkItemEnqueue)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWORKITEM WorkItem
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFOBJECT
NTAPI
WDFEXPORT(WdfWorkItemGetParentObject)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWORKITEM WorkItem
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
NTAPI
WDFEXPORT(WdfWorkItemFlush)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWORKITEM WorkItem
    );


extern WDFVERSION WdfVersion;

#ifdef FX_DYNAMICS_GENERATE_TABLE

WDFVERSION WdfVersion = {
    sizeof(WDFVERSION),
    sizeof(WDFFUNCTIONS)/sizeof(PVOID),
    {
        WDFEXPORT(WdfCollectionCreate),
        WDFEXPORT(WdfCollectionGetCount),
        WDFEXPORT(WdfCollectionAdd),
        WDFEXPORT(WdfCollectionRemove),
        WDFEXPORT(WdfCollectionRemoveItem),
        WDFEXPORT(WdfCollectionGetItem),
        WDFEXPORT(WdfCollectionGetFirstItem),
        WDFEXPORT(WdfCollectionGetLastItem),
        WDFEXPORT(WdfCxDeviceInitAllocate),
        WDFEXPORT(WdfCxDeviceInitSetRequestAttributes),
        WDFEXPORT(WdfCxDeviceInitSetFileObjectConfig),
        WDFEXPORT(WdfCxVerifierKeBugCheck),
        WDFEXPORT(WdfDeviceGetDeviceState),
        WDFEXPORT(WdfDeviceSetDeviceState),
        WDFEXPORT(WdfDeviceGetDriver),
        WDFEXPORT(WdfDeviceGetIoTarget),
        WDFEXPORT(WdfDeviceAssignS0IdleSettings),
        WDFEXPORT(WdfDeviceAssignSxWakeSettings),
        WDFEXPORT(WdfDeviceOpenRegistryKey),
        WDFEXPORT(WdfDeviceInitSetPnpPowerEventCallbacks),
        WDFEXPORT(WdfDeviceInitSetPowerPolicyEventCallbacks),
        WDFEXPORT(WdfDeviceInitSetPowerPolicyOwnership),
        WDFEXPORT(WdfDeviceInitSetIoType),
        WDFEXPORT(WdfDeviceInitSetFileObjectConfig),
        WDFEXPORT(WdfDeviceInitSetRequestAttributes),
        WDFEXPORT(WdfDeviceCreate),
        WDFEXPORT(WdfDeviceSetStaticStopRemove),
        WDFEXPORT(WdfDeviceCreateDeviceInterface),
        WDFEXPORT(WdfDeviceSetDeviceInterfaceState),
        WDFEXPORT(WdfDeviceRetrieveDeviceInterfaceString),
        WDFEXPORT(WdfDeviceCreateSymbolicLink),
        WDFEXPORT(WdfDeviceQueryProperty),
        WDFEXPORT(WdfDeviceAllocAndQueryProperty),
        WDFEXPORT(WdfDeviceSetPnpCapabilities),
        WDFEXPORT(WdfDeviceSetPowerCapabilities),
        WDFEXPORT(WdfDeviceSetFailed),
        WDFEXPORT(WdfDeviceStopIdleNoTrack),
        WDFEXPORT(WdfDeviceResumeIdleNoTrack),
        WDFEXPORT(WdfDeviceGetFileObject),
        WDFEXPORT(WdfDeviceGetDefaultQueue),
        WDFEXPORT(WdfDeviceConfigureRequestDispatching),
        WDFEXPORT(WdfDeviceGetSystemPowerAction),
        WDFEXPORT(WdfDeviceInitSetReleaseHardwareOrderOnFailure),
        WDFEXPORT(WdfDeviceInitSetIoTypeEx),
        WDFEXPORT(WdfDevicePostEvent),
        WDFEXPORT(WdfDeviceMapIoSpace),
        WDFEXPORT(WdfDeviceUnmapIoSpace),
        WDFEXPORT(WdfDeviceGetHardwareRegisterMappedAddress),
        WDFEXPORT(WdfDeviceReadFromHardware),
        WDFEXPORT(WdfDeviceWriteToHardware),
        WDFEXPORT(WdfDeviceAssignInterfaceProperty),
        WDFEXPORT(WdfDeviceAllocAndQueryInterfaceProperty),
        WDFEXPORT(WdfDeviceQueryInterfaceProperty),
        WDFEXPORT(WdfDeviceGetDeviceStackIoType),
        WDFEXPORT(WdfDeviceQueryPropertyEx),
        WDFEXPORT(WdfDeviceAllocAndQueryPropertyEx),
        WDFEXPORT(WdfDeviceAssignProperty),
        WDFEXPORT(WdfDriverCreate),
        WDFEXPORT(WdfDriverGetRegistryPath),
        WDFEXPORT(WdfDriverOpenParametersRegistryKey),
        WDFEXPORT(WdfDriverRetrieveVersionString),
        WDFEXPORT(WdfDriverIsVersionAvailable),
        WDFEXPORT(WdfFdoInitOpenRegistryKey),
        WDFEXPORT(WdfFdoInitQueryProperty),
        WDFEXPORT(WdfFdoInitAllocAndQueryProperty),
        WDFEXPORT(WdfFdoInitQueryPropertyEx),
        WDFEXPORT(WdfFdoInitAllocAndQueryPropertyEx),
        WDFEXPORT(WdfFdoInitSetFilter),
        WDFEXPORT(WdfFileObjectGetFileName),
        WDFEXPORT(WdfFileObjectGetDevice),
        WDFEXPORT(WdfFileObjectGetInitiatorProcessId),
        WDFEXPORT(WdfFileObjectGetRelatedFileObject),
        WDFEXPORT(WdfInterruptCreate),
        WDFEXPORT(WdfInterruptQueueDpcForIsr),
        WDFEXPORT(WdfInterruptQueueWorkItemForIsr),
        WDFEXPORT(WdfInterruptSynchronize),
        WDFEXPORT(WdfInterruptAcquireLock),
        WDFEXPORT(WdfInterruptReleaseLock),
        WDFEXPORT(WdfInterruptEnable),
        WDFEXPORT(WdfInterruptDisable),
        WDFEXPORT(WdfInterruptGetInfo),
        WDFEXPORT(WdfInterruptSetPolicy),
        WDFEXPORT(WdfInterruptSetExtendedPolicy),
        WDFEXPORT(WdfInterruptGetDevice),
        WDFEXPORT(WdfInterruptTryToAcquireLock),
        WDFEXPORT(WdfIoQueueCreate),
        WDFEXPORT(WdfIoQueueGetState),
        WDFEXPORT(WdfIoQueueStart),
        WDFEXPORT(WdfIoQueueStop),
        WDFEXPORT(WdfIoQueueStopSynchronously),
        WDFEXPORT(WdfIoQueueGetDevice),
        WDFEXPORT(WdfIoQueueRetrieveNextRequest),
        WDFEXPORT(WdfIoQueueRetrieveRequestByFileObject),
        WDFEXPORT(WdfIoQueueFindRequest),
        WDFEXPORT(WdfIoQueueRetrieveFoundRequest),
        WDFEXPORT(WdfIoQueueDrainSynchronously),
        WDFEXPORT(WdfIoQueueDrain),
        WDFEXPORT(WdfIoQueuePurgeSynchronously),
        WDFEXPORT(WdfIoQueuePurge),
        WDFEXPORT(WdfIoQueueReadyNotify),
        WDFEXPORT(WdfIoQueueStopAndPurge),
        WDFEXPORT(WdfIoQueueStopAndPurgeSynchronously),
        WDFEXPORT(WdfIoTargetCreate),
        WDFEXPORT(WdfIoTargetOpen),
        WDFEXPORT(WdfIoTargetCloseForQueryRemove),
        WDFEXPORT(WdfIoTargetClose),
        WDFEXPORT(WdfIoTargetStart),
        WDFEXPORT(WdfIoTargetStop),
        WDFEXPORT(WdfIoTargetPurge),
        WDFEXPORT(WdfIoTargetGetState),
        WDFEXPORT(WdfIoTargetGetDevice),
        WDFEXPORT(WdfIoTargetSendReadSynchronously),
        WDFEXPORT(WdfIoTargetFormatRequestForRead),
        WDFEXPORT(WdfIoTargetSendWriteSynchronously),
        WDFEXPORT(WdfIoTargetFormatRequestForWrite),
        WDFEXPORT(WdfIoTargetSendIoctlSynchronously),
        WDFEXPORT(WdfIoTargetFormatRequestForIoctl),
        WDFEXPORT(WdfMemoryCreate),
        WDFEXPORT(WdfMemoryCreatePreallocated),
        WDFEXPORT(WdfMemoryGetBuffer),
        WDFEXPORT(WdfMemoryAssignBuffer),
        WDFEXPORT(WdfMemoryCopyToBuffer),
        WDFEXPORT(WdfMemoryCopyFromBuffer),
        WDFEXPORT(WdfObjectGetTypedContextWorker),
        WDFEXPORT(WdfObjectAllocateContext),
        WDFEXPORT(WdfObjectContextGetObject),
        WDFEXPORT(WdfObjectReferenceActual),
        WDFEXPORT(WdfObjectDereferenceActual),
        WDFEXPORT(WdfObjectCreate),
        WDFEXPORT(WdfObjectDelete),
        WDFEXPORT(WdfObjectQuery),
        WDFEXPORT(WdfRegistryOpenKey),
        WDFEXPORT(WdfRegistryCreateKey),
        WDFEXPORT(WdfRegistryClose),
        WDFEXPORT(WdfRegistryRemoveKey),
        WDFEXPORT(WdfRegistryRemoveValue),
        WDFEXPORT(WdfRegistryQueryValue),
        WDFEXPORT(WdfRegistryQueryMemory),
        WDFEXPORT(WdfRegistryQueryMultiString),
        WDFEXPORT(WdfRegistryQueryUnicodeString),
        WDFEXPORT(WdfRegistryQueryString),
        WDFEXPORT(WdfRegistryQueryULong),
        WDFEXPORT(WdfRegistryAssignValue),
        WDFEXPORT(WdfRegistryAssignMemory),
        WDFEXPORT(WdfRegistryAssignMultiString),
        WDFEXPORT(WdfRegistryAssignUnicodeString),
        WDFEXPORT(WdfRegistryAssignString),
        WDFEXPORT(WdfRegistryAssignULong),
        WDFEXPORT(WdfRequestCreate),
        WDFEXPORT(WdfRequestReuse),
        WDFEXPORT(WdfRequestChangeTarget),
        WDFEXPORT(WdfRequestFormatRequestUsingCurrentType),
        WDFEXPORT(WdfRequestSend),
        WDFEXPORT(WdfRequestGetStatus),
        WDFEXPORT(WdfRequestMarkCancelable),
        WDFEXPORT(WdfRequestMarkCancelableEx),
        WDFEXPORT(WdfRequestUnmarkCancelable),
        WDFEXPORT(WdfRequestIsCanceled),
        WDFEXPORT(WdfRequestCancelSentRequest),
        WDFEXPORT(WdfRequestIsFrom32BitProcess),
        WDFEXPORT(WdfRequestSetCompletionRoutine),
        WDFEXPORT(WdfRequestGetCompletionParams),
        WDFEXPORT(WdfRequestAllocateTimer),
        WDFEXPORT(WdfRequestComplete),
        WDFEXPORT(WdfRequestCompleteWithInformation),
        WDFEXPORT(WdfRequestGetParameters),
        WDFEXPORT(WdfRequestRetrieveInputMemory),
        WDFEXPORT(WdfRequestRetrieveOutputMemory),
        WDFEXPORT(WdfRequestRetrieveInputBuffer),
        WDFEXPORT(WdfRequestRetrieveOutputBuffer),
        WDFEXPORT(WdfRequestSetInformation),
        WDFEXPORT(WdfRequestGetInformation),
        WDFEXPORT(WdfRequestGetFileObject),
        WDFEXPORT(WdfRequestGetRequestorMode),
        WDFEXPORT(WdfRequestForwardToIoQueue),
        WDFEXPORT(WdfRequestGetIoQueue),
        WDFEXPORT(WdfRequestRequeue),
        WDFEXPORT(WdfRequestStopAcknowledge),
        WDFEXPORT(WdfRequestImpersonate),
        WDFEXPORT(WdfRequestGetRequestorProcessId),
        WDFEXPORT(WdfRequestIsFromUserModeDriver),
        WDFEXPORT(WdfRequestSetUserModeDriverInitiatedIo),
        WDFEXPORT(WdfRequestGetUserModeDriverInitiatedIo),
        WDFEXPORT(WdfRequestSetActivityId),
        WDFEXPORT(WdfRequestRetrieveActivityId),
        WDFEXPORT(WdfRequestGetEffectiveIoType),
        WDFEXPORT(WdfCmResourceListGetCount),
        WDFEXPORT(WdfCmResourceListGetDescriptor),
        WDFEXPORT(WdfStringCreate),
        WDFEXPORT(WdfStringGetUnicodeString),
        WDFEXPORT(WdfObjectAcquireLock),
        WDFEXPORT(WdfObjectReleaseLock),
        WDFEXPORT(WdfWaitLockCreate),
        WDFEXPORT(WdfWaitLockAcquire),
        WDFEXPORT(WdfWaitLockRelease),
        WDFEXPORT(WdfSpinLockCreate),
        WDFEXPORT(WdfSpinLockAcquire),
        WDFEXPORT(WdfSpinLockRelease),
        WDFEXPORT(WdfTimerCreate),
        WDFEXPORT(WdfTimerStart),
        WDFEXPORT(WdfTimerStop),
        WDFEXPORT(WdfTimerGetParentObject),
        WDFEXPORT(WdfUsbTargetDeviceCreate),
        WDFEXPORT(WdfUsbTargetDeviceCreateWithParameters),
        WDFEXPORT(WdfUsbTargetDeviceRetrieveInformation),
        WDFEXPORT(WdfUsbTargetDeviceGetDeviceDescriptor),
        WDFEXPORT(WdfUsbTargetDeviceRetrieveConfigDescriptor),
        WDFEXPORT(WdfUsbTargetDeviceQueryString),
        WDFEXPORT(WdfUsbTargetDeviceAllocAndQueryString),
        WDFEXPORT(WdfUsbTargetDeviceFormatRequestForString),
        WDFEXPORT(WdfUsbTargetDeviceGetNumInterfaces),
        WDFEXPORT(WdfUsbTargetDeviceSelectConfig),
        WDFEXPORT(WdfUsbTargetDeviceSendControlTransferSynchronously),
        WDFEXPORT(WdfUsbTargetDeviceFormatRequestForControlTransfer),
        WDFEXPORT(WdfUsbTargetDeviceResetPortSynchronously),
        WDFEXPORT(WdfUsbTargetDeviceQueryUsbCapability),
        WDFEXPORT(WdfUsbTargetPipeGetInformation),
        WDFEXPORT(WdfUsbTargetPipeIsInEndpoint),
        WDFEXPORT(WdfUsbTargetPipeIsOutEndpoint),
        WDFEXPORT(WdfUsbTargetPipeGetType),
        WDFEXPORT(WdfUsbTargetPipeSetNoMaximumPacketSizeCheck),
        WDFEXPORT(WdfUsbTargetPipeWriteSynchronously),
        WDFEXPORT(WdfUsbTargetPipeFormatRequestForWrite),
        WDFEXPORT(WdfUsbTargetPipeReadSynchronously),
        WDFEXPORT(WdfUsbTargetPipeFormatRequestForRead),
        WDFEXPORT(WdfUsbTargetPipeConfigContinuousReader),
        WDFEXPORT(WdfUsbTargetPipeAbortSynchronously),
        WDFEXPORT(WdfUsbTargetPipeFormatRequestForAbort),
        WDFEXPORT(WdfUsbTargetPipeResetSynchronously),
        WDFEXPORT(WdfUsbTargetPipeFormatRequestForReset),
        WDFEXPORT(WdfUsbInterfaceGetInterfaceNumber),
        WDFEXPORT(WdfUsbInterfaceGetNumEndpoints),
        WDFEXPORT(WdfUsbInterfaceGetDescriptor),
        WDFEXPORT(WdfUsbInterfaceGetNumSettings),
        WDFEXPORT(WdfUsbInterfaceSelectSetting),
        WDFEXPORT(WdfUsbInterfaceGetEndpointInformation),
        WDFEXPORT(WdfUsbTargetDeviceGetInterface),
        WDFEXPORT(WdfUsbInterfaceGetConfiguredSettingIndex),
        WDFEXPORT(WdfUsbInterfaceGetNumConfiguredPipes),
        WDFEXPORT(WdfUsbInterfaceGetConfiguredPipe),
        WDFEXPORT(WdfVerifierDbgBreakPoint),
        WDFEXPORT(WdfVerifierKeBugCheck),
        WDFEXPORT(WdfGetTriageInfo),
        WDFEXPORT(WdfWorkItemCreate),
        WDFEXPORT(WdfWorkItemEnqueue),
        WDFEXPORT(WdfWorkItemGetParentObject),
        WDFEXPORT(WdfWorkItemFlush),
        WDFEXPORT(WdfRegistryWdmGetHandle),
        WDFEXPORT(WdfDeviceStopIdleActual),
        WDFEXPORT(WdfDeviceResumeIdleActual),
        WDFEXPORT(WdfDeviceInitEnableHidInterface),
        WDFEXPORT(WdfDeviceHidNotifyPresence),
        WDFEXPORT(WdfDeviceGetSelfIoTarget),
        WDFEXPORT(WdfDeviceInitAllowSelfIoTarget),
        WDFEXPORT(WdfIoTargetSelfAssignDefaultIoQueue),
        WDFEXPORT(WdfDeviceOpenDevicemapKey),
        WDFEXPORT(WdfIoTargetWdmGetTargetFileHandle),
        WDFEXPORT(WdfDeviceWdmDispatchIrp),
        WDFEXPORT(WdfDeviceWdmDispatchIrpToIoQueue),
        WDFEXPORT(WdfDeviceConfigureWdmIrpDispatchCallback),
        WDFEXPORT(WdfCxDeviceInitSetPnpPowerEventCallbacks),
        WDFEXPORT(WdfCompanionCreate),
        WDFEXPORT(WdfCompanionWdmGetSecureDeviceHandle),
        WDFEXPORT(WdfCompanionCreateTaskQueue),
        WDFEXPORT(WdfDeviceInitSetCompanionEventCallbacks),
        WDFEXPORT(WdfDriverOpenPersistentStateRegistryKey),
        WDFEXPORT(WdfDriverErrorReportApiMissing),
        WDFEXPORT(WdfDeviceRetrieveDeviceDirectoryString),
        WDFEXPORT(WdfDriverRetrieveDriverDataDirectoryString),
        WDFEXPORT(WdfCxDeviceInitAllocateContext),
        WDFEXPORT(WdfCxDeviceInitGetTypedContextWorker),
        WDFEXPORT(WdfCxDeviceInitSetPowerPolicyEventCallbacks),
        WDFEXPORT(WdfDeviceSetDeviceInterfaceStateEx),
        WDFEXPORT(WdfDeviceWdmAssignPowerFrameworkSettings),
    },
    sizeof(WDFSTRUCTURES)/sizeof(size_t),
    {
        sizeof(WDF_CHILD_ADDRESS_DESCRIPTION_HEADER),
        sizeof(WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER),
        sizeof(WDF_CHILD_LIST_CONFIG),
        sizeof(WDF_CHILD_LIST_ITERATOR),
        sizeof(WDF_CHILD_RETRIEVE_INFO),
        sizeof(WDF_CLASS_BIND_INFO),
        sizeof(WDF_CLASS_BIND_INFO2),
        sizeof(WDF_CLASS_LIBRARY_INFO),
        sizeof(WDF_CLASS_VERSION),
        sizeof(WDF_COMPANION_EVENT_CALLBACKS),
        sizeof(WDF_CUSTOM_TYPE_CONTEXT),
        sizeof(WDF_DEVICE_INTERFACE_PROPERTY_DATA),
        sizeof(WDF_DEVICE_PNP_CAPABILITIES),
        sizeof(WDF_DEVICE_PNP_NOTIFICATION_DATA),
        sizeof(WDF_DEVICE_POWER_CAPABILITIES),
        sizeof(WDF_DEVICE_POWER_NOTIFICATION_DATA),
        sizeof(WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS),
        sizeof(WDF_DEVICE_POWER_POLICY_NOTIFICATION_DATA),
        sizeof(WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS),
        sizeof(WDF_DEVICE_PROPERTY_DATA),
        sizeof(WDF_DEVICE_STATE),
        sizeof(WDF_DRIVER_CONFIG),
        sizeof(WDF_DRIVER_GLOBALS),
        sizeof(WDF_DRIVER_VERSION_AVAILABLE_PARAMS),
        sizeof(WDF_FDO_EVENT_CALLBACKS),
        sizeof(WDF_FILEOBJECT_CONFIG),
        sizeof(WDF_INTERRUPT_CONFIG),
        sizeof(WDF_INTERRUPT_EXTENDED_POLICY),
        sizeof(WDF_INTERRUPT_INFO),
        sizeof(WDF_IO_FORWARD_PROGRESS_RESERVED_POLICY_SETTINGS),
        sizeof(WDF_IO_QUEUE_CONFIG),
        sizeof(WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY),
        sizeof(WDF_IO_TARGET_OPEN_PARAMS),
        sizeof(WDF_IO_TYPE_CONFIG),
        sizeof(WDF_MEMORY_DESCRIPTOR),
        sizeof(WDF_OBJECT_ATTRIBUTES),
        sizeof(WDF_OBJECT_CONTEXT_TYPE_INFO),
        sizeof(WDF_PDO_EVENT_CALLBACKS),
        sizeof(WDF_PNPPOWER_EVENT_CALLBACKS),
        sizeof(WDF_POWER_FRAMEWORK_SETTINGS),
        sizeof(WDF_POWER_POLICY_EVENT_CALLBACKS),
        sizeof(WDF_POWER_ROUTINE_TIMED_OUT_DATA),
        sizeof(WDF_QUERY_INTERFACE_CONFIG),
        sizeof(WDF_QUEUE_FATAL_ERROR_DATA),
        sizeof(WDF_REMOVE_LOCK_OPTIONS),
        sizeof(WDF_REQUEST_COMPLETION_PARAMS),
        sizeof(WDF_REQUEST_FATAL_ERROR_INFORMATION_LENGTH_MISMATCH_DATA),
        sizeof(WDF_REQUEST_FORWARD_OPTIONS),
        sizeof(WDF_REQUEST_PARAMETERS),
        sizeof(WDF_REQUEST_REUSE_PARAMS),
        sizeof(WDF_REQUEST_SEND_OPTIONS),
        sizeof(WDF_TASK_QUEUE_CONFIG),
        sizeof(WDF_TIMER_CONFIG),
        sizeof(WDF_TRIAGE_INFO),
        sizeof(WDF_USB_CONTINUOUS_READER_CONFIG),
        sizeof(WDF_USB_DEVICE_CREATE_CONFIG),
        sizeof(WDF_USB_DEVICE_INFORMATION),
        sizeof(WDF_USB_DEVICE_SELECT_CONFIG_PARAMS),
        sizeof(WDF_USB_INTERFACE_SELECT_SETTING_PARAMS),
        sizeof(WDF_USB_INTERFACE_SETTING_PAIR),
        sizeof(WDF_USB_PIPE_INFORMATION),
        sizeof(WDF_USB_REQUEST_COMPLETION_PARAMS),
        sizeof(WDF_WMI_INSTANCE_CONFIG),
        sizeof(WDF_WMI_PROVIDER_CONFIG),
        sizeof(WDF_WORKITEM_CONFIG),
        sizeof(WDFCONTEXT_TRIAGE_INFO),
        sizeof(WDFCONTEXTTYPE_TRIAGE_INFO),
        sizeof(WDFCX_FILEOBJECT_CONFIG),
        sizeof(WDFCX_PNPPOWER_EVENT_CALLBACKS),
        sizeof(WDFDEVICE_TRIAGE_INFO),
        sizeof(WDFFWDPROGRESS_TRIAGE_INFO),
        sizeof(WDFIRP_TRIAGE_INFO),
        sizeof(WDFIRPQUEUE_TRIAGE_INFO),
        sizeof(WDFMEMORY_OFFSET),
        sizeof(WDFOBJECT_TRIAGE_INFO),
        sizeof(WDFQUEUE_TRIAGE_INFO),
        sizeof(WDFREQUEST_TRIAGE_INFO),
        sizeof(WDFCX_POWER_POLICY_EVENT_CALLBACKS),
    },
};

#endif // FX_DYNAMICS_GENERATE_TABLE

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _FXDYNAMICS_H_

