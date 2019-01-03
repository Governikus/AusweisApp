Deutsch
=======

Windows
-------

Der Installer der AusweisApp2 kann über die Kommandozeile gestartet werden, um
den Installationsprozess zu konfigurieren und systemweite Standardeinstellungen
vorzugeben. Neben den üblichen Parametern [1]_ enthält das folgende Kommando
alle unterstützten Parameter, die im Anschluss erläutert werden.

.. code-block:: winbatch

  msiexec /i AusweisApp2-X.YY.Z.msi /quiet INSTALL_ROOT="C:\AusweisApp2" SYSTEMSETTINGS=false DESKTOPSHORTCUT=false AUTOSTART=false AUTOHIDE=false REMINDTOCLOSE=false ASSISTANT=false TRANSPORTPINREMINDER=false UPDATECHECK=false ONSCREENKEYBOARD=true HISTORY=false

INSTALL_ROOT
  Gibt das Installationsverzeichnis an. Ohne Angabe wird der Ordner
  "C:\\Programme (x86)\\AusweisApp2 X.YY.Z" genutzt.

SYSTEMSETTINGS
  Betrifft die Erstellung von Firewall-Regeln der Windows Firewall. Ohne Angabe
  des Parameters werden die Firewall-Regeln erstellt (true). Durch Angabe von
  SYSTEMSETTINGS=false werden keine Firewall-Regeln erstellt.

DESKTOPSHORTCUT
  Durch Angabe von DESKTOPSHORTCUT=false kann die Erstellung einer
  Desktop-Verknüpfung vermieden werden. Ohne Angabe des Parameters wird eine
  Desktop-Verknüpfung für alle Benutzer erstellt (true).

AUTOSTART
  Durch Angabe von AUTOSTART=true wird ein Autostart-Eintrag für alle Benutzer
  erstellt. Die Deaktivierung des Autostarts ist den Benutzern in der AusweisApp2
  dadurch nicht möglich. Ohne Angabe wird der Autostart-Eintrag nicht erstellt
  (false). In diesem Fall ist es jedoch jedem Benutzer möglich, die Autostart-
  Funktion innerhalb der AusweisApp2 für sich zu aktivieren.

AUTOHIDE
  Betrifft die automatische Minimierung nach Abschluss einer erfolgreichen
  Authentisierung. Ohne Angabe ist diese aktiviert (true). Durch AUTOHIDE=false
  wird diese deaktiviert. Der Benutzer kann diese Einstellung anpassen.

REMINDTOCLOSE
  Wenn der Benutzer die AusweisApp2 per Klick auf das X schließt, wird er darauf
  hingewiesen, dass nur die Benutzeroberfläche geschlossen wird und die
  AusweisApp2 weiterhin im Infobereich zur Verfügung steht. Zu diesem Zeitpunkt
  ist es möglich, den Hinweis zukünftig zu unterdrücken. Durch REMINDTOCLOSE=false
  kann dieser Hinweis von vornherein deaktiviert werden. Ohne Angabe ist er
  aktiviert (true).

ASSISTANT
  Startet der Benutzer die AusweisApp2 zum ersten Mal, wird die Benutzeroberfläche
  geöffnet und ein Einrichtungsassistent angezeigt. Bei jedem weiteren Start wird
  die AusweisApp2 im Hintergrund gestartet und der Einrichtungsassistent erscheint
  nicht. Durch ASSISTANT=false wird die AusweisApp2 auch beim ersten Start im
  Hintergrund ohne Einrichtungsassistenten gestartet. Ohne Angabe ist der
  Einrichtungsassistent aktiviert (true).

TRANSPORTPINREMINDER
  Zu Beginn einer Selbstauskunft oder Authentisierung wird der Benutzer einmalig
  danach gefragt, ob er die Transport-PIN schon geändert hat. Durch
  TRANSPORTPINREMINDER=false kann diese Abfrage deaktiviert werden. Ohne Angabe
  ist die Abfrage aktiviert (true).

