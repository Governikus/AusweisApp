Libraries
=========

Um die AusweisApp2 zu bauen ist eine Toolchain erforderlich, die die
Abhängigkeiten und die Compilertools beinhaltet.

Unterstützte C++17 Compiler:

- MinGW / GCC >= 7.3

- Clang >= 5.0

- MSVC >= 2017


Notwendige Bibliotheken:

- Qt >= 5.10

  - http://www.qt.io/download/

- OpenSSL >= 1.0.2

  - https://www.openssl.org/source/

  - LibreSSL wird auf Grund des fehlenden RSA-PSK nicht unterstützt.

- pcsclite >= 1.8 (nur Linux/FreeBSD)


Notwendige Tools:

- CMake >= 3.8.0 (3.14.0 >= für iOS)

  - http://www.cmake.org



Build
-----
Die Abhängigkeiten lassen sich mittels der CMakeLists.txt in diesem Ordner
automatisch aufbauen.
Das Skript kann die abhängigen Bibliotheken als Quellcode selbständig herunterladen,
entpacken und bauen.

Lokale Pakete können mittels -DPACKAGES_DIR=C:/packages verwendet werden.
Wichtig: Bei PACKAGES_DIR muss ein absoluter Pfad angegeben werden!

Der Build umfasst unter anderem das Qt-Framework, daher kann (je nach Rechenleistung)
der Build einige Stunden dauern.

Wichtig bei der Angabe in CMake ist der Verweis auf den Ordner "libs". Ein Verweis
direkt auf "AusweisApp2" würde den Build für die "AusweisApp2" konfigurieren.

Nach dem Aufruf "make" bzw. "mingw32-make" werden nun alle Bibliotheken gebaut und in
dem Ordner ./dist installiert. Dieser Ordner kann beim Build von der AusweisApp2 mittels
-DCMAKE_PREFIX_PATH als Toolchain angegeben werden.
Zusätzlich kann mit dem make Target "compress" der Inhalt der dist-Ordner bereinigt und
ein Tarball aus den gebauten Bibliotheken erzeugt werden.


Proxy
^^^^^
Sofern beim Download der Pakete ein Proxy notwendig ist, müssen vorm Aufruf von "make"
folgende Umgebungsvariablen gesetzt werden. (Ein erneutes Ausführen von CMake ist nicht
notwendig.)

::

   http_proxy=host:port
   https_proxy=host:port
   ftp_proxy=host:port



macOS
^^^^^
Unter MacOS ist die Einrichtung relativ einfach und bedarf nur der oben genannten Voreinstellungen.
Es wird der von Apple ausgelieferte clang compiler verwendet.

Beispiel: Innerhalb von /Users/governikus/AusweisApp2 befindet sich der Quellcode.

::

   $ cd /Users/governikus
   $ mkdir build
   $ cd build
   $ cmake -DCMAKE_BUILD_TYPE=release -DPACKAGES_DIR=/Users/governikus/packages ../AusweisApp2/libs
   $ make


Windows MinGW
^^^^^^^^^^^^^
Unter Windows ist es derzeit empfohlen einen Teil der Toolchain mittels MSYS2 zu bauen.
Perl muss dafür in MSYS2 nachinstalliert werden. Außerdem ist das Windows SDK notwendig.

MinGW
"""""

- http://sourceforge.net/projects/mingw-w64/

  - Dabei wurde das folgende Paket getestet:
    https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/7.3.0/threads-posix/dwarf/i686-7.3.0-release-posix-dwarf-rt_v5-rev0.7z/download


MSYS2
"""""

- https://sourceforge.net/projects/msys2/files/Base/x86_64/

  - Getestet: msys2-base-x86_64-20180531.tar.xz


