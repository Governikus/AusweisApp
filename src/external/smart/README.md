eid-applet-service jni bridge
=============================

- Current `eid_applet_interface.h` version: `0.23.0`
- Current `eid-applet-service-lib.aar` version: `1.1.0`

## Description

Creates the interface between the `eID Client` (AA2) application and the
`eid-applet-service-lib` (delivered as .aar - android archive)

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

- Currently used ndk version: `25.2.9519653`

License
-------

EN version:

```
Copyright (C) 2023 by Bundesdruckerei GmbH and Governikus GmbH & Co. KG

The Software is licensed under EUPL-1.2, which can be downloaded from
https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12 in a language of the European Union.
The German version of the EUPL-1.2 is available in LICENSE-DE.txt, the English version in
the LICENSE.txt.
```

DE version:

```
Copyright (C) 2023 by Bundesdruckerei GmbH and Governikus GmbH & Co. KG

Die vorliegende Software steht unter der Lizenz EUPL-1.2, die unter
https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12 in einer Sprache der Europäischen
Union heruntergeladen werden kann. Die deutsche Fassung der EUPL-1.2 liegt als Datei LICENSE-DE.txt
vor, die englische Fassung in der LICENSE.txt.
```

