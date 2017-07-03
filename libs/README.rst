Libraries
=========

Um die AusweisApp2 zu bauen ist eine Toolchain erforderlich, die die
Abhängigkeiten und die Compilertools beinhaltet.

Unterstützte Compiler:

- MinGW 32 / 64 >= 4.9

- GCC >= 4.7

- Clang >= 3.4

- MSVC >= 2015


Notwendige Bibliotheken:

- Qt >= 5.7

  - http://www.qt.io/download/

- OpenSSL >= 1.0.2

  - https://www.openssl.org/source/

  - Aus dem Ordner "patches" müssen die folgenden Patches angewandt werden.
    (Sofern der automatische Build mittels CMake gestartet wird, werden
    die Patches automatisch angewandt.)

    - openssl_rsa_psk.patch

- pcsclite >= 1.8 (nur Linux)


Notwendige Tools:

- CMake >= 3.1.0

  - http://www.cmake.org



Build
-----
Die Abhängigkeiten lassen sich mittels der CMakeLists.txt in diesem Ordner
automatisch aufbauen.

Das Skript kann die abhängigen Bibliotheken als Quellcode selbständig herunterladen,
entpacken und bauen. Dabei ist allerdings zu beachten, dass CMake erst ab v3.1.0
die notwendigen Kompressionen beherrscht [1].

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


[1] http://www.cmake.org/Bug/view.php?id=13515

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
Unter MacOS ist die Einrichtung relativ einfach und bedarf nur der oben gennanten Voreinstellungen.
Es wird der von Apple ausgelieferte clang compiler verwendet.

Beispiel: Innerhalb von /Users/governikus/AusweisApp2 befindet sich der Quellcode.

::

   $ cd /Users/governikus
   $ mkdir build
   $ cd build
   $ cmake -DPACKAGES_DIR=/Users/governikus/packages ../AusweisApp2/libs
   $ make


Windows MinGW
^^^^^^^^^^^^^
Unter Windows ist es derzeit empfohlen einen Teil der Toolchain mittels MSYS zu bauen.
Perl muss dafür ebenfalls installiert sein.

MinGW
"""""

- http://sourceforge.net/projects/mingw-w64/

  - Dabei wurde das folgende Paket getestet:
    https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/5.3.0/threads-posix/dwarf/i686-5.3.0-release-posix-dwarf-rt_v4-rev0.7z/download


MSYS
""""

- http://www.mingw.org/wiki/msys

  - Getestet: 1.0.11


Perl
""""

- http://www.activestate.com/activeperl/downloads

  - Getestet: 5.24.0


Vorbereitung
""""""""""""
#. Download und Entpacken des MinGW Paketes.

#. Eintragen des Ordners "bin" von der MinGW-Installation in %PATH%.

#. Installation von MSYS, welche sich auf die MinGW-Installation bezieht.

#. Der Ordner von Perl muss sich in %PATH% befinden.

#. In der Datei "fstab" unter C:\msys\1.0\etc folgende Einträge mit den jeweiligen Pfaden:

::

   C:/mingw32/i686-5.3.0-release-posix-dwarf-rt_v4-rev0    /mingw
   C:/Perl    /perl


Durch einige Probleme mit Unix-Shell-Skripten ist es derzeit leider
notwendig die Toolchain in zwei Schritten aufzubauen.
Hierzu muss OpenSSL und Qt separat gebaut werden.


openssl / Qt
""""""""""""
Da Qt mittels Batchskript gebaut werden muss, ist es leider nicht möglich dies innerhalb
von MSYS zu bauen [2]. Daher wird OpenSSL und Qt mittels Windows-CLI konfiguriert.
Dabei wird Qt über Windows-CLI und OpenSSL unter MSYS gebaut.

#. cmd.exe von Windows starten

#. mkdir c:\msys\1.0\home\user\qt ("user" ist der Benutzer, der unter MSYS verwendet wird)

#. cd c:\msys\1.0\home\user\qt

#. cmake -DPACKAGES_DIR=C:/packages C:/AusweisApp2/libs -G "MinGW Makefiles"

#. MSYS Shell starten

#. cd qt

#. mingw32-make openssl

#. MSYS Shell verlassen

#. In der cmd.exe: c:\msys\1.0\home\user\qt

#. mingw32-make qt


[2] http://sourceforge.net/p/mingw/bugs/1902/


iOS
"""
Die Toolchain für iOS kann nur auf MacOS gebaut werden. Dabei müssen XCode und
die Command Line Tools (siehe "xcode-select -p" bzw. "xcode-select --install")
auf dem Mac vorhanden sein. Die folgende Anleitung wurde unter MacOS 10.9 und 10.11 getestet.

Ebenfalls muss für den Build-Vorgang von Qt ein iOS Developer-Zertifikat mit Wildcard (*)
im Keystore von MacOS hinterlegt sein.

Beispiel: Innerhalb von /Users/governikus/AusweisApp2 befindet sich der Quellcode.

::

   $ cd /Users/governikus
   $ mkdir build
   $ cd build
   $ cmake -DPACKAGES_DIR=/Users/governikus/packages -DCMAKE_TOOLCHAIN_FILE=../AusweisApp2/cmake/iOS.toolchain.cmake ../AusweisApp2/libs
   $ make


Android
"""""""
Die Toolchain für Android wird derzeit nur unter Linux unterstützt. Dabei müssen folgende
Komponenten vorhanden sein:

- Android NDK mit gesetztem ANDROID_NDK

  - https://developer.android.com/tools/sdk/ndk/index.html

  - Getestet: r12b (x86_64)

- Android SDK mit gesetztem ANDROID_HOME

  - https://developer.android.com/sdk/index.html#Other

  - Getestet: 25.1.7

  - Unter bestimmten Umständen kann es vorkommen, dass die Build-Tools-Version nicht erkannt
    wird. Dies kann mittels der Umgebungsvariable ANDROID_BUILD_TOOLS_REVISION behoben werden.
    Die genaue Version ist im Android Manager vom Android SDK (./tools/android) hinterlegt.

- Um Qt erfolgreich zu bauen, sind verschiedene API Level von Android notwendig.
  Diese sollten mindestens Level 10, 11, 16 und 18 sein. Nähere Informationen dazu
  sind im Wiki von Qt enthalten: http://wiki.qt.io/Android
  Die Plattformen können mittels Android Manager nachinstalliert werden.

- JDK mit gesetztem JAVA_HOME

- Apache Ant mit gesetztem ANT_HOME


Beispiel: Innerhalb von /home/governikus/AusweisApp2 befindet sich der Quellcode.

::

   $ cd /home/governikus
   $ mkdir build
   $ cd build
   $ cmake -DPACKAGES_DIR=/home/governikus/packages -DCMAKE_TOOLCHAIN_FILE=../AusweisApp2/cmake/android.toolchain.cmake ../AusweisApp2/libs
   $ export PATH=/home/governikus/build/standalone/bin:$PATH
   $ make

Standardmäßig wird die Architektur "armeabi-v7a" gewählt. Um zum Beispiel die Toolchain für x86-Architektur
zu bauen , ist beim Aufruf von CMake der Parameter "-DANDROID_ABI=x86" mitzugeben.

Der "export" der PATH-Variable wird auch beim Konfigurieren angezeigt. Siehe dazu "You need to call ..."!
Sofern die PATH-Variable nicht um den standalone-Ordner erweitert wird, wird es beim Build zu dem Fehler kommen,
dass der Cross-Compiler für die jeweilige Architektur nicht gefunden werden konnte.

