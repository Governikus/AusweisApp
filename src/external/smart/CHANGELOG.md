# Change Log

## Version 1.1.0

_2023-08-17_

- NEW: Upgrade TSM-API SDK to v1.3.0 (`tsm-api-sdk-1.3.0-20230807-release.aar`)
- NEW: implementation and use of `CommandAPDU` and `ResponseAPDU` to replace `ByteArray` (Kotlin only)
- NEW: eID-Service-Library returns SW=`6F00` for known OMAPI exceptions upon transmit

## Version 1.0.0

_2023-06-29_

- NEW: improve the treatment of exceptions (#10827, #10828)
- NEW: return `ServiceResult#NETWORK_CONNECTION_ERROR` if a TSM-API SDK command was not
    started or interrupted due to a network issue
- NEW: return `ServiceResult#ERROR` when calling `deleteSmartEid()` if the eID-Applet was provisioned by the TSMS but still exists after this call.

### eid_applet_interface/jni-bridge

- NEW: jni-bridge upgrade to v0.23.0
- NEW: add `EidServiceResult#NETWORK_CONNECTION_ERROR`

## Version 0.18.1

_2023-05-04_

### eid_applet_interface/jni-bridge

- FIX: change wrong `findClass` calls
- NEW: jni-bridge upgrade to v0.22.3
- NEW: update ndk version to `25.2.9519653`

## Version 0.18.0

_2023-04-28_

- NEW: Upgrade TSM-API SDK to v1.2.0 (`tsm-api-sdk-1.2.0-20230420-release.aar`)
- FIX: prevent NoClassDefFoundError for the IProcessListener
- FIX: `SmartEidCard` instance could still be used without reconnecting to the eID-Applet even
    if the `ActionTimer` already closed the channel.
- FIX: add missing array boundary check (#11523)

### eid_applet_interface/jni-bridge

- NEW: jni-bridge upgrade to v0.22.2
- NEW: Updated jni to handle mapping between objects and integer
- FIX: add missing nullptr checks (#11562)
- FIX: resolve issues with concurrent calls of the `shutdownService()` function (#10885)
- FIX: prevent redundant `findClass()` calls (#10904)
- FIX: delete local references (#10906)

## Version 0.17.4

_2023-03-03_

- NEW: Upgrade TSM-API SDK to v1.0.3 (`tsm-api-sdk-1.0.3-20230224-release.aar`)
- NEW: Automatically close the `SEService` if no successful binding could be established and a
  timeout was exceeded.

## Version 0.17.3

_2023-02-09_

- NEW: Upgrade TSM-API SDK to v1.0.2 (`tsm-api-sdk-1.0.2-20230207-release.aar`)
- FIX: Issue #173, Catch `ClassCastException` for `android.os.BinderProxy` in an unknown process

## Version 0.17.2

_2023-01-25_

- NEW: upgrade Kotlin-Version
- NEW: Upgrade TSM-API SDK to v1.0.0 (`tsm-api-sdk-1.0.0-20230123-release.aar`)

## Version 0.17.1

_2023-01-19_

- FIX: downgrade Kotlin-Version

## Version 0.17.0

_2023-01-13_

- NEW: multiple eID-Applets on different Secure Elements are handled according to the
  documentation `Description of eID-Applet Status Computation v0.2.1`
- NEW: improvement of the architecture of the eID-Service-Library
- NEW: change library package name from `de.bdr.eid_applet_service_lib`
  to `de.bundesdruckerei.android.eid_applet_service_lib`
- NEW: jni-bridge interface upgrade to v0.22.0

## Version 0.16.1

_2022-12-19_

- NEW: Upgrade TSM-API SDK to v0.12.2 (`tsm-api-sdk-0.12.2-20221130-release.aar`)
- FIX: fix `challengeType` value in the `ServiceInformationResult`
- FIX: fix `GET CHALLENGE` response handling

## Version 0.16.0

_2022-10-14_

- NEW: jni-bridge interface upgrade to v0.21.0
- NEW: Upgrade TSM-API SDK to v0.12.1 (`tsm-api-sdk-0.12.1-20221012-release.aar`)
- NEW: updating the integration of the progress information from tsms-api-sdk

## Version 0.15.0

_2022-09-20_

- NEW: jni-bridge interface upgrade to v0.20.0
- NEW: initialization of the personalization is possible only in EidStatus `NO_PERSONALIZATION`
- NEW: the function `finalizePersonalization(status)` delete the IEA key if the eID-Applet was
  rejected by the personalization system

## Version 0.14.0

_2022-08-30_

- NEW: jni-bridge interface upgrade to v0.19.2
- NEW: handling of the status code of a previous personalization according to the
  documentation `Error_Handling_Smart-eID-Personalization_v0.5`
- FIX: detection of library support with multiple major versions supported
- FIX: Proper evaluation of the TA2,CA2 and PACE protocol within the determination of the applet
  status.
- NEW: Unbind the `TSMAPIService` after calling the function `installSmartEid()`
  and `smartEidSupportInfo()`
- NEW: Change within the `deleteSmartEid()` function: Delete the IEA KeyPair only after the
  TSMS `terminateService()` command was successful.

## Version 0.13.0

_2022-08-11_

- NEW: jni-bridge interface upgrade to v0.19.1
- NEW: upgrade `smartEidStatus()` and `smartEidSupportInfo()` implementation according to the
  documentation `Description of eID-Applet Status Computation v0.2.1`
- NEW: handling of ServiceInstances within the `installSmartEid()` function that are either in
  state `NotDeployed` or `InError`
- NEW: implementation of eID-Service-Library support check
- NEW: automatic detection of the TSM ServiceVersionTag, which is supported by the
  eID-Service-Library. Passing the ServiceVersionTag using `initializeService()` function is now
  optional and should only be set explicitly for test purposes.
- NEW: Upgrade TSM-API SDK to v0.11.0 (`tsm-api-sdk-0.11.0-20220620-release.aar`)

## Version 0.12.1

_2022-06-09_

- FIX: Avoid the call of the `PersonalizeServiceCommand` command within the service deployment
- FIX: AttestationToken encoding for calling 'initializePersonalization(...)'
- NEW: The `ActionTimer` to automatically close the eID-Applet channel is now thread-safe

## Version 0.12.0

_2022-06-03_

- NEW: `deletePersonalization()` now executes TERMINATE APDU before deleting IEA key pair
- NEW: `finalizePersonalization()` now SELECTs eID-Applet in order to accommodate G+D for NXP CSP
  case (move initial PIN to CSP Admin)
- NEW: Upgrade TSM-API SDK to v0.10.0 (`tsm-api-sdk-0.10.0-20220517-release.aar`)
- NEW: perform the `TSMS#PersonalizeServiceCommand` command within the service deployment

## Version 0.11.0

_2022-04-28_

- NEW: Upgrade TSM-API SDK to v0.9.5 (`tsm-api-sdk-0.9.5-20220413-release.aar`)
- NEW: jni-bridge interface upgrade to v0.17.0
- NEW: Proper status for unsupported devices is returned (#95)
- FIX: Correction of the return values for `getSmartEidStatus()` and `getUpdateInfo()` in case of an
  error

## Version 0.10.3

_2022-04-08_

- NEW: Tests the current thread used for initializing the library. If this is the main thread, the
  initialization is interrupted and the return value `GenericDataResult` contains an `ERROR` code.
- NEW: Catch the case when the `SEService´ is already bound and the `SEService.OnConnectedListener`
  callback is not called.
- NEW: Automatic closing of the open OMAPI channel after 4 minutes and 50 seconds, if no further
  APDU was transferred.
- NEW: Upgrade TSM-API SDK to v0.9.4 (`tsm-api-sdk-0.9.4-20220408-release.aar`)

## Version 0.10.2

_2022-03-18_

- FIX: Avoid throwing a `GeneralSecurityException` if the application can participate in the backup
  and restore infrastructure.
- FIX: A persisted and not updated `getUpdateInfo()` result is no longer considered when
  calling `getSmartEidStatus()`, if there is no applet present anymore.
- FIX: Ensures the support of the device if an applet is installed but no `getUpdateInfo()` has been
  called before. (#85)

## Version 0.10.1

_2022-03-16_

- NEW: Upgrade TSM-API SDK to v0.9.2 (`tsm-api-sdk-0.9.2-20220315-release.aar`)
- NEW: Renamed Logger from `SmartEidServiceLib` to `bdr`
- FIX: The last known result of `getUpdateInfo()` is no longer deleted in
  function `deleteSmartEid()`. (according to the
  documentation `Description of eID-Applet Status Management v0.8`)

## Version 0.10.0

_2022-03-15_

- FIX: Faulty binding between applet and application (IEA) when ECDSA signature size is different. (
  #69)
- NEW: Revision of the jni-bridge in collaboration with Governikus.
- NEW: Upgrade the smartEid status management according to the
  documentation `Description of eID-Applet Status Management v0.8`
  - Function `getSmartEidStatus()` can be used without calling `getUpdateInfo()` before, if an
    applet has already been installed.
  - Personalization Status and the used CSP are requested directly by the installed eID-Applet.
  - A distinction is made between the CSP Types GuD and Thales. The corresponding return values
    for the status are produced if a personalization can no longer be used.
- NEW: Upgrade TSM-API SDK to v0.9.1 (`tsm-api-sdk-0.9.1-20220314-release.aar`) (#47)

## Version 0.9.1

_2022-03-03_

- NEW: Upgrade TSM-API SDK to v0.9.0

## Version 0.9.0

_2022-02-25_

### eid_applet_interface/jni-bridge

- NEW: interface upgrade to v0.16.0

### eid-applet-service-lib

- NEW: Add `processHandler` parameter to `installSmartEid()` and `deleteSmartEid()`. Remove
  global `setProgressHandler()` function and `Operation` enum.
- NEW: Initial External Authentication can no longer be disabled. Implementation has been updated.

## Version 0.8.0

_2021-02-18_

### eid_applet_interface/jni-bridge

- NEW: interface upgrade to v0.15.0

### eid-applet-service-lib

- NEW: Remove the `TSMSServiceId` attribute from `AndroidManifest`. For the initialization of the
  library now three properties (`ServiceId`, `ServiceTag` and `SSD AID`) are required, which must be
  passed directly to the function `initializeService`.
- NEW: Upgrade TSM-API SDK to v0.8.1 (`tsm-api-sdk-0.8.1-20220210-release.aar`)

## Version 0.7.1

_2022-02-11_

### eid-applet-service-lib

- NEW: After calling the `deleteSmartEid` function a TSM-ServiceInstanceId is no longer queried
  immediately.
- NEW: Unbind the `TSMAPIService` immediately when the `deleteSmartEid` is called.
- NEW: Updated logging mechanism. Messages are now forwarded to the `java.util.logging.Logger`.

## Version 0.7.0

_2022-02-04_

### eid-applet-service-lib

- NEW: Integration of new API for the Personalization Data Provider Service in v1.0.0. This version
  is not backward compatible.

### eid_applet_interface/jni-bridge

- NEW: interface upgrade to v0.14.4

## Version 0.6.2

_2022-01-12_

### eid-applet-service-lib

- FIX: fix implementation of function 'initializePrePersonalization(...)' with modified signature
- FIX: Issue #34, Returns the proper response of the `getSmartEidStatus` function if
  a `deletePersonalization` was previously performed. IEA is disabled for the time being.
- FIX: Binding of the SEService is now performed in the `Dispatchers.Default` to avoid a blocking
  main thread.

## Version 0.6.1

_2021-12-16_

### eid_applet_interface/jni-bridge

- NEW: interface upgrade to v0.14.3

### eid-applet-service-lib

- NEW: function `getSmartEidStatus` returns `UNAVAILABLE` if the the function `getUpdateInfo` could
  not be detected the status online
- NEW: first implementation of the initial external authentication (IEA), if it is actively used by
  the personalization process the binding between Applet and the eID-Service-Library exists.
  If this is not active, the applet can currently still be used, even without binding.
- NEW: function `deletePersonalization` is implemented and sets the status of the eID-Applet
  to `APPLET_UNUSABLE`
- NEW: A mandatory tag is required in the `AndroidManifest` that must contain a
  additional `TSMSServiceId` attribute. The value is the `ServiceId` from the TSMS that must be
  specified for eID-Client. Example:

```xml
<manifest ... >
    <application ... >
        <meta-data
            android:name="TSMSServiceId"
            android:value="cdab0244-5cef-1e27-822e-63891e86c0ff" />
    </application>
</manifest>
```

## Version 0.6.0

_2021-10-14_

### eid_applet_interface/jni-bridge

- NEW: interface upgrade to v0.14.0
- FIX: fixed return values for the functions `initializeService` and `shutdownService`

### eid-applet-service-lib

- NEW: Upgrade TSM-API SDK to v0.6.1 (tsm-api-sdk-0.6.1-20210916-release.aar)
- NEW: first version of the function `getUpdateInfo`
  - execute the `TSM#performDeviceCheck` function
  - currently the following return values are to be expected: `INTERNAL_ERROR`, `NO_PROVISIONING`
    , `UNAVAILABLE` and `UP_TO_DATE`
  - if the provisioned applet be personalized, `UP_TO_DATE` is returned
- NEW: first version of the function `getSmartEidStatus`
  - currently the following return values are to be expected: `INTERNAL_ERROR`, `UNAVAILABLE`
    , `NO_PROVISIONING`, `NO_PERSONALIZATION`, `PERSONALIZED_READY` and `APPLET_UNUSABLE`
- NEW: `deletePersonalization` function returns only the static value `UNSUPPORTED`

## Version 0.5.0

_2021-09-22_

### eid_applet_interface/jni-bridge

- NEW: interface upgrade to v0.10.0

### eid-applet-service-lib

- NEW: proper return values for the `installApplet` and `deleteApplet` functions are generated,
  according to the TR-03165
- NEW: implementation of the `finalizePersonalization` function
- NEW: personalization of the generated initial-eID-PIN
- NEW: late initialization of the `TSMAPIService` implemented, service binding is performed only
  before the first use

## Version 0.4.4

_2021-09-06_

### eid-applet-service-lib

- FIX: Updated TSM-API service version tag parameter value from 0.1.0 to 1.0.0 required for TSM_API
  SDK v0.5.0

## Version 0.4.3

_2021-09-06_

### eid-applet-service-lib

- Upgrade TSM-API SDK to v0.5.0 (tsm-api-sdk-0.5.0-20210903-release.aar)

## Version 0.4.2

_2021-08-26_

### eid-applet-service-lib

- New: current static `init-eID-PIN` set to "123456" for now

## Version 0.4.1

_2021-08-19_

### eid-applet-service-lib

- Fix: upgrade the tsm-api-sdk dependency check to avoid app crashes
- Fix: refresh `TSM#GetInstanceServiceId` after a terminate of the applet has been performed

## Version 0.4.0

_2021-08-09_

### eid-applet-service-lib

- New: first integration of the TMS-API-SDK (tsm-api-sdk-0.3.0-20210705-release.aar / TR-03165 v0.6
  Draft)
  - Provisioning (`InstallServiceCommand`, `ActivateServiceCommand`) and
    Deletion (`TerminateService`) of the eID-Applet possible
  - Personalization untested so far
  - upgrade `Service#installApplet()` function
  - add `Service#deleteApplet()` function
  - legacy TSM API removed
- New: upgrade the `Service#initializePrePersonalization()` function`
- New: Coroutines added to be able to use suspend functions
- New: implement the `Service#performPersonalization()` function
- New: add new `eSE` lifecycle handling

### eid_applet_interface/jni-bridge

- New: interface upgrade to v0.9.0
- New: attach the current thread in some functions to get the current `JNIEnv` of the calling
  function

## Version 0.3.0

_2021-07-13_

### eid-applet-service-lib

- New: Support the SELECT DF_EID that is not supported by the Android OMAPI implementation. The
  latest eID-Applet must be used (4a0f7c9f).

### eid_applet_interface/jni-bridge

- New: remove the `USE_ANDROID_JNI` preprocessor

## Version 0.2.0

_2021-06-30_

### eid-applet-service-lib

- New: disabled the legacy TSMS-API-SDK version
- New: Preliminary version of the `AppletStatus` implemented
- New: `Service#initializePrePersonalization` implemented
- New: `Service#releaseAppletConnection` implemented
- New: Due to MS-1, mocked Responses implemented for the following functions:
  - `Service#appletStatus` returns only the status `AppletStatus#READY`
  - `Service#installApplet` returns only the status `ServiceResult#SUCCESS`
  - `Service#performPersonalization` returns only the `GenericDataResult`
    with `ServiceResult#SUCCESS` and the RAPDU `9000`

### eid_applet_interface/jni-bridge

- New: interface upgrade to v0.7.11
- New: Added class `EidAppletServiceAndroid` for better testability of the jni-bridge
- New: `eid_applet_interface` implementation instantiates and delegates now the functions of the
  class `EidAppletServiceAndroid`

## Version 0.1.2

_2021-04-29_

### eid-applet-service-lib

- Fix: packaging type within the pom file

### eid_applet_interface/jni-bridge

- Fix: use the `getClassLoader` method of the `android.app.NativeActivity`
  instance to prevent the `ClassNotFoundException` for the `Service.kt`

## Version 0.1.1

_2021-04-21_

### eid_applet_interface/jni-bridge

- New: upgrade to version 0.7.7
- Fix: remove old-style casts
- Fix: unused function parameters
- Fix: missing default value for `getGenericDataResultWithSuccess`

### eid-applet-service-lib

- Fix: automated pom file generation
- New: currently used sdk version is tested within the library
  initialization process

## Version 0.1.0

_2021-04-01_

Initial version of the eid-applet-service-lib, which reflects the
current work status and is used exclusively for initial integration.

- eid_applet_interface/jni-bridge version 0.7.5
- uses the legacy TSMAPI Client v0.6.1 from the project Optimos
