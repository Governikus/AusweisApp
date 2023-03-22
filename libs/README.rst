Libraries
=========

Um die AusweisApp2 zu bauen ist eine Toolchain erforderlich, die die
Abhängigkeiten und die Compilertools beinhaltet.

Unterstützte C++17 Compiler:

- MinGW / GCC >= 11.2

- Clang >= 5.0

- MSVC >= 2017


Notwendige Bibliotheken:

- Qt >= 5.15

  - http://www.qt.io/download/

- OpenSSL >= 1.1.0

  - https://www.openssl.org/source/

  - LibreSSL wird auf Grund des fehlenden RSA-PSK nicht unterstützt.

- pcsclite >= 1.8 (nur Linux/FreeBSD)


Notwendige Tools:

- CMake >= 3.13.0 (3.14.0 >= für iOS)

  - http://www.cmake.org



Build
-----
Die Abhängigkeiten lassen sich mittels der CMakeLists.txt in diesem Ordner
automatisch aufbauen.
Das Skript kann die abhängigen Bibliotheken als Quellcode selbständig herunterladen,
entpacken und bauen.

Lokale Pakete können optional mittels -DPACKAGES_DIR=C:/packages verwendet werden.
Wichtig: Bei PACKAGES_DIR muss ein absoluter Pfad angegeben werden!

Der Build umfasst unter anderem das Qt-Framework, daher kann (je nach Rechenleistung)
der Build einige Stunden dauern.

Wichtig bei der Angabe in CMake ist der Verweis auf den Ordner "libs". Ein Verweis
direkt auf "AusweisApp2" würde den Build für die "AusweisApp2" konfigurieren.

Nach dem Aufruf "nmake"/"mingw32-make"/"ninja" werden nun alle Bibliotheken gebaut und
in dem Ordner ./dist installiert. Dieser Ordner kann beim Build von der AusweisApp2
mittels -DCMAKE_PREFIX_PATH als Toolchain angegeben werden.
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
Beispiel: Innerhalb von macOS /Users/governikus/AusweisApp2 befindet sich der Quellcode.
::

   $ cd /Users/governikus
   $ mkdir build
   $ cd build
   $ cmake -DCMAKE_BUILD_TYPE=release ../AusweisApp2/libs
   $ make


Linux / Unix
^^^^^
Die Einrichtung unter Linux/Unix ist einfach und erfordert nur die oben genannten Einstellungen. Die meisten Distributionen haben den clang Compiler bereits vorinstalliert, aber er kann auch nachträglich installiert werden.
Beispiel: Innerhalb von Linux/Unix /home/governikus/AusweisApp2. befindet sich der Quellcode
::

   $ cd /home/governikus
   $ mkdir build
   $ cd build
   $ cmake -DCMAKE_BUILD_TYPE=release ../AusweisApp2/libs
   $ make


Windows
^^^^^^^
Unter Windows müssen zunächst die Vorraussetzungen für die Buildumgebung geschaffen
werden. Hierzu müssen folgende Programme im System installiert und wie vermerkt
konfiguriert werden.


Windows SDK
"""""""""""

- Manuelle Installation nur für MinGW notwendig, kann für MSVC per Visual Studio
  installiert werden.

- https://developer.microsoft.com/de-de/windows/downloads/windows-sdk/

  - Getestet: 10.0.22621.1

  - Umgebungsvariable setzen:

    - Für das Windows SDK 10.0.15063.0 und neuer:

      - WindowsSdkVerBinPath = C:\Program Files (x86)\Windows Kits\10\bin\%VERSION%

    - Für alle älteren Versionen:

      - WindowsSdkDir = C:\Program Files (x86)\Windows Kits\10


CMake
"""""

- https://cmake.org/download/

  - Getestet: 3.23.2

  - Pfad zur CMake-Executable muss zur Path-Umgebungsvariable hinzugefügt werden


Ninja
"""""

- https://github.com/ninja-build/ninja/releases

- Getestet: v1.11.0

- Qt verwendet intern ninja als Buildtool. Ninja ist nicht zwingend notwendig,
  wird jedoch von Qt empfohlen.

- Pfad zur ninja-Executable muss zur Path-Umgebungsvariable hinzugefügt werden


MinGW
"""""

- https://wiki.qt.io/MinGW

- Getestet: x86_64-11.2.0-release-posix-seh-rt_v9-rev3

- Pfad zu MinGW-bin-Verzeichnis muss zur Path-Umgebungsvariable hinzugefügt
  werden


- Eventuell muss für MinGW folgende Option gesetzt werden [1]:

#. Windows --> gpedit.msc --> Enter (als Administrator)

#. Richtlinien für Lokaler Computer

#. Computerkonfiguration

#. Administrative Vorlagen

#. System

#. Dateisystem

#. Lange Win32-Pfade aktivieren

[1] https://bugreports.qt.io/browse/QTBUG-16443


