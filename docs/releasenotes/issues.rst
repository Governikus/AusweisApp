Bekannte Fehler
===============

Folgende Fehler und Einschränkungen sind zum Auslieferungszeitpunkt bekannt:

Windows / macOS
"""""""""""""""

- Wenn die |AppName| heruntergefahren wird, während eine Authentisierung
  oder eine PIN-Änderung mit einem Komfort-Kartenleser durchgeführt wird, kann
  dieser Vorgang erst abgeschlossen werden, wenn der Prozess auch auf dem
  Kartenleser beendet wird, oder dieser den Prozess selbstständig wegen einer
  Zeitüberschreitung abbricht.

- Unter macOS können per Tastatur mit den Standardsystemeinstellungen nur
  Textfelder angesprungen werden. Mit Änderung der Tastaturnavigationsoption
  unter "Systemeinstellungen/Tastatur/Kurzbefehle" auf "Alle Steuerungen" kann
  das Verhalten von macOS geändert werden, sodass auch alle anderen
  Komponenten in der App fokussiert werden können.

- Der "Datei speichern unter"-Dialog wird nicht in allen Fällen automatisch
  bei Beginn einer Authentisierung geschlossen.

- Vereinzelt kann es bei Problemen mit dem Grafikkartentreiber, insbesondere
  bei der Verwendung einer virtuellen Maschine, zu Darstellungsfehlern in
  der Anwendung kommen. Unter Umständen wird kein Fensterinhalt dargestellt.
  Dies kann durch das Setzen einer Umgebungsvariablen umgangen werden.
  Hierfür muss QT_QUICK_BACKEND=software gesetzt und die |AppName|
  neugestartet werden.

- Der Installationsdialog (bei Installation und auch Deinstallation) richtet
  sich nicht nach der Systemsprache.

- Wenn mit einer MSI-Datei ein Update für die AusweisApp durchgeführt wird,
  während diese noch läuft, kann es zu einer falschen Fehlermeldung kommen,
  dass sie nicht beendet werden konnte.
  Die Installation ist dennoch erfolgreich.


Android / iOS
"""""""""""""

- Zu Nutzungseinschränkungen einzelner Geräte beachten Sie bitte unsere
  Homepage https://www.ausweisapp.bund.de/aa2/mobile-devices

- Unter Umständen kommt es zu Stabilitätsproblemen der NFC-Schnittstelle.

- Unter Android führen einige Änderungen der Systemeinstellungen bei laufender
  |AppName| zu einem Absturz. Dazu zählt die Änderung der Systemsprache und
  die Änderung des Typs der Navigationsleiste (Gesten / Schaltflächen).

- Bei der Verwendung eines "Smartphone als Kartenleser" mit aktivierter
  Passworteingabe kommt es bei Verwendung des PUKs zu Fehlern, wenn der
  Ausweis dauerhaft am Gerät anliegt.


Barrierearmut
"""""""""""""

- Eine vollständige Barrierearmut ist zum aktuellen Zeitpunkt nicht
  gegeben. Wir arbeiten kontinuierlich an einer Verbesserung.

- Unter Android werden unsichtbare Inhalte vom Screenreader vorgelesen.

- Unter iOS kann der sichtbare Bereich vom Systemfokus abweichen.

- Die aktivierbaren Elemente von eingeklappten Listen können ungewollte
  Aktionen auslösen.

- Aktuell sind die Bereiche "Softwarelizenz" und "Release Notes" nicht mit
  dem Screenreader ansteuerbar.

- Die Toggle-Elemente, die Hilfeseiten und die Überschriften der Startseite
  besitzen zurzeit noch keine Sprachausgabe.

- Bei angeschlossenem Kartenleser wird im Ausweisvorgang direkt auf
  Schritt 2 gesprungen, ohne dass eine entsprechende Sprachausgabe erfolgt.

- In Texten wird zum Teil auf grafische Elemente hingewiesen, die keine textuelle
  Repräsentation desselben Inhalts besitzen.

- Die angeforderten Rechte während eines Ausweisvorgangs werden nicht optimal vom
  Screenreader ausgegeben.

- Der Screenreader kann teilweise Informationen an einigen Stellen doppelt ausgeben.