Windows SDK
"""""""""""

- https://developer.microsoft.com/de-de/windows/downloads/windows-10-sdk

  - Getestet: 10.0.14393.795 und 10.0.16299.0


Python 2
""""""""

- https://www.python.org/downloads/

  - Getestet: python-2.7.15.amd64.msi


Vorbereitung
""""""""""""
#. Download und Entpacken des MinGW Paketes.

#. Eintragen des Ordners "bin" von der MinGW-Installation in %PATH%.

#. Entpacken von MSYS2.

#. Start von "msys2_shell.cmd -use-full-path".

#. System aktualisieren mittels "pacman -Syu" (danach MSYS2 neustarten).

#. Perl nachinstallieren mittels "pacman -S perl".

#. Installation des Windows SDK und setzen einer Systemvariable:

::

   Für das Windows SDK 10.0.15063.0 und neuer (getestet: 10.0.17763.0):
       WindowsSdkVerBinPath = C:\Program Files (x86)\Windows Kits\10\bin\%VERSION%
   Für alle älteren Versionen:
       WindowsSdkDir = C:\Program Files (x86)\Windows Kits\10

Durch einige Probleme mit Unix-Shell-Skripten ist es derzeit leider
notwendig die Toolchain in zwei Schritten zu bauen.
Hierzu muss OpenSSL und Qt separat gebaut werden.

#. Download und Start der Installation von Python.

#. Sicherstellen, dass die python.exe während der Installation zum PATH hinzugefügt wird.


Eventuell muss für MinGW folgende Option gesetzt werden (QTBUG-16443):

#. Windows --> gpedit.msc --> Enter (als Administrator)

#. Richtlinien für Lokaler Computer

#. Computerkonfiguration

#. Administrative Vorlagen

#. System

#. Dateisystem

#. Lange Win32-Pfade aktivieren


openssl / Qt
""""""""""""
Da Qt mittels Batchskript gebaut werden muss, ist es leider nicht möglich dies innerhalb
von MSYS2 zu bauen [2]. Daher wird OpenSSL und Qt mittels Windows-CLI konfiguriert.
Dabei wird Qt über Windows-CLI und OpenSSL unter MSYS2 gebaut.

#. cmd.exe von Windows starten

#. mkdir c:\msys64\home\user\qt ("user" ist der Benutzer, der unter MSYS2 verwendet wird)

#. cd c:\msys64\home\user\qt

#. cmake -DCMAKE_BUILD_TYPE=release -DPACKAGES_DIR=C:/packages C:/AusweisApp2/libs -G "MinGW Makefiles"

#. MSYS2 Shell starten ("msys2_shell.cmd -use-full-path")

#. cd qt

#. mingw32-make openssl

#. MSYS2 Shell verlassen

#. In der cmd.exe: c:\msys64\home\user\qt

#. mingw32-make qt


[2] http://sourceforge.net/p/mingw/bugs/1902/


iOS
"""
Die Toolchain für iOS kann nur auf MacOS gebaut werden. Dabei müssen XCode und
die Command Line Tools (siehe "xcode-select -p" bzw. "xcode-select --install")
auf dem Mac vorhanden sein. Die folgende Anleitung wurde unter macOS 10.12 getestet.

Ebenfalls muss für den Build-Vorgang von Qt ein iOS Developer-Zertifikat mit Wildcard (*)
im Keystore von MacOS hinterlegt sein.

Beispiel: Innerhalb von /Users/governikus/AusweisApp2 befindet sich der Quellcode.

::

   $ cd /Users/governikus
   $ mkdir build
   $ cd build
   $ cmake -DCMAKE_BUILD_TYPE=release -DPACKAGES_DIR=/Users/governikus/packages -DCMAKE_TOOLCHAIN_FILE=../AusweisApp2/cmake/iOS.toolchain.cmake ../AusweisApp2/libs
   $ make


Android
"""""""
Die Toolchain für Android wird derzeit nur unter Linux unterstützt. Dabei müssen folgende
Komponenten vorhanden sein:

- Android NDK mit gesetztem ANDROID_NDK

  - https://developer.android.com/tools/sdk/ndk/index.html

  - Getestet: r18 (https://wiki.qt.io/Qt_for_Android_known_issues)

- Android SDK mit gesetztem ANDROID_HOME

  - https://developer.android.com/studio/releases/sdk-tools.html

  - Getestet: 26.1.1

- SDK build tools

  - https://developer.android.com/studio/releases/build-tools

  - Getestet: 28.0.3

- SDK platform tools

  - https://developer.android.com/studio/releases/platform-tools

  - Getestet: 28.0.1

- Um Qt erfolgreich zu bauen, ist mindestens ein API-Levelpaket von Android notwendig.
  Dieses sollte mindestens Level 21 sein. Nähere Informationen dazu
  sind im Wiki von Qt enthalten: http://wiki.qt.io/Android
  Die Plattformen können mittels Android Manager nachinstalliert werden.

- JDK mit gesetztem JAVA_HOME



Beispiel: Innerhalb von /home/governikus/AusweisApp2 befindet sich der Quellcode.

::

   $ cd /home/governikus
   $ mkdir build
   $ cd build
   $ cmake -DCMAKE_BUILD_TYPE=release -DPACKAGES_DIR=/home/governikus/packages -DCMAKE_TOOLCHAIN_FILE=../AusweisApp2/cmake/android.toolchain.cmake ../AusweisApp2/libs
   $ make

Standardmäßig wird die Architektur "armeabi-v7a" gewählt. Um zum Beispiel die Toolchain für x86-Architektur
zu bauen, ist beim Aufruf von CMake der Parameter "-DCMAKE_ANDROID_ARCH_ABI=x86" mitzugeben.