MSVC
""""

- Installation von MSVC (und evtl. Windows SDK) über Visual Studio:
  https://visualstudio.microsoft.com/de/

- Getestet: Visual Studio 2022

- Pfad zu VS-Environment-Skripten zur Path-Umgebungsvariable hinzufügen

  - Je nach VS-Edition: C:\Program Files\Microsoft Visual Studio\<jahr>\<edition>\VC\Auxiliary\Build


Python
""""""

- https://www.python.org/downloads/

  - Getestet: 3.10.5

  - Pfad zur Python-Executable muss zur Path-Umgebungsvariable hinzugefügt werden


Perl
""""

- Sowohl für Qt als auch für OpenSSL ist Perl erforderlich. Für OpenSSL ist
  relevant, welche Art von Pfaden (Unix oder Windows) Perl verwendet.

    - Für Builds mit MinGW wird MSYS2 Perl benötigt.

    - Für Builds mit MSVC wird ActivePerl oder StrawberryPerl benötigt.


- MSYS2 Perl

    - https://github.com/msys2/msys2-installer/releases/

    - Getestet: msys2-base-x86_64-20220603.tar.xz

    - MSYS2 sollte immer über den Befehl "msys2_shell.cmd -use-full-path" gestartet
      werden, da MSYS2 sonst einige eigene Pfade nicht findet.

    - Nach Entpacken MSYS2 mit "pacman -Syu" aktualisieren.

    - Wenn sich kein weiteres Perl im Pfad befindet muss für den Build von Qt
      <msys_base>/usr/bin zum Pfad hinzugefügt werden.

- ActivePerl/StrawberryPerl

    - Es kann entweder ActivePerl(https://www.activestate.com/products/perl/) oder
      StrawberryPerl(https://strawberryperl.com/, hierbei den QTBUG-102828[1]
      beachten) verwendet werden.

    - Getestet: strawberry-perl-5.32.1.1-64bit.msi

    - Pfad zur Perl-Executable muss zur Path-Umgebungsvariable hinzugefügt werden.

        - Hierbei muss darauf geachtet werden, dass das Perl-Verzeichnis
          vor andere Perl Installationen (z.B. MSYS2) aufgeführt wird, sodass
          diese Version anderen Perl Installationen vorgezogen wird.

[2] https://bugreports.qt.io/browse/QTBUG-102828


OpenSSL / Qt mit MinGW
""""""""""""""""""""""

Da Qt mittels Batchskript gebaut werden muss, ist es leider nicht möglich dies innerhalb
von MSYS2 zu bauen [3]. Daher wird OpenSSL und Qt mittels Windows-CLI konfiguriert.
Dabei wird Qt über Windows-CLI und OpenSSL unter MSYS2 gebaut.

#. cmd.exe von Windows starten

#. mkdir c:\msys64\home\user\qt ("user" ist der Benutzer, der unter MSYS2 verwendet wird)

#. cd c:\msys64\home\user\qt

#. cmake -DCMAKE_BUILD_TYPE=release C:/AusweisApp2/libs -G "MinGW Makefiles"

#. MSYS2 Shell starten ("msys2_shell.cmd -use-full-path")

#. cd qt

#. mingw32-make openssl

#. MSYS2 Shell verlassen

#. In der cmd.exe: c:\msys64\home\user\qt

#. mingw32-make qt


[3] http://sourceforge.net/p/mingw/bugs/1902/


OpenSSL / Qt mit MSVC
"""""""""""""""""""""

#. cmd.exe von Windows starten

#. mkdir c:\qt

#. cd c:\qt

#. call vcvarsall.bat amd64

#. cmake -DCMAKE_BUILD_TYPE=release C:/AusweisApp2/libs -G "NMake Makefiles"

#. nmake


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
   $ cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=../AusweisApp2/cmake/iOS.toolchain.cmake ../AusweisApp2/libs
   $ make


Android
"""""""
Die Toolchain für Android wird derzeit nur unter Linux unterstützt. Dabei müssen folgende
Komponenten vorhanden sein:

- Android NDK mit gesetztem ANDROID_NDK_ROOT

  - https://developer.android.com/tools/sdk/ndk/index.html

  - Getestet: r21e (https://wiki.qt.io/Qt_for_Android_known_issues)

- Android SDK (cmdline) mit gesetztem ANDROID_SDK_ROOT

  - https://developer.android.com/studio#cmdline-tools

  - Getestet: 26.1.1 / 3.0

- SDK build tools

  - https://developer.android.com/studio/releases/build-tools

  - Getestet: 30.0.3

- SDK platform tools

  - https://developer.android.com/studio/releases/platform-tools

  - Getestet: 30.0.3

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
   $ cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=../AusweisApp2/cmake/android.toolchain.cmake ../AusweisApp2/libs
   $ make

Standardmäßig wird die Architektur "armeabi-v7a" gewählt. Um zum Beispiel die Toolchain für x86-Architektur
zu bauen, ist beim Aufruf von CMake der Parameter "-DCMAKE_ANDROID_ARCH_ABI=x86" mitzugeben.

