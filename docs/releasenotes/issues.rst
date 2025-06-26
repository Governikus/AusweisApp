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

- Vereinzelt kann es bei Problemen mit dem Grafikkartentreiber, insbesondere
  bei der Verwendung einer virtuellen Maschine, zu Darstellungsfehlern in
  der Anwendung kommen. Unter Umständen wird kein Fensterinhalt dargestellt.
  Dies kann durch das Setzen einer Umgebungsvariablen umgangen werden.
  Hierfür muss QT_QUICK_BACKEND=software gesetzt und die |AppName|
  neugestartet werden.

- Der Installationsdialog (bei Installation und auch Deinstallation) richtet
  sich nicht nach der Systemsprache.


Android / iOS
"""""""""""""

- Zu Nutzungseinschränkungen einzelner Geräte beachten Sie bitte unsere
  Webseite https://www.ausweisapp.bund.de/aa2/mobile-devices

- Unter Umständen kommt es zu Stabilitätsproblemen der NFC-Schnittstelle.

- Unter iOS ist derzeit keine Unterstützung für die Funktion "Full Keyboard
  Access" gegeben.

- Unter iOS 16 kommt es bei der Verwendung von VoiceOver zu einem
  Absturz, wenn versucht wird ein Anwendungsprotokoll zu teilen.


Barrierearmut
"""""""""""""

- Eine vollständige Barrierearmut ist zum aktuellen Zeitpunkt nicht
  gegeben. Wir arbeiten kontinuierlich an einer Verbesserung.

- Der Screenreader kann teilweise Informationen an einigen Stellen doppelt ausgeben.
