Bekannte Fehler
===============

Folgende Fehler und Einschränkungen sind zum Auslieferungszeitpunkt bekannt:

Alle Plattformen
""""""""""""""""

- Eine vollständige Barrierearmut ist zum aktuellen Zeitpunkt nicht
  gegeben. Wir arbeiten kontinuierlich an einer Verbesserung.


Windows / macOS
"""""""""""""""

- Wenn die AusweisApp2 heruntergefahren wird, während eine Authentisierung
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
  Hierfür muss QT_QUICK_BACKEND=software gesetzt und die AusweisApp2
  neugestartet werden.

- Unter Windows werden bei einem Update der AusweisApp2 über die Kommandozeile
  die Kommandozeilenparameter DESKTOPSHORTCUT und SYSTEMSETTINGS ignoriert.
  Dies kann durch eine Deinstallation und eine erneute Installation umgangen
  werden.

- Unter Windows werden bei einem Update der AusweisApp2 über die Kommandozeile
  Kommandozeilenparameter, die nicht schon bei der installierten Version der
  AusweisApp2 genutzt wurden, ignoriert. Dies kann durch eine Deinstallation
  und eine erneute Installation umgangen werden.

- Das Beenden der AusweisApp2 über das Kontextmenü in der Menüleiste unter
  macOS funktioniert nur, wenn das Kontextmenü mit der linken Maustaste
  geöffnet wird. Alternativ kann das Tastaturkürzel CMD + Q zum beenden der
  AusweisApp2 verwendet werden.

Android / iOS
"""""""""""""

- Zu Nutzungseinschränkungen einzelner Geräte beachten Sie bitte unsere
  Homepage https://www.ausweisapp.bund.de/aa2/mobile-devices

- Unter Umständen kommt es zu Stabilitätsproblemen der NFC-Schnittstelle.

- Unter iOS kann derzeit das Hoch- bzw. Querformat nicht geändert werden.

- Die Systemtastatur wird nach der Eingabe von Daten unter iOS nicht
  automatisch geschlossen. Das Schließen muss manuell über die entsprechenden
  Buttons "Fertig" bzw. "Suchen" erfolgen.

- Bei der Nutzung von TalkBack unter Android ist es nicht möglich, eine
  Authentisierung zu starten, wenn die AusweisApp2 bereits geöffnet ist.
  Um das Problem zu vermeiden, muss die AusweisApp2 zunächst aus der Liste
  der zuletzt verwendeten Apps entfernt werden.
