AusweisApp2
===========

Kontakt
-------
| Governikus GmbH & Co. KG
| Hochschulring 4
| 28359 Bremen
| support@ausweisapp.de


Lizenz
------
Der vorliegende Quellcode wird unter der EUPL v1.2 bereitgestellt, mit
Ausnahme der Bibliothek OpenSSL, die unter der OpenSSL License / SSLeay License
lizenziert ist. Die Datei ``LICENSE.officially.txt`` gilt ausschließlich für
die offizielle Version der AusweisApp2, welche von der Governikus GmbH & Co. KG
im Auftrag des Bundes unter https://www.ausweisapp.bund.de bereitgestellt wird.


Toolchain / Bibliotheken
------------------------
Die Einrichtung der Toolchain wird im Unterordner ``./libs``
bzw. in der darin enthaltenen README erläutert.

Die separate README und das Skript unter "libs" dienen dem Aufsetzen
der notwendigen Build-Umgebung und dem automatisierten Bauen der
notwendigen Bibliotheken mit den entsprechenden Patches.



Build
-----
Um die AusweisApp2 zu bauen, ist es notwendig, ein Makefile mittels CMake zu
generieren. Dazu kann CMake auf der Kommandozeile oder mit der von CMake
mitgelieferten CMake-GUI ausgeführt werden.

CMake erkennt während der Konfigurationszeit automatisch die Abhängigkeiten.
Dazu kann die Variable *CMAKE_PREFIX_PATH* verwendet werden, um die Toolchain CMake
bekannt zu machen. Alternativ zu `%PATH%` bzw. `$PATH` können alle Ordner, die dort
für den Build eingetragen wurden, über diesen Mechanismus an CMake übergeben werden.

Als Generator für Makefiles sollte unter Windows eine Variante von "MinGW Makefiles"
gewählt werden.

Beim Generieren des Makefiles ist zu beachten, dass die AusweisApp2 nur sogenannte
"out of source tree"-Builds erlaubt. Daher ist die empfohlene Variante von CMake
zwingend einzuhalten, und der Build-Ordner darf sich nicht im Source-Ordner
befinden.

Beispiel über die CLI:

::

   C:/AusweisApp2/
   C:/Toolchain/
   C:/build/

::

   $ cd C:/build
   $ cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:/Toolchain ../AusweisApp2 -DCMAKE_BUILD_TYPE=release
   -- The CXX compiler identification is GNU 4.9.1
   -- Check for working CXX compiler: C:/mingw32/bin/g++.exe
   -- Check for working CXX compiler: C:/mingw32/bin/g++.exe -- works
   -- Detecting CXX compiler ABI info
   -- Detecting CXX compiler ABI info - done
   -- VERSION: 1.0.0
   -- CMAKE_INSTALL_PREFIX: C:/build/dist
   -- CMAKE_BUILD_TYPE: RELEASE
   -- CMAKE_PREFIX_PATH: C:/Toolchain
   -- CMAKE_INCLUDE_PATH:
   -- CMAKE_LIBRARY_PATH:
   -- CMAKE_SYSTEM_PREFIX_PATH: C:\Program Files;C:\Program Files (x86);C:/Program Files (x86)/CMake;C:/Program Files (x86)/AusweisApp2
   -- CMAKE_SYSTEM_INCLUDE_PATH:
   -- BUILD_SHARED_LIBS: OFF
   -- Found OpenSSL: C:/Toolchain/lib/libssl.dll.a;C:/Toolchain/lib/libcrypto.dll.a (found suitable version "1.0.1i", minimum required is "1.0.1")
   -- Found Hg: C:/Program Files/TortoiseHg/hg.exe (found version "3.1.1")
   -- Configuring done
   -- Generating done
   -- Build files have been written to: C:/build


Um die mobile Variante der AusweisApp2 zu bauen, benötigt man je nach Plattform zusätzliche
externe Komponenten, die in der README in ``./libs`` im Abschnitt Android / iOS beschrieben
sind.



Android
^^^^^^^
Bei Android ist zu beachten, dass ein CMAKE_TOOLCHAIN_FILE angegeben werden muss.

::

   $ cd build
   $ cmake -DCMAKE_PREFIX_PATH=/home/governikus/Toolchain/dist -DCMAKE_TOOLCHAIN_FILE=../AusweisApp2/cmake/android.toolchain.cmake ../AusweisApp2
   $ make
   $ make install
   $ make apk

