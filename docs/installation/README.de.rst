Deutsch
=======

Windows
-------

Der Installer der AusweisApp2 kann über die Kommandozeile gestartet werden, um
den Installationsprozess zu konfigurieren und systemweite Standardeinstellungen
vorzugeben.
Der Rückgabewert von msiexec informiert über das Ergebnis der Installation [#msiexecreturnvalues]_.
Neben den üblichen Parametern [#standardarguments]_ enthält das folgende Kommando
alle unterstützten Parameter, die im Anschluss erläutert werden.

.. code-block:: winbatch

  msiexec /i AusweisApp2-X.YY.Z.msi /quiet INSTALLDIR="C:\AusweisApp2" SYSTEMSETTINGS=false DESKTOPSHORTCUT=false PROXYSERVICE=false AUTOSTART=false AUTOHIDE=false REMINDTOCLOSE=false ASSISTANT=false TRANSPORTPINREMINDER=false CUSTOMPROXYTYPE="HTTP" CUSTOMPROXYHOST="proxy.example.org" CUSTOMPROXYPORT=1337 UPDATECHECK=false ONSCREENKEYBOARD=true SHUFFLESCREENKEYBOARD=true HISTORY=false ENABLECANALLOWED=true SKIPRIGHTSONCANALLOWED=true LAUNCH=true

INSTALLDIR
  Gibt das Installationsverzeichnis an. Ohne Angabe wird der Ordner
  "C:\\Programme\\AusweisApp2" genutzt.

SYSTEMSETTINGS
  Betrifft die Erstellung von Firewall-Regeln der Windows Firewall. Ohne Angabe
  des Parameters werden die Firewall-Regeln erstellt (true). Durch Angabe von
  SYSTEMSETTINGS=false werden keine Firewall-Regeln erstellt.

DESKTOPSHORTCUT
  Durch Angabe von DESKTOPSHORTCUT=false kann die Erstellung einer
  Desktop-Verknüpfung vermieden werden. Ohne Angabe des Parameters wird eine
  Desktop-Verknüpfung für alle Benutzer erstellt (true).

PROXYSERVICE
  Um den parallelen Betrieb mehrer Instanzen der AusweisApp2 zu ermöglichen, ist
  der Proxy-Dienst notwendig. Der Proxy-Dienst übernimmt die Überwachung von Port
  24727 (definiert in BSI TR-03124-1) und leitet Anfragen an die lokalen Instanzen
  der AusweisApp2 weiter. Eine Weiterleitung der Discovery-Nachrichten (Ergänzung
  zu BSI TR-03112-6 - IFD Service - Kapitel 3) erfolgt nicht, so dass SaK-Geräte
  in diesem Betriebsmodus nicht erkannt bzw. genutzt werden können. Ohne Angabe des
  Parameters wird der Proxy-Dienst automatisch eingerichtet, wenn Terminaldienste
  installiert sind und das System im Anwendungsservermodus ausgeführt wird.

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

CUSTOMPROXYTYPE
  Teil der Konfiguration eines Proxys. Gültige Typen sind SOCKS5 und HTTP.
  Um einen Proxy zu nutzen müssen alle Parameter gesetzt sein (siehe
  CUSTOMPROXYHOST und CUSTOMPROXYPORT). Der Proxy kann nach der Installation
  über eine Checkbox in den Einstellungen deaktiviert werden.

CUSTOMPROXYHOST
  Teil der Konfiguration eines Proxys. Angabe des Hosts, unter dem der Proxy zu
  erreichen ist. Um einen Proxy zu nutzen müssen alle Parameter gesetzt sein
  (siehe CUSTOMPROXYTYPE und CUSTOMPROXYPORT). Der Proxy kann nach der
  Installation über eine Checkbox in den Einstellungen deaktiviert werden.

CUSTOMPROXYPORT
  Teil der Konfiguration eines Proxys. Angabe des Proxyports. Nur Werte von
  1 bis 65536 sind gültig. Um einen Proxy zu nutzen müssen alle Parameter
  gesetzt sein (siehe CUSTOMPROXYTYPE und CUSTOMPROXYHOST). Der Proxy kann nach
  der Installation über eine Checkbox in den Einstellungen deaktiviert werden.

UPDATECHECK
  Wird die Benutzeroberfläche der AusweisApp2 geöffnet, wird eine Überprüfung auf
  eine neue Version der AusweisApp2 gestartet, falls seit der letzten Überprüfung
  mindestens 24 Stunden vergangen sind. Liegt eine neue Version vor, wird der
  Benutzer darüber in einem Dialog informiert. Durch Setzen von UPDATECHECK auf
  false oder true kann diese Überprüfung deaktiviert bzw. aktiviert werden.
  Die Einstellung kann dann durch den Benutzer in der AusweisApp2 nicht geändert
  werden. Ohne Angabe ist die Überprüfung aktiviert, der Benutzer kann die
  Einstellung jedoch ändern. Der UPDATECHECK Parameter beeinflusst weder die
  Aktualisierung der Anbieterliste noch die Aktualisierung der
  Kartenleserinformationen.

ONSCREENKEYBOARD
  Für die Eingabe von PIN, CAN und PUK kann eine Bildschirmtastatur verwendet
  werden. Durch Setzen von ONSCREENKEYBOARD auf false oder true kann diese
  deaktiviert bzw. aktiviert werden. Der Benutzer kann diese Einstellung anpassen.

SHUFFLESCREENKEYBOARD
  Ist die Bildschirmtastatur aktiviert, können die Zifferntasten zufällig angeordnet werden.
  Durch Setzen von SHUFFLESCREENKEYBOARD auf false oder true kann die zufällige Anordnung
  deaktiviert bzw. aktiviert werden. Der Benutzer kann diese Einstellung anpassen.

HISTORY
  Jede Selbstauskunft oder Authentisierung wird im Verlauf gespeichert. Dabei
  werden jedoch keine persönlichen Daten gespeichert, sondern nur der Zeitpunkt,
  der Anbieter und die ausgelesenen Datenfelder (ohne Inhalt). Durch Setzen
  von HISTORY auf false oder true kann der Verlauf deaktiviert bzw. aktiviert
  werden. Der Benutzer kann diese Einstellung anpassen.

ENABLECANALLOWED
  Aktiviert die Unterstützung für den CAN-Allowed-Modus (Vor-Ort-Auslesen). Wenn ein entsprechendes
  Berechtigungszertifikat vorliegt, muss zum Auslesen die CAN anstelle der PIN eingegeben werden.

SKIPRIGHTSONCANALLOWED
  Überspringt die Anzeige des Berechtigungszertifikat im CAN-Allowed-Modus und wechselt direkt zur
  CAN-Eingabe.

LAUNCH
  Startet die AusweisApp2 nach dem Ende der Installation.

Alternativ kann mit Orca [#orca]_ eine MST-Datei erzeugt werden, die die oben
genannten Parameter definiert. Die Parameter sind in den Tabellen "Directory"
und "Property" verfügbar. Übergeben lässt sich die MST-Datei mit dem folgenden
Kommando:

.. code-block:: winbatch

  msiexec /i AusweisApp2-X.YY.Z.msi /quiet TRANSFORMS=file.mst

Um den Start der AusweisApp2 auf Systemen mit fehlender Grafikbeschleunigung
zu optimieren, kann die Systemvariable "QT_QUICK_BACKEND" auf den Wert
"software" gesetzt werden. In diesem Fall verzichtet die AusweisApp2 auf den
Versuch die Grafikbeschleunigung zu nutzen und startet direkt mit dem
alternativen Softwarerenderer.

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
    <key>customProxyType</key>
    <string>HTTP</string>
    <key>customProxyHost</key>
    <string>proxy.example.org</string>
    <key>customProxyPort</key>
    <integer>1337</integer>
    <key>autoUpdateCheck</key>
    <false/>
    <key>keylessPassword</key>
    <true/>
    <key>shuffleScreenKeyboard</key>
    <true/>
    <key>history.enable</key>
    <false/>
    <key>enableCanAllowed</key>
    <true/>
    <key>skipRightsOnCanAllowed</key>
    <true/>
  </dict>
  </plist>

Für die einzelnen Werte gelten die gleichen Beschreibungen wie für die
Windows-Version wobei die Bennennung der Attribute der folgenden Tabelle zu
entnehmen ist.

======================= =======================
macOS                   Windows
======================= =======================
autoCloseWindow         AUTOHIDE
remindToClose           REMINDTOCLOSE
showSetupAssistant      ASSISTANT
transportPinReminder    TRANSPORTPINREMINDER
customProxyType         CUSTOMPROXYTYPE
customProxyPort         CUSTOMPROXYPORT
customProxyHost         CUSTOMPROXYHOST
autoUpdateCheck         UPDATECHECK
keylessPassword         ONSCREENKEYBOARD
shuffleScreenKeyboard   SHUFFLESCREENKEYBOARD
history.enable          HISTORY
enableCanAllowed        ENABLECANALLOWED
skipRightsOnCanAllowed  SKIPRIGHTSONCANALLOWED
======================= =======================

Nach Änderung der Datei kann es notwending sein, ein erneutes Laden der vom
Betriebssystem gecachten Daten zu erzwingen: :code:`killall -u $USER cfprefsd`

.. [#msiexecreturnvalues] https://docs.microsoft.com/de-de/windows/desktop/msi/error-codes
.. [#standardarguments] https://docs.microsoft.com/de-de/windows/desktop/msi/standard-installer-command-line-options
.. [#orca] https://docs.microsoft.com/de-de/windows/desktop/Msi/orca-exe



Anforderungen an die Einsatzumgebung
------------------------------------

Rechte für Installation und Ausführung
''''''''''''''''''''''''''''''''''''''

Für die Installation der AusweisApp2 sind Administratorrechte erforderlich.

Die Ausführung der AusweisApp2 erfordert keine Administratorrechte.


Verwendete Netzwerk-Ports
'''''''''''''''''''''''''

In :numref:`porttable_de` werden alle von der AusweisApp2 genutzten Ports
aufgelistet.
Eine schematische Darstellung der einzelnen Verbindungen, die von der
AusweisApp2 genutzt werden, ist in :numref:`communicationmodel_de` dargestellt.

Die AusweisApp2 startet einen HTTP-Server, der über Port 24727 erreichbar
ist.
Der Server empfängt nur auf der localhost Netzwerkschnittstelle.
Die Erreichbarkeit dieses lokalen Servers ist für die Onlineausweisfunktion
notwendig, da Anbieter mit einem HTTP-Redirect auf den lokalen Server
umleiten um den Ausweisvorgang in der AusweisApp2 fortzuführen (eID1).
Außerdem wird über den Server die Verwendung der AusweisApp2 von anderen
Anwendungen über eine Websocket-Schnittstelle angeboten (SDK-Funktion, eID-SDK).
Daher müssen eingehende lokale Netzwerkverbindungen auf dem TCP Port 24727
ermöglicht werden.

Bei aktiviertem Proxy-Dienst übernimmt der AusweisApp2-Proxy die Serverfunktionen
der AusweisApp2 auf Port 24727. Die Instanzen der AusweisApp2 erkennen den Proxy
und benutzen in diesem Fall einen zufälligen freien Port auf den der Proxy die
Anfragen weiterleitet.

Für die Verwendung von der "Smartphone als Kartenleser"-Funktion über WLAN
müssen außerdem Broadcasts auf UDP Port 24727 im lokalen Subnetz empfangen
werden können.
Hierzu muss eventuell die AP Isolation im Router deaktiviert werden.

.. _communicationmodel_de:
.. figure:: CommunicationModel_de.pdf

    Kommunikationsmodell der AusweisApp2

Der Installer der AusweisApp2 bietet die Option, für alle angebotenen
Funktionen der AusweisApp2 die erforderlichen Firewall-Regeln in der
Windows-Firewall zu registrieren.
Erfolgt die Registrierung der Firewall-Regeln nicht, wird der Benutzer bei
einem Verbindungsaufbau der AusweisApp2 mit einem Dialog der Windows-Firewall
aufgefordert, die ausgehenden Datenverbindungen zuzulassen.
Durch Registrierung der Firewall-Regeln während der Installation werden diese
Aufforderungen unterbunden.

Für die lokalen Verbindungen eID1 und eID-SDK müssen (unter den gängigen
Standardeinstellungen der Windows-Firewall) keine Regeln in der
Windows-Firewall eingetragen werden.

Die durch den Installer angelegten Regeln werden in Tabelle :numref:`firewalltable_de`
aufgelistet.


TLS-Verbindungen
''''''''''''''''

Es ist generell nicht möglich, die AusweisApp2 mit einem TLS-Termination-Proxy
zu verwenden, da die übertragenen TLS-Zertifikate über eine Verschränkung mit
dem Berechtigungszertifikat aus der Personalausweis-PKI validiert werden.
CA-Zertifikate im Windows-Truststore werden daher ignoriert.

.. raw:: latex

    \begin{landscape}

.. _porttable_de:
.. csv-table:: Netzwerkverbindungen der AusweisApp2
   :header: "Referenz", "Protokoll", "Port", "Richtung", "Optional", "Zweck", "Anmerkungen"
   :widths: 8, 8, 8, 8, 8, 35, 25

   "eID1",    TCP, 24727 [#aa2proxy]_,  "eingehend", "Nein", "Online-Ausweisvorgang, eID-Aktivierung [#TR-03124]_",                                                                   "Nur erreichbar von localhost [#TR-03124]_"
   "eID2",    TCP, 443 [#eidports]_,    "ausgehend", "Nein", "Online-Ausweisvorgang, Verbindung zum Anbieter, TLS-1-2-Kanal [#TR-03124]_",                                            "TLS-Zertifikate verschränkt mit Berechtigungs-Zertifikat [#TR-03124]_"
   "eID3",    TCP, 443 [#eidports]_,    "ausgehend", "Nein", "Online-Ausweisvorgang, Verbindung zum eID-Server, TLS-2-Kanal [#TR-03124]_",                                            "TLS-Zertifikate verschränkt mit Berechtigungs-Zertifikat [#TR-03124]_"
   "eID-SDK", TCP, 24727 [#aa2proxy]_,  "eingehend", "Nein", "Verwendung der SDK-Schnittstelle",                                                                                      "Nur erreichbar von localhost [#TR-03124]_"
   "SaK1",    UDP, 24727 [#aa2proxy]_,  "eingehend", "Ja",   "Smartphone als Kartenleser, Erkennung [#TR-03112]_",                                                                    "Broadcasts"
   "SaK2",    TCP, ,                    "ausgehend", "Ja",   "Smartphone als Kartenleser, Verwendung [#TR-03112]_",                                                                   "Verbindung im lokalen Subnetz"
   "Update",  TCP, 443,                 "ausgehend", "Ja",   "Updates [#govurl]_ zu Anbietern und Kartenlesern sowie Informationen zu neuen AusweisApp2-Versionen [#updatecheck]_ .", "Die Zertifikate der TLS-Verbindung werden mit in der AusweisApp2 mitgelieferten CA-Zertifikaten validiert. Im Betriebssystem hinterlegte CA-Zertifikate werden ignoriert."

.. [#aa2proxy] Oder ein zufälliger Port bei Verwendung des AusweisApp2-Proxys.
.. [#TR-03124] Siehe TR-03124 des BSI
.. [#eidports] Port 443 wird für die initiale Kontaktaufnahme zum Anbieter bzw.
   eID-Server verwendet. Durch die Konfiguration des Dienstes durch den
   Diensteanbieter können durch Weiterleitungen beliebige andere Ports zum
   Einsatz kommen.
.. [#TR-03112] Siehe TR-03112-6 des BSI
.. [#govurl] Erreichbar unter dem URL https://appl.governikus-asp.de/ausweisapp2/
.. [#updatecheck] Die Überprüfung auf neue AusweisApp2-Versionen kann deaktiviert werden, siehe
    Kommandozeilenparameter UPDATECHECK

.. _firewalltable_de:
.. csv-table:: Firewallregeln der AusweisApp2
   :header: "Name", "Protokoll", "Port", "Richtung", "Umgesetzte Verbindung"
   :widths: 25, 15, 15, 15, 30
   :align: left

   "AusweisApp2-Firewall-Rule", TCP, \*, "ausgehend", "eID2, eID3, SaK2, Update"
   "AusweisApp2-SaC", UDP, 24727, "eingehend", "SaK1"

.. raw:: latex

    \end{landscape}