UPDATECHECK
  Wird die Benutzeroberfläche der AusweisApp2 geöffnet, wird eine Überprüfung auf
  eine neue Version der AusweisApp2 gestartet, falls seit der letzten Überprüfung
  mindestens 24 Stunden vergangen sind. Liegt eine neue Version vor, wird der
  Benutzer darüber in einem Dialog informiert. Durch Setzen von UPDATECHECK auf
  false oder true kann diese Überprüfung deaktiviert bzw. aktiviert werden.
  Die Einstellung kann dann durch den Benutzer in der AusweisApp2 nicht geändert
  werden. Ohne Angabe ist die Überprüfung aktiviert, der Benutzer kann die
  Einstellung jedoch ändern.

ONSCREENKEYBOARD
  Für die Eingabe von PIN, CAN und PUK kann eine Bildschirmtastatur verwendet
  werden. Durch Setzen von ONSCREENKEYBOARD auf false oder true kann diese
  deaktiviert bzw. aktiviert werden. Der Benutzer kann diese Einstellung anpassen.

HISTORY
  Jede Selbstauskunft oder Authentisierung wird im Verlauf gespeichert. Dabei
  werden jedoch keine persönlichen Daten gespeichert, sondern nur der Zeitpunkt,
  der Diensteanbieter und die ausgelesenen Datenfelder (ohne Inhalt). Durch Setzen
  von HISTORY auf false oder true kann der Verlauf deaktiviert bzw. aktiviert
  werden. Der Benutzer kann diese Einstellung anpassen.

Alternativ kann mit Orca [2]_ eine MST-Datei erzeugt werden, die die oben
genannten Parameter definiert. Die Parameter sind in den Tabellen "Directory"
und "Property" verfügbar. Übergeben lässt sich die MST-Datei mit dem folgenden
Kommando:

.. code-block:: winbatch

  msiexec /i AusweisApp2-X.YY.Z.msi /quiet TRANSFORMS=file.mst

macOS
-----

Unter macOS ist keine Installation per Kommandozeile vorgesehen. Jedoch können
einige der oben genannten Einstellung durch eine plist-Datei im Verzeichnis
/Library/Preferences systemweit vorgegeben werden. Diese plist-Datei muss dabei
manuell durch den Administrator des Systems hinterlegt werden und wird von allen
(zukünftigen) Installationen der AusweisApp2 verwendet. Alle nicht genannten
Einstellungen werden auf macOS nicht unterstützt. Der Name der Datei muss
"com.governikus.AusweisApp2.plist" lauten. Der Inhalt wird im folgenden
dargestellt:

.. code-block:: xml

  <?xml version="1.0" encoding="UTF-8"?>
  <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
  <plist version="1.0">
  <dict>
    <key>autoCloseWindow</key>
    <false/>
    <key>remindToClose</key>
    <false/>
    <key>showSetupAssistant</key>
    <false/>
    <key>transportPinReminder</key>
    <false/>
    <key>common.autoUpdateCheck</key>
    <false/>
    <key>common.keylessPassword</key>
    <true/>
    <key>history.enable</key>
    <false/>
  </dict>
  </plist>

Für die einzelnen Werte gelten die gleichen Beschreibungen wie für die
Windows-Version wobei die Bennennung der Attribute der folgenden Tabelle zu
entnehmen ist.

====================== ====================
macOS                  Windows
====================== ====================
autoCloseWindow        AUTOHIDE
remindToClose          REMINDTOCLOSE
showSetupAssistant     ASSISTANT
transportPinReminder   TRANSPORTPINREMINDER
common.autoUpdateCheck UPDATECHECK
common.keylessPassword ONSCREENKEYBOARD
history.enable         HISTORY
====================== ====================

.. [1] https://docs.microsoft.com/de-de/windows/desktop/msi/standard-installer-command-line-options
.. [2] https://docs.microsoft.com/de-de/windows/desktop/Msi/orca-exe
