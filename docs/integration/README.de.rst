Deutsch - Entwickleroptionen
============================

Die AusweisApp2 verfügt über sogenannte Entwickleroptionen. Diese sind
für die unterstützten Betriebssystem Windows und macOS verfügbar. Sie
unterstützen die Integration eines eID-Dienstes.


Windows & macOS
---------------

Das Aktivieren der Entwickleroptionen erfolgt sowohl für Windows als auch
für macOS über 10 Klicks auf die Versionsnummer im Bereich "Hilfe" ->
"Versionsinformationen". Nach der Aktivierung sind die Entwickleroptionen
über den Bereich "Einstellungen" erreichbar.


Android & iOS
-------------

In den mobilen Versionen der AusweisApp2 ist der Entwicklermodus nicht
verfügbar. Lediglich der Testmodus (Test-PKI) für die Selbstauskunft kann
durch 10 Klicks auf die Lupe auf der Startseite aktiviert und deaktiviert werden.


Einstellungen
-------------

Die Entwickleroptionen bieten zwei Einstellungsmöglichkeiten:

Testmodus für die Selbstauskunft (Test-PKI)
'''''''''''''''''''''''''''''''''''''''''''

Die Selbstauskunft ist ein fest integrierter Dienst der AusweisApp2 und kann
nur mit Echtausweisen genutzt werden. Wird der Testmodus (Test-PKI) aktiviert,
nutzt die AusweisApp2 einen Test-Dienst, der es ermöglicht, eine Selbstauskunft
mit einem Testausweis durchzuführen.

Entwicklermodus
'''''''''''''''

Mit der Aktivierung des Entwicklermodus werden einige Sicherheitsabfragen
während einer Authentisierung ignoriert. In Entwicklungsszenarien, in denen
ohnehin mit Test-Diensten gearbeitet wird, führt das Ignorieren der
Sicherheitsabfragen dazu, dass eine Authentisierung erfolgreich durchgeführt
werden kann. Dazu gehört beispielweise, dass neben sicheren TLS-Verbindungen
(https) auch unsichere Verbindungen ohne TLS (http) akzeptiert werden. Auch
werden abgelaufene Zertifikate ignoriert. Auf jede Sicherheitsverletzung wird
in den internen Benachrichtigungen der AusweisApp2 bzw. des Betriebssystems
hingewiesen.


**Wichtig:** Der Entwicklermodus kann nur für Test-Dienste verwendet werden,
eine Verwendung mit echten Berechtigungszertifikaten ist nicht möglich.