Unter dem Ordner "./dist/bin" ist nun ein "AusweisApp2...apk" erstellt worden.

Sofern der Parameter *CMAKE_BUILD_TYPE* auf RELEASE gesetzt wird, sind folgende CMake
Parameter notwendig um das APK zu signieren.

::

   -DAPK_SIGN_KEYSTORE=/home/governikus/ausweisapp2.apk.keystore.jks
   -DAPK_SIGN_KEYSTORE_ALIAS=ausweisapp
   -DAPK_SIGN_KEYSTORE_PSW=123456

Wie schon bei der Toolchain wird standardmäßig die Architektur "armeabi-v7a" verwendet.
Dies kann mittels CMake Parameter "-DCMAKE_ANDROID_ARCH_ABI=x86" verändert werden.



iOS
^^^
Bei iOS wird ein Bundle gebaut, welches mit einem zusätzlichen Befehl in eine IPA-Datei
eingebettet werden kann. Zu beachten ist vor allem, dass in der Umgebungsvariable
*PROVISIONING_PROFILE* das jeweilige Profil verwendet wird, welches unter XCode
eingerichtet ist. Im Ordner "~/Library/MobileDevice/Provisioning Profiles"
sind diese ebenfalls einsehbar.
Unter Umständen kann es zu Berechtigungsproblemen mit XCode und dem Zugriff auf
die Keys kommen. Dabei ist es oft hilfreich, die Keys im macOS-Schlüsselbund
freizuschalten bzw. den Schlüsselbund freizugeben.

security unlock-keychain -pPASSWORD ${HOME}/Library/Keychains/login.keychain

Für iOS wird die AusweisApp2 mittels XCode gebaut!

::

   $ cd build
   $ cmake -DCMAKE_PREFIX_PATH=/Users/governikus/Toolchain/dist -DCMAKE_TOOLCHAIN_FILE=../AusweisApp2/cmake/iOS.toolchain.cmake -DCMAKE_BUILD_TYPE=MinSizeRel ../AusweisApp2 -GXcode
   $ xcodebuild -configuration MinSizeRel -archivePath AusweisApp2.xcarchive -scheme AusweisApp archive
   $ xcodebuild -configuration MinSizeRel -archivePath AusweisApp2.xcarchive -exportArchive -exportOptionsPlist exportOptions.plist -exportPath .


Im Build-Ordner befindet sich nun ein *.ipa und ein *.xcarchive, welches jeweils das
signierte Bundle enthält!



Packages
--------
Nachdem die Build-Umgebung eingerichtet ist, kann je nach System ein Package erstellt werden.

- Unter Windows ist hierfür noch das WiX Toolset (http://wixtoolset.org/ Getestet: 3.8 bis 3.10)
  notwendig.
  Mittels "mingw32-make package" wird die AusweisApp2 gebaut und ein MSI Paket bereitgestellt.

- Unter macOS wird mittels "make package" die AusweisApp2 gebaut und ein DMG bereitgestellt.

- Um ein APK für Android zu bauen, sind zwei Schritte notwendig, da CMake das Format bisher
  nicht unterstützt. Daher sind nach der Konfiguration des Projektes folgende Befehle notwendig,
  um ein endgültiges APK zu erhalten:

  ::

     $ make install
     $ make apk



Reproduzierbarer Build
----------------------
Wir sind stets bemüht, den Build des offiziellen Binaries der AusweisApp2 nachvollziehbar zu gestalten.
Daher haben wir unter anderem eine README in dem Unterordner ``./libs`` hinterlegt, die den Aufbau
der Buildumgebung und den Build der externen Bibliotheken beschreibt.
Anhand dieser Anleitung können Sie nachvollziehen, wie unser internes Buildsystem aufgebaut ist und
welche Compiler bzw. Compiler-Versionen wir verwenden.

Im Unterordner ``./resources/jenkins/`` ist es möglich, unsere Konfiguration des CI-Servers einzusehen.
Die Konfiguration besteht aus mehreren Dockerfiles und JobDSL-Dateien.

Anhand dieser Skripte ist es möglich, den Build der AusweisApp2 zu reproduzieren.
Ein Unterschied zum offiziellen Binary sollte lediglich in eventuellen Pfaden,
einem Datum bzw. Zeitstempel und Signaturen bestehen.

.. seealso::
  https://reproducible-builds.org/
