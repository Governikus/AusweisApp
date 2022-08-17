eid-applet-service jni bridge
=============================

- Current `eid_applet_interface.h` version: `0.16.0`
- Current `eid-applet-service-lib.aar` version: `0.10.3-180`

## Availability

If you have access to the `partner.bdr.de` network, the
`eid-applet-service-lib` library is also available through the
artifactory maven repository. `app/build.gradle` integration:

```gradle
    implementation "de.bdr.android.eid-applet-service-lib:eid-applet-service-lib:0.10.3-180"
```

Parse and set the credentials within the root `build.gradle`.

```gradle
allprojects {
    repositories {
        // ...
        maven {
            url 'https://partner.bdr.de/artifactory/mid-mvn/'
            credentials {
                username "Your_User_Name"
                password "Your_Password_Or_API_Key"
            }
        }
    }
}
```

## Usage

To use the service library, it must first be initialized correctly. The
function `initializeService(JNIEnv *mEnv, jobject mApplicationContext)`
must be called so that the `JNIEnv` reference and the android
`ApplicationContext` can be passed correctly. If the functionality is no
longer needed, the service can be released by the function
`shutdownService()`. If a function is called without previous
initialization of the library, the return value of the function contains
a corresponding error message. The function could not be executed
successfully.

## Additional remarks

- Currently used ndk version: `21.3.6528147`

License
-------

```
Copyright (C) 2021 Bundesdruckerei GmbH and Governikus GmbH
```

