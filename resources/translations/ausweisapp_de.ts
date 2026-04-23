<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de">
<context>
    <name>AbortedProgressView</name>
    <message>
        <source>Aborting process and informing the service provider</source>
        <extracomment>DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider</extracomment>
        <translation>Der Vorgang wird beendet und der Anbieter informiert</translation>
    </message>
    <message>
        <source>Please wait a moment.</source>
        <extracomment>DESKTOP Information message about cancellation process with present network connectivity</extracomment>
        <translation>Bitte warten Sie einen Moment.</translation>
    </message>
    <message>
        <source>Network problems detected, trying to reach server within 30 seconds.</source>
        <extracomment>DESKTOP Information message about cancellation process without working network connectivity</extracomment>
        <translation>Es wurden Netzwerkprobleme erkannt. Die Verbindung wird weiterhin für 30 Sekunden versucht.</translation>
    </message>
</context>
<context>
    <name>App</name>
    <message>
        <source>The program remains available via the icon in the menu bar. Click on the %1 icon to reopen the user interface.</source>
        <extracomment>DESKTOP Content of the popup that is shown when the AA is closed and the close/minimize info was not disabled. macOS specific if autostart is enabled.</extracomment>
        <translation>Das Programm steht weiterhin in der Menüleiste zur Verfügung. Klicken Sie auf das Symbol der %1, um die Anwendung wieder zu öffnen.</translation>
    </message>
    <message>
        <source>The program remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.</source>
        <extracomment>DESKTOP Content of the popup that is shown when the AA is closed and the close/minimize info was not disabled.</extracomment>
        <translation>Das Programm steht weiterhin im Infobereich zur Verfügung. Klicken Sie auf das Symbol der %1, um die Anwendung wieder zu öffnen.</translation>
    </message>
    <message>
        <source>Failed to retrieve update information.</source>
        <extracomment>DESKTOP Message that the update data is invalid and can&apos;t be used.</extracomment>
        <translation>Das Herunterladen der Aktualisierungsinformationen ist fehlgeschlagen.</translation>
    </message>
    <message>
        <source>An update is available (version %1).</source>
        <extracomment>DESKTOP An update was found which matches the current platform, the new version number is shown in the message.</extracomment>
        <translation>Eine Aktualisierung ist verfügbar (Version %1).</translation>
    </message>
    <message>
        <source>To close the app, tap the back button 2 times.</source>
        <extracomment>MOBILE Hint that is shown if the users pressed the &quot;back&quot; button on the top-most navigation level for the first time (a second press closes the app).</extracomment>
        <translation>Zum Schließen erneut &quot;Zurück&quot; tippen.</translation>
    </message>
</context>
<context>
    <name>AuthCanceledView</name>
    <message>
        <source>This may take a few moments.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Das kann einen kurzen Moment dauern.</translation>
    </message>
    <message>
        <source>The authentication was cancelled successfully.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Authentisierung wurde erfolgreich abgebrochen.</translation>
    </message>
    <message>
        <source>Back to provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Zurück zum Anbieter</translation>
    </message>
    <message>
        <source>Authentication was canceled</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Authentisierung wurde abgebrochen</translation>
    </message>
    <message>
        <source>The authentication is being canceled at the provider (%1)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Authentisierung wird seitens des Anbieters (%1) beendet</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Zurück zur Startseite</translation>
    </message>
    <message>
        <source>The cancellation was triggered on the card reader.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der Abbruch wurde auf dem Kartenleser ausgelöst.</translation>
    </message>
    <message>
        <source>Back to setup</source>
        <extracomment>ALL_PLATFORM</extracomment>
        <translation>Zurück zur Einrichtung</translation>
    </message>
    <message>
        <source>You may start a new authentication after clicking the button.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Sie können eine neue Authentisierung starten, nachdem Sie den Button geklickt haben.</translation>
    </message>
    <message>
        <source>You may start a new authentication after tapping the button.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Sie können eine neue Authentisierung starten, nachdem Sie den Button getippt haben.</translation>
    </message>
    <message>
        <source>Cancellation progress</source>
        <extracomment>ALL_PLATFORMS Name of an progress indicator during the cancellation of an authentication read by screen readers</extracomment>
        <translation>Fortschritt des Abbruchs</translation>
    </message>
</context>
<context>
    <name>AuthController</name>
    <message>
        <source>Identify</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Ausweisen</translation>
    </message>
    <message>
        <source>Cancel authentication process</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Beende Ausweisvorgang</translation>
    </message>
    <message>
        <source>Acquiring provider certificate</source>
        <extracomment>DESKTOP Header of the progress status message during the authentication process.
----------
MOBILE Header of the progress status message during the authentication process.</extracomment>
        <translation>Berechtigungszertifikat wird heruntergeladen</translation>
    </message>
    <message>
        <source>Authentication in progress</source>
        <extracomment>DESKTOP Header of the progress status message during the authentication process.
----------
MOBILE Header of the progress status message during the authentication process.</extracomment>
        <translation>Ausweisvorgang wird durchgeführt</translation>
    </message>
    <message>
        <source>Please wait a moment.</source>
        <extracomment>DESKTOP Generic status message during the authentication process.
----------
DESKTOP Information message about cancellation process with present network connectivity
----------
MOBILE Generic status message during the authentication process.</extracomment>
        <translation>Bitte warten Sie einen Moment.</translation>
    </message>
    <message>
        <source>Please do not move the ID card.</source>
        <extracomment>DESKTOP Second line text if a basic card reader is used and background communication with the card/server is running. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
----------
MOBILE Second line text if a basic card reader is used and background communication with the card/server is running. Is not actually visible since the basic reader password handling is done by EnterPasswordView.</extracomment>
        <translation>Bitte den Ausweis nicht bewegen.</translation>
    </message>
    <message>
        <source>Please observe the display of your card reader.</source>
        <extracomment>DESKTOP The card reader requests the user&apos;s attention.
----------
MOBILE The card reader requests the user&apos;s attention.</extracomment>
        <translation>Bitte beachten Sie die Anzeige Ihres Kartenlesers.</translation>
    </message>
    <message>
        <source>Send log</source>
        <extracomment>MOBILE</extracomment>
        <translation>Protokoll senden</translation>
    </message>
    <message>
        <source>Authenticate with provider</source>
        <extracomment>MOBILE A11y button to confirm the PIN and start the provider authentication</extracomment>
        <translation>Bei Anbieter ausweisen</translation>
    </message>
    <message>
        <source>Authentication failed</source>
        <extracomment>MOBILE</extracomment>
        <translation>Authentisierung fehlgeschlagen</translation>
    </message>
    <message>
        <source>Please establish an internet connection.</source>
        <extracomment>DESKTOP Content of the message that no network connection is present during the authentication procedure.</extracomment>
        <translation>Bitte stellen Sie eine Internetverbindung her.</translation>
    </message>
    <message>
        <source>No network connectivity</source>
        <extracomment>DESKTOP Header of the message that no network connection is present during the authentication procedure.</extracomment>
        <translation>Keine Verbindung möglich</translation>
    </message>
    <message>
        <source>Weak NFC signal. Please
 change the card position
 remove the mobile phone case (if present)
 connect the smartphone with a charging cable</source>
        <extracomment>DESKTOP The NFC signal is weak or unstable, the user is asked to change the card&apos;s position to (hopefully) reduce the distance to the NFC chip.</extracomment>
        <translation>Schwacher NFC-Empfang. Bitte
- ändern Sie die Position des Ausweises
- entfernen Sie Handyhüllen (falls vorhanden)
- verbinden Sie das Smartphone mit dem Ladegerät</translation>
    </message>
    <message>
        <source>Weak NFC signal. Please
 make sure the card is positioned correctly on the reader
 do note move the card while it is being accessed</source>
        <extracomment>DESKTOP The NFC signal is weak or unstable, while using a stationary card reader.</extracomment>
        <translation>Schwacher NFC-Empfang. Bitte
- stellen Sie sicher, dass der Ausweis korrekt auf dem Kartenleser positioniert ist
- bewegen Sie den Ausweis nicht, während auf diesen zugegriffen wird</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>MOBILE</extracomment>
        <translation>Zur Startseite</translation>
    </message>
    <message>
        <source>Back to setup</source>
        <extracomment>MOBILE</extracomment>
        <translation>Zurück zur Einrichtung</translation>
    </message>
    <message>
        <source>Aborting process and informing the service provider</source>
        <extracomment>DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider</extracomment>
        <translation>Der Vorgang wird beendet und der Anbieter informiert</translation>
    </message>
    <message>
        <source>Network problems detected, trying to reach server within 30 seconds.</source>
        <extracomment>DESKTOP Information message about cancellation process without working network connectivity</extracomment>
        <translation>Es wurden Netzwerkprobleme erkannt. Die Verbindung wird weiterhin für 30 Sekunden versucht.</translation>
    </message>
    <message>
        <source>Authentication progress</source>
        <extracomment>DESKTOP Name of an progress indicator during an authentication read by screen readers
----------
MOBILE Name of an progress indicator during an authentication read by screen readers</extracomment>
        <translation>Authentifizierungsfortschritt</translation>
    </message>
</context>
<context>
    <name>AuthView</name>
    <message>
        <source>Identify</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Ausweisen</translation>
    </message>
</context>
<context>
    <name>AutoRedirectDecision</name>
    <message>
        <source>Currently, the app automatically redirects you back to the service provider after authentication. This may not allow your screen reader to provide all information. To ensure you receive all information, enable manual redirection to the service provider.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Aktuell leitet Sie die App nach einer Authentisierung automatisch zurück zum Anbieter. Hierbei gibt Ihr Bildschirmleser möglicherweise nicht alle Informationen wieder. Um zu gewährleisten, dass Sie alle Informationen erhalten, schalten Sie die manuelle Weiterleitung zum Anbieter ein.</translation>
    </message>
    <message>
        <source>You can change your preference at any time in the settings.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie können diese Option jederzeit in den Einstellungen ändern.</translation>
    </message>
    <message>
        <source>Skip and keep automatic redirection</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Überspringen und automatische Weiterleitung behalten</translation>
    </message>
    <message>
        <source>Enable manual redirection</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Manuelle Weiterleitung einschalten</translation>
    </message>
    <message>
        <source>Optimize your settings for screen reading</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Optimieren Sie Ihre Einstellung für das Bildschirmlesen</translation>
    </message>
    <message>
        <source>Do not display this message in future.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Diesen Hinweis in Zukunft nicht mehr anzeigen.</translation>
    </message>
</context>
<context>
    <name>AutostartView</name>
    <message>
        <source>Do you want to automatically start the %1 after boot?</source>
        <extracomment>DESKTOP Question if the App shall be started automatically after boot</extracomment>
        <translation>Wollen Sie die %1 automatisch nach dem Hochfahren starten?</translation>
    </message>
    <message>
        <source>Yes, start app automatically</source>
        <extracomment>DESKTOP Button to decide start the app automatically</extracomment>
        <translation>Ja, App automatisch starten</translation>
    </message>
    <message>
        <source>The %1 gets started on system boot, so that it can be automatically opened on an authentication.</source>
        <extracomment>DESKTOP Information text why autostart of the App is advisable 1/2</extracomment>
        <translation>Die %1 wird bei Systemstart ausgeführt, sodass sie bei einer Authentisierung automatisch aufgerufen wird.</translation>
    </message>
    <message>
        <source>An authentication cannot be carried out if the %1 is not running. Then it has to be started manually. The automatic start is therefore recommended.</source>
        <extracomment>DESKTOP Information text why autostart of the App is advisable 2/2</extracomment>
        <translation>Falls die %1 nicht geöffnet ist, kann keine Authentisierung durchgeführt werden. Dann muss sie vor einer Authentisierung manuell gestartet werden. Daher wird der automatische Start empfohlen.</translation>
    </message>
    <message>
        <source>No, don&apos;t start app automatically</source>
        <extracomment>DESKTOP Button to decide to not start the app automatically</extracomment>
        <translation>Nein, App nicht automatisch starten</translation>
    </message>
</context>
<context>
    <name>BaseConfirmationPopup</name>
    <message>
        <source>OK</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>OK</translation>
    </message>
    <message>
        <source>Cancel</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Abbrechen</translation>
    </message>
</context>
<context>
    <name>BaseHeading</name>
    <message>
        <source>Heading</source>
        <extracomment>WINDOWS Screenreader announcement, that the current item is a heading.</extracomment>
        <translation>Überschrift</translation>
    </message>
</context>
<context>
    <name>BaseSkipOnboardingConfirmation</name>
    <message>
        <source>Set up the eID function</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Richten Sie die Online-Ausweisfunktion ein</translation>
    </message>
    <message>
        <source>Are you sure you want to skip the setup?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sind Sie sicher, dass Sie die Einrichtung überspringen möchten?</translation>
    </message>
    <message>
        <source>Yes, skip setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ja, Einrichtung überspringen</translation>
    </message>
    <message>
        <source>No, set up %1</source>
        <extracomment>ALL_PLATFORMS %1 will be replaced with the app name</extracomment>
        <translation>Nein, %1 einrichten</translation>
    </message>
    <message>
        <source>You can call up the initial setup again at any time under %1Help%2.</source>
        <extracomment>ALL_PLATFORMS The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Sie können die Ersteinrichtung jederzeit in der %1Hilfe%2 neustarten.</translation>
    </message>
</context>
<context>
    <name>BaseTransportPinReminderView</name>
    <message>
        <source>Set up card PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Karten-PIN einrichten</translation>
    </message>
    <message>
        <source>You need an %1ID Card PIN%2 in order to use the eID function. If you just have %1Transport PIN%2, you have to %1replace%2 it with an %1ID Card PIN once%2.</source>
        <extracomment>ALL_PLATFORMS %1 and %2 are replaced with bold highlighting</extracomment>
        <translation>Für die Nutzung der Online-Ausweisfunktion benötigen Sie eine %1Karten-PIN%2. Falls Sie nur eine %1Transport-PIN%2 haben, müssen Sie diese %1einmalig durch%2 eine %1Karten-PIN ersetzen%2.</translation>
    </message>
    <message>
        <source>What kind of PIN do you have?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Was für eine PIN haben Sie?</translation>
    </message>
</context>
<context>
    <name>BuildHelper</name>
    <message>
        <source>Application</source>
        <translation>Anwendung</translation>
    </message>
    <message>
        <source>Application Version</source>
        <translation>Anwendungsversion</translation>
    </message>
    <message>
        <source>Organization</source>
        <translation>Organisation</translation>
    </message>
    <message>
        <source>Organization Domain</source>
        <translation>Organisation Homepage</translation>
    </message>
    <message>
        <source>System</source>
        <translation>System</translation>
    </message>
    <message>
        <source>Kernel</source>
        <translation>Kernel</translation>
    </message>
    <message>
        <source>Architecture</source>
        <translation>Architektur</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Gerät</translation>
    </message>
    <message>
        <source>VersionCode</source>
        <translation>Versions-Code</translation>
    </message>
    <message>
        <source>Certificate</source>
        <translation>Zertifikat</translation>
    </message>
    <message>
        <source>Qt Version</source>
        <translation>Qt-Version</translation>
    </message>
    <message>
        <source>OpenSSL Version</source>
        <translation>OpenSSL-Version</translation>
    </message>
</context>
<context>
    <name>CardNotActivatedBaseView</name>
    <message>
        <source>Activate ID card with PIN Reset Letter</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausweis mit dem PIN-Rücksetzbrief freischalten</translation>
    </message>
    <message>
        <source>How do I activate the eID function?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wie kann ich die Online-Ausweisfunktion aktivieren?</translation>
    </message>
    <message>
        <source>eID function is not activated</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Online-Ausweisfunktion ist nicht aktiviert</translation>
    </message>
    <message>
        <source>Did you recently order a PIN Reset Letter with an activation code?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Haben Sie kürzlich einen PIN-Rücksetzbrief mit Aktivierungscode bestellt?</translation>
    </message>
    <message>
        <source>Activate your ID card using the activation code</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Schalten Sie Ihren Ausweis mit dem Aktivierungscode frei</translation>
    </message>
    <message>
        <source>Yes, I already have an activation code</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ja, ich habe bereits einen Aktivierungscode</translation>
    </message>
    <message>
        <source>Request the activation of the eID function</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Fordern Sie die Aktivierung der Online-Ausweisfunktion an</translation>
    </message>
    <message>
        <source>No, I don&apos;t have an activation code</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Nein, ich habe keinen Aktivierungscode</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Einrichtung abbrechen</translation>
    </message>
    <message>
        <source>Enter your activation code of your present PIN Reset Letter into the following website.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Geben Sie den Aktivierungscode aus Ihrem vorliegenden PIN-Rücksetzbrief auf der nachfolgenden Webseite ein.</translation>
    </message>
    <message>
        <source>Enter activation code</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Aktivierungscode eingeben</translation>
    </message>
</context>
<context>
    <name>CardPositionView</name>
    <message>
        <source>Retry</source>
        <extracomment>MOBILE</extracomment>
        <translation>Erneut versuchen</translation>
    </message>
    <message>
        <source>Weak NFC signal</source>
        <extracomment>MOBILE</extracomment>
        <translation>Schwacher NFC-Empfang</translation>
    </message>
    <message>
        <source>Connection between ID card and smartphone cannot be established</source>
        <extracomment>MOBILE</extracomment>
        <translation>Verbindung zwischen Ausweis und Smartphone kann nicht hergestellt werden</translation>
    </message>
    <message>
        <source>This is what you can do:</source>
        <extracomment>MOBILE The NFC signal is weak, by repositioning the card the signal might improve.</extracomment>
        <translation>Das können Sie tun:</translation>
    </message>
    <message>
        <source>keep the ID card close to the device&apos;s backside</source>
        <extracomment>MOBILE</extracomment>
        <translation>halten Sie den Ausweis dicht an die Smartphone-Rückseite</translation>
    </message>
    <message>
        <source>change the position of the ID card until the connection is established</source>
        <extracomment>MOBILE</extracomment>
        <translation>verändern Sie die Position des Ausweises bis die Verbindung aufgebaut wird</translation>
    </message>
    <message>
        <source>remove any present mobile phone cases</source>
        <extracomment>MOBILE</extracomment>
        <translation>entfernen Sie vorhandene Handyhüllen</translation>
    </message>
    <message>
        <source>connect your smartphone to a charger and turn off battery saver mode if necessary</source>
        <extracomment>MOBILE</extracomment>
        <translation>verbinden Sie Ihr Smartphone mit einem Ladegerät und schalten Sie ggf. den Energiesparmodus aus</translation>
    </message>
</context>
<context>
    <name>CardReaderDelegate</name>
    <message>
        <source>Press space to open link.</source>
        <extracomment>DESKTOP Text read by screen reader if the text contains a web link to a card reader driver which may be opened.</extracomment>
        <translation>Drücken Sie die Leertaste um den Link zu öffnen.</translation>
    </message>
</context>
<context>
    <name>CardReaderView</name>
    <message>
        <source>Connected USB card readers</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Verbundene USB-Kartenleser</translation>
    </message>
    <message>
        <source>The connection to your system&apos;s smartcard service could not be established. You can try to resolve this issue and restart the scan.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Es konnte keine Verbindung zum Smartcard-Dienst Ihres Systems hergestellt werden. Sie können versuchen, dieses Problem zu beheben und den Scan neustarten.</translation>
    </message>
    <message>
        <source>Restart smartcard scan</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Smartcard-Scan neustarten</translation>
    </message>
    <message>
        <source>List of connected card readers.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Liste der verbundenen Kartenleser.</translation>
    </message>
    <message>
        <source>After connecting a new card reader it may take a few seconds to recognize the driver.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Nachdem ein neuer Kartenleser angeschlossen wurde, kann es einige Sekunden dauern bis der Treiber erkannt wird.</translation>
    </message>
    <message>
        <source>It may be necessary to restart your system after installing the driver.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Unter Umständen kann ein Neustart Ihres Betriebssystems notwendig sein.</translation>
    </message>
    <message>
        <source>Only connected card readers are shown here.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Es werden hier nur angeschlossene Kartenleser angezeigt.</translation>
    </message>
    <message>
        <source>No card reader connected</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kein Kartenleser verbunden</translation>
    </message>
</context>
<context>
    <name>ChangePinController</name>
    <message>
        <source>Setting new ID card PIN</source>
        <extracomment>DESKTOP Processing screen label while the card communication is running after the new PIN has been entered during PIN change process.
----------
MOBILE Processing screen label while the card communication is running after the new ID card PIN has been entered during PIN change process.</extracomment>
        <translation>Neue Karten-PIN wird gesetzt</translation>
    </message>
    <message>
        <source>Change ID card PIN</source>
        <extracomment>DESKTOP Processing screen label while the card communication is running after the old PIN has been entered during PIN change process.
----------
MOBILE Processing screen label while the card communication is running before the new ID card PIN has been entered during PIN change process.</extracomment>
        <translation>Karten-PIN ändern</translation>
    </message>
    <message>
        <source>Please wait a moment.</source>
        <extracomment>DESKTOP Generic progress message during PIN change process.</extracomment>
        <translation>Bitte warten Sie einen Moment.</translation>
    </message>
    <message>
        <source>Please do not move the ID card.</source>
        <extracomment>DESKTOP Processing screen text while the card communication is running after the PIN has been entered during PIN change process.
----------
MOBILE Loading screen during PIN change process, data communication is currently ongoing. Message is usually not visible since the password handling with basic reader is handled by EnterPasswordView.</extracomment>
        <translation>Bitte den Ausweis nicht bewegen.</translation>
    </message>
    <message>
        <source>Please observe the display of your card reader.</source>
        <extracomment>DESKTOP Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.
----------
MOBILE Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.</extracomment>
        <translation>Bitte beachten Sie die Anzeige Ihres Kartenlesers.</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Zur Startseite</translation>
    </message>
    <message>
        <source>Change PIN</source>
        <extracomment>DESKTOP</extracomment>
        <translation>PIN ändern</translation>
    </message>
    <message>
        <source>Weak NFC signal. Please
 change the card position
 remove the mobile phone case (if present)
 connect the smartphone with a charging cable</source>
        <extracomment>DESKTOP The NFC signal is weak or unstable, the user is asked to change the card&apos;s position to (hopefully) reduce the distance to the NFC chip.</extracomment>
        <translation>Schwacher NFC-Empfang. Bitte
- ändern Sie die Position des Ausweises
- entfernen Sie Handyhüllen (falls vorhanden)
- verbinden Sie das Smartphone mit dem Ladegerät</translation>
    </message>
    <message>
        <source>Weak NFC signal. Please
 make sure the card is positioned correctly on the reader
 do note move the card while it is being accessed</source>
        <extracomment>DESKTOP The NFC signal is weak or unstable, while using a stationary card reader.</extracomment>
        <translation>Schwacher NFC-Empfang. Bitte
- stellen Sie sicher, dass der Ausweis korrekt auf dem Kartenleser positioniert ist
- bewegen Sie den Ausweis nicht, während auf diesen zugegriffen wird</translation>
    </message>
    <message>
        <source>Check ID card PIN</source>
        <extracomment>DESKTOP Processing screen label while the card communication is running after the PIN has been entered during PIN check process.</extracomment>
        <translation>Karten-PIN prüfen</translation>
    </message>
</context>
<context>
    <name>ChangePinInfoView</name>
    <message>
        <source>6-digit card PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>6-stellige Karten-PIN</translation>
    </message>
    <message>
        <source>Have your documents ready</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Halten Sie Ihre Dokumente bereit</translation>
    </message>
    <message>
        <source>You need:</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie benötigen:</translation>
    </message>
    <message>
        <source>Your %1ID card%2</source>
        <extracomment>ALL_PLATFORMS The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Ihr %1Ausweisdokument%2</translation>
    </message>
    <message>
        <source>And either:</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Und entweder:</translation>
    </message>
    <message>
        <source>Your %1card PIN%2 (self-chosen)</source>
        <extracomment>ALL_PLATFORMS The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Ihre %1Karten-PIN%2 (selbst gewählt)</translation>
    </message>
    <message>
        <source>or:</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>oder:</translation>
    </message>
    <message>
        <source>Letter from the PIN Reset Service containing the 6-digit PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Brief vom PIN-Rücksetzdienst mit der 6-stelligen PIN</translation>
    </message>
</context>
<context>
    <name>ChangePinView</name>
    <message>
        <source>Change PIN</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>PIN ändern</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <translation>Einrichtung abbrechen</translation>
    </message>
</context>
<context>
    <name>ChangePinViewContent</name>
    <message>
        <source>What kind of PIN do you have?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Was für eine PIN haben Sie?</translation>
    </message>
</context>
<context>
    <name>ChangeTransportPinInfoView</name>
    <message>
        <source>5-digit Transport PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>5-stellige Transport-PIN</translation>
    </message>
    <message>
        <source>Have your documents ready</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Halten Sie Ihre Dokumente bereit</translation>
    </message>
    <message>
        <source>You need:</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie benötigen:</translation>
    </message>
    <message>
        <source>Your %1ID card%2</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ihr %1Ausweisdokument%2</translation>
    </message>
    <message>
        <source>Letter with 5-digit %1Transport PIN%2</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Brief mit 5-stelliger %1Transport-PIN%2</translation>
    </message>
    <message>
        <source>You are not using the ID card online for the 1st time?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie nutzen Ihren Ausweis nicht zum 1. Mal online?</translation>
    </message>
    <message>
        <source>You have to enter the Transport PIN only when activating the ID card for the first time. Your own card PIN is linked on all devices with your ID card as soon as you have set it.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie müssen die Transport-PIN nur bei der erstmaligen Aktivierung des Ausweises eingeben. Sobald Sie eine eigene Karten-PIN festgelegt haben, ist diese auf allen Geräten mit Ihrem Ausweis verknüpft.</translation>
    </message>
    <message>
        <source>In this case, tap on Back and choose 6-digit PIN.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>In dem Fall, tippen Sie auf Zurück, und wählen 6-stellige Karten-PIN.</translation>
    </message>
</context>
<context>
    <name>CheckConnectivityView</name>
    <message>
        <source>No network connectivity</source>
        <extracomment>MOBILE</extracomment>
        <translation>Keine Verbindung möglich</translation>
    </message>
    <message>
        <source>Please establish an internet connection.</source>
        <extracomment>MOBILE No network connection, the user needs to active the network interface or abort the procedure.</extracomment>
        <translation>Bitte stellen Sie eine Internetverbindung her.</translation>
    </message>
</context>
<context>
    <name>CheckIDCardNoNfcSuggestion</name>
    <message>
        <source>Pair a NFC-enabled smartphone</source>
        <extracomment>MOBILE</extracomment>
        <translation>NFC-fähiges Smartphone koppeln</translation>
    </message>
    <message>
        <source>If you want to use %1 on this device anyway, you can %2pair%3 another %2NFC-enabled smartphone%3 as a card reader.</source>
        <extracomment>MOBILE %1 will be replaced with the application name. The text between %2 and %3 will be emphasized.</extracomment>
        <translation>Möchten Sie die %1 auf diesem Gerät nutzen, können Sie ein %2NFC-fähiges Smartphone%3 als Kartenleser %2koppeln%3.</translation>
    </message>
    <message>
        <source>We recommend that you set up your ID card on another smartphone. You can call the setup again at any time under %1Help%2.</source>
        <extracomment>MOBILE The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Wir empfehlen Ihnen, stattdessen die Einrichtung Ihres Ausweises auf einem anderen Smartphone durchzuführen. Sie können die Einrichtung jederzeit unter %1Hilfe%2 wieder aufrufen.</translation>
    </message>
    <message>
        <source>Alternatively you may use the %1 with an %2USB card reader%3 on your %2PC%3.</source>
        <extracomment>MOBILE %1 will be replaced with the application name. The text between %2 and %3 will be emphasized.</extracomment>
        <translation>Alternativ können Sie die %1 mit einem %2USB-Kartenleser%3 am %2PC%3 nutzen.</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <extracomment>MOBILE</extracomment>
        <translation>Einrichtung abbrechen</translation>
    </message>
    <message>
        <source>No NFC available</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kein NFC verfügbar</translation>
    </message>
    <message>
        <source>This device cannot read the ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Dieses Gerät kann den Ausweis nicht auslesen</translation>
    </message>
</context>
<context>
    <name>CheckIDCardResultView</name>
    <message>
        <source>Check device and ID card</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Gerät und Ausweis prüfen</translation>
    </message>
    <message>
        <source>Test Result</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ergebnis der Prüfung</translation>
    </message>
    <message>
        <source>Continue</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Weiter</translation>
    </message>
    <message>
        <source>How to proceed?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wie geht es weiter?</translation>
    </message>
    <message>
        <source>What does that mean?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Was bedeutet das?</translation>
    </message>
    <message>
        <source>You may now try the function: &quot;See my personal data&quot;. Click the &quot;%1&quot; button to do so now.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Probieren Sie zum Abschluss der Prüfung die Funktion &quot;Meine Daten einsehen&quot;. Klicken Sie &quot;%1&quot; um fortzufahren.</translation>
    </message>
    <message>
        <source>You may now try the function: &quot;See my personal data&quot;. Tap the &quot;%1&quot; button to do so now.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Probieren Sie zum Abschluss der Prüfung die Funktion &quot;Meine Daten einsehen&quot;. Tippen Sie &quot;%1&quot; um fortzufahren.</translation>
    </message>
    <message>
        <source>NFC supported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>NFC unterstützt</translation>
    </message>
    <message>
        <source>Your device doesn&apos;t support NFC</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ihr Gerät unterstützt kein NFC</translation>
    </message>
    <message>
        <source>NFC enabled</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>NFC aktiviert</translation>
    </message>
    <message>
        <source>No supported card detected</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Keine unterstützte Karte erkannt</translation>
    </message>
    <message>
        <source>ID card detected</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausweis erkannt</translation>
    </message>
    <message>
        <source>Extended length not supported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Extended Length nicht unterstützt</translation>
    </message>
    <message>
        <source>Extended length supported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Extended Length unterstützt</translation>
    </message>
    <message>
        <source>ID card access failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausweiszugriff fehlgeschlagen</translation>
    </message>
    <message>
        <source>ID card access successful</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausweiszugriff erfolgreich</translation>
    </message>
    <message>
        <source>ID card PIN suspended</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Karten-PIN pausiert</translation>
    </message>
    <message>
        <source>ID card PIN blocked</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Karten-PIN gesperrt</translation>
    </message>
    <message>
        <source>ID card functional</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausweis einsatzbereit</translation>
    </message>
    <message>
        <source>eID function disabled</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Online-Ausweis deaktiviert</translation>
    </message>
    <message>
        <source>eID function enabled</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Online-Ausweisfunktion aktiviert</translation>
    </message>
    <message>
        <source>Card reader supported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Kartenleser geeignet</translation>
    </message>
    <message>
        <source>Smartphone as card reader detected</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Smartphone als Kartenleser erkannt</translation>
    </message>
</context>
<context>
    <name>CheckIDCardSuggestionView</name>
    <message>
        <source>Unknown result code: %1</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Unbekannter Ergebniscode: %1</translation>
    </message>
    <message>
        <source>Open website</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Öffne Webseite</translation>
    </message>
    <message>
        <source>No supported card detected</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Keine unterstützte Karte erkannt</translation>
    </message>
    <message>
        <source>Retry</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Erneut versuchen</translation>
    </message>
    <message>
        <source>No extended length</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Kein Extended Length</translation>
    </message>
    <message>
        <source>The NFC interface of your mobile device does not support Extended Length communication and cannot be used to read the ID card. Unfortunately, the %1 has no influence on this restriction.&lt;br&gt;&lt;br&gt;You can find smartphones compatible with the %1 on our website.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die NFC-Schnittstelle Ihres Mobilgeräts unterstützt keine Extended Length Kommunikation und kann nicht zum Auslesen des Ausweises genutzt werden. Auf diese Einschränkung hat die %1 leider keinen Einfluss.&lt;br&gt;&lt;br&gt;Mit der %1 kompatible Smartphones finden Sie auf unserer Webseite.</translation>
    </message>
    <message>
        <source>ID card access failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausweiszugriff fehlgeschlagen</translation>
    </message>
    <message>
        <source>It was not possible to establish a stable connection with your ID card.&lt;br&gt;&lt;br&gt;Please start the check again. Try a different card position and make sure not to move the card during the test.&lt;br&gt;&lt;br&gt;If a connection to the ID card cannot be established even with different card positions, this indicates that the NFC interface of your mobile device cannot supply the ID card with sufficient power.&lt;br&gt;&lt;br&gt;Smartphones compatible with %1 can be found on our &lt;a href=&quot;%2&quot;&gt;website&lt;/a&gt;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Es war nicht möglich, eine stabile Verbindung mit Ihrem Ausweis herzustellen.&lt;br&gt;&lt;br&gt;Bitte starten Sie die Prüfung erneut. Versuchen Sie eine andere Kartenposition und achten Sie darauf, die Karte während der Prüfung nicht zu bewegen.&lt;br&gt;&lt;br&gt;Wenn auch bei unterschiedlichen Kartenpositionen keine Verbindung zur Ausweiskarte hergestellt werden kann, deutet dies darauf hin, dass die NFC-Schnittstelle Ihres mobilen Geräts die Ausweiskarte nicht ausreichend mit Strom versorgen kann.&lt;br&gt;&lt;br&gt;Mit der %1 kompatible Smartphones finden Sie auf unserer &lt;a href=&quot;%2&quot;&gt;Website&lt;/a&gt;.</translation>
    </message>
    <message>
        <source>ID card PIN suspended</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Karten-PIN pausiert</translation>
    </message>
    <message>
        <source>The ID card PIN has been entered incorrectly 2 times in a row. This is why you must first enter the 6-digit Card Access Number (CAN) for the next identification process. You can find it at the bottom right of the front of your ID card.</source>
        <extracomment>ALL_PLATFORMS Sentence 1 of 3 of CAN explanation</extracomment>
        <translation>Die Karten-PIN wurde 2 Mal hintereinander falsch eingegeben. Deshalb müssen Sie beim nächsten Ausweisvorgang zunächst die 6-stellige Zugangsnummer (CAN) eingeben. Sie finden diese auf der Vorderseite Ihres Ausweises unten rechts.</translation>
    </message>
    <message>
        <source>You may now try the function: &quot;See my personal data&quot;.</source>
        <extracomment>ALL_PLATFORMS Sentence 2 of 3 of CAN explanation
----------
ALL_PLATFORMS Sentence 2 of 3 of PUK explanation</extracomment>
        <translation>&lt;br&gt;&lt;br&gt;Probieren Sie zum Abschluss der Prüfung die Funktion &quot;Meine Daten einsehen&quot;.</translation>
    </message>
    <message>
        <source>There, you can also use the CAN to unblock the ID card PIN.</source>
        <extracomment>ALL_PLATFORMS Sentence 3 of 3 of CAN explanation</extracomment>
        <translation>Dort können Sie auch die CAN verwenden, um die PIN-Eingabe fortzusetzen.</translation>
    </message>
    <message>
        <source>ID card PIN blocked</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Karten-PIN gesperrt</translation>
    </message>
    <message>
        <source>The ID card PIN has been entered incorrectly 3 times. Therefore, you must first enter the 10-digit PUK during the next authentication process. You can find it in the PIN letter you received after applying for your ID card.</source>
        <extracomment>ALL_PLATFORMS Sentence 1 of 3 of PUK explanation</extracomment>
        <translation>Die Karten-PIN wurde 3 Mal hintereinander falsch eingegeben. Deshalb müssen Sie beim nächsten Ausweisvorgang zunächst die 10-stellige PUK eingeben. Sie finden diese im PIN-Brief, den Sie nach Beantragung Ihres Ausweises erhalten haben.</translation>
    </message>
    <message>
        <source>Have your PUK ready to unlock the ID card PIN.</source>
        <extracomment>ALL_PLATFORMS Sentence 3 of 3 of PUK explanation</extracomment>
        <translation>Halten Sie zum Entsperren der Karten-PIN Ihre PUK bereit.</translation>
    </message>
    <message>
        <source>Try a different card position, make sure that you do not move the ID card during the check and that there are no other NFC cards (e.g. credit cards) near the device.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Versuchen Sie eine andere Kartenposition, stellen Sie sicher, dass Sie den Ausweis während der Prüfung nicht bewegen und dass sich keine anderen NFC-Karten (bspw. Kreditkarten) in der Nähe des Geräts befinden.</translation>
    </message>
    <message>
        <source>No supported ID card was detected. The %1 supports:&lt;p&gt;&lt;ul&gt;&lt;li&gt;German ID cards&lt;/li&gt;&lt;li&gt;Electronic residence permits (eAT)&lt;/li&gt;&lt;li&gt;eID cards for citizen of the EU/EEA/&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;If you have used one of the above documents and this error message still appears, please restart the check.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Es wurde kein unterstützter Ausweis erkannt. Die %1 unterstützt:&lt;p&gt;&lt;ul&gt;&lt;li&gt;deutsche Personalausweise&lt;/li&gt;&lt;li&gt;elektronische Aufenthaltstitel (eAT)&lt;/li&gt;&lt;li&gt;eID-Karten für EU-/EWR-Bürger&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;Haben Sie eines der genannten Dokumente verwendet und diese Fehlermeldung erscheint dennoch, starten Sie bitte die Prüfung erneut.</translation>
    </message>
    <message>
        <source>You may continue the setup and change your PIN.</source>
        <extracomment>ALL_PLATFORMS Sentence 2 of 3 of CAN explanation
----------
ALL_PLATFORMS Sentence 2 of 3 of PUK explanation</extracomment>
        <translation>Sie können mit der Einrichtung fortfahren und Ihre PIN ändern.</translation>
    </message>
    <message>
        <source>I can&apos;t recall my PUK. How do I set a new card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ich kenne meine PUK nicht. Wie kann ich eine neue PIN setzen?</translation>
    </message>
</context>
<context>
    <name>CheckIDCardView</name>
    <message>
        <source>Check device and ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Gerät und Ausweis prüfen</translation>
    </message>
    <message>
        <source>To use the eID function, your device must meet certain technical requirements. Furthermore, the eID function must be activated.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Um die Online-Ausweisfunktion nutzen zu können, muss Ihr Gerät gewisse technische Anforderungen erfüllen. Zudem muss die Online-Ausweisfunktion Ihres Ausweises aktiviert sein.</translation>
    </message>
    <message>
        <source>Check if your device &amp; ID card are ready for use</source>
        <extracomment>MOBILE</extracomment>
        <translation>Prüfen Sie, ob Ihr Gerät &amp; Ausweis einsatzbereit sind</translation>
    </message>
    <message>
        <source>Start check</source>
        <extracomment>MOBILE</extracomment>
        <translation>Prüfung starten</translation>
    </message>
</context>
<context>
    <name>CheckIDCardWorkflow</name>
    <message>
        <source>Check device and ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Gerät und Ausweis prüfen</translation>
    </message>
    <message>
        <source>Checking ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ausweis wird geprüft</translation>
    </message>
    <message>
        <source>Please do not move the ID card.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Bitte den Ausweis nicht bewegen.</translation>
    </message>
</context>
<context>
    <name>ChooseReaderType</name>
    <message>
        <source>How would you like to read your ID card?</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Wie möchten Sie den Ausweis auslesen?</translation>
    </message>
    <message>
        <source>Connect an USB card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Schließen Sie einen USB-Kartenleser an</translation>
    </message>
    <message>
        <source>By card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Per Kartenleser</translation>
    </message>
    <message>
        <source>Use a NFC-enabled smartphone as card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Nutzen Sie ein NFC-fähiges Smartphone als Kartenleser</translation>
    </message>
    <message>
        <source>By smartphone</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Per Smartphone</translation>
    </message>
</context>
<context>
    <name>CloseHandler</name>
    <message>
        <source>This will cancel the current operation and hide the UI of %1. You can restart the operation at any time.</source>
        <extracomment>DESKTOP Content of the popup that is shown when the AA is closed and a workflow is still active.</extracomment>
        <translation>Hiermit wird der aktuelle Vorgang abgebrochen und die Benutzeroberfläche der %1 geschlossen. Sie können den Vorgang jederzeit erneut starten.</translation>
    </message>
    <message>
        <source>This will cancel the current operation and shut the %1 down. You will have to restart the %1 to restart the operation.</source>
        <extracomment>DESKTOP Content of the popup that is shown when the AA is shut down and a workflow is still active.</extracomment>
        <translation>Hierdurch wird der aktuelle Vorgang abgebrochen und die %1 beendet. Sie müssen die %1 erneut starten, um den Vorgang neuzustarten.</translation>
    </message>
    <message>
        <source>Abort operation</source>
        <extracomment>DESKTOP Header of the popup that is shown when the AA is closed and a workflow is still active</extracomment>
        <translation>Vorgang abbrechen</translation>
    </message>
    <message>
        <source>How should the %1 be closed in the future?</source>
        <extracomment>DESKTOP Header of the popup that is shown when the AA is closed for the first time.</extracomment>
        <translation>Wie soll die %1 zukünftig beendet werden?</translation>
    </message>
    <message>
        <source>Close completely</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Vollständig beenden</translation>
    </message>
    <message>
        <source>Leave active in the background</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Im Hintergrund aktiv lassen</translation>
    </message>
    <message>
        <source>If the %1 is closed, it is no longer available for authentication. You must then restart the app to authenticate yourself to service providers.</source>
        <extracomment>DESKTOP %1 is replaced with the application name</extracomment>
        <translation>Wenn die %1 beendet wird, steht sie nicht länger für eine Authentisierung zur Verfügung. Sie müssen die App dann erneut starten, um sich gegenüber Anbietern auszuweisen.</translation>
    </message>
    <message>
        <source>Do not display this message in future.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Diesen Hinweis in Zukunft nicht mehr anzeigen.</translation>
    </message>
    <message>
        <source>If the %1 remains active in the background, it will open automatically as soon as you start an authentication. You can still open the %1 manually at any time.</source>
        <extracomment>DESKTOP %1 is replaced with the application name</extracomment>
        <translation>Wenn die %1 im Hintergrund aktiv bleibt, wird sie automatisch geöffnet, sobald Sie eine Authentisierung starten. Sie können die AusweisApp auch weiterhin jederzeit manuell öffnen.</translation>
    </message>
    <message>
        <source>Abort process</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Vorgang abbrechen</translation>
    </message>
    <message>
        <source>You can change your selection at any time in the settings.</source>
        <extracomment>DESKTOP Note to the user that the setting is available in the settings</extracomment>
        <translation>Sie können Ihre Auswahl jederzeit in den Einstellungen ändern.</translation>
    </message>
</context>
<context>
    <name>ConnectSacView</name>
    <message>
        <source>Pairing</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kopplung</translation>
    </message>
    <message>
        <source>Pairing the device...</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Das Gerät wird gekoppelt...</translation>
    </message>
</context>
<context>
    <name>ContentArea</name>
    <message>
        <source>Change PIN</source>
        <extracomment>MOBILE</extracomment>
        <translation>PIN ändern</translation>
    </message>
</context>
<context>
    <name>DarkModeButtonData</name>
    <message>
        <source>System</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Systemeinstellungen</translation>
    </message>
    <message>
        <source>Dark</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Dunkel</translation>
    </message>
    <message>
        <source>Light</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Hell</translation>
    </message>
    <message>
        <source>Set the app appearance to system mode</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Setzt das Erscheinungsbild der Anwendung auf die Systemeinstellungen</translation>
    </message>
    <message>
        <source>Set the app appearance to dark mode</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Setzt das Erscheinungsbild der Anwendung auf dunkel</translation>
    </message>
    <message>
        <source>Set the app appearance to light mode</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Setzt das Erscheinungsbild der Anwendung auf hell</translation>
    </message>
</context>
<context>
    <name>DataGroupDelegate</name>
    <message>
        <source>%1, optional right, element %2 of %3</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>%1, optionales Recht, Element %2 von %3</translation>
    </message>
    <message>
        <source>%1, required right, element %2 of %3</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>%1, erforderliches Recht, Element %2 von %3</translation>
    </message>
</context>
<context>
    <name>DebugSettings</name>
    <message>
        <source>Create dummy entries</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Erstelle Scheindaten</translation>
    </message>
    <message>
        <source>Logfile</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Protokolldatei</translation>
    </message>
    <message>
        <source>Show beta testing image</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Zeige Beta-Test-Symbol</translation>
    </message>
    <message>
        <source>Support CAN allowed mode</source>
        <extracomment>DESKTOP</extracomment>
        <translation>CAN-erlaubt-Modus unterstützen</translation>
    </message>
    <message>
        <source>Skip rights page in CAN allowed mode</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Überspringe Berechtigungen-Seite im CAN-erlaubt-Modus</translation>
    </message>
    <message>
        <source>Reset hideable dialogs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Setze versteckbare Dialoge zurück</translation>
    </message>
    <message>
        <source>Show Transport PIN reminder, store feedback and close reminder dialogs.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Zeige Transport-PIN Hinweis, Store Feedback und Fenster-Schließ Hinweis Dialoge.</translation>
    </message>
</context>
<context>
    <name>DetachedLogView</name>
    <message>
        <source>Zoom:</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Zoom:</translation>
    </message>
    <message>
        <source>Save log</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Protokoll speichern</translation>
    </message>
    <message>
        <source>Logfiles (*.log)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Protokolldateien (*.log)</translation>
    </message>
    <message>
        <source>Filter</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Filter</translation>
    </message>
    <message>
        <source>Select level:</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Level auswählen:</translation>
    </message>
    <message>
        <source>Select category:</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kategorie auswählen:</translation>
    </message>
    <message>
        <source>Currently there are no log entries matching your filter.</source>
        <extracomment>DESKTOP No log entries, placeholder text.</extracomment>
        <translation>Derzeit gibt es keine Einträge im Log, die zu Ihrem Filter passen.</translation>
    </message>
    <message>
        <source>Filter. Activated.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Filter. Aktiviert.</translation>
    </message>
    <message>
        <source>Filter. Deactivated.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Filter. Deaktiviert.</translation>
    </message>
    <message>
        <source>Current Log</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Aktuelles Protokoll</translation>
    </message>
</context>
<context>
    <name>DetachedLogViewWindow</name>
    <message>
        <source>Detached log viewer</source>
        <translation>Protokollbetrachter</translation>
    </message>
</context>
<context>
    <name>DeveloperModeWarning</name>
    <message>
        <source>Developer Mode: Enabled!</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Entwicklermodus: Aktiviert!</translation>
    </message>
    <message>
        <source>Disable</source>
        <extracomment>DESKTOP Global button to disable developer mode.</extracomment>
        <translation>Deaktivieren</translation>
    </message>
</context>
<context>
    <name>DeveloperSettings</name>
    <message>
        <source>Developer options</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Entwickleroptionen</translation>
    </message>
    <message>
        <source>Testmode for the self-authentication</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Testmodus für die Selbstauskunft</translation>
    </message>
    <message>
        <source>The internal card simulator allows to run an authentication in the test PKI without any ID card or card reader. Note that no other card reader can be used while the simulator is activated.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Der interne Kartensimulator ermöglicht die Durchführung einer Authentisierung in der Test-PKI ohne Ausweis oder Kartenleser. Beachten Sie, dass kein anderer Kartenleser verwendet werden kann, während der Simulator aktiviert ist.</translation>
    </message>
    <message>
        <source>Developer mode</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Entwicklermodus</translation>
    </message>
    <message>
        <source>Custom config.json</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Benutzerdefinierte config.json</translation>
    </message>
    <message>
        <source>Place the config.json into the application folder to override the embedded config.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Speichern Sie die Datei config.json in den Anwendungsordner, um die eingebettete Konfiguration zu überschreiben.</translation>
    </message>
    <message>
        <source>Application folder: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Anwendungsordner: %1</translation>
    </message>
    <message>
        <source>Save config.json</source>
        <extracomment>DESKTOP</extracomment>
        <translation>config.json speichern</translation>
    </message>
    <message>
        <source>JSON config (*.json)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>JSON-Konfigurationsdatei (*.json)</translation>
    </message>
    <message>
        <source>Allow test sample card usage</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Testmusterkarten erlauben</translation>
    </message>
    <message>
        <source>Internal card simulator</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Interner Kartensimulator</translation>
    </message>
    <message>
        <source>The developer mode deactivates some security checks and the authentication process will continue even if some errors occur. Skipped errors will be shown as notifications. The developer mode is only usable with the test PKI.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Im Entwicklermodus werden einige Sicherheitsprüfungen abgestellt und die Authentisierung bei bestimmten Fehlern trotzdem fortgesetzt. Übergangene Fehler werden in den Benachrichtigungen angezeigt. Der Entwicklermodus funktioniert nur in der Test-PKI.</translation>
    </message>
    <message>
        <source>Using the developer mode forces the notifications to be enabled.</source>
        <extracomment>DESKTOP Only visible when the user activates the developer mode in the settings.</extracomment>
        <translation>Im Entwicklermodus werden die internen Benachrichtigungen erzwungen.</translation>
    </message>
    <message>
        <source>Show notifications inside of %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Benachrichtigungen in der %1 anzeigen</translation>
    </message>
</context>
<context>
    <name>DevicesListDelegate</name>
    <message>
        <source>Device %1. %2.</source>
        <translation>Gerät %1. %2.</translation>
    </message>
</context>
<context>
    <name>DiagnosisView</name>
    <message>
        <source>Save to file</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Speichern unter</translation>
    </message>
    <message>
        <source>Diagnosis is still running</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Die Diagnose läuft noch</translation>
    </message>
    <message>
        <source>Diagnosis may be incomplete</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Die Diagnose könnte unvollständig sein</translation>
    </message>
    <message>
        <source>Textfiles (*.txt)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Textdateien (*.txt)</translation>
    </message>
    <message>
        <source>System data</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Systemdaten</translation>
    </message>
    <message>
        <source>Save system data to textfile</source>
        <translation>Speichere Systemdaten in Textdatei</translation>
    </message>
    <message>
        <source>SystemData</source>
        <translation>Systemdaten</translation>
    </message>
    <message>
        <source>Save system data</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Systemdaten speichern</translation>
    </message>
</context>
<context>
    <name>DominationHandler</name>
    <message>
        <source>Another application uses %1</source>
        <extracomment>DESKTOP The AA is currently remote controlled via the SDK interface, concurrent usage of the AA is not possible.</extracomment>
        <translation>Eine andere Anwendung verwendet %1</translation>
    </message>
</context>
<context>
    <name>EditRights</name>
    <message>
        <source>Proceed to %1 entry</source>
        <extracomment>DESKTOP %1 can be &quot;CAN&quot; or &quot;PIN&quot;
----------
MOBILE %1 can be &quot;CAN&quot; or &quot;PIN&quot;</extracomment>
        <translation>Weiter zur %1-Eingabe</translation>
    </message>
    <message>
        <source>CAN</source>
        <extracomment>DESKTOP Inserted into &quot;Proceed to %1 entry&quot;
----------
IOS Inserted into &quot;Proceed to %1 entry&quot;</extracomment>
        <translation>CAN</translation>
    </message>
    <message>
        <source>PIN</source>
        <extracomment>DESKTOP Inserted into &quot;Proceed to %1 entry&quot;
----------
IOS Inserted into &quot;Proceed to %1 entry&quot;</extracomment>
        <translation>PIN</translation>
    </message>
    <message>
        <source>By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Mit Eingabe der CAN gewähren Sie dem oben genannten Anbieter folgende Datenzugriffe auf den Ausweis:</translation>
    </message>
    <message>
        <source>By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Mit Eingabe Ihrer PIN gewähren Sie dem oben genannten Anbieter folgende Datenzugriffe auf Ihren Ausweis:</translation>
    </message>
    <message>
        <source>Identify</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ausweisen</translation>
    </message>
</context>
<context>
    <name>EnterPasswordView</name>
    <message>
        <source>Enter CAN</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>CAN eingeben</translation>
    </message>
    <message>
        <source>Enter PUK</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>PUK eingeben</translation>
    </message>
    <message>
        <source>Enter pairing code</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Kopplungscode eingeben</translation>
    </message>
    <message>
        <source>Choose new ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Neue Karten-PIN wählen</translation>
    </message>
    <message>
        <source>Confirm new ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Neue Karten-PIN bestätigen</translation>
    </message>
    <message>
        <source>Enter Transport PIN</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Transport-PIN eingeben</translation>
    </message>
    <message>
        <source>Enter ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Karten-PIN eingeben</translation>
    </message>
    <message>
        <source>You will find the Transport PIN in the letter that was sent to you after you applied for your ID card.</source>
        <extracomment>ALL_PLATFORMS The Transport PIN is required by AA, it needs to be change to an actual PIN. This is the description for the main text.</extracomment>
        <translation>Sie finden die Transport-PIN in dem Brief, der Ihnen nach der Ausweis-Beantragung zugesandt wurde.</translation>
    </message>
    <message>
        <source>You have chosen the 6-digit ID card PIN yourself or received it via the PIN Reset Service.</source>
        <extracomment>ALL_PLATFORMS The AA expects the current ID card PIN with six digits in a PIN change. This is the description for the main text.</extracomment>
        <translation>Sie haben die 6-stellige Karten-PIN selbst gewählt oder per Rücksetzdienst erhalten.</translation>
    </message>
    <message>
        <source>Please enter your 6-digit ID card PIN.</source>
        <extracomment>ALL_PLATFORMS The AA expects a ID card PIN with six digits in an authentication. This is the description for the main text.</extracomment>
        <translation>Bitte geben Sie Ihre 6-stellige Karten-PIN ein.</translation>
    </message>
    <message>
        <source>You will enter this 6-digit PIN every time you want to use your ID card online.</source>
        <extracomment>ALL_PLATFORMS A new 6-digit ID card PIN needs to be supplied. This is the description for the main text.</extracomment>
        <translation>Diese 6-stellige PIN geben Sie jedes Mal ein, wenn Sie Ihren Ausweis online benutzen.</translation>
    </message>
    <message>
        <source>The PIN is going to be tied to your ID card. It always stays the same regardless of the device you want to use your ID card with.</source>
        <extracomment>ALL_PLATFORMS The new ID card PIN needs to be confirmed. This is the description for the main text.</extracomment>
        <translation>Die Karten-PIN wird mit Ihrem Ausweis verknüpft. Sie bleibt gleich, egal auf welchem Gerät Sie Ihren Online-Ausweis nutzen.</translation>
    </message>
    <message>
        <source>Do you have a 5-digit Transport PIN?</source>
        <extracomment>ALL_PLATFORMS Button to start a change of the Transport PIN.</extracomment>
        <translation>Haben Sie eine 5-stellige Transport-PIN?</translation>
    </message>
    <message>
        <source>Enter the pairing code shown on the device you want to pair.</source>
        <extracomment>MOBILE The pairing code for the smartphone is required. This is the description for the main text.</extracomment>
        <translation>Geben Sie den Kopplungscode ein, der auf dem zu koppelnden Smartphone angezeigt wird.</translation>
    </message>
    <message>
        <source>Do you have a 6-digit ID card PIN?</source>
        <extracomment>ALL_PLATFORMS Button to switch to a 6-digit ID card PIN.</extracomment>
        <translation>Haben Sie eine 6-stellige Karten-PIN?</translation>
    </message>
    <message>
        <source>Send CAN</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>CAN senden</translation>
    </message>
    <message>
        <source>Send PUK</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>PUK senden</translation>
    </message>
    <message>
        <source>Send pairing code</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Kopplungscode senden</translation>
    </message>
    <message>
        <source>Send new ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Neue Karten-PIN senden</translation>
    </message>
    <message>
        <source>Send Transport PIN</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Transport-PIN senden</translation>
    </message>
    <message>
        <source>Send ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Karten-PIN senden</translation>
    </message>
    <message>
        <source>Enter the pairing code shown on your smartphone.</source>
        <extracomment>DESKTOP The pairing code needs to be supplied. This is the description for the main text.</extracomment>
        <translation>Geben Sie den Kopplungscode ein, der auf Ihrem Smartphone angezeigt wird.</translation>
    </message>
    <message>
        <source>Please enter the CAN. You can find the CAN in the bottom right on the front of the ID card.</source>
        <extracomment>ALL_PLATFORMS The user is required to enter the 6-digit CAN. This is the description for the main text.</extracomment>
        <translation>Bitte geben Sie Ihre CAN ein. Sie finden die CAN unten rechts auf der Vorderseite Ihres Ausweises.</translation>
    </message>
    <message>
        <source>Please enter your 10-digit PUK.</source>
        <extracomment>ALL_PLATFORMS The PUK is required to unlock the ID card since the wrong ID card PIN entered three times. This is the description for the main text.</extracomment>
        <translation>Bitte geben Sie Ihre 10-stellige PUK ein.</translation>
    </message>
    <message>
        <source>Confirm ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Karten-PIN bestätigen</translation>
    </message>
</context>
<context>
    <name>GCollapsible</name>
    <message>
        <source>Currently selected is %1</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Aktuell ausgewählt ist %1</translation>
    </message>
</context>
<context>
    <name>GContinueButton</name>
    <message>
        <source>Continue</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Weiter</translation>
    </message>
</context>
<context>
    <name>GProgressBar</name>
    <message>
        <source>Progress</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Fortschritt</translation>
    </message>
</context>
<context>
    <name>GRadioButton</name>
    <message>
        <source>checked</source>
        <extracomment>DESKTOP</extracomment>
        <translation>aktiviert</translation>
    </message>
</context>
<context>
    <name>GStagedProgressBar</name>
    <message>
        <source>Step %1 of %2. This step is %3 percent complete.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Schritt %1 von %2 zu %3 Prozent abgeschlossen.</translation>
    </message>
</context>
<context>
    <name>GTextField</name>
    <message>
        <source>Maximum allowed length reached.</source>
        <translation>Maximal zulässige Länge erreicht.</translation>
    </message>
</context>
<context>
    <name>GeneralSettings</name>
    <message>
        <source>Behavior</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Verhalten</translation>
    </message>
    <message>
        <source>Network</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Netzwerk</translation>
    </message>
    <message>
        <source>Use the proxy (%1) specified during the installation.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Benutze den bei der Installation angegebenen Proxy (%1).</translation>
    </message>
    <message>
        <source>Appearance</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Erscheinungsbild</translation>
    </message>
    <message>
        <source>Use system font</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Systemschriftart verwenden</translation>
    </message>
    <message>
        <source>Toggling will restart the %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Ein Umschalten startet die %1 neu</translation>
    </message>
    <message>
        <source>Close %1 window after authentication</source>
        <extracomment>DESKTOP</extracomment>
        <translation>%1-Fenster nach Authentisierung schließen</translation>
    </message>
    <message>
        <source>Change language</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Sprache wechseln</translation>
    </message>
    <message>
        <source>Use images instead of animations</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Bilder statt Animationen verwenden</translation>
    </message>
    <message>
        <source>Accessibility</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Barrierefreiheit</translation>
    </message>
    <message>
        <source>Hide key animations when entering PIN</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Tastenanimationen bei PIN-Eingabe ausblenden</translation>
    </message>
    <message>
        <source>After identification, you will only be redirected back to the provider after confirmation. Otherwise, you will be redirected automatically after a few seconds.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Nach dem Ausweisen erfolgt die Weiterleitung zurück zum Anbieter erst nach Bestätigung. Ansonsten erfolgt die Weiterleitung automatisch nach wenigen Sekunden.</translation>
    </message>
    <message>
        <source>Manual redirection back to the provider</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Manuelle Weiterleitung zurück zum Anbieter</translation>
    </message>
    <message>
        <source>The %1 gets started on system boot, so that it can be opened automatically on an authentication. It has to be started manually otherwise.</source>
        <extracomment>DESKTOP Description for auto-start option</extracomment>
        <translation>Die %1 wird bei Systemstart ausgeführt, sodass sie bei einer Authentisierung automatisch aufgerufen wird. Andernfalls muss sie zuvor manuell gestartet werden.</translation>
    </message>
    <message>
        <source>Automatically start %1 (recommended)</source>
        <extracomment>DESKTOP Text for auto-start option</extracomment>
        <translation>%1 automatisch starten (empfohlen)</translation>
    </message>
    <message>
        <source>The %1 continues to run in the background after the application window is closed, so that it can be opened automatically on an authentication.</source>
        <extracomment>MACOS Description for attaching the AA to the menu bar/system tray</extracomment>
        <translation>Die %1 läuft nach Schließen des Anwendungsfensters im Hintergrund weiter, sodass sie bei einer Authentisierung automatisch aufgerufen wird.</translation>
    </message>
    <message>
        <source>Attach %1 to menu bar (recommended)</source>
        <extracomment>MACOS Text for attaching the AA to the menu bar</extracomment>
        <translation>%1 im Benachrichtigungsbereich anheften (empfohlen)</translation>
    </message>
    <message>
        <source>Attach %1 to system tray (recommended)</source>
        <extracomment>WINDOWS Text for attaching the AA to the system tray</extracomment>
        <translation>%1 im Infobereich anheften (empfohlen)</translation>
    </message>
</context>
<context>
    <name>GeneralWorkflow</name>
    <message>
        <source>Step %1 of 3</source>
        <translation>Schritt %1 von 3</translation>
    </message>
    <message>
        <source>The used card reader does not meet the technical requirements (Extended Length not supported).</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Der verwendete Kartenleser erfüllt leider nicht die technischen Voraussetzungen (Extended Length wird nicht unterstützt).</translation>
    </message>
    <message>
        <source>Read out ID card with connected device</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Ausweis mit verbundenem Gerät auslesen</translation>
    </message>
    <message>
        <source>Connect USB card reader or smartphone</source>
        <extracomment>DESKTOP</extracomment>
        <translation>USB-Kartenleser oder Smartphone verbinden</translation>
    </message>
    <message>
        <source>Information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Information</translation>
    </message>
    <message>
        <source>No ID card detected. Please ensure that your ID card is placed on the card reader.</source>
        <extracomment>DESKTOP The AA is waiting for an ID card to be inserted into the card reader.</extracomment>
        <translation>Es wurde kein Ausweis erkannt. Bitte stellen Sie sicher, dass Ihr Ausweis auf dem Kartenleser aufliegt.</translation>
    </message>
    <message>
        <source>No card reader detected. Connect an USB card reader or set up a smartphone as a card reader.</source>
        <extracomment>DESKTOP AA is waiting for the card reader or the ID card.</extracomment>
        <translation>Es wurde kein Kartenleser erkannt. Schließen Sie einen USB-Kartenleser an oder richten Sie ein Smartphone als Kartenleser ein.</translation>
    </message>
    <message>
        <source>Please observe the display of your card reader.</source>
        <extracomment>DESKTOP The card reader is a comfort reader with its own display, the user is requested to pay attention to that display (instead of the AA).</extracomment>
        <translation>Bitte beachten Sie die Anzeige Ihres Kartenlesers.</translation>
    </message>
    <message>
        <source>Set up card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kartenleser einrichten</translation>
    </message>
    <message>
        <source>No ID card detected. Please follow the instructions on your smartphone (connected to %1) to use it as card reader.</source>
        <extracomment>DESKTOP The AA is waiting for the smartphone to be placed on the id.</extracomment>
        <translation>Es wurde kein Ausweis erkannt. Bitte folgen Sie den Anweisungen auf Ihrem Smartphone (verbunden mit %1), um es als Kartenleser zu verwenden.</translation>
    </message>
    <message>
        <source>Please follow the instructions on your smartphone (connected to %1) or put the ID card on the card reader.</source>
        <extracomment>DESKTOP The AA is waiting for an ID card to be inserted into the card reader (or smartphone for that matter).</extracomment>
        <translation>Bitte folgen Sie den Anweisungen auf Ihrem Smartphone (verbunden mit %1) oder legen Sie den Ausweis auf den Kartenleser.</translation>
    </message>
    <message>
        <source>The device &quot;%1&quot; was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.</source>
        <extracomment>DESKTOP The paired devices was removed since it did not respond to connection attempts. It needs to be paired again if it should be used as card reader.
----------
MOBILE The paired smartphone was removed since it did not respond to connection attempts. It needs to be paired again before using it.</extracomment>
        <translation>Das Gerät &quot;%1&quot; wurde entkoppelt, da es nicht auf Verbindungsversuche reagiert hat. Koppeln Sie das Gerät erneut, um es wieder als Kartenleser zu verwenden.</translation>
    </message>
</context>
<context>
    <name>InputErrorView</name>
    <message>
        <source>Wrong CAN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Falsche CAN</translation>
    </message>
    <message>
        <source>Wrong PUK</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Falsche PUK</translation>
    </message>
    <message>
        <source>Wrong Transport PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Falsche Transport-PIN</translation>
    </message>
    <message>
        <source>Wrong ID card PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Falsche Karten-PIN</translation>
    </message>
    <message>
        <source>Wrong new ID card PIN confirmation</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Falsche Bestätigung der neuen Karten-PIN</translation>
    </message>
</context>
<context>
    <name>InputSuccessView</name>
    <message>
        <source>PUK is correct</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>PUK ist korrekt</translation>
    </message>
    <message>
        <source>You now have &lt;b&gt;3 more attempts&lt;/b&gt; to enter your correct &lt;b&gt;ID card PIN&lt;/b&gt;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie haben nun &lt;b&gt;3 weitere Versuche&lt;/b&gt;, um Ihre &lt;b&gt;Karten-PIN&lt;/b&gt; korrekt einzugeben.</translation>
    </message>
    <message>
        <source>CAN is correct</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>CAN ist korrekt</translation>
    </message>
    <message>
        <source>Please enter your &lt;b&gt;6-digit ID card PIN&lt;/b&gt;. &lt;b&gt;This is your last attempt&lt;/b&gt;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Bitte geben Sie nun Ihre &lt;b&gt;6-stellige Karten-PIN&lt;/b&gt; ein. &lt;b&gt;Das ist Ihr letzter Versuch&lt;/b&gt;.</translation>
    </message>
    <message>
        <source>If you enter a wrong ID Card PIN again, the PIN will be blocked. This block may only be removed using the PUK.</source>
        <translation>Falls Sie erneut eine falsche Karten-PIN eingeben, wird die PIN gesperrt. Diese Sperre können Sie nur mit der PUK aufheben.</translation>
    </message>
    <message>
        <source>Please enter your &lt;b&gt;5-digit Transport PIN&lt;/b&gt;. &lt;b&gt;This is your last attempt&lt;/b&gt;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Bitte geben Sie nun Ihre &lt;b&gt;5-stellige Transport-PIN&lt;/b&gt; ein. &lt;b&gt;Das ist Ihr letzter Versuch&lt;/b&gt;.</translation>
    </message>
    <message>
        <source>If you enter a wrong Transport PIN again, the PIN will be blocked. This block may only be removed using the PUK.</source>
        <translation>Falls Sie erneut eine falsche Transport-PIN eingeben, wird die PIN gesperrt. Diese Sperre können Sie nur mit der PUK aufheben.</translation>
    </message>
    <message>
        <source>You now have &lt;b&gt;3 more attempts&lt;/b&gt; to enter your correct &lt;b&gt;Transport PIN&lt;/b&gt;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie haben nun &lt;b&gt;3 weitere Versuche&lt;/b&gt;, um Ihre &lt;b&gt;Transport-PIN&lt;/b&gt; korrekt einzugeben.</translation>
    </message>
    <message>
        <source>Transport PIN correct</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Transport-PIN korrekt</translation>
    </message>
    <message>
        <source>Now set your personal ID card PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Setzen Sie nun Ihre persönliche Karten-PIN</translation>
    </message>
    <message>
        <source>The Transport PIN is now replaced by your personal 6-digit ID card PIN.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Transport-PIN wird nun durch Ihre persönliche 6-stellige Karten-PIN ersetzt.</translation>
    </message>
    <message>
        <source>Afterwards the %1Transport PIN is no longer required.%2</source>
        <extracomment>ALL_PLATFORMS The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Danach wird die %1Transport-PIN nicht mehr benötigt.%2</translation>
    </message>
</context>
<context>
    <name>LanguageButtonData</name>
    <message>
        <source>German</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Deutsch</translation>
    </message>
    <message>
        <source>Set language to german</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Verwende deutsche Sprache</translation>
    </message>
    <message>
        <source>English</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Englisch</translation>
    </message>
    <message>
        <source>Set language to english</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Verwende englische Sprache</translation>
    </message>
    <message>
        <source>Ukrainian</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ukrainisch</translation>
    </message>
    <message>
        <source>Set language to ukrainian</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Verwende ukrainische Sprache</translation>
    </message>
    <message>
        <source>Russian</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Russisch</translation>
    </message>
    <message>
        <source>Set language to russian</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Verwende russische Sprache</translation>
    </message>
</context>
<context>
    <name>LicenseInformation</name>
    <message>
        <source>Software license</source>
        <extracomment>MOBILE</extracomment>
        <translation>Softwarelizenz</translation>
    </message>
</context>
<context>
    <name>LinkQualityAnimation</name>
    <message>
        <source>Link quality unavailable.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Verbindungsqualität nicht verfügbar.</translation>
    </message>
    <message>
        <source>%1% link quality.</source>
        <extracomment>ALL_PLATFORMS %1 is replaced with a number between 0 and 100</extracomment>
        <translation>%1% Verbindungsqualität.</translation>
    </message>
</context>
<context>
    <name>LocalNetworkInfo</name>
    <message>
        <source>Go to application settings</source>
        <extracomment>IOS Link to application settings</extracomment>
        <translation>Zu den Anwendungseinstellungen</translation>
    </message>
    <message>
        <source>Ensure that access to the local network is allowed in your settings.</source>
        <extracomment>IOS Let user know to check the application settings for local network permission</extracomment>
        <translation>In den Einstellungen muss der Zugriff auf das lokale Netzwerk erlaubt sein.</translation>
    </message>
</context>
<context>
    <name>LogFilesView</name>
    <message>
        <source>Select Log</source>
        <extracomment>MOBILE</extracomment>
        <translation>Protokoll auswählen</translation>
    </message>
    <message>
        <source>Delete</source>
        <extracomment>MOBILE</extracomment>
        <translation>Löschen</translation>
    </message>
    <message>
        <source>All old logs will be deleted.</source>
        <extracomment>MOBILE All logfiles are about to be removed, user confirmation required.</extracomment>
        <translation>Alle Protokolle werden gelöscht.</translation>
    </message>
    <message>
        <source>Delete all logs</source>
        <extracomment>MOBILE</extracomment>
        <translation>Alle Protokolle löschen</translation>
    </message>
</context>
<context>
    <name>LogTitleBarControls</name>
    <message>
        <source>Share log</source>
        <extracomment>MOBILE</extracomment>
        <translation>Protokoll teilen</translation>
    </message>
    <message>
        <source>Delete all logs</source>
        <extracomment>MOBILE</extracomment>
        <translation>Alle Protokolle löschen</translation>
    </message>
    <message>
        <source>Filter</source>
        <extracomment>MOBILE</extracomment>
        <translation>Filter</translation>
    </message>
</context>
<context>
    <name>LogView</name>
    <message>
        <source>Logs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Protokolle</translation>
    </message>
    <message>
        <source>Save log</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Protokoll speichern</translation>
    </message>
    <message>
        <source>Logfiles (*.log)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Protokolldateien (*.log)</translation>
    </message>
    <message>
        <source>The current log will be automatically deleted at exit.</source>
        <translation>Das aktuelle Protokoll wird beim Beenden automatisch gelöscht.</translation>
    </message>
    <message>
        <source>Detach log viewer</source>
        <translation>Protokoll separat anzeigen</translation>
    </message>
    <message>
        <source>All old logs will be deleted.</source>
        <extracomment>DESKTOP All logfiles are about to be removed, user confirmation required.</extracomment>
        <translation>Alle Protokolle werden gelöscht.</translation>
    </message>
    <message>
        <source>Delete all logs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Alle Protokolle löschen</translation>
    </message>
    <message>
        <source>Delete</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Löschen</translation>
    </message>
    <message>
        <source>Filter</source>
        <extracomment>MOBILE</extracomment>
        <translation>Filter</translation>
    </message>
    <message>
        <source>Level</source>
        <extracomment>MOBILE</extracomment>
        <translation>Level</translation>
    </message>
    <message>
        <source>Category</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kategorie</translation>
    </message>
    <message>
        <source>Currently there are no log entries matching your filter.</source>
        <extracomment>MOBILE No log entries, placeholder text.</extracomment>
        <translation>Derzeit gibt es keine Einträge im Log die zu Ihrem Filter passen.</translation>
    </message>
</context>
<context>
    <name>LogViewDelegate</name>
    <message>
        <source>The log entry was copied to the clipboard.</source>
        <extracomment>DESKTOP Toast message used to confirm the copy of a log entry.
----------
MOBILE Toast message used to confirm the copy of a log entry.</extracomment>
        <translation>Der Protokolleintrag wurde in die Zwischenablage kopiert.</translation>
    </message>
</context>
<context>
    <name>MainView</name>
    <message>
        <source>See my&lt;br&gt;personal data</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Meine Daten&lt;br&gt;einsehen</translation>
    </message>
    <message>
        <source>Settings</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <source>Help</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Hilfe</translation>
    </message>
    <message>
        <source>Check device and ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Gerät und Ausweis prüfen</translation>
    </message>
    <message>
        <source>See my personal data</source>
        <extracomment>MOBILE</extracomment>
        <translation>Meine Daten einsehen</translation>
    </message>
    <message>
        <source>Two finger swipe to scroll.</source>
        <translation>Mit zwei Fingern wischen zum Scrollen.</translation>
    </message>
    <message>
        <source>Item %1 of %2</source>
        <translation>Element %1 von %2</translation>
    </message>
    <message>
        <source>Change PIN</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>PIN ändern</translation>
    </message>
    <message>
        <source>Three finger swipe to scroll.</source>
        <translation>Mit drei Fingern wischen zum Scrollen.</translation>
    </message>
</context>
<context>
    <name>MoreInformationLink</name>
    <message>
        <source>More information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Mehr Informationen</translation>
    </message>
</context>
<context>
    <name>MoreView</name>
    <message>
        <source>Help</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Hilfe</translation>
    </message>
    <message>
        <source>General</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Allgemein</translation>
    </message>
    <message>
        <source>%1 version</source>
        <extracomment>MOBILE %1 is replaced with the application name</extracomment>
        <translation>%1-Version</translation>
    </message>
    <message>
        <source>Software license</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Softwarelizenz</translation>
    </message>
    <message>
        <source>Release notes</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Release Notes</translation>
    </message>
    <message>
        <source>Privacy statement</source>
        <extracomment>MOBILE</extracomment>
        <translation>Datenschutzerklärung</translation>
    </message>
    <message>
        <source>Accessibility statement</source>
        <extracomment>MOBILE</extracomment>
        <translation>Barrierefreiheitserklärung</translation>
    </message>
    <message>
        <source>Rate %1</source>
        <extracomment>MOBILE</extracomment>
        <translation>Bewerten Sie die %1</translation>
    </message>
    <message>
        <source>Logs</source>
        <extracomment>MOBILE</extracomment>
        <translation>Protokolle</translation>
    </message>
    <message>
        <source>Information</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Information</translation>
    </message>
    <message>
        <source>List of Providers</source>
        <extracomment>MOBILE</extracomment>
        <translation>Anbieterliste</translation>
    </message>
    <message>
        <source>Data and logs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Daten und Protokolle</translation>
    </message>
    <message>
        <source>FAQ - Frequently asked questions</source>
        <extracomment>MOBILE</extracomment>
        <translation>FAQ – Häufig gestellte Fragen</translation>
    </message>
    <message>
        <source>Contact</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kontakt</translation>
    </message>
    <message>
        <source>Show Logs</source>
        <extracomment>MOBILE</extracomment>
        <translation>Protokolle anzeigen</translation>
    </message>
    <message>
        <source>Send log to the support</source>
        <extracomment>MOBILE</extracomment>
        <translation>Protokoll an den Support senden</translation>
    </message>
    <message>
        <source>Terms of use and software license</source>
        <extracomment>MOBILE</extracomment>
        <translation>Nutzungsbedingungen und Softwarelizenz</translation>
    </message>
    <message>
        <source>Start setup</source>
        <extracomment>MOBILE</extracomment>
        <translation>Einrichtung starten</translation>
    </message>
</context>
<context>
    <name>MoreViewDiagnosis</name>
    <message>
        <source>Logs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Protokolle</translation>
    </message>
    <message>
        <source>Show logs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Zeige Protokolle</translation>
    </message>
    <message>
        <source>Show system data</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Zeige Systemdaten</translation>
    </message>
    <message>
        <source>System data</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Systemdaten</translation>
    </message>
</context>
<context>
    <name>MoreViewGeneral</name>
    <message>
        <source>Open website</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Öffne Webseite</translation>
    </message>
    <message>
        <source>List of Providers</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Anbieterliste</translation>
    </message>
    <message>
        <source>FAQ - Frequently asked questions</source>
        <extracomment>DESKTOP</extracomment>
        <translation>FAQ – Häufig gestellte Fragen</translation>
    </message>
    <message>
        <source>Contact</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kontakt</translation>
    </message>
    <message>
        <source>Start setup</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Einrichtung starten</translation>
    </message>
    <message>
        <source>Setup</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Einrichtung</translation>
    </message>
</context>
<context>
    <name>MultiInfoContent</name>
    <message>
        <source>More information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Mehr Informationen</translation>
    </message>
</context>
<context>
    <name>MultiInfoData</name>
    <message>
        <source>What is the card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Was ist die Karten-PIN?</translation>
    </message>
    <message>
        <source>PIN information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>PIN-Information</translation>
    </message>
    <message>
        <source>Where can I find the card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wo finde ich die Karten-PIN?</translation>
    </message>
    <message>
        <source>How do I choose a secure PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wie wähle ich eine sichere PIN?</translation>
    </message>
    <message>
        <source>For your 6-digit PIN, choose a combination of numbers that cannot be guessed - i.e. neither &quot;123456&quot;, nor your date of birth, nor any other numbers printed on your ID card.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;How do I choose a secure PIN?&apos; paragraph 1/3</extracomment>
        <translation>Wählen Sie für Ihre 6-stellige PIN eine Zahlenkombination, die nicht zu erraten ist - also weder „123456“, noch Ihr Geburtsdatum oder andere Zahlen, die auf dem Ausweis aufgedruckt sind.</translation>
    </message>
    <message>
        <source>You can change your 6-digit PIN at any time and an unlimited number of times as long as you know your valid PIN.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;How do I choose a secure PIN?&apos; paragraph 2/3</extracomment>
        <translation>Sie können Ihre 6-stellige PIN jederzeit und unbegrenzt oft ändern, solange Ihnen Ihre gültige PIN bekannt ist.</translation>
    </message>
    <message>
        <source>Keep your PIN secret and change it if another person becomes aware of it.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;How do I choose a secure PIN?&apos; paragraph 3/3</extracomment>
        <translation>Halten Sie Ihre PIN geheim und ändern Sie diese, wenn eine andere Person hiervon Kenntnis erlangt.</translation>
    </message>
    <message>
        <source>What is the Transport PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Was ist die Transport-PIN?</translation>
    </message>
    <message>
        <source>The Transport PIN is a %1one-time PIN%2 which is required for the initial %1setup of your ID card%2. Once you replace the %1Transport PIN with a card PIN%2, it is tied to your ID card and valid for all devices.</source>
        <extracomment>ALL_PLATFORMS ALL_PLATFORMS Answer to the question &apos;What is the Transport PIN?&apos;</extracomment>
        <translation>Die Transport-PIN ist eine %1Einmal-PIN%2, die zur erstmaligen %1Einrichtung Ihres Online-Ausweises%2 benötigt wird. Sobald Sie die %1Transport-PIN mit einer Karten-PIN ersetzen%2, ist diese mit Ihrem Ausweis verknüpft und gilt auf allen Geräten.</translation>
    </message>
    <message>
        <source>Where do I find the Transport PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wo finde ich die Transport-PIN?</translation>
    </message>
    <message>
        <source>The 5-digit Transport PIN is a %1one-time PIN%2, that you received per %1letter%2 when applying for the ID card.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where do I find the Transport PIN?&apos; paragraph 1/2</extracomment>
        <translation>Die 5-stellige Transport-PIN ist eine %1Einmal-PIN%2, die Sie bei Beantragung des Online-Ausweises per %1Brief%2 erhalten.</translation>
    </message>
    <message>
        <source>When you set up your ID card, you will %1replace%2 this 5-digit %1Transport PIN with%2 a 6-digit, %1self chosen card PIN%2.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where do I find the Transport PIN?&apos; paragraph 2/2
----------
ALL_PLATFORMS Description text explaining the PINs 3/7</extracomment>
        <translation>Wenn Sie Ihren Online-Ausweis einrichten, %1ersetzen Sie%2 diese 5-stellige %1Transport-PIN durch%2 eine 6-stellige, %1selbstgewählte Karten-PIN%2.</translation>
    </message>
    <message>
        <source>Where do I find the PUK?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wo finde ich die PUK?</translation>
    </message>
    <message>
        <source>The PUK is a%1 10-digit number%2 that you can find in the %1PIN letter%2 that was sent to you by mail after you %1applied for your ID card%2. You may find it to the %1right%2 of the 5-digit %1Transport PIN%2.</source>
        <extracomment>ALL_PLATFORMS ALL_PLATFORMS Answer to the question &apos;Where do I find the PUK?&apos;</extracomment>
        <translation>Die PUK ist eine%1 10-stellige Zahl%2, die Sie in dem %1PIN-Brief%2 finden, den Sie %1nach Beantragung Ihres Ausweises%2 erhalten. Sie befindet sich %1rechts neben%2 der 5-stelligen %1Transport-PIN%2.</translation>
    </message>
    <message>
        <source>Why is the PUK required?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Warum wird die PUK verlangt?</translation>
    </message>
    <message>
        <source>The PUK is required if the %1card PIN has been entered incorrectly 3 times%2 in a row. As a result, the card PIN is blocked. By entering the PUK you will %1unblock the card PIN%2 and have%1 3 more attempts%2 to enter the correct PIN.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Why is the PUK required?&apos;</extracomment>
        <translation>Die PUK wird verlangt, wenn die %1Karten-PIN 3 Mal falsch eingegeben%2 wurde. Die Karten-PIN ist dadurch gesperrt. Durch die Eingabe der PUK %1entsperren Sie die Karten-PIN%2 und haben%1 3 weitere Versuche%2, die richtige PIN einzugeben.</translation>
    </message>
    <message>
        <source>My PUK does not work</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Meine PUK funktioniert nicht</translation>
    </message>
    <message>
        <source>Make sure, that the letter at hand is the %1letter for your current ID card%2. The PIN letter is valid only for your current ID card. With a new ID card, you receive a new PIN letter with a new PUK.</source>
        <extracomment>ALL_PLATFORMS Text block regarding &quot;My PUK does not work&quot;</extracomment>
        <translation>Stellen Sie sicher, dass es sich bei dem vorliegenden Brief um den %1Brief zu Ihrem aktuellen Ausweis%2 handelt. Der PIN-Brief ist nur an den jeweiligen Ausweis geknüpft. Bei Wechsel des Ausweises bekommen Sie daher einen neuen PIN-Brief mit neuer PUK.</translation>
    </message>
    <message>
        <source>If you applied for a PIN Reset Letter, make sure that you don&apos;t mix up the letters. The %1PIN letter you need contains the 5-digit Transport PIN and the PUK%2. The PIN Reset Letter contains the 6-digit card PIN and the activation code.</source>
        <extracomment>ALL_PLATFORMS Text block regarding &quot;My PUK does not work&quot;</extracomment>
        <translation>Falls Sie einen PIN-Rücksetzbrief bestellt haben, stellen Sie sicher, dass Sie die Briefe nicht verwechseln. Der %1PIN-Brief, den Sie benötigen, enthält die 5-stellige Transport-PIN und die PUK%2. Der PIN-Rücksetzbrief enthält jedoch die 6-stellige Karten-PIN und den Aktivierungscode.</translation>
    </message>
    <message>
        <source>Why is the CAN required?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Warum wird die CAN verlangt?</translation>
    </message>
    <message>
        <source>The card access number (CAN) is required when the %1card PIN has been entered incorrectly 2 times%2.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;When is the card access number (CAN) required?&apos;</extracomment>
        <translation>Die Zugangsnummer (CAN) wird verlangt, wenn die %1Karten-PIN 2 Mal falsch eingegeben wurde%2.</translation>
    </message>
    <message>
        <source>Why do I have to enter the CAN before a 3rd attempt?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Warum muss ich vor einem 3. Versuch die CAN eingeben?</translation>
    </message>
    <message>
        <source>A 3rd incorrect entry blocks your PIN and you can no longer use the eID function until it is unblocked. Requesting the CAN ensures that %1direct access to your ID card%2 is given.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Why do I have to enter the CAN before a 3rd attempt?&apos;</extracomment>
        <translation>Eine 3. Fehleingabe sperrt Ihre PIN und Sie können die Online-Ausweisfunktion bis zur Entsperrung nicht mehr nutzen. Durch die Abfrage der CAN wird sichergestellt, dass ein %1direkter Zugriff zum Ausweis vorliegt%2.</translation>
    </message>
    <message>
        <source>Where can I find the CAN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wo finde ich die CAN?</translation>
    </message>
    <message>
        <source>The CAN is a 6-digit number that can be found on the %1bottom right%2 of the %1front of the ID card%2.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where can I find the CAN?&apos;</extracomment>
        <translation>Die CAN ist eine 6-stellige Zahl, die %1unten rechts%2 auf der %1Vorderseite des Ausweisdokuments%2 zu finden ist.</translation>
    </message>
    <message>
        <source>The Card Access Number (CAN) allows to access the imprinted data of the ID card. The CAN is a 6-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in picture).</source>
        <extracomment>ALL_PLATFORMS Description text of CAN-allowed authentication</extracomment>
        <translation>Die Zugangsnummer (CAN) ermöglicht den Zugriff auf die auf dem Ausweis gespeicherten Daten. Die CAN ist eine 6-stellige Zahl, die auf der Vorderseite des Ausweises unten rechts zu finden ist (auf Abbildung markiert).</translation>
    </message>
    <message>
        <source>No PIN known</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Keine PIN bekannt</translation>
    </message>
    <message>
        <source>You do not know your PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie kennen Ihre PIN nicht?</translation>
    </message>
    <message>
        <source>You have not yet set a 6-digit card PIN and cannot find the PIN letter with the Transport PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie haben noch keine 6-stellige PIN gesetzt und können den Brief mit der Transport-PIN nicht finden?</translation>
    </message>
    <message>
        <source>You set a card PIN when picking up your ID card or later by yourself, but you can no longer remember it?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie haben bei der Abholung des Ausweises oder später selbst eine Karten-PIN gesetzt, können sich aber nicht mehr an diese erinnern?</translation>
    </message>
    <message>
        <source>How do the PIN types differ?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wie unterscheiden sich die PIN-Arten?</translation>
    </message>
    <message>
        <source>Your ID card comes with a 5-digit %1Transport PIN%2 which you need to %1replace with%2 a 6-digit %1card PIN%2 that you choose yourself.</source>
        <extracomment>ALL_PLATFORMS Description text explaining the PINs 1/7</extracomment>
        <translation>Ihr Ausweis wurde bei Erstellung mit einer 5-stelligen %1Transport-PIN%2 versehen, die Sie %1durch eine%2 6-stellige, selbstgewählte %1Karten-PIN ersetzen%2 müssen.</translation>
    </message>
    <message>
        <source>5-digit Transport PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>5-stellige Transport-PIN</translation>
    </message>
    <message>
        <source>6-digit PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>6-stellige PIN</translation>
    </message>
    <message>
        <source>The 5-digit Transport PIN is a %1one-time PIN%2 that was sent to you by %1mail%2 after you applied for your ID card.</source>
        <extracomment>ALL_PLATFORMS Description text explaining the PINs 2/7</extracomment>
        <translation>Die 5-stellige Transport-PIN ist eine %1Einmal-PIN%2, die Sie bei Beantragung des Online-Ausweises per %1Brief%2 erhalten haben.</translation>
    </message>
    <message>
        <source>The 6-digit card PIN is a %1number that you choose yourself%2 when you set up the eID function for the first time. It %1replaces%2 your%1 5-digit Transport PIN%2.</source>
        <extracomment>ALL_PLATFORMS Description text explaining the PINs 4/7</extracomment>
        <translation>Die 6-stellige Karten-PIN ist %1eine Geheimnummer, die Sie selbst wählen%2, wenn Sie Ihren Online-Ausweis zum ersten Mal einrichten. Sie %1ersetzt%2 Ihre%1 5-stellige Transport-PIN%2.</translation>
    </message>
    <message>
        <source>The card PIN is a 6-digit PIN that you set yourself. You always need this %1PIN%2 if you want to use the %1eID function%2.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;what is the card pin?&apos;</extracomment>
        <translation>Die Karten-PIN ist eine 6-stellige PIN, die Sie selbst gewählt haben. Diese %1PIN%2 brauchen Sie immer, wenn Sie die %1Online-Ausweisfunktion%2 nutzen möchten.</translation>
    </message>
    <message>
        <source>The card PIN was %1either%2...</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where can I find the card PIN?&apos; - Intro</extracomment>
        <translation>Die Karten-PIN wurde %1entweder%2...</translation>
    </message>
    <message>
        <source>set by you when picking up the ID card %1at the citiziens&apos; office%2</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where can I find the card PIN?&apos; - Parapgraph 1/3</extracomment>
        <translation>von Ihnen direkt bei der Abholung des Ausweises %1in der Ausweisbehörde%2 gesetzt</translation>
    </message>
    <message>
        <source>set %1in the %3 using%2 the 5-digit %1Transport PIN%2</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where can I find the card PIN?&apos; - Parapgraph 2/3</extracomment>
        <translation>%1in der %3 mithilfe%2 der 5-stelligen %1Transport-PIN%2 gesetzt</translation>
    </message>
    <message>
        <source>requested using the %1PIN Reset Service%2 - you may continue using the 6-digit card PIN from the letter or change it via &quot;Change PIN&quot; to a self chosen card PIN</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where can I find the card PIN?&apos; - Parapgraph 3/3</extracomment>
        <translation>per %1PIN-Rücksetzdienst%2 beantragt - die 6-stellige Karten-PIN aus dem Brief können Sie weiterverwenden oder per &quot;PIN ändern&quot; durch eine selbstgewählte Karten-PIN ersetzen</translation>
    </message>
    <message>
        <source>I just have a 5-digit Transport PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ich habe nur eine 5-stellige Transport-PIN</translation>
    </message>
    <message>
        <source>You need to change the%1 5-digit Transport PIN%2 to your personal card PIN. Use %1Change PIN%2 from the startpage to do so.</source>
        <extracomment>ALL_PLATFORMS Explanation if only the Transport PIN is at hand. The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Die%1 5-stellige Transport-PIN%2 müssen Sie durch Ihre persönliche Karten-PIN ersetzen. Wählen Sie hierfür auf der Startseite %1PIN ändern%2.</translation>
    </message>
    <message>
        <source>With this 6-digit PIN you prove online that the ID card belongs to you. %1No one can use the eID function without this PIN%2.</source>
        <extracomment>ALL_PLATFORMS Description text explaining the PINs 6/7</extracomment>
        <translation>Mit der 6-stelligen, selbstgewählten PIN weisen Sie online nach, dass der Ausweis Ihnen gehört. %1Ohne diese PIN kann keiner Ihren Ausweis online nutzen%2.</translation>
    </message>
    <message>
        <source>If you applied for a %1PIN Reset Letter%2, a new card PIN was already set for you. You can change your card PIN at %1any time in %3%2.</source>
        <extracomment>ALL_PLATFORMS Description text explaining the PINs (%1 is replaced with the application name) 7/7</extracomment>
        <translation>Falls Sie einen %1PIN-Rücksetzbrief%2 bestellt haben, wurde Ihnen bereits eine Karten-PIN gesetzt. Sie können Ihre Karten-PIN jedoch %1jederzeit in der %3 ändern%2.</translation>
    </message>
    <message>
        <source>My Transport PIN does not work</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Meine Transport-PIN funktioniert nicht</translation>
    </message>
    <message>
        <source>If your Transport PIN from the PIN letter does work, this might be caused by the following reasons:</source>
        <extracomment>ALL_PLATFORMS ALL_PLATFORMS Introduction why the Transport PIN might not work.</extracomment>
        <translation>Wenn Ihre Transport-PIN aus dem PIN-Brief nicht funktioniert, kann das folgende Gründe haben:</translation>
    </message>
    <message>
        <source>You already set a 6-digit PIN either at the competent authority or using the %1.</source>
        <extracomment>ALL_PLATFORMS Information why the Transport PIN is void: a 6-digit PIN was already set</extracomment>
        <translation>Sie haben bereits eine 6-stellige PIN gewählt, in der Behörde oder per %1.</translation>
    </message>
    <message>
        <source>You applied for PIN Reset Letter (letter containing a 6-digit PIN and corresponding Activation Code).</source>
        <extracomment>ALL_PLATFORMS Information why the Transport PIN is void: the PIN Reset Service was used</extracomment>
        <translation>Sie haben einen PIN-Rücksetzbrief bestellt (Brief mit 6-stelliger PIN &amp; Aktivierungscode).</translation>
    </message>
    <message>
        <source>Once you set a 6-digit PIN, the Transport PIN is no longer valid. You may then only use the 6-digit PIN to authenticate yourself.</source>
        <extracomment>ALL_PLATFORMS Information that the Transport PIN is void once a 6-digit PIN was set.</extracomment>
        <translation>Sobald Sie eine 6-stellige PIN gesetzt haben, ist die Transport-PIN hinfällig. Sie können dann ausschließlich die 6-stellige PIN zum Ausweisen verwenden.</translation>
    </message>
    <message>
        <source>Set up the ID card in three steps</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Richten Sie den Ausweis in drei Schritten ein</translation>
    </message>
    <message>
        <source>Set up a smartphone or card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Smartphone oder Kartenleser einrichten</translation>
    </message>
    <message>
        <source>To read your card, first pair an NFC-enabled smartphone or set up the USB card reader.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Um Ihren Ausweis auszulesen, wird zuerst ein NFC-fähiges Smartphone gekoppelt oder der USB-Kartenleser eingerichtet.</translation>
    </message>
    <message>
        <source>Read the ID card</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausweis auslesen</translation>
    </message>
    <message>
        <source>This checks whether your device and ID card meet all the requirements.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>So wird geprüft, ob Ihr Gerät und Ausweis alle Anforderungen erfüllen.</translation>
    </message>
    <message>
        <source>Set up the PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>PIN einrichten</translation>
    </message>
    <message>
        <source>You use the PIN every time you use your ID card online.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Die PIN verwenden Sie jedes Mal, wenn Sie Ihren Ausweis online verwenden.</translation>
    </message>
    <message>
        <source>Set up the ID card in two steps</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Richten Sie den Ausweis in zwei Schritten ein</translation>
    </message>
    <message>
        <source>If you require assistance with this, please contact our support team.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wünschen Sie Unterstützung, wenden Sie sich an unseren Support.</translation>
    </message>
    <message>
        <source>Send mail</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Mail senden</translation>
    </message>
    <message>
        <source>Support</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Support</translation>
    </message>
    <message>
        <source>My smartphone does not show up</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Mein Smartphone wird nicht gefunden</translation>
    </message>
    <message>
        <source>If your smartphone does not appear in the list of available devices, there may be various reasons for this.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wenn Ihr Smartphone in der Liste der verfügbaren Geräte nicht auftaucht, kann das verschiedene Gründe haben.</translation>
    </message>
    <message>
        <source>Security software</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sicherheitssoftware</translation>
    </message>
    <message>
        <source>You may be using security software that prevents pairing.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Es kann sein, dass Sie Sicherheitssoftware verwenden, welche die Kopplung verhindert.</translation>
    </message>
    <message>
        <source>Updates</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Aktualisierungen</translation>
    </message>
    <message>
        <source>Make sure that you have installed the latest version on both devices.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Stellen Sie sicher, dass Sie auf den beiden Geräten die aktuelle Version installiert haben.</translation>
    </message>
    <message>
        <source>Same local network</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Selbes lokales Netzwerk</translation>
    </message>
    <message>
        <source>Both devices must be in the same network. This applies in the following cases:</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Beide Geräte müssen sich im selben Netzwerk befinden. Das ist in den folgenden Fällen gegeben:</translation>
    </message>
    <message>
        <source>Visibility of the devices</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sichtbarkeit der Geräte</translation>
    </message>
    <message>
        <source>The devices in the network must be visible to each other. Networks at the workplace, in public places or guest networks are therefore often not suitable, as functions such as &quot;Client Isolation&quot; or &quot;Broadcasting Filtering&quot; must not be active.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Geräte im Netzwerk müssen untereinander sichtbar sein. Daher sind Netzwerke am Arbeitsplatz, an öffentlichen Orten oder Gastnetze oft nicht geeignet, da Funktionen wie &quot;Client Isolation&quot; oder &quot;Broadcasting Filtering&quot; nicht aktiv sein dürfen.</translation>
    </message>
    <message>
        <source>If a device is not found in card reader mode, try to pair the device again via &quot;Pair device&quot;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wenn ein Gerät im Kartenleser-Modus nicht gefunden wird, versuchen Sie das Gerät über &quot;Gerät koppeln&quot; erneut zu koppeln.</translation>
    </message>
    <message>
        <source>&quot;Pair device&quot; instead of &quot;Activate card reader&quot;</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>&quot;Gerät koppeln&quot; statt &quot;Kartenleser aktivieren&quot;</translation>
    </message>
    <message>
        <source>Both devices in one WiFi network</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Beide Geräte in einem WLAN</translation>
    </message>
    <message>
        <source>The PC via LAN cable and the smartphone via WiFi in the same network</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Den PC per LAN Kabel und das Smartphone per WLAN im selben Netz</translation>
    </message>
    <message>
        <source>Alternatively: set up a mobile WiFi hotspot via smartphone and dial in the device to be paired there.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Alternativ: mobilen WLAN-Hotspot per Smartphone einrichten und das zu koppelnde Gerät dort einwählen.</translation>
    </message>
    <message>
        <source>I can&apos;t recall my card PIN. How do I set a new card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ich kenne meine Karten-PIN nicht. Wie kann ich eine neue PIN setzen?</translation>
    </message>
    <message>
        <source>I can&apos;t recall neither my card PIN nor my transport PIN. How do I set a new card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ich kenne weder Karten-PIN noch Transport-PIN. Wie kann ich eine neue PIN setzen?</translation>
    </message>
    <message>
        <source>How do I set a new card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wie kann ich eine neue PIN setzen?</translation>
    </message>
    <message>
        <source>I can&apos;t recall my PUK. How do I set a new card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ich kenne meine PUK nicht. Wie kann ich eine neue PIN setzen?</translation>
    </message>
</context>
<context>
    <name>MultiInfoHintContent</name>
    <message>
        <source>Hint</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Tipp</translation>
    </message>
</context>
<context>
    <name>NavigationAction</name>
    <message>
        <source>Cancel</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <source>Back</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Zurück</translation>
    </message>
    <message>
        <source>Close</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Schließen</translation>
    </message>
</context>
<context>
    <name>NavigationView</name>
    <message>
        <source>Start</source>
        <translation>Start</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <source>Help</source>
        <translation>Hilfe</translation>
    </message>
    <message>
        <source>Card reader</source>
        <translation>Kartenleser</translation>
    </message>
    <message>
        <source>%1 of %2</source>
        <extracomment>MOBILE Relative position of current navigation tab in navigation view. %1 is replaced with the current tab&apos;s index, %2 with the total count of tabs</extracomment>
        <translation>%1 von %2</translation>
    </message>
    <message>
        <source>Tab</source>
        <extracomment>MOBILE</extracomment>
        <translation>Tab</translation>
    </message>
    <message>
        <source>Selection</source>
        <extracomment>IOS Selected navigation tab.</extracomment>
        <translation>Auswahl</translation>
    </message>
    <message>
        <source>Tab bar</source>
        <extracomment>IOS Name of a11y element of selected navigation tab.</extracomment>
        <translation>Tab-Leiste</translation>
    </message>
    <message>
        <source>Selected</source>
        <extracomment>ANDROID Currently selected navigation tab of navigation view.</extracomment>
        <translation>Ausgewählt</translation>
    </message>
</context>
<context>
    <name>NfcConnectionInfoView</name>
    <message>
        <source>My ID card is not detected</source>
        <extracomment>MOBILE</extracomment>
        <translation>Mein Ausweis wird nicht erkannt</translation>
    </message>
    <message>
        <source>If your ID card is not recognized when it is read, there could be various reasons.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Wenn Ihr Ausweis beim Auslesen nicht erkannt wird, kann das verschiedene Gründe haben.</translation>
    </message>
    <message>
        <source>NFC interface</source>
        <extracomment>MOBILE</extracomment>
        <translation>NFC-Schnittstelle</translation>
    </message>
    <message>
        <source>The NFC interface, and therefore the point at which the ID card must be placed, varies depending on the smartphone model. Find out where the NFC interface is on your smartphone model.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Die NFC-Schnittstelle, und damit der Punkt, an den der Ausweis angelegt werden muss, variiert je nach Smartphone Modell. Informieren Sie sich, wo die NFC-Schnittstelle Ihres Smartphone Modells ist.</translation>
    </message>
    <message>
        <source>Tutorial: Using NFC</source>
        <extracomment>MOBILE</extracomment>
        <translation>Tutorial: Verbindung per NFC</translation>
    </message>
    <message>
        <source>Sources of interference</source>
        <extracomment>MOBILE</extracomment>
        <translation>Störfaktoren</translation>
    </message>
    <message>
        <source>Remove your phone case and allow the ID card to come into direct contact with the device. Low battery power can negatively impact NFC functionality, so make sure your smartphone is charged and not in power saving mode.</source>
        <translation>Entfernen Sie Ihre Handyhülle und ermöglichen Sie einen direkten Kontakt des Ausweises zum Gerät. Eine geringe Akkuleistung kann sich negativ auf die NFC Funktion auswirken, daher stellen Sie sicher, dass Ihr Smartphone aufgeladen ist und sich nicht im Energiesparmodus befindet.</translation>
    </message>
    <message>
        <source>Alternatives</source>
        <extracomment>MOBILE</extracomment>
        <translation>Alternativen</translation>
    </message>
    <message>
        <source>If the connection does not work despite following the tips, you can use another smartphone as a card reader.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Sollte die Verbindung trotz Befolgen der Tipps nicht funktionieren, können Sie ein anderes Smartphone als Kartenleser verwenden.</translation>
    </message>
    <message>
        <source>Connect using a card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Mit Kartenleser verbinden</translation>
    </message>
    <message>
        <source>You can also use %1 on a PC and connect an USB card reader to read the ID card.</source>
        <extracomment>MOBILE &quot;%1&quot; is replaced with &quot;AusweisApp&quot;</extracomment>
        <translation>Sie können die %1 auch auf einem PC verwenden und einen USB-Kartenleser zum Auslesen des Ausweises anschließen.</translation>
    </message>
</context>
<context>
    <name>NfcWorkflow</name>
    <message>
        <source>Go to NFC settings</source>
        <extracomment>MOBILE</extracomment>
        <translation>Zu den NFC Einstellungen</translation>
    </message>
    <message>
        <source>Start NFC scan</source>
        <extracomment>MOBILE</extracomment>
        <translation>NFC-Scan starten</translation>
    </message>
    <message>
        <source>NFC is switched off</source>
        <extracomment>MOBILE NFC is available but needs to be activated in the settings of the smartphone.</extracomment>
        <translation>NFC ist nicht aktiv</translation>
    </message>
    <message>
        <source>Please enable NFC in your system settings.</source>
        <extracomment>MOBILE NFC is available but needs to be activated in the settings of the smartphone.</extracomment>
        <translation>Bitte aktivieren Sie NFC in Ihren Systemeinstellungen.</translation>
    </message>
    <message>
        <source>NFC scan is not running</source>
        <extracomment>MOBILE</extracomment>
        <translation>Der NFC-Scan ist nicht aktiv</translation>
    </message>
    <message>
        <source>Please start the NFC scan.</source>
        <extracomment>MOBILE NFC is available and enabled but needs to be started.</extracomment>
        <translation>Bitte starten Sie den NFC-Scan.</translation>
    </message>
    <message>
        <source>NFC is not available</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kein NFC verfügbar</translation>
    </message>
    <message>
        <source>NFC is disabled</source>
        <extracomment>MOBILE</extracomment>
        <translation>NFC ist deaktiviert</translation>
    </message>
    <message>
        <source>Start scan</source>
        <extracomment>MOBILE NFC is available and enabled but needs to be started.</extracomment>
        <translation>Scan starten</translation>
    </message>
    <message>
        <source>Searching for ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ausweis wird gesucht</translation>
    </message>
    <message>
        <source>Your device does not meet the technical requirements (Extended Length not supported).</source>
        <extracomment>MOBILE The NFC interface does not meet the minimum requirements.</extracomment>
        <translation>Ihr Gerät erfüllt leider nicht die technischen Voraussetzungen (Extended Length).</translation>
    </message>
    <message>
        <source>Please place your ID card on the top of the device&apos;s back side</source>
        <extracomment>IOS The ID card may be inserted, the authentication process may be started.</extracomment>
        <translation>Bitte platzieren Sie Ihren Ausweis oben an der Geräterückseite</translation>
    </message>
    <message>
        <source>The device &quot;%1&quot; wants to use this smartphone as card reader and connect to your ID card</source>
        <extracomment>MOBILE %1 will be replaced with the name of the device.</extracomment>
        <translation>Das Gerät &quot;%1&quot; möchte dieses Smartphone als Kartenleser nutzen und sich mit Ihrem Ausweis verbinden</translation>
    </message>
    <message>
        <source>Keep one position for several seconds before trying another one and do not move the ID card after contact was established.</source>
        <extracomment>ANDROID Text that one ID card position should be kept for several seconds</extracomment>
        <translation>Halten Sie jede Position einige Sekunden, bevor Sie eine andere ausprobieren und bewegen Sie den Ausweis nicht mehr, sobald der Kontakt hergestellt wurde.</translation>
    </message>
    <message>
        <source>Please place your ID card directly on the device&apos;s back side</source>
        <extracomment>ANDROID Text regarding the ID card position on the device</extracomment>
        <translation>Bitte platzieren Sie Ihren Ausweis direkt an der Geräterückseite</translation>
    </message>
    <message>
        <source>This device cannot read the ID card</source>
        <extracomment>MOBILE AA can&apos;t use NFC on this device, suggest to use SaK instead.</extracomment>
        <translation>Dieses Gerät kann den Ausweis nicht auslesen</translation>
    </message>
    <message>
        <source>If you want to use the online identification on this device anyway, you can %1pair%2 another %1NFC-enabled smartphone%2 as a card reader.</source>
        <extracomment>MOBILE AA can&apos;t use NFC on this device, suggest to use SaC instead. The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Möchten Sie die Online-Ausweisfunktion auf diesem Gerät nutzen, können Sie ein %1NFC-fähiges Smartphone%2 als Kartenleser %1koppeln%2.</translation>
    </message>
    <message>
        <source>Alternatively, you may use the %3 with a %1USB card reader%2 on your %1PC%2.</source>
        <extracomment>MOBILE AA can&apos;t use NFC on this device, suggest to use a PC instead. The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Alternativ können Sie die %3 mit einem %1USB-Kartenleser%2 am %1PC%2 nutzen.</translation>
    </message>
    <message>
        <source>My ID card is not detected</source>
        <extracomment>MOBILE</extracomment>
        <translation>Mein Ausweis wird nicht erkannt</translation>
    </message>
    <message>
        <source>Read ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ausweis auslesen</translation>
    </message>
    <message>
        <source>Pair NFC-enabled smartphone</source>
        <extracomment>MOBILE</extracomment>
        <translation>NFC-fähiges Smartphone koppeln</translation>
    </message>
    <message>
        <source>Please place your ID card directly on the device&apos;s back side and try several positions.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Legen Sie den Ausweis dafür direkt an die Rückseite Ihres Smartphones und probieren Sie verschiedene Positionen aus.</translation>
    </message>
    <message>
        <source>Once the connection has been established, %1hold%2 the card in the %1same position%2 until you are asked to remove it again.</source>
        <extracomment>MOBILE %1 + %2 are for bold formatting</extracomment>
        <translation>Sobald die Verbindung hergestellt wurde, %1halten%2 Sie den Ausweis %1in der selben Position%2, bis Sie wieder dazu aufgerufen werden, die Karte zu entfernen.</translation>
    </message>
    <message>
        <source>Ensure a stable connection to the ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Stellen Sie die Verbindung zum Ausweis dauerhaft her</translation>
    </message>
    <message>
        <source>Connection to ID card lost</source>
        <extracomment>MOBILE</extracomment>
        <translation>Verbindung zum Ausweis unterbrochen</translation>
    </message>
</context>
<context>
    <name>Notifications</name>
    <message>
        <source>Notification: %1</source>
        <extracomment>DESKTOP %1 will be replaced with a notification text</extracomment>
        <translation>Benachrichtigung: %1</translation>
    </message>
</context>
<context>
    <name>NumberField</name>
    <message>
        <source>The number is hidden.</source>
        <extracomment>ALL_PLATFORMS Screenreader text for the password field</extracomment>
        <translation>Die Geheimzahl ist ausgeblendet.</translation>
    </message>
    <message>
        <source>You entered %1 of %2 digits.</source>
        <extracomment>ALL_PLATFORMS Screenreader text for the password field</extracomment>
        <translation>Sie haben %1 von %2 Ziffern eingegeben.</translation>
    </message>
    <message>
        <source>Click to hide the number</source>
        <extracomment>DESKTOP Screenreader text for the eye icon to change the password visibility</extracomment>
        <translation>Klicken Sie die Taste um die Geheimnummer auszublenden</translation>
    </message>
    <message>
        <source>Tap to hide the number</source>
        <extracomment>MOBILE Screenreader text for the eye icon to change the password visibility</extracomment>
        <translation>Tippen Sie die Taste um die Geheimnummer auszublenden</translation>
    </message>
    <message>
        <source>Click to show the number</source>
        <extracomment>DESKTOP Screenreader text for the eye icon to change the password visibility</extracomment>
        <translation>Klicken Sie die Taste um die Geheimnummer einzublenden</translation>
    </message>
    <message>
        <source>Tap to show the number</source>
        <extracomment>MOBILE Screenreader text for the eye icon to change the password visibility</extracomment>
        <translation>Tippen Sie die Taste um die Geheimnummer einzublenden</translation>
    </message>
    <message>
        <source>The number is visible. Digits entered so far: %1</source>
        <extracomment>ALL_PLATFORMS Screenreader text for the password field</extracomment>
        <translation>Die Geheimnummer ist sichtbar. Bisher eingegebene Ziffern: %1</translation>
    </message>
</context>
<context>
    <name>NumberPad</name>
    <message>
        <source>Number pad</source>
        <extracomment>MOBILE</extracomment>
        <translation>Bildschirmtastatur</translation>
    </message>
    <message>
        <source>Delete last digit</source>
        <extracomment>MOBILE A11y text for the &quot;delete&quot; button image.</extracomment>
        <translation>Lösche letzte Ziffer</translation>
    </message>
    <message>
        <source>Submit</source>
        <extracomment>MOBILE A11y text for the &quot;submit&quot; button image.</extracomment>
        <translation>Absenden</translation>
    </message>
    <message>
        <source>Delete last digit, disabled until input is present.</source>
        <extracomment>MOBILE A11y text for the &quot;delete&quot; button text when the button is disabled.</extracomment>
        <translation>Lösche letzte Ziffer, deaktiviert da keine Eingabe vorliegt.</translation>
    </message>
    <message>
        <source>, disabled until input is complete.</source>
        <extracomment>MOBILE A11y text, appended onto the &quot;submit&quot; button text when the button is disabled.</extracomment>
        <translation>, deaktiviert bis die Eingabe vollständig ist.</translation>
    </message>
</context>
<context>
    <name>NumberPadButton</name>
    <message>
        <source>Disabled</source>
        <translation>Deaktiviert</translation>
    </message>
</context>
<context>
    <name>OnboardingCheckIDView</name>
    <message>
        <source>Test result</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Ergebnis der Prüfung</translation>
    </message>
    <message>
        <source>Read ID card</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Ausweis auslesen</translation>
    </message>
    <message>
        <source>Please ensure that the ID card is placed on the card reader.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Stellen Sie sicher, dass Ihr Ausweis auf dem Kartenleser aufliegt.</translation>
    </message>
    <message>
        <source>Please follow the instructions on your smartphone.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Beachten Sie die Anweisungen auf Ihrem Smartphone.</translation>
    </message>
    <message>
        <source>Looking for ID card</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Ausweis wird gesucht</translation>
    </message>
    <message>
        <source>Connected to %1. Please follow the instructions on your smartphone.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Verbunden mit %1. Bitte folgen Sie den Anweisungen auf dem verbundenen Smartphone.</translation>
    </message>
    <message>
        <source>Looking for device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Gerät wird gesucht</translation>
    </message>
</context>
<context>
    <name>OnboardingCompletionView</name>
    <message>
        <source>provider list</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Anbieterliste</translation>
    </message>
    <message>
        <source>Complete setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Einrichtung abschließen</translation>
    </message>
    <message>
        <source>If you are asked to authenticate yourself online on a provider&apos;s website, the provider will automatically redirect you to %1. The app guides you through the authentication process and then takes you back to the provider&apos;s website. You can find many examples of use in the %2.</source>
        <extracomment>ALL_PLATFORMS %1 will be replaced with the application name, %2 will be replaced with &quot;provider list&quot; as hyperlink.</extracomment>
        <translation>Werden Sie auf der Webseite eines Anbieters zum online Ausweisen aufgefordert, leitet dieser Sie automatisch zur %1 weiter. Die App führt Sie durch den Ausweisvorgang und leitet Sie anschließend wieder zurück zur Webseite des Anbieters. Viele Anwendungsbeispiele finden Sie in der %2.</translation>
    </message>
    <message>
        <source>Use the eID function</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Online-Ausweis nutzen</translation>
    </message>
    <message>
        <source>You can use %1 to authenticate yourself to providers online</source>
        <extracomment>ALL_PLATFORMS %1 will be replaced with the application name</extracomment>
        <translation>Mit der %1 können Sie sich bei Anbietern online ausweisen</translation>
    </message>
</context>
<context>
    <name>OnboardingConfirmStageBaseView</name>
    <message>
        <source>Step %1 of %2 was successful</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Schritt %1 von %2 erfolgreich</translation>
    </message>
    <message>
        <source>Hint</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Tipp</translation>
    </message>
</context>
<context>
    <name>OnboardingConfirmationViewData</name>
    <message>
        <source>You can now use your ID card in combination with your 6-digit card PIN to identify yourself online.</source>
        <extracomment>DESKTOP Common part of the description of the confirmation view after the Change PIN stage.</extracomment>
        <translation>Sie können Ihren Ausweis nun in Kombination mit Ihrer 6-stelligen Karten-PIN nutzen, um sich online auszuweisen.</translation>
    </message>
    <message>
        <source>You have now completed the main part of the setup - there is only one more query about personalized settings.</source>
        <extracomment>DESKTOP Description of the confirmation view about upcoming steps after the Change PIN stage.</extracomment>
        <translation>Sie haben den Hauptteil der Einrichtung nun abgeschlossen - es folgt nur noch eine Abfrage zu personalisierten Einstellungen.</translation>
    </message>
    <message>
        <source>You have now completed the main part of the setup - there are only two more queries about personalized settings.</source>
        <extracomment>DESKTOP Description of the confirmation view about upcoming steps after the Change PIN stage.</extracomment>
        <translation>Sie haben den Hauptteil der Einrichtung nun abgeschlossen - es folgen nur noch zwei Abfragen zu personalisierten Einstellungen.</translation>
    </message>
    <message>
        <source>Leave the ID card on the card reader.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp stage.</extracomment>
        <translation>Lassen Sie den Ausweis auf dem Kartenleser liegen.</translation>
    </message>
    <message>
        <source>Now have your PIN ready.</source>
        <extracomment>ALL_PLATFORMS Common part of the description of the confirmation view after the set up AusweisApp (with SaC) stage.</extracomment>
        <translation>Halten Sie nun Ihre PIN bereit.</translation>
    </message>
    <message>
        <source>If you have already %1chosen a 6-digit card PIN%2 for this ID card (either in the %3 or at the competent authority) %1or%2 have ordered a %1PIN Reset Letter%2, this will apply.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp stage. %1 + %2 are for bold formatting, %3 is replaced with AusweisApp.</extracomment>
        <translation>Falls Sie für diesen Ausweis bereits eine%1 6-stellige Karten-PIN gewählt%2 haben (entweder in der %3 oder bei der zuständigen Behörde) %1oder%2 einen %1PIN-Rücksetzbrief bestellt%2 haben, gilt diese.</translation>
    </message>
    <message>
        <source>Otherwise, use your %1Transport PIN%2 that you received by letter when you first applied for the ID card. You then replace this %1one-time PIN%2 with your personal card PIN in the %3.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp with card reader stage. %1 + %2 are for bold formatting, %3 is replaced with AusweisApp.</extracomment>
        <translation>Andernfalls verwenden Sie Ihre %1Transport-PIN%2, die Sie bei erstmaliger Beantragung des Ausweises per Brief erhalten haben. Diese %1Einmal-PIN%2 ersetzen Sie dann in der %3 durch Ihre persönliche Karten-PIN.</translation>
    </message>
    <message>
        <source>PIN has been successfully changed</source>
        <extracomment>ALL_PLATFORMS Headline of the confirmation view after the change PIN stage</extracomment>
        <translation>PIN wurde erfolgreich geändert</translation>
    </message>
    <message>
        <source>ID card detected</source>
        <extracomment>ALL_PLATFORMS Headline of the confirmation view after the set up AusweisApp stage</extracomment>
        <translation>Ausweis wurde erkannt</translation>
    </message>
    <message>
        <source>Keep the position of your ID card on your smartphone so that it can be read immediately to request the PIN.</source>
        <extracomment>ALL_PLATFORMS Hint text of the confirmation view after the set up AusweisApp stage</extracomment>
        <translation>Behalten Sie die Position Ihres Ausweises am Smartphone bei, damit er für die Abfrage der PIN unmittelbar ausgelesen werden kann.</translation>
    </message>
    <message>
        <source>Your smartphone has been paired as a card reader and can be used to read your ID card in the next step.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp with SaC stage.</extracomment>
        <translation>Ihr Smartphone als Kartenleser wurde gekoppelt und kann im nächsten Schritt zum Auslesen Ihres Ausweises verwendet werden.</translation>
    </message>
    <message>
        <source>Once your devices have been paired, you only need to select &quot;Activate card reader&quot; in the card reader menu on your smartphone in future to use your smartphone as a card reader for this device.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after pairing a SaC stage.</extracomment>
        <translation>Sind Ihre Geräte einmal gekoppelt, müssen Sie in Zukunft nur &quot;Kartenleser aktivieren&quot; im Kartenleser-Menü auf dem Smartphone auswählen, um Ihr Smartphone als Kartenleser für dieses Gerät zu verwenden.</translation>
    </message>
    <message>
        <source>Your card reader has been recognized and can be used to read your ID card in the next step.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after detecting an USB Reader stage.</extracomment>
        <translation>Ihr Kartenleser wurde erkannt und kann im nächsten Schritt zum Auslesen Ihres Ausweises verwendet werden.</translation>
    </message>
    <message>
        <source>Pairing successful</source>
        <extracomment>ALL_PLATFORMS Headline of the confirmation view after pairing a SaC stage.</extracomment>
        <translation>Kopplung erfolgreich</translation>
    </message>
    <message>
        <source>PIN is correct</source>
        <extracomment>ALL_PLATFORMS Headline of the confirmation view after pairing a SaC stage.</extracomment>
        <translation>PIN ist korrekt</translation>
    </message>
    <message>
        <source>Card reader detected</source>
        <extracomment>ALL_PLATFORMS Headline of the confirmation view after after detecting an USB Reader stage.</extracomment>
        <translation>Kartenleser erkannt</translation>
    </message>
    <message>
        <source>If you want to change your PIN, select the “Change PIN” button on the start page.</source>
        <extracomment>MOBILE Description of the confirmation view after the Change PIN stage.</extracomment>
        <translation>Wenn Sie Ihre PIN ändern möchten, wählen Sie auf der Startseite die Schaltfläche “PIN ändern”.</translation>
    </message>
    <message>
        <source>Card reader is ready</source>
        <extracomment>MOBILE Text that replaces _step x of y_ on the pair device confirm page</extracomment>
        <translation>Kartenleser ist bereit</translation>
    </message>
</context>
<context>
    <name>OnboardingConnectUsbReaderView</name>
    <message>
        <source>Connect an USB card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Verbinden Sie einen USB-Kartenleser</translation>
    </message>
    <message>
        <source>Set up card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kartenleser einrichten</translation>
    </message>
</context>
<context>
    <name>OnboardingFailedView</name>
    <message>
        <source>Requirements not met</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Anforderungen nicht erfüllt</translation>
    </message>
    <message>
        <source>You cannot use the %1</source>
        <extracomment>ALL_PLATFORMS %1 is replaced with the name &quot;AusweisApp&quot;.</extracomment>
        <translation>Sie können die %1 nicht nutzen</translation>
    </message>
    <message>
        <source>Unfortunately you do not meet all the requirements necessary to use the %1.</source>
        <extracomment>ALL_PLATFORMS %1 is replaced with the name &quot;AusweisApp&quot;.</extracomment>
        <translation>Leider erfüllen Sie nicht alle Anforderungen, um die %1 zu nutzen.</translation>
    </message>
    <message>
        <source>You may restart the setup anytime under %1Help%2.</source>
        <extracomment>ALL_PLATFORMS %1 and %2 are replaced with bold emphasis.</extracomment>
        <translation>Sie können die Einrichtung jederzeit unter %1Hilfe%2 erneut ausführen.</translation>
    </message>
    <message>
        <source>The setup will now be aborted - afterwards %1one prompt regarding personalized settings in the %3%2 will follow.</source>
        <extracomment>DESKTOP %1 and %2 are replaced with bold emphasis, %3 with the name &quot;AusweisApp&quot;.</extracomment>
        <translation>Die Einrichtung wird nun abgebrochen - es folgt nur noch %1eine Abfrage zu personalisierten Einstellungen%2 der %3.</translation>
    </message>
    <message>
        <source>The setup will now be aborted - afterwards %1two prompts regarding personalized settings in the %3%2 will follow.</source>
        <extracomment>DESKTOP %1 and %2 are replaced with bold emphasis, %3 with the name &quot;AusweisApp&quot;.</extracomment>
        <translation>Die Einrichtung wird nun abgebrochen - es folgen nur noch %1zwei Abfragen zu personalisierten Einstellungen%2 der %3.</translation>
    </message>
    <message>
        <source>The setup will now be aborted.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Einrichtung wird nun abgebrochen.</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <extracomment>MOBILE</extracomment>
        <translation>Einrichtung abbrechen</translation>
    </message>
</context>
<context>
    <name>OnboardingPairSacStartView</name>
    <message>
        <source>Set up smartphone as card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Smartphone als Kartenleser einrichten</translation>
    </message>
    <message>
        <source>Continue pairing</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Mit der Kopplung fortfahren</translation>
    </message>
    <message>
        <source>Install %1 on your smartphone</source>
        <extracomment>DESKTOP %1 is replaced with the name AusweisApp</extracomment>
        <translation>Installieren Sie die %1 auf Ihrem Smartphone</translation>
    </message>
    <message>
        <source>Scan the QR code, search your %1app store%2 for %3 or open the following link in your smartphone:</source>
        <extracomment>DESKTOP %1 and %2 are replaced with bold highlighting, %3 with the name AusweisApp</extracomment>
        <translation>Scannen Sie den QR-Code, suchen Sie in Ihrem %1App Store%2 nach der %3 oder öffnen Sie den folgenden Link auf Ihrem Smartphone:</translation>
    </message>
    <message>
        <source>Open %1 on your smartphone</source>
        <extracomment>DESKTOP %1 is replaced with the name AusweisApp</extracomment>
        <translation>Öffnen Sie die %1 auf Ihrem Smartphone</translation>
    </message>
    <message>
        <source>Select &quot;%1Set up %2%3&quot; there.</source>
        <extracomment>DESKTOP %1/%3 is replaced with bold highlighting, %2 with the name AusweisApp</extracomment>
        <translation>Wählen Sie dort &quot;%1%2 einrichten%3&quot;.</translation>
    </message>
    <message>
        <source>You will then be asked whether you want to set up a PC/Mac too. Select the option &quot;%1Yes, I want to use a PC/Mac too%2&quot; here. You now select &quot;%1Continue pairing%2&quot; on both devices.</source>
        <extracomment>DESKTOP %1/%2 is replaced with bold highlighting</extracomment>
        <translation>Sie werden dann gefragt, ob Sie auch einen PC/Mac einrichten möchten. Hier wählen Sie die Option &quot;%1Ja, ich möchte auch einen PC/Mac nutzen%2&quot;. Nun wählen Sie &quot;%1Mit der Kopplung fortfahren%2&quot; auf beiden Geräten.</translation>
    </message>
</context>
<context>
    <name>OnboardingPairSacView</name>
    <message>
        <source>Set up smartphone as card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Smartphone als Kartenleser einrichten</translation>
    </message>
    <message>
        <source>Click the arrow to show.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Klicken Sie zum Anzeigen auf den Pfeil.</translation>
    </message>
    <message>
        <source>Pairing instructions</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Anleitung zum Koppeln</translation>
    </message>
    <message>
        <source>Available devices</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Verfügbare Geräte</translation>
    </message>
    <message>
        <source>No device within reach</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kein Gerät in Reichweite</translation>
    </message>
    <message>
        <source>My smartphone does not show up</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Mein Smartphone wird nicht gefunden</translation>
    </message>
    <message>
        <source>Ensure that the %1%2%3 on the smartphone that you want to use as card reader is %1up to date%3.</source>
        <extracomment>DESKTOP %1/%3 are replaced with bold highlighting, %2 with the name AusweisApp</extracomment>
        <translation>Stellen Sie sicher, dass die %1%2%3 auf Ihrem Smartphone, welches Sie als Kartenleser verwenden möchten, %1auf dem neusten Stand ist%3.</translation>
    </message>
    <message>
        <source>Connect both devices to the %1same network%2, e.g. via WiFi.</source>
        <extracomment>DESKTOP %1/%2 are replaced with bold highlighting</extracomment>
        <translation>Verbinden Sie beide Geräte mit %1demselben Netzwerk%2, z.B. per WLAN.</translation>
    </message>
    <message>
        <source>Select the %1Card reader%2 entry in the bottom menu bar on your smartphone.</source>
        <extracomment>DESKTOP %1/%2 are replaced with bold highlighting</extracomment>
        <translation>Wählen Sie auf Ihrem Smartphone den Menüpunkt %1Kartenleser%2 in der unteren Menüleiste.</translation>
    </message>
    <message>
        <source>Then select %1Pair device%2 or, if a device was already paired, %1Pair new device%2.</source>
        <extracomment>DESKTOP %1/%2 are replaced with bold highlighting</extracomment>
        <translation>Wählen Sie dann %1Gerät koppeln%2 oder, falls bereits ein Gerät gekoppelt ist, %1Neues Gerät koppeln%2.</translation>
    </message>
    <message>
        <source>or</source>
        <extracomment>DESKTOP</extracomment>
        <translation>oder</translation>
    </message>
    <message>
        <source>Afterwards, a pairing code is displayed on your smartphone. You may then select the device in the %1 list to enter the pairing code.</source>
        <extracomment>DESKTOP %1 will be replaced with the Available Devices list name</extracomment>
        <translation>Anschließend wird ein Kopplungscode auf Ihrem Smartphone angezeigt. Dann können Sie das Gerät in der Liste %1 auswählen, um den Kopplungscode einzugeben.</translation>
    </message>
    <message>
        <source>Use device</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Verwenden</translation>
    </message>
</context>
<context>
    <name>OnboardingPinErrorView</name>
    <message>
        <source>Try again</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Erneut versuchen</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Einrichtung abbrechen</translation>
    </message>
</context>
<context>
    <name>OnboardingStartView</name>
    <message>
        <source>Setup</source>
        <extracomment>ALL_PLATFORMS Headline</extracomment>
        <translation>Einrichtung</translation>
    </message>
    <message>
        <source>Set up the eID function</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Richten Sie die Online-Ausweisfunktion ein</translation>
    </message>
    <message>
        <source>Securely identify yourself online with the %1</source>
        <extracomment>ALL_PLATFORMS %1 is replaced with &quot;AusweisApp&quot;</extracomment>
        <translation>Sicher online ausweisen mit der %1</translation>
    </message>
    <message>
        <source>The eID function allows you to handle administrative or business matters easily and securely online.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Online-Ausweisfunktion ermöglicht Ihnen, Behördengänge oder geschäftliche Angelegenheiten einfach und sicher online zu erledigen.</translation>
    </message>
    <message>
        <source>To identify yourself to a service provider using the %3, you need your %1ID card%2, your %1PIN%2, and an %1NFC-enabled smartphone%2 or a USB card reader.</source>
        <extracomment>DESKTOP The text between %1 and %2 will be emphasized, %3 is replaced with &quot;AusweisApp&quot;</extracomment>
        <translation>Um sich mittels %3 bei einem Diensteanbieter zu identifizieren, benötigen Sie Ihren %1Ausweis%2, Ihre %1PIN%2 und ein %1NFC-fähiges Smartphone%2 bzw. einen USB-Kartenleser.</translation>
    </message>
    <message>
        <source>To identify yourself to a service provider using the %3, you need your %1ID card%2, your %1PIN%2, and an %1NFC-enabled smartphone%2.</source>
        <extracomment>Mobile The text between %1 and %2 will be emphasized, %3 is replaced with &quot;AusweisApp&quot;</extracomment>
        <translation>Um sich mittels %3 bei einem Diensteanbieter zu identifizieren, benötigen Sie Ihren %1Ausweis%2, Ihre %1PIN%2 und ein %1NFC-fähiges Smartphone%2.</translation>
    </message>
    <message>
        <source>Start setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Einrichtung starten</translation>
    </message>
    <message>
        <source>The setup is recommended for first-time user. This way it can be ensured that all the technical requirements are met for your online authentication processes.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Einrichtung wird für Erstnutzer empfohlen. So kann sichergestellt werden, dass alle technischen Voraussetzungen erfüllt sind.</translation>
    </message>
    <message>
        <source>Are you using the app for the first time?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie nutzen die App zum ersten Mal?</translation>
    </message>
    <message>
        <source>Skip setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Einrichtung überspringen</translation>
    </message>
</context>
<context>
    <name>OnboardingView</name>
    <message>
        <source>Continue</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Weiter</translation>
    </message>
    <message>
        <source>Skip this step</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Diesen Schritt überspringen</translation>
    </message>
</context>
<context>
    <name>PairingCodeInfoView</name>
    <message>
        <source>Pairing Information</source>
        <extracomment>MOBILE</extracomment>
        <translation>Informationen zur Kopplung</translation>
    </message>
    <message>
        <source>Open %1 on your %2other device%3.</source>
        <extracomment>MOBILE Assistance text for pairing new devices. Step 1 of 5</extracomment>
        <translation>Öffnen Sie auf Ihrem %2anderen Gerät%3 die %1.</translation>
    </message>
    <message>
        <source>Make sure that both devices are on the %1same network%2 (e.g. WiFi).</source>
        <extracomment>MOBILE Assistance text for pairing new devices. Step 2 of 5. %1 and %2 are surrounding tags for bold font.</extracomment>
        <translation>Stellen Sie sicher, dass beide Geräte mit %1demselben Netzwerk%2, z.B. per WLAN verbunden sind.</translation>
    </message>
    <message>
        <source>On your other device, go to %1Settings%2 and then %1Smartphone as card reader%2 resp. %1Manage pairings%2.</source>
        <extracomment>MOBILE Assistance text for pairing new devices. Step 3 of 5. %1 and %2 are surrounding tags for bold font.</extracomment>
        <translation>Gehen Sie auf Ihrem anderen Gerät in die %1Einstellungen%2 und dann zu %1Smartphone als Kartenleser%2 bzw. %1Kopplungen verwalten%2.</translation>
    </message>
    <message>
        <source>Choose this smartphone in the list to pair it.</source>
        <extracomment>MOBILE Assistance text for pairing new devices. Step 4 of 5</extracomment>
        <translation>Wählen Sie in der angezeigten Liste dieses Smartphone aus, um es zu koppeln.</translation>
    </message>
    <message>
        <source>Enter the pairing code &quot;%1&quot;.</source>
        <extracomment>MOBILE Provide pairing code. Step 5 of 5</extracomment>
        <translation>Geben Sie den Kopplungscode &quot;%1&quot; ein.</translation>
    </message>
    <message>
        <source>Please follow these steps:</source>
        <extracomment>MOBILE</extracomment>
        <translation>Bitte folgen Sie diesen Schritten:</translation>
    </message>
</context>
<context>
    <name>PairingFailedView</name>
    <message>
        <source>Pairing to &quot;%1&quot; failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Kopplung mit &quot;%1&quot; ist fehlgeschlagen</translation>
    </message>
    <message>
        <source>Pairing failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Kopplung fehlgeschlagen</translation>
    </message>
</context>
<context>
    <name>PairingProcessInfo</name>
    <message>
        <source>Connect both devices to the %1same network%2, e.g. via WLAN.</source>
        <extracomment>ALL_PLATFORM Assistance text for pairing new devices. Step 2 of 5. %1 and %2 are for bold formatting.</extracomment>
        <translation>Verbinden Sie beide Geräte mit %1demselben Netzwerk%2, z.B. per WLAN.</translation>
    </message>
    <message>
        <source>Then select %1Pair (new) device%2. You may need to activate NFC first.</source>
        <extracomment>ALL_PLATFORM Assistance text for pairing new devices. Step 4 of 5. %1 and %2 are for bold formatting.</extracomment>
        <translation>Wählen Sie dann %1(Neues) Gerät koppeln%2. Ggf. müssen Sie hierzu vorher NFC aktivieren.</translation>
    </message>
    <message>
        <source>As soon as a pairing code is displayed on the smartphone, you can use it for pairing here.</source>
        <extracomment>ALL_PLATFORM Assistance text for pairing new devices. Step 5 of 5</extracomment>
        <translation>Sobald auf dem Smartphone ein Kopplungscode angezeigt wird, können Sie es hier zum Koppeln auswählen.</translation>
    </message>
    <message>
        <source>My smartphone does not show up</source>
        <extracomment>ALL_PLATFORM</extracomment>
        <translation>Mein Smartphone wird nicht gefunden</translation>
    </message>
    <message>
        <source>%3Install and open%4 the %3%1%4 (at least version %2) on your NFC-enabled smartphone that you want to use as a card reader.</source>
        <extracomment>ALL_PLATFORM Assistance text for pairing new devices. Step 1 of 5. %1 is a placeholder for the app name. %2 is a version placeholder. %3 and %4 are for bold formatting.</extracomment>
        <translation>%3Installieren und öffnen%4 Sie die %3%1%4 (Mindestversion %2) auf Ihrem NFC-fähigen Smartphone, welches Sie als Kartenleser verwenden möchten.</translation>
    </message>
    <message>
        <source>On the NFC-enabled smartphone, select the %1Card reader%2 menu item in the lower menu bar. You may need to skip the initial setup first.</source>
        <extracomment>ALL_PLATFORM Assistance text for pairing new devices. Step 3 of 5. %1 and %2 are for bold formatting.</extracomment>
        <translation>Wählen Sie auf dem NFC-fähigen Smartphone den Menüpunkt %1Kartenleser%2 in der unteren Menüleiste. Ggf. müssen Sie hierzu zuerst die Ersteinrichtung überspringen.</translation>
    </message>
</context>
<context>
    <name>PairingSuccessView</name>
    <message>
        <source>Pairing successful</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Kopplung erfolgreich</translation>
    </message>
    <message>
        <source>Your device &quot;%1&quot; was successfully paired</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ihr Gerät &quot;%1&quot; wurde erfolgreich gekoppelt</translation>
    </message>
    <message>
        <source>Your smartphone as card reader has been paired and can now be used to read your ID card when you start a process.</source>
        <extracomment>ALL_PLATFORMS The pairing with the SaC server was successful.</extracomment>
        <translation>Ihr Smartphone als Kartenleser wurde gekoppelt und kann nun zum Auslesen Ihres Ausweises verwendet werden, wenn Sie einen Vorgang starten.</translation>
    </message>
    <message>
        <source>In the future, you only need to select &quot;Activate card reader&quot; in the Card reader menu on the smartphone to use your smartphone as a card reader for this device.</source>
        <extracomment>ALL_PLATFORMS The pairing with the SaC server was successful.</extracomment>
        <translation>In Zukunft müssen Sie nur &quot;Kartenleser aktivieren&quot; im Kartenleser-Menü auf dem Smartphone auswählen, um Ihr Smartphone als Kartenleser für dieses Gerät zu verwenden.</translation>
    </message>
</context>
<context>
    <name>PinResetHints</name>
    <message>
        <source>How do I activate the eID function?</source>
        <extracomment>ALL_PLATFORMS Hint title to assist the user on how to set a new PIN</extracomment>
        <translation>Wie kann ich die Online-Ausweisfunktion aktivieren?</translation>
    </message>
    <message>
        <source>My PUK is used up. How do I set a new PIN?</source>
        <extracomment>ALL_PLATFORMS Hint title to assist the user on how to set a new PIN</extracomment>
        <translation>Meine PUK ist verbraucht. Wie kann ich eine neue PIN setzen?</translation>
    </message>
</context>
<context>
    <name>PinSelectionButtons</name>
    <message>
        <source>Set by yourself or received by the PIN Reset Service</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Selbst gewählt oder per PIN-Rücksetzdienst erhalten</translation>
    </message>
    <message>
        <source>6-digit PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>6-stellige PIN</translation>
    </message>
    <message>
        <source>Received by letter while ordering your ID card</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Bei Beantragung des Ausweises als Brief erhalten</translation>
    </message>
    <message>
        <source>5-digit Transport PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>5-stellige Transport-PIN</translation>
    </message>
    <message>
        <source>Lost, forgotten, or never received it</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Verloren, vergessen oder nie erhalten</translation>
    </message>
    <message>
        <source>No PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Keine PIN</translation>
    </message>
</context>
<context>
    <name>PkiSwitch</name>
    <message>
        <source>%1 more clicks to toggle the environment (prod/test) for integrated functions.</source>
        <extracomment>DESKTOP Used in notifications when the user taps the icon</extracomment>
        <translation>%1 weitere Klicks um die Umgebung (prod/test) für die integrierten Funktionen umzuschalten.</translation>
    </message>
    <message>
        <source>%1 more taps to toggle the environment (prod/test) for integrated functions.</source>
        <extracomment>MOBILE Used in notifications when the user taps the icon</extracomment>
        <translation>%1 weitere Tipps um die Umgebung (prod/test) für die integrierten Funktionen umzuschalten.</translation>
    </message>
    <message>
        <source>Testmode for the integrated functions activated.</source>
        <extracomment>ALL_PLATFORMS Used in notifications when the user taps the icon</extracomment>
        <translation>Testmodus für die integrierten Funktionen aktiviert.</translation>
    </message>
    <message>
        <source>Testmode for the integrated functions deactivated.</source>
        <extracomment>ALL_PLATFORMS Used in notifications when the user taps the icon</extracomment>
        <translation>Testmodus für die integrierten Funktionen deaktiviert.</translation>
    </message>
</context>
<context>
    <name>PreparationInfoView</name>
    <message>
        <source>Your ID card</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Ihr Ausweisdokument</translation>
    </message>
    <message>
        <source>A smartphone or a card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Ein Smartphone oder einen Kartenleser</translation>
    </message>
    <message>
        <source>Your PIN</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Ihre PIN</translation>
    </message>
    <message>
        <source>I can&apos;t recall my PIN</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Ich kenne meine PIN nicht</translation>
    </message>
    <message>
        <source>A NFC-enabled smartphone</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ein Smartphone mit NFC-Funktion</translation>
    </message>
    <message>
        <source>What is required to use %1?</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Was benötigen Sie für die Nutzung der %1?</translation>
    </message>
    <message>
        <source>The following documents are allowed:</source>
        <extracomment>MOBILE</extracomment>
        <translation>Folgende Dokumente sind möglich:</translation>
    </message>
    <message>
        <source>german ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Deutscher Personalausweis</translation>
    </message>
    <message>
        <source>electronic residence permit</source>
        <extracomment>MOBILE</extracomment>
        <translation>elektronischer Aufenthaltstitel</translation>
    </message>
    <message>
        <source>eID card for EU citizens</source>
        <extracomment>MOBILE</extracomment>
        <translation>eID-Karte für Unionsbürger</translation>
    </message>
    <message>
        <source>You may use your %2NFC-enabled smartphone%3. To do this you will have to install %1 on this smartphone. Alternatively you may use an %2USB card reader with your PC%3.</source>
        <extracomment>DESKTOP %1 = Application Name, %2 + %3 = Bold Tags</extracomment>
        <translation>Sie können Ihr %2Smartphone mit NFC Funktion%3 als Kartenleser nutzen. Dafür müssen Sie die %1 auch auf dem Smartphone installieren. Alternativ können Sie einen %2USB-Kartenleser an den PC anschließen%3.</translation>
    </message>
    <message>
        <source>You have received a one-time PIN, the %1Transport PIN%2, as a letter from your competent authority. You can replace this with a%1 6-digit card PIN%2 in the %3 or at the Citizens&apos; Registration Office. If you do not have a PIN or do not remember your card PIN, click here:</source>
        <extracomment>DESKTOP %1 + %2 = Bold Tags, %3 = AusweisApp</extracomment>
        <translation>Sie haben eine Einmal-PIN, die %1Transport-PIN%2, als Brief von Ihrer Ausweisbehörde erhalten. Diese ersetzen Sie in der %3 oder im Bürgeramt durch eine%1 6-stellige Karten-PIN%2. Wenn Ihnen keine PIN vorliegt oder Sie sich nicht an Ihre Karten-PIN erinnern, klicken Sie hier:</translation>
    </message>
    <message>
        <source>You have received a one-time PIN, the %1Transport PIN%2, as a letter from your competent authority. You can replace this with a%1 6-digit card PIN%2 in the %3 or at the Citizens&apos; Registration Office. If you do not have a PIN or do not remember your card PIN, tap here:</source>
        <extracomment>MOBILE %1 + %2 = Bold Tags, %3 = AusweisApp</extracomment>
        <translation>Sie haben eine Einmal-PIN, die %1Transport-PIN%2, als Brief von Ihrer Ausweisbehörde erhalten. Diese ersetzen Sie in der %3 oder im Bürgeramt durch eine%1 6-stellige Karten-PIN%2. Wenn Ihnen keine PIN vorliegt oder Sie sich nicht an Ihre Karten-PIN erinnern, tippen Sie hier:</translation>
    </message>
    <message>
        <source>The chip in your ID card is read using %1NFC%2. To do this, simply place the ID card on the %1back of the smartphone%2.</source>
        <extracomment>MOBILE %1 + %2 = Bold Tags</extracomment>
        <translation>Der Chip im Ausweis wird mit %1NFC%2 ausgelesen. Dafür legen Sie den Ausweis einfach an die %1Rückseite des Smartphones%2.</translation>
    </message>
    <message>
        <source>You could either use the %1German ID card%2, an %1electronic residence permit%2 or the %1eID card for EU citizens%2.</source>
        <extracomment>DESKTOP %1 + %2 = Bold Tags</extracomment>
        <translation>Sie können entweder den %1deutschen Personalausweis%2, den %1elektronischen Aufenthaltstitel%2 oder die %1eID-Karte für Unionsbürger%2 nutzen.</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Einrichtung abbrechen</translation>
    </message>
</context>
<context>
    <name>PrimaryDeviceDecisionView</name>
    <message>
        <source>Do you want to also use the %1 on a PC/Mac?</source>
        <extracomment>MOBILE</extracomment>
        <translation>Möchten Sie die %1 auch auf einem PC/Mac nutzen?</translation>
    </message>
    <message>
        <source>In order to also perform online authentications on a PC/Mac, you may pair your smartphone as a card reader. To do so, you need to have the %1 installed on both devices.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Um Ausweisvorgänge auch auf einem PC/Mac zu erledigen, können Sie dazu einfach Ihr Smartphone als Kartenleser koppeln. Dazu benötigen Sie die %1 auf beiden Geräten.</translation>
    </message>
    <message>
        <source>No, I just want to use this device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Nein, ich möchte nur dieses Gerät nutzen</translation>
    </message>
    <message>
        <source>Yes, I also want to use a PC/Mac</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ja, ich möchte auch einen PC/Mac nutzen</translation>
    </message>
</context>
<context>
    <name>PrivacyStatement</name>
    <message>
        <source>Your personal data is neither saved nor processed in any way. Please see our %1 for details on how your personal data is processed.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Es erfolgt keine Speicherung oder Weiterverarbeitung Ihrer persönlichen Daten. Näheres dazu erfahren Sie in unserer %1.</translation>
    </message>
    <message>
        <source>data privacy statement</source>
        <extracomment>ALL_PLATFORMS Text of the self authentication html link inside of a sentence</extracomment>
        <translation>Datenschutzerklärung</translation>
    </message>
</context>
<context>
    <name>ProviderInfoButton</name>
    <message>
        <source>Show more information about the service provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Zeige mehr Informationen über den Anbieter</translation>
    </message>
    <message>
        <source>Service Provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Anbieter</translation>
    </message>
</context>
<context>
    <name>ProxyCredentialsPopup</name>
    <message>
        <source>Sign in</source>
        <extracomment>DESKTOP Text of the button in the proxy credentials popup.
----------
DESKTOP Title of the proxy credentials popup.</extracomment>
        <translation>Anmelden</translation>
    </message>
    <message>
        <source>The proxy %1 requires username and password.</source>
        <extracomment>DESKTOP Text of the proxy credentials popup. An example for %1 is http://proxy.example.com:1337.</extracomment>
        <translation>Für den Proxy %1 sind ein Nutzername und ein Passwort erforderlich.</translation>
    </message>
    <message>
        <source>Proxy credential username</source>
        <extracomment>DESKTOP Accessible name.</extracomment>
        <translation>Benutzername für den Proxy</translation>
    </message>
    <message>
        <source>Username</source>
        <extracomment>DESKTOP Label of the textfield for the username.</extracomment>
        <translation>Nutzername</translation>
    </message>
    <message>
        <source>Proxy credential password</source>
        <extracomment>DESKTOP Accessible name.</extracomment>
        <translation>Passwort für den Proxy</translation>
    </message>
    <message>
        <source>Password</source>
        <extracomment>DESKTOP Label of the textfield for the password.</extracomment>
        <translation>Passwort</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>An error occurred in log handling: %1</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Es ist ein Fehler bei der Protokollverwaltung aufgetreten: %1</translation>
    </message>
    <message>
        <source>Please describe the error that occurs. Our support is available to you in German and English.</source>
        <translation>Bitte beschreiben Sie den auftretenden Fehler. Unser Support steht Ihnen in deutscher und englischer Sprache zur Verfügung.</translation>
    </message>
    <message>
        <source>You may want to attach the logfile which can be saved from the error dialog.</source>
        <translation>Bitte fügen Sie der E-Mail das Protokoll als Anhang zu.</translation>
    </message>
    <message>
        <source>Error code</source>
        <translation>Fehlernummer</translation>
    </message>
    <message>
        <source>Service URL</source>
        <translation>Service URL</translation>
    </message>
    <message>
        <source>Parameter of occurred error:</source>
        <translation>Parameter des aufgetretenen Fehlers:</translation>
    </message>
    <message>
        <source>Critical errors:</source>
        <translation>Kritische Fehler:</translation>
    </message>
</context>
<context>
    <name>ReaderConfigurationInfo</name>
    <message>
        <source>Unknown reader</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Unbekannter Kartenleser</translation>
    </message>
</context>
<context>
    <name>ReaderFoundConfirmation</name>
    <message>
        <source>Found new smartphone as card reader that is suitable for the ID card. The workflow may now be continued.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Es wurde ein neues Smartphone als Kartenleser gefunden, das für den Ausweis geeignet ist. Der Vorgang kann nun fortgesetzt werden.</translation>
    </message>
    <message>
        <source>Found new USB card reader that is suitable for the ID card. The workflow may now be continued.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Es wurde ein neuer USB-Kartenleser gefunden, der für den Ausweis geeignet ist. Der Vorgang kann nun fortgesetzt werden.</translation>
    </message>
    <message>
        <source>Continue</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Weiter</translation>
    </message>
</context>
<context>
    <name>RedirectView</name>
    <message>
        <source>Remove the ID card from the card reader</source>
        <extracomment>DESKTOP Hint to user that the ID card should be removed</extracomment>
        <translation>Entfernen Sie nun den Ausweis vom Kartenleser</translation>
    </message>
    <message>
        <source>You may now remove your ID card from the device.</source>
        <extracomment>MOBILE Hint to user that the ID card should be removed</extracomment>
        <translation>Sie können nun Ihren Ausweis vom Gerät entfernen.</translation>
    </message>
    <message>
        <source>If you have any questions or encounter any errors during the process, please contact the corresponding provider.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Bei Rückfragen oder auftretenden Fehlern zum Vorgang wenden Sie sich bitte an den jeweiligen Anbieter.</translation>
    </message>
    <message>
        <source>You will be automatically redirected to the provider in a few seconds. If you are not automatically redirected, click the &quot;%1&quot; button.</source>
        <extracomment>DESKTOP Redirect information when automatic redirect is enabled</extracomment>
        <translation>Die automatische Weiterleitung zum Anbieter erfolgt in wenigen Sekunden. Falls Sie nicht automatisch weitergeleitet werden, klicken Sie auf den Button &quot;%1&quot;.</translation>
    </message>
    <message>
        <source>Click the button to complete the authentication and return to the provider.</source>
        <extracomment>DESKTOP Redirect information when automatic redirect is disabled</extracomment>
        <translation>Klicken Sie auf den Button, um die Authentisierung abzuschließen und zum Anbieter zurückzukehren.</translation>
    </message>
    <message>
        <source>You will be automatically redirected to the provider in a few seconds. If you are not automatically redirected, tap the &quot;%1&quot; button.</source>
        <extracomment>MOBILE Redirect information when automatic redirect is enabled</extracomment>
        <translation>Die automatische Weiterleitung zum Anbieter erfolgt in wenigen Sekunden. Falls Sie nicht automatisch weitergeleitet werden, tippen Sie auf den Button &quot;%1&quot;.</translation>
    </message>
    <message>
        <source>Tap the button to complete the authentication and return to the provider.</source>
        <extracomment>MOBILE Redirect information when automatic redirect is disabled</extracomment>
        <translation>Tippen Sie auf den Button, um die Authentisierung abzuschließen und zum Anbieter zurückzukehren.</translation>
    </message>
    <message>
        <source>Authentication successful</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Authentisierung erfolgreich</translation>
    </message>
</context>
<context>
    <name>ReleaseNotes</name>
    <message>
        <source>Retry</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Erneut versuchen</translation>
    </message>
</context>
<context>
    <name>ReleaseNotesView</name>
    <message>
        <source>Release notes</source>
        <extracomment>MOBILE</extracomment>
        <translation>Release Notes</translation>
    </message>
</context>
<context>
    <name>RemoteReaderDelegate</name>
    <message>
        <source>Smartphone named &quot;%1&quot;.</source>
        <extracomment>DESKTOP Name of remote device. %1 is replaced with the name.</extracomment>
        <translation>Smartphone namens &quot;%1&quot;.</translation>
    </message>
    <message>
        <source>Status: &quot;%1&quot;.</source>
        <extracomment>DESKTOP Status of remote device. %1 is replaced with the status.</extracomment>
        <translation>Status: &quot;%1&quot;.</translation>
    </message>
    <message>
        <source>Pair</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Koppeln</translation>
    </message>
    <message>
        <source>Unpair</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Entkoppeln</translation>
    </message>
    <message>
        <source>%1 device &quot;%2&quot;</source>
        <extracomment>DESKTOP Text of pairing button, %1 will be Pair/Unpair and %2 is replaced with device name</extracomment>
        <translation>Gerät &quot;%2&quot; %1</translation>
    </message>
</context>
<context>
    <name>RemoteReaderView</name>
    <message>
        <source>Paired devices</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Gekoppelte Geräte</translation>
    </message>
    <message>
        <source>Add pairing</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kopplung hinzufügen</translation>
    </message>
    <message>
        <source>Last connected</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Zuletzt verbunden</translation>
    </message>
</context>
<context>
    <name>RemoteServiceController</name>
    <message>
        <source>Card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kartenleser</translation>
    </message>
</context>
<context>
    <name>RemoteServiceSettings</name>
    <message>
        <source>Manage pairings</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kopplungen verwalten</translation>
    </message>
    <message>
        <source>Paired devices</source>
        <extracomment>MOBILE</extracomment>
        <translation>Gekoppelte Geräte</translation>
    </message>
    <message>
        <source>Available</source>
        <extracomment>MOBILE</extracomment>
        <translation>Verfügbar</translation>
    </message>
    <message>
        <source>Last connected</source>
        <extracomment>MOBILE</extracomment>
        <translation>Zuletzt verbunden</translation>
    </message>
    <message>
        <source>Tap to remove device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Tippen, um das Gerät zu entfernen</translation>
    </message>
    <message>
        <source>Remove</source>
        <extracomment>MOBILE</extracomment>
        <translation>Entfernen</translation>
    </message>
    <message>
        <source>Do you want to remove the pairing of the device &quot;%1&quot;?</source>
        <extracomment>MOBILE</extracomment>
        <translation>Wollen Sie die Kopplung mit dem Gerät &quot;%1&quot; aufheben?</translation>
    </message>
    <message>
        <source>Remove pairing</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kopplung entfernen</translation>
    </message>
    <message>
        <source>Add pairing</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kopplung hinzufügen</translation>
    </message>
    <message>
        <source>Tap to pair</source>
        <extracomment>MOBILE</extracomment>
        <translation>Tippen zum Koppeln</translation>
    </message>
    <message>
        <source>Please connect your WiFi to use another smartphone as card reader (SaC).</source>
        <extracomment>MOBILE Wifi is not enabled and no new devices can be paired.</extracomment>
        <translation>Bitte verbinden Sie sich mit Ihrem WLAN, um ein anderes Smartphone als Kartenleser (SaK) benutzen zu können.</translation>
    </message>
    <message>
        <source>Enable WiFi</source>
        <extracomment>MOBILE</extracomment>
        <translation>WLAN aktivieren</translation>
    </message>
    <message>
        <source>Pairing code</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kopplungscode</translation>
    </message>
    <message>
        <source>Pairing the device...</source>
        <extracomment>MOBILE</extracomment>
        <translation>Das Gerät wird gekoppelt...</translation>
    </message>
    <message>
        <source>Tap to use device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Tippen, um das Gerät zu nutzen</translation>
    </message>
</context>
<context>
    <name>RemoteServiceView</name>
    <message>
        <source>WiFi not active</source>
        <extracomment>MOBILE</extracomment>
        <translation>WLAN nicht aktiv</translation>
    </message>
    <message>
        <source>NFC not active</source>
        <extracomment>MOBILE</extracomment>
        <translation>NFC nicht aktiv</translation>
    </message>
    <message>
        <source>Card access in progress</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kartenzugriff</translation>
    </message>
    <message>
        <source>Card reader ready</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kartenleser bereit</translation>
    </message>
    <message>
        <source>To do this, start a process on a paired device.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Starten Sie hierzu auf einem gekoppelten Gerät einen Vorgang.</translation>
    </message>
    <message>
        <source>Enable WiFi</source>
        <extracomment>MOBILE</extracomment>
        <translation>WLAN aktivieren</translation>
    </message>
    <message>
        <source>Enable NFC</source>
        <extracomment>MOBILE</extracomment>
        <translation>NFC aktivieren</translation>
    </message>
    <message>
        <source>Pair device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Gerät koppeln</translation>
    </message>
    <message>
        <source>Activate card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kartenleser aktivieren</translation>
    </message>
    <message>
        <source>You can use this Smartphone as a card reader for the %1 on other devices e.g. a laptop.

To do this you first have to pair that device with this smartphone.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Sie können dieses Smartphone als Kartenleser für die %1 auf einem anderen Gerät, z.B. Ihrem Laptop, nutzen.

Hierfür müssen Sie zuvor das entsprechende Gerät mit diesem Smartphone koppeln.</translation>
    </message>
    <message>
        <source>Card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kartenleser</translation>
    </message>
    <message>
        <source>Paired Devices</source>
        <extracomment>MOBILE</extracomment>
        <translation>Gekoppelte Geräte</translation>
    </message>
    <message>
        <source>Pair new device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Neues Gerät koppeln</translation>
    </message>
    <message>
        <source>Waiting for pairing</source>
        <extracomment>MOBILE</extracomment>
        <translation>Warte auf Kopplung</translation>
    </message>
    <message>
        <source>Start pairing of a new device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kopplung mit einem neuen Gerät starten</translation>
    </message>
    <message>
        <source>Where do I enter the pairing code?</source>
        <extracomment>MOBILE</extracomment>
        <translation>Wo gebe ich den Kopplungscode ein?</translation>
    </message>
    <message>
        <source>Cancel pairing</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kopplung abbrechen</translation>
    </message>
    <message>
        <source>Paired devices may use this Smartphone as a card reader now.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Gekoppelte Geräte können dieses Smartphone jetzt als Kartenleser nutzen.</translation>
    </message>
    <message>
        <source>NFC is not available</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kein NFC verfügbar</translation>
    </message>
    <message>
        <source>Stop card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kartenleser beenden</translation>
    </message>
    <message>
        <source>No device paired</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kein Gerät gekoppelt</translation>
    </message>
    <message>
        <source>Card reader not active</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kartenleser nicht aktiv</translation>
    </message>
    <message>
        <source>Use this smartphone as a card reader for a paired device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Nutzen Sie das Smartphone als Kartenleser für ein gekoppeltes Gerät</translation>
    </message>
    <message>
        <source>Activate the card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Aktivieren Sie den Kartenleser</translation>
    </message>
    <message>
        <source>Both devices have to be on the same network (e.g. WiFi).</source>
        <extracomment>MOBILE The remote service is active. Hint that both devices need to be connected to the same network.</extracomment>
        <translation>Beide Geräte müssen hierfür im selben Netzwerk (z.B. WLAN) sein.</translation>
    </message>
    <message>
        <source>Pairing code:</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kopplungscode:</translation>
    </message>
    <message>
        <source>This allows the paired devices to use this smartphone as a card reader.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Dann können die gekoppelten Geräte dieses Smartphone als Kartenleser nutzen.</translation>
    </message>
    <message>
        <source>Enter the pairing code</source>
        <extracomment>MOBILE</extracomment>
        <translation>Eingabe des Kopplungscodes</translation>
    </message>
    <message>
        <source>Enter the pairing code &quot;%1&quot; in the %2 on your other device. Both devices have to be on the same network (e.g. WiFi).</source>
        <extracomment>MOBILE %1 is replaced with the pairing code, %2 with the name &quot;AusweisApp&quot;</extracomment>
        <translation>Geben Sie den Kopplungscode &quot;%1&quot; in der %2 auf Ihrem anderen Gerät ein. Beide Geräte müssen hierfür im selben Netzwerk (z.B. WLAN) sein.</translation>
    </message>
    <message>
        <source>Pairing progress</source>
        <extracomment>MOBILE Name of an progress indicator during the pairing process read by screen readers</extracomment>
        <translation>Kopplungsfortschritt</translation>
    </message>
</context>
<context>
    <name>RemoteWorkflow</name>
    <message>
        <source>Enable WiFi</source>
        <extracomment>MOBILE</extracomment>
        <translation>WLAN aktivieren</translation>
    </message>
    <message>
        <source>To use the remote service WiFi has to be activated. Please activate WiFi in your device settings.</source>
        <extracomment>MOBILE The WiFi module needs to be enabled in the system settings to use the remote service.</extracomment>
        <translation>Um den Fernzugriff zu nutzen, muss WLAN aktiviert werden. Bitte aktivieren Sie WLAN in Ihren Einstellungen.</translation>
    </message>
    <message>
        <source>Wifi disabled</source>
        <extracomment>MOBILE</extracomment>
        <translation>WLAN ist deaktiviert</translation>
    </message>
    <message>
        <source>Determine card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ermittle Ausweis</translation>
    </message>
    <message>
        <source>The connected smartphone as card reader (SaC) unfortunately does not meet the technical requirements (Extended Length not supported).</source>
        <extracomment>MOBILE The device does not support Extended Length and can not be used as card reader.</extracomment>
        <translation>Das verbundene Smartphone als Kartenleser erfüllt leider nicht die technischen Voraussetzungen (Extended Length wird nicht unterstützt).</translation>
    </message>
    <message>
        <source>Connected to %1. Please follow the instructions on the connected smartphone.</source>
        <extracomment>MOBILE The connection to the smartphone was established, the ID card may be inserted.</extracomment>
        <translation>Verbunden mit %1. Bitte folgen Sie den Anweisungen auf dem verbundenen Smartphone.</translation>
    </message>
    <message>
        <source>Manage pairings</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kopplungen verwalten</translation>
    </message>
    <message>
        <source>No smartphone as card reader connected</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kein Smartphone als Kartenleser verbunden</translation>
    </message>
    <message>
        <source>Allow a connection on a paired smartphone or pair a new smartphone.</source>
        <extracomment>MOBILE No paired and reachable device was found, hint that the remote device needs to be actually started for this feature.</extracomment>
        <translation>Erlauben Sie die Verbindung auf einem bereits gekoppelten Smartphone oder koppeln Sie ein neues Smartphone.</translation>
    </message>
</context>
<context>
    <name>RequestedRights</name>
    <message>
        <source>Write access (update)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Schreibzugriff (Aktualisierung)</translation>
    </message>
    <message>
        <source>Read access (optional)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Lesezugriff (optional)</translation>
    </message>
    <message>
        <source>Read access</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Lesezugriff</translation>
    </message>
</context>
<context>
    <name>ResultErrorView</name>
    <message>
        <source>Show Details</source>
        <extracomment>MOBILE</extracomment>
        <translation>Details anzeigen</translation>
    </message>
</context>
<context>
    <name>ResultView</name>
    <message>
        <source>Send email</source>
        <extracomment>DESKTOP</extracomment>
        <translation>E-Mail senden</translation>
    </message>
    <message>
        <source>Save log</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Protokoll speichern</translation>
    </message>
    <message>
        <source>Logfiles (*.log)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Protokolldateien (*.log)</translation>
    </message>
    <message>
        <source>See details</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Details anzeigen</translation>
    </message>
    <message>
        <source>OK</source>
        <extracomment>MOBILE</extracomment>
        <translation>OK</translation>
    </message>
    <message>
        <source>Hint</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Tipp</translation>
    </message>
</context>
<context>
    <name>ScreenshotPreventer</name>
    <message>
        <source>Attention: Screen Recording in progress</source>
        <extracomment>MOBILE</extracomment>
        <translation>Achtung: Bildschirmaufnahme läuft</translation>
    </message>
    <message>
        <source>Attention: Screenshot taken</source>
        <extracomment>IOS</extracomment>
        <translation>Achtung: Bildschirmfoto aufgenommen</translation>
    </message>
</context>
<context>
    <name>SecurityAndPrivacySettings</name>
    <message>
        <source>Numeric keypad</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Ziffernblock</translation>
    </message>
    <message>
        <source>Shuffle keys</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Tasten zufällig anordnen</translation>
    </message>
    <message>
        <source>Hide key animations</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Tasten-Animationen ausblenden</translation>
    </message>
    <message>
        <source>Makes it difficult for outsiders to detect PIN entry</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Erschwert eine Erkennung der PIN-Eingabe durch Außenstehende</translation>
    </message>
</context>
<context>
    <name>SelfAuthStartView</name>
    <message>
        <source>See my personal data</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Meine Daten einsehen</translation>
    </message>
    <message>
        <source>This way you can make sure that everything is working and that the data stored on your eID card is up to date before you identify yourself to a provider.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>So können Sie sicherstellen, dass alles funktioniert und die auf Ihrem Ausweis gespeicherten Daten aktuell sind, bevor Sie sich bei einem Anbieter ausweisen.</translation>
    </message>
    <message>
        <source>To do this, you go through an authentication process at Governikus GmbH &amp; Co. KG, the manufacturer of the %1.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Dafür durchlaufen Sie einen Authentisierungsprozess bei Governikus GmbH &amp; Co. KG, dem Hersteller der %1.</translation>
    </message>
    <message>
        <source>Use eID function</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Online-Ausweis nutzen</translation>
    </message>
    <message>
        <source>Have a look which data is stored on your eID card</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Schauen Sie sich jetzt an, welche Daten auf Ihrem Ausweis gespeichert sind</translation>
    </message>
    <message>
        <source>Identify</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausweisen</translation>
    </message>
</context>
<context>
    <name>SelfAuthenticationData</name>
    <message>
        <source>Read data</source>
        <extracomment>DESKTOP Title of the self authentication result data view</extracomment>
        <translation>Ausgelesene Daten</translation>
    </message>
    <message>
        <source>Identify</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ausweisen</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>MOBILE</extracomment>
        <translation>Zur Startseite</translation>
    </message>
</context>
<context>
    <name>SelfAuthenticationHeader</name>
    <message>
        <source>Successfully read data.</source>
        <extracomment>ALL_PLATFORMS Status message that the self authentication successfully completed (1/2).</extracomment>
        <translation>Lesevorgang erfolgreich.</translation>
    </message>
    <message>
        <source>You may now remove your ID card from the device.</source>
        <extracomment>ALL_PLATFORMS Status message that the self authentication successfully completed (2/2).</extracomment>
        <translation>Sie können nun Ihren Ausweis vom Gerät entfernen.</translation>
    </message>
</context>
<context>
    <name>SettingsView</name>
    <message>
        <source>Settings</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <source>General</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Allgemein</translation>
    </message>
    <message>
        <source>Smartphone as card reader</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Smartphone als Kartenleser</translation>
    </message>
    <message>
        <source>USB card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>USB-Kartenleser</translation>
    </message>
    <message>
        <source>Security and privacy</source>
        <extracomment>DESKTOP
----------
ANDROID</extracomment>
        <translation>Sicherheit und Datenschutz</translation>
    </message>
    <message>
        <source>Debug options</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Debugoptionen</translation>
    </message>
    <message>
        <source>Developer options</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Entwickleroptionen</translation>
    </message>
    <message>
        <source>Change language</source>
        <extracomment>MOBILE</extracomment>
        <translation>Sprache wechseln</translation>
    </message>
    <message>
        <source>Device name</source>
        <extracomment>MOBILE</extracomment>
        <translation>Gerätename</translation>
    </message>
    <message>
        <source>Enter PIN on this device</source>
        <extracomment>MOBILE</extracomment>
        <translation>PIN-Eingabe auf diesem Gerät</translation>
    </message>
    <message>
        <source>Shuffle keys</source>
        <extracomment>MOBILE</extracomment>
        <translation>Tasten zufällig anordnen</translation>
    </message>
    <message>
        <source>Hide key animations</source>
        <extracomment>MOBILE</extracomment>
        <translation>Tasten-Animationen ausblenden</translation>
    </message>
    <message>
        <source>Skip rights page</source>
        <extracomment>MOBILE</extracomment>
        <translation>Anzeige der Berechtigungen überspringen</translation>
    </message>
    <message>
        <source>Testmode for the self-authentication</source>
        <extracomment>MOBILE</extracomment>
        <translation>Testmodus für die Selbstauskunft</translation>
    </message>
    <message>
        <source>Internal card simulator</source>
        <extracomment>MOBILE</extracomment>
        <translation>Interner Kartensimulator</translation>
    </message>
    <message>
        <source>Developer mode</source>
        <extracomment>MOBILE</extracomment>
        <translation>Entwicklermodus</translation>
    </message>
    <message>
        <source>Use a more tolerant mode</source>
        <extracomment>MOBILE</extracomment>
        <translation>Benutze einen toleranten Modus</translation>
    </message>
    <message>
        <source>Create dummy entries</source>
        <extracomment>MOBILE</extracomment>
        <translation>Erstelle Scheindaten</translation>
    </message>
    <message>
        <source>New Logfile</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Neue Protokolldatei</translation>
    </message>
    <message>
        <source>15 days old Logfile</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>15 Tage alte Protokolldatei</translation>
    </message>
    <message>
        <source>Show requested rights on this device as well</source>
        <extracomment>MOBILE</extracomment>
        <translation>Angefragte Berechtigung auch auf diesem Gerät anzeigen</translation>
    </message>
    <message>
        <source>Manage pairings</source>
        <extracomment>MOBILE</extracomment>
        <translation>Kopplungen verwalten</translation>
    </message>
    <message>
        <source>Show Transport PIN reminder, store feedback and close reminder dialogs.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Zeige Transport-PIN Hinweis, Store Feedback und Fenster-Schließ Hinweis Dialoge.</translation>
    </message>
    <message>
        <source>Reset hideable dialogs</source>
        <extracomment>MOBILE</extracomment>
        <translation>Setze versteckbare Dialoge zurück</translation>
    </message>
    <message>
        <source>Toggling will restart the %1</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ein Umschalten startet die %1 neu</translation>
    </message>
    <message>
        <source>Use system font</source>
        <extracomment>MOBILE</extracomment>
        <translation>Systemschriftart verwenden</translation>
    </message>
    <message>
        <source>Appearance</source>
        <extracomment>MOBILE</extracomment>
        <translation>Erscheinungsbild</translation>
    </message>
    <message>
        <source>Add and remove devices</source>
        <extracomment>MOBILE</extracomment>
        <translation>Geräte hinzufügen oder entfernen</translation>
    </message>
    <message>
        <source>On-site reading</source>
        <extracomment>MOBILE</extracomment>
        <translation>Vor-Ort-Auslesen</translation>
    </message>
    <message>
        <source>Support CAN allowed mode for on-site reading</source>
        <extracomment>MOBILE</extracomment>
        <translation>CAN-Allowed Modus für Vor-Ort-Auslesen unterstützen</translation>
    </message>
    <message>
        <source>Allow test sample card usage</source>
        <extracomment>MOBILE</extracomment>
        <translation>Testmusterkarten erlauben</translation>
    </message>
    <message>
        <source>Simulate a test sample card in authentications</source>
        <extracomment>MOBILE</extracomment>
        <translation>Bei der Authentisierung eine Testmusterkarte simulieren</translation>
    </message>
    <message>
        <source>Makes it difficult for outsiders to detect PIN entry</source>
        <extracomment>MOBILE</extracomment>
        <translation>Erschwert eine Erkennung der PIN-Eingabe durch Außenstehende</translation>
    </message>
    <message>
        <source>Use images instead of animations</source>
        <extracomment>MOBILE</extracomment>
        <translation>Bilder statt Animationen verwenden</translation>
    </message>
    <message>
        <source>Accessibility</source>
        <extracomment>MOBILE</extracomment>
        <translation>Barrierefreiheit</translation>
    </message>
    <message>
        <source>Hide key animations when entering PIN</source>
        <extracomment>MOBILE</extracomment>
        <translation>Tastenanimationen bei PIN-Eingabe ausblenden</translation>
    </message>
    <message>
        <source>After identification, you will only be redirected back to the provider after confirmation. Otherwise, you will be redirected automatically after a few seconds.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Nach dem Ausweisen erfolgt die Weiterleitung zurück zum Anbieter erst nach Bestätigung. Ansonsten erfolgt die Weiterleitung automatisch nach wenigen Sekunden.</translation>
    </message>
    <message>
        <source>Manual redirection back to the provider</source>
        <extracomment>MOBILE</extracomment>
        <translation>Manuelle Weiterleitung zurück zum Anbieter</translation>
    </message>
    <message>
        <source>Extend Transport PIN</source>
        <extracomment>MOBILE</extracomment>
        <translation>Erweitere die Transport-PIN</translation>
    </message>
    <message>
        <source>Disable</source>
        <extracomment>MOBILE</extracomment>
        <translation>Deaktivieren</translation>
    </message>
    <message>
        <source>Later</source>
        <extracomment>MOBILE</extracomment>
        <translation>Später</translation>
    </message>
    <message>
        <source>Restart</source>
        <extracomment>IOS</extracomment>
        <translation>Neustarten</translation>
    </message>
    <message>
        <source>Restart now</source>
        <extracomment>ANDROID</extracomment>
        <translation>Jetzt neustarten</translation>
    </message>
    <message>
        <source>The font change applies only after restarting the application.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Die Änderung der Schriftart wird erst nach einem Neustart der Anwendung aktiv.</translation>
    </message>
    <message>
        <source>Restart required</source>
        <extracomment>MOBILE</extracomment>
        <translation>Systemfont</translation>
    </message>
    <message>
        <source>Prevent screenshots</source>
        <extracomment>ANDROID</extracomment>
        <translation>Screenshots verhindern</translation>
    </message>
    <message>
        <source>Screenshots are prevented while your ID card data is displayed and you enter your PIN/CAN/PUK. If your screen is recorded, you will be notified of the potential collection of sensitive data.</source>
        <extracomment>ANDROID</extracomment>
        <translation>Während der Anzeige Ihrer Ausweisdaten und der Eingabe Ihrer PIN/CAN/PUK werden Screenshots verhindert. Im Falle einer Videoaufnahme des Bildschirms werden Sie über die mögliche Erfassung sensibler Daten benachrichtigt.</translation>
    </message>
    <message>
        <source>In the event of a screenshot or video recording of the screen, you will be notified about the possible collection of sensitive data.</source>
        <extracomment>IOS</extracomment>
        <translation>Im Falle eines Screenshots oder einer Videoaufnahme des Bildschirms werden Sie über die mögliche Erfassung sensibler Daten benachrichtigt.</translation>
    </message>
    <message>
        <source>Detect screen recording</source>
        <extracomment>IOS</extracomment>
        <translation>Bildschirmaufnahme erkennen</translation>
    </message>
</context>
<context>
    <name>SetupDesktopUsage</name>
    <message>
        <source>Set up %1 on the PC/Mac</source>
        <extracomment>MOBILE</extracomment>
        <translation>%1 auf dem PC/Mac einrichten</translation>
    </message>
    <message>
        <source>Install %1</source>
        <extracomment>MOBILE</extracomment>
        <translation>%1 installieren</translation>
    </message>
    <message>
        <source>In order to install the %1, open the following website on your PC/Mac:</source>
        <extracomment>MOBILE %1 is replaced with the name &quot;AusweisApp&quot;</extracomment>
        <translation>Um die %1 zu installieren, öffnen Sie auf Ihrem PC/Mac die Webseite:</translation>
    </message>
    <message>
        <source>Open %1</source>
        <extracomment>MOBILE %1 is replaced with the name &quot;AusweisApp&quot;</extracomment>
        <translation>%1 öffnen</translation>
    </message>
    <message>
        <source>Open the %1 on your PC/Mac.</source>
        <extracomment>MOBILE %1 is replaced with the name &quot;AusweisApp&quot;</extracomment>
        <translation>Öffnen Sie die %1 auf Ihrem PC/Mac.</translation>
    </message>
    <message>
        <source>Follow the instruction on your second device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Befolgen der Anweisungen auf Ihrem zweiten Gerät</translation>
    </message>
    <message>
        <source>Now follow the instruction for the setup on your PC/Mac. If the setup does not start automatically, you may find it under %1Help%2.</source>
        <extracomment>MOBILE The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Folgen Sie nun den Anweisungen auf Ihrem PC/Mac. Sollte sich die Einrichtung nicht automatisch öffnen, können Sie diese unter %1Hilfe%2 finden.</translation>
    </message>
    <message>
        <source>Continue with pairing</source>
        <extracomment>MOBILE</extracomment>
        <translation>Mit der Kopplung fortfahren</translation>
    </message>
</context>
<context>
    <name>SimulatorWorkflow</name>
    <message>
        <source>Simulator</source>
        <extracomment>MOBILE</extracomment>
        <translation>Simulator</translation>
    </message>
    <message>
        <source>Continue</source>
        <extracomment>MOBILE</extracomment>
        <translation>Weiter</translation>
    </message>
    <message>
        <source>Switch to:</source>
        <extracomment>MOBILE</extracomment>
        <translation>Wechseln zu:</translation>
    </message>
    <message>
        <source>NFC</source>
        <extracomment>MOBILE</extracomment>
        <translation>NFC</translation>
    </message>
    <message>
        <source>WiFi</source>
        <extracomment>MOBILE</extracomment>
        <translation>WLAN</translation>
    </message>
</context>
<context>
    <name>SkipOnboardingConfirmation</name>
    <message>
        <source>Only skip this step, if you already set up a 6-digit ID card PIN and a card reader.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Überspringen Sie diesen Schritt nur, wenn Sie bereits eine 6-stellige Karten-PIN und einen Kartenleser eingerichtet haben.</translation>
    </message>
    <message>
        <source>Only skip this step, if you already used your ID card online and know your 6-digit ID card PIN.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Überspringen Sie diesen Schritt nur, wenn Sie Ihren Ausweis zuvor online verwendet haben und Ihre 6-stellige Karten-PIN kennen.</translation>
    </message>
    <message>
        <source>If you want to use the eID function on your PC, install %1 there and follow the initial setup.</source>
        <extracomment>MOBILE %1 will be replaced with the name of the application</extracomment>
        <translation>Falls Sie die Online-Ausweisfunktion auf dem PC nutzen möchten, installieren Sie dort die %1 und folgen Sie der Ersteinrichtung.</translation>
    </message>
</context>
<context>
    <name>StoreFeedbackPopup</name>
    <message>
        <source>Are you satisfied with %1?</source>
        <extracomment>ANDROID Header of the app rating popup.</extracomment>
        <translation>Zufrieden mit der %1?</translation>
    </message>
    <message>
        <source>We would be very grateful if you could leave a rating on the Google Play Store!</source>
        <extracomment>ANDROID Content of the app rating popup.</extracomment>
        <translation>Wir würden uns sehr über eine Bewertung im Google Play Store freuen!</translation>
    </message>
    <message>
        <source>Do not ask again</source>
        <extracomment>ANDROID</extracomment>
        <translation>Ignorieren</translation>
    </message>
    <message>
        <source>Rate app</source>
        <extracomment>ANDROID</extracomment>
        <translation>App bewerten</translation>
    </message>
</context>
<context>
    <name>SuggestionData</name>
    <message>
        <source>Continue</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Weiter</translation>
    </message>
</context>
<context>
    <name>TabbedPane</name>
    <message>
        <source>Content of tab &quot;%1&quot;</source>
        <extracomment>DESKTOP %1 will be replaced with the title of the tab</extracomment>
        <translation>Inhalt der Registerkarte &quot;%1&quot;</translation>
    </message>
    <message>
        <source>Sidebar</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Seitenleiste</translation>
    </message>
</context>
<context>
    <name>TabbedPaneDelegate</name>
    <message>
        <source>%1 of %2</source>
        <extracomment>DESKTOP %1 is the current selected Page of %2 Pages</extracomment>
        <translation>%1 von %2</translation>
    </message>
    <message>
        <source>Tab selected</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Registerkarte ausgewählt</translation>
    </message>
    <message>
        <source>Tab</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Registerkarte</translation>
    </message>
</context>
<context>
    <name>TabbedReaderView</name>
    <message>
        <source>Card readers</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kartenleser</translation>
    </message>
    <message>
        <source>Smartphone as card reader</source>
        <translation>Smartphone als Kartenleser</translation>
    </message>
    <message>
        <source>USB card reader</source>
        <translation>USB-Kartenleser</translation>
    </message>
</context>
<context>
    <name>TechnologySwitch</name>
    <message>
        <source>Readout mode</source>
        <extracomment>MOBILE</extracomment>
        <translation>Auslese-Modus</translation>
    </message>
    <message>
        <source>by NFC</source>
        <extracomment>MOBILE</extracomment>
        <translation>per NFC</translation>
    </message>
    <message>
        <source>by smartphone as card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>per Smartphone als Kartenleser</translation>
    </message>
    <message>
        <source>by internal card simulator</source>
        <extracomment>MOBILE</extracomment>
        <translation>per internem Kartensimulator</translation>
    </message>
</context>
<context>
    <name>TitleBar</name>
    <message>
        <source>Start page</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Startseite</translation>
    </message>
    <message>
        <source>The update can only be performed after the current operation has been completed.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Das Update kann erst nach dem Abschluss des Vorgangs durchgeführt werden.</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <source>Open settings view of %1</source>
        <translation>Öffne die %1 Einstellungen</translation>
    </message>
    <message>
        <source>Notifications</source>
        <translation>Benachrichtigungen</translation>
    </message>
    <message>
        <source>Show in-app notifications of %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Internen Benachrichtigungsdialog der %1 einblenden</translation>
    </message>
    <message>
        <source>Hide in-app notifications of %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Internen Benachrichtigungsdialog der %1 ausblenden</translation>
    </message>
    <message>
        <source>Update available (version %1)</source>
        <extracomment>DESKTOP %1 will be replaced with an application version number</extracomment>
        <translation>Aktualisierung verfügbar (Version %1)</translation>
    </message>
</context>
<context>
    <name>TitleBarNavigation</name>
    <message>
        <source>Cancel</source>
        <extracomment>MOBILE</extracomment>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <source>Back</source>
        <extracomment>MOBILE</extracomment>
        <translation>Zurück</translation>
    </message>
    <message>
        <source>Close</source>
        <extracomment>MOBILE</extracomment>
        <translation>Schließen</translation>
    </message>
</context>
<context>
    <name>TransactionInfoButton</name>
    <message>
        <source>Show more information about the transaction.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Zeige mehr Informationen zur Transaktion</translation>
    </message>
    <message>
        <source>Information about the transaction.</source>
        <translation>Informationen zur Transaktion</translation>
    </message>
    <message>
        <source>Transactional information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Informationen zum Vorgang</translation>
    </message>
    <message>
        <source>The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der oben genannte Anbieter benötigt keine Daten von Ihrem Ausweis. Dieser möchte lediglich sicherstellen, dass Sie im Besitz eines gültigen Ausweises sind.</translation>
    </message>
</context>
<context>
    <name>TransactionInfoView</name>
    <message>
        <source>Transactional information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Informationen zum Vorgang</translation>
    </message>
</context>
<context>
    <name>TrayIconView</name>
    <message>
        <source>Yes, attach app to menu bar</source>
        <extracomment>MACOS Button to decide to attach app to the menu bar on macOS</extracomment>
        <translation>Ja, App in der Menüleiste anheften</translation>
    </message>
    <message>
        <source>Yes, attach app to tray</source>
        <extracomment>WINDOWS Button to decide to attach app to the tray on Windows</extracomment>
        <translation>Ja, App im Infobereich anheften</translation>
    </message>
    <message>
        <source>The %1 continues to run in the background after the application window is closed, so that it can be automatically opened on an authentication.</source>
        <extracomment>DESKTOP Information text why attaching the app to the tray/menu bar is advisable 1/3</extracomment>
        <translation>Die %1 läuft nach Schließen des Anwendungsfensters im Hintergrund weiter, sodass sie bei einer Authentisierung automatisch aufgerufen wird.</translation>
    </message>
    <message>
        <source>An authentication cannot be carried out if the %1 is not running. Then the app has to be started manually before another authentication.</source>
        <extracomment>DESKTOP Information text why attaching the app to the tray/menu bar is advisable 2/3</extracomment>
        <translation>Falls die %1 nicht geöffnet ist, kann keine Authentisierung durchgeführt werden. Sie muss dann vor einer Authentisierung erneut manuell geöffnet werden.</translation>
    </message>
    <message>
        <source>Attaching the app to the menu bar is therefore recommended.</source>
        <extracomment>MACOS Information text why attaching the app to the tray/menu bar is advisable 3/3</extracomment>
        <translation>Daher wird es empfohlen, sie im Benachrichtigungsbereich der Menüleiste anzuheften.</translation>
    </message>
    <message>
        <source>Attaching the app to the tray is therefore recommended.</source>
        <extracomment>WINDOWS Information text why attaching the app to the tray/menu bar is advisable 3/3</extracomment>
        <translation>Daher wird es empfohlen, sie im Benachrichtigungsbereich der Windows-Taskleiste anzuheften.</translation>
    </message>
    <message>
        <source>No, don&apos;t attach app to menu bar</source>
        <extracomment>MACOS Button to decide not to attach app to the menu bar on macOS</extracomment>
        <translation>Nein, App nicht im Benachrichtigungsbereich anheften</translation>
    </message>
    <message>
        <source>No, don&apos;t attach app to tray</source>
        <extracomment>WINDOWS Button to decide not to attach app to the tray on Windows</extracomment>
        <translation>Nein, App nicht im Benachrichtigungsbereich anheften</translation>
    </message>
    <message>
        <source>Do you want to attach the %1 to the menu bar?</source>
        <extracomment>MACOS Question if the App shall be attached to the menu bar on macOS</extracomment>
        <translation>Möchten Sie die %1 im Benachrichtigungsbereich anheften?</translation>
    </message>
    <message>
        <source>Do you want to attach the %1 to the tray?</source>
        <extracomment>WINDOWS Question if the App shall be attached to the tray on windows</extracomment>
        <translation>Möchten Sie die %1 im Benachrichtigungsbereich anheften?</translation>
    </message>
</context>
<context>
    <name>UpdateOptions</name>
    <message>
        <source>When you restart %1 and every 24 hours, it automatically checks for updates. Updates are not performed automatically. If this option is disabled, you have to manually check for updates in the settings.</source>
        <extracomment>DESKTOP %1 is replaced with the application name</extracomment>
        <translation>Bei Neustart der %1 und alle 24 Stunden wird automatisch nach Updates gesucht. Eine automatische Ausführung der Aktualisierung findet nicht statt. Ist diese Option ausgeschaltet, müssen mögliche Aktualisierungen manuell in den Einstellungen gesucht werden.</translation>
    </message>
    <message>
        <source>Automatically check for software updates at program start (recommended)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Automatisch nach Software-Aktualisierungen suchen (empfohlen)</translation>
    </message>
    <message>
        <source>Show update</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Aktualisierung anzeigen</translation>
    </message>
    <message>
        <source>Start manual search for software update</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Manuelle Suche nach Software-Aktualisierung starten</translation>
    </message>
    <message>
        <source>Searching for software updates...</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Die Suche nach Software-Aktualisierungen wird durchgeführt...</translation>
    </message>
    <message>
        <source>Abort search</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Suche abbrechen</translation>
    </message>
</context>
<context>
    <name>UpdateView</name>
    <message>
        <source>Application update</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Anwendungsaktualisierung</translation>
    </message>
    <message>
        <source>An update for the outdated installed version (%1) is available for download.</source>
        <extracomment>DESKTOP %1 is replaced with the current version number</extracomment>
        <translation>Eine Aktualisierung der veralteten installierten Version (%1) ist zum Download verfügbar.</translation>
    </message>
    <message>
        <source>Update information is being checked...</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Update-Informationen werden geprüft...</translation>
    </message>
    <message>
        <source>Update failed</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Update fehlgeschlagen</translation>
    </message>
    <message>
        <source>Application update could not be performed</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Anwendungsaktualisierung konnte nicht durchgeführt werden</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Zurück zur Startseite</translation>
    </message>
    <message>
        <source>Retry</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Erneut versuchen</translation>
    </message>
    <message>
        <source>Show update</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Aktualisierung anzeigen</translation>
    </message>
    <message>
        <source>A newer update (version %1) has been found and is available for download.</source>
        <extracomment>DESKTOP %1 will be replaced with the update version number</extracomment>
        <translation>Eine neuere Aktualisierung (Version %1) wurde gefunden und ist zum Download verfügbar.</translation>
    </message>
    <message>
        <source>Newer version %1 available</source>
        <extracomment>DESKTOP %1 will be replaced with the update version number</extracomment>
        <translation>Neuere Version %1 verfügbar</translation>
    </message>
    <message>
        <source>Warning - Your operating system is no longer supported</source>
        <extracomment>DESKTOP Header of the popup that is shown when the requested update is not compatible with the OS.</extracomment>
        <translation>Warnung - Ihr Betriebssystem wird nicht mehr unterstützt</translation>
    </message>
    <message>
        <source>Troubleshooting and user support for the %1 is no longer available for your current operating system. Version %2 has not been tested with your current operating system, use is at your own risk.</source>
        <extracomment>DESKTOP Text of the popup that is shown when the requested update is not compatible with the OS.</extracomment>
        <translation>Für das verwendete Betriebssystem ist keine Fehlerbehebung und kein Nutzersupport für die %1 mehr erhältlich. Die Version %2 wurde mit dem verwendeten Betriebssystem nicht getestet, eine Nutzung erfolgt auf eigene Gefahr.</translation>
    </message>
    <message>
        <source>Update anyway</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Trotzdem updaten</translation>
    </message>
    <message>
        <source>Warning - Update failed</source>
        <extracomment>DESKTOP Header of the popup that is shown when the app download failed.</extracomment>
        <translation>Warnung - Die Aktualisierung ist fehlgeschlagen</translation>
    </message>
    <message>
        <source>Update available</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Aktualisierung verfügbar</translation>
    </message>
</context>
<context>
    <name>UpdateViewButtonRow</name>
    <message>
        <source>Cancel update</source>
        <extracomment>DESKTOP Cancel the download of the update on Windows</extracomment>
        <translation>Aktualisierung abbrechen</translation>
    </message>
    <message>
        <source>Start update</source>
        <extracomment>DESKTOP Start to download the update and execute it on Windows</extracomment>
        <translation>Aktualisierung starten</translation>
    </message>
    <message>
        <source>Download progress</source>
        <extracomment>DESKTOP Name of an progress indicator during a download read by screen readers</extracomment>
        <translation>Downloadfortschritt</translation>
    </message>
    <message>
        <source>The update (version %1) is being performed...</source>
        <extracomment>DESKTOP %1 is replaced with the version number of the software update.</extracomment>
        <translation>Die Aktualisierung (Version %1) wird durchgeführt...</translation>
    </message>
</context>
<context>
    <name>UpdateViewInformation</name>
    <message>
        <source>New version</source>
        <extracomment>DESKTOP Information about the available, new version number.</extracomment>
        <translation>Neue Version</translation>
    </message>
    <message>
        <source>Release date</source>
        <extracomment>DESKTOP Date when the available update was released.</extracomment>
        <translation>Erscheinungsdatum</translation>
    </message>
    <message>
        <source>Download size</source>
        <extracomment>DESKTOP Download size of the available update in megabyte.</extracomment>
        <translation>Größe der Aktualisierung</translation>
    </message>
    <message>
        <source>Download link</source>
        <extracomment>DESKTOP Plaintext link to the update download.</extracomment>
        <translation>Link zum Download</translation>
    </message>
    <message>
        <source>Checksum link</source>
        <extracomment>DESKTOP Link to download checksum to verify the downloaded update file.</extracomment>
        <translation>Link zur Prüfsumme</translation>
    </message>
</context>
<context>
    <name>Utils</name>
    <message>
        <source>Tap to open the following website in your browser: %1</source>
        <extracomment>MOBILE Hint that a link is present, which will open in the browser</extracomment>
        <translation>Tippen um die Webseite in Ihrem Browser zu öffnen: %1</translation>
    </message>
    <message>
        <source>Press space to open the following website in your browser: %1</source>
        <extracomment>DESKTOP Hint that a link is present, which will open in the browser</extracomment>
        <translation>Drücken Sie die Leertaste um die Webseite in Ihrem Browser zu öffnen: %1</translation>
    </message>
</context>
<context>
    <name>VersionInformation</name>
    <message>
        <source>Developer options activated.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Entwickleroptionen aktiviert.</translation>
    </message>
    <message>
        <source>Developer options deactivated.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Entwickleroptionen deaktiviert.</translation>
    </message>
    <message>
        <source>%1 version</source>
        <extracomment>MOBILE %1 is replaced with the application name</extracomment>
        <translation>%1-Version</translation>
    </message>
    <message>
        <source>Advanced settings activated.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Erweiterte Einstellungen aktiviert.</translation>
    </message>
    <message>
        <source>Advanced settings deactivated.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Erweiterte Einstellungen deaktiviert.</translation>
    </message>
    <message>
        <source>Open website</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Öffne Webseite</translation>
    </message>
    <message>
        <source>Privacy statement</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Datenschutzerklärung</translation>
    </message>
    <message>
        <source>Accessibility statement</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Barrierefreiheitserklärung</translation>
    </message>
    <message>
        <source>%1 more taps to toggle the advanced settings.</source>
        <extracomment>MOBILE Used in notifications when the user taps the version information</extracomment>
        <translation>%1 weitere Tipps um die erweiterten Einstellungen umzuschalten.</translation>
    </message>
</context>
<context>
    <name>WhiteListSurveyView</name>
    <message>
        <source>Feedback</source>
        <extracomment>MOBILE</extracomment>
        <translation>Feedback</translation>
    </message>
    <message>
        <source>Send device data?</source>
        <extracomment>MOBILE Request to the user if the device information should be shared for statistics (Whitelist) - Header</extracomment>
        <translation>Gerätedaten senden?</translation>
    </message>
    <message>
        <source>Would you like to help us to improve the %1?</source>
        <extracomment>MOBILE Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text</extracomment>
        <translation>Möchten Sie uns helfen, die %1 zu verbessern?</translation>
    </message>
    <message>
        <source>Supplying your device characteristics helps us to gather reliable information about the compatibility of your device.</source>
        <extracomment>MOBILE Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text</extracomment>
        <translation>Mit der Übermittlung Ihrer Gerätedaten helfen Sie uns, verlässliche Aussagen über die Kompatibilität Ihres Gerätes zu treffen.</translation>
    </message>
    <message>
        <source>The transmission is anonymous. No personal data is collected or transmitted!</source>
        <extracomment>MOBILE Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text</extracomment>
        <translation>Die Übermittlung erfolgt anonym und es werden keine personenbezogenen Daten übermittelt!</translation>
    </message>
    <message>
        <source>Collected data</source>
        <translation>Identifizierte Eigenschaften</translation>
    </message>
    <message>
        <source>Thank you for your assistance!</source>
        <extracomment>MOBILE Request to the user if the device information should be shared for statistics (Whitelist) - Thank you message</extracomment>
        <translation>Vielen Dank für Ihre Mithilfe!</translation>
    </message>
    <message>
        <source>Do not send</source>
        <extracomment>MOBILE</extracomment>
        <translation>Nicht senden</translation>
    </message>
    <message>
        <source>Send</source>
        <extracomment>MOBILE</extracomment>
        <translation>Senden</translation>
    </message>
</context>
<context>
    <name>governikus::AccessRoleAndRightsUtil</name>
    <message>
        <source>Optional data</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Optionale Daten</translation>
    </message>
    <message>
        <source>Residence permit II</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Nebenbestimmungen II</translation>
    </message>
    <message>
        <source>Residence permit I</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Nebenbestimmungen I (nur eAT)</translation>
    </message>
    <message>
        <source>Community-ID</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wohnort-ID</translation>
    </message>
    <message>
        <source>Address</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Anschrift</translation>
    </message>
    <message>
        <source>RFU</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>RFU</translation>
    </message>
    <message>
        <source>Birth name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Geburtsname</translation>
    </message>
    <message>
        <source>Gender</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Geschlecht</translation>
    </message>
    <message>
        <source>Nationality</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Staatsangehörigkeit</translation>
    </message>
    <message>
        <source>Place of birth</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Geburtsort</translation>
    </message>
    <message>
        <source>Date of birth</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Geburtsdatum</translation>
    </message>
    <message>
        <source>Doctoral degree</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Doktorgrad</translation>
    </message>
    <message>
        <source>Religious / artistic name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ordens- / Künstlername</translation>
    </message>
    <message>
        <source>Family name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Familienname</translation>
    </message>
    <message>
        <source>Given name(s)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Vorname(n)</translation>
    </message>
    <message>
        <source>Valid until</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Gültig bis</translation>
    </message>
    <message>
        <source>Issuing country</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausstellender Staat</translation>
    </message>
    <message>
        <source>Document type</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Dokumentenart</translation>
    </message>
    <message>
        <source>Installation of qualified signature certificates</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Installation qualifizierter Signaturzertifikate</translation>
    </message>
    <message>
        <source>Installation of signature certificates</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Installation von Signaturzertifikaten</translation>
    </message>
    <message>
        <source>PIN Management</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>PIN-Verwaltung</translation>
    </message>
    <message>
        <source>CAN allowed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>CAN erlaubt</translation>
    </message>
    <message>
        <source>Privileged terminal</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Privilegiertes Terminal</translation>
    </message>
    <message>
        <source>Pseudonym</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Pseudonym</translation>
    </message>
    <message>
        <source>Address verification</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Wohnortbestätigung</translation>
    </message>
    <message>
        <source>Age verification</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Altersbestätigung</translation>
    </message>
    <message>
        <source>Unknown (reserved)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Unbekannt (reserviert)</translation>
    </message>
    <message>
        <source>Unknown</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Unbekannt</translation>
    </message>
</context>
<context>
    <name>governikus::AppUpdateDataModel</name>
    <message>
        <source>The received data is broken. Check your network connection and try to restart the update.</source>
        <extracomment>DESKTOP Text of the popup that is shown when the app download failed because of a wrong checksum.</extracomment>
        <translation>Die empfangenen Daten sind nicht korrekt. Überprüfen Sie Ihre Netzwerkverbindung und versuchen Sie, die Aktualisierung neu zu starten.</translation>
    </message>
    <message>
        <source>The update could not be started automatically after a successful download. Please try to do a manual update. You can find the downloaded file %1here%2.</source>
        <extracomment>DESKTOP Text of the popup that is shown when the execution of the update failed (1/2).</extracomment>
        <translation>Die Aktualisierung konnte nach dem erfolgreichen Herunterladen nicht automatisch ausgeführt werden. Versuchen Sie bitte eine manuelle Aktualisierung. Die heruntergeladene Datei finden Sie %1hier%2.</translation>
    </message>
    <message>
        <source>An unknown network error occurred. Check your network connection and try to restart the update.</source>
        <extracomment>DESKTOP Generic text of the popup that is shown when the app download failed.</extracomment>
        <translation>Ein unbekannter Netzwerkfehler ist aufgetreten. Überprüfen Sie Ihre Netzwerkverbindung und versuchen Sie, die Aktualisierung neu zu starten.</translation>
    </message>
    <message>
        <source>If this does not help, contact our %1support%2.</source>
        <extracomment>DESKTOP Text of the popup that is shown when the execution of the update failed (2/2).</extracomment>
        <translation>Wenn dies nicht hilft, kontaktieren Sie unseren %1Support%2.</translation>
    </message>
    <message>
        <source>An update is available (version %1).</source>
        <extracomment>DESKTOP %1 is replaced with the version number of the software update</extracomment>
        <translation>Eine Aktualisierung ist verfügbar (Version %1).</translation>
    </message>
    <message>
        <source>hh:mm AP</source>
        <extracomment>DESKTOP Time format according to https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>HH:mm</translation>
    </message>
    <message>
        <source>Last search on %1 at %2</source>
        <extracomment>DESKTOP %1 will be replaced with the date and %2 with the time of the last search.</extracomment>
        <translation>Letzte Suche am %1 um %2 Uhr</translation>
    </message>
    <message>
        <source>Your version %1 of %2 is up to date.</source>
        <extracomment>DESKTOP %1 is replaced with the version number of the software and %2 is replaced with the application name.</extracomment>
        <translation>Ihre Version %1 der %2 ist aktuell.</translation>
    </message>
    <message>
        <source>An update information for your platform is not available.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Für Ihre Plattform sind keine Aktualisierungsinformationen verfügbar.</translation>
    </message>
    <message>
        <source>An update is available but retrieving the information failed.</source>
        <extracomment>DESKTOP The updater found an update but not all required update information are valid, this should be a very rare case.</extracomment>
        <translation>Eine Aktualisierung ist verfügbar, das Herunterladen der Informationen ist jedoch fehlgeschlagen.</translation>
    </message>
    <message>
        <source>The update information could not be retrieved. Please check your network connection.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Die Aktualisierungeninformationen konnten nicht heruntergeladen werden. Bitte überprüfen Sie die Netzwerkverbindung.</translation>
    </message>
    <message>
        <source>dd.MM.yyyy</source>
        <extracomment>DESKTOP Date format according to https://doc.qt.io/qt/qdate.html#toString</extracomment>
        <translation>dd.MM.yyyy</translation>
    </message>
</context>
<context>
    <name>governikus::ApplicationModel</name>
    <message>
        <source>Could not load license text.</source>
        <translation>Konnte Lizenztext nicht laden.</translation>
    </message>
    <message>
        <source>Error while saving config to &quot;%1&quot;</source>
        <translation>Fehler beim Speichern der Konfiguration unter &quot;%1&quot;</translation>
    </message>
</context>
<context>
    <name>governikus::AuthModel</name>
    <message>
        <source>Reason:</source>
        <extracomment>ALL_PLATFORMS Failure code (string) of current workflow error.</extracomment>
        <translation>Grund:</translation>
    </message>
    <message>
        <source>Redirect failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Weiterleitung fehlgeschlagen</translation>
    </message>
    <message>
        <source>Authentication canceled</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Authentisierung abgebrochen</translation>
    </message>
    <message>
        <source>Authentication failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Authentisierung fehlgeschlagen</translation>
    </message>
    <message>
        <source>Authentication successful</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Authentisierung erfolgreich</translation>
    </message>
    <message>
        <source>Back to provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Zurück zum Anbieter</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Zur Startseite</translation>
    </message>
    <message>
        <source>Connection to ID card lost</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Verbindung zum Ausweis unterbrochen</translation>
    </message>
    <message>
        <source>Provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Anbieter</translation>
    </message>
</context>
<context>
    <name>governikus::CardInfo</name>
    <message>
        <source>not inserted</source>
        <comment>Karte</comment>
        <extracomment>ALL_PLATFORMS No card is present/inserted. The text is only used in DiagnosisView.</extracomment>
        <translation>nicht eingelegt</translation>
    </message>
    <message>
        <source>unknown type</source>
        <comment>Karte</comment>
        <extracomment>ALL_PLATFORMS An unknown card is present/inserted. The text is only used in DiagnosisView.</extracomment>
        <translation>unbekannter Typ</translation>
    </message>
    <message>
        <source>ID card (PA/eAT/eID)</source>
        <extracomment>ALL_PLATFORMS An ID card is present/inserted. The text is only used in DiagnosisView.</extracomment>
        <translation>Ausweis (PA/eAT/eID)</translation>
    </message>
</context>
<context>
    <name>governikus::CertificateDescriptionModel</name>
    <message>
        <source>Provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Anbieter</translation>
    </message>
    <message>
        <source>Certificate issuer</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Aussteller des Berechtigungszertifikats</translation>
    </message>
    <message>
        <source>Name, address and mail address of the provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Name, Adresse und E-Mail vom Anbieter</translation>
    </message>
    <message>
        <source>Purpose</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Zweck</translation>
    </message>
    <message>
        <source>Indication of the bodies responsible for the provider, that verify the compliance with data security regulations</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Angabe der für den Anbieter zuständigen Datenschutzaufsicht</translation>
    </message>
    <message>
        <source>Provider Information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Anbieterinformationen</translation>
    </message>
    <message>
        <source>Validity</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Gültigkeit</translation>
    </message>
</context>
<context>
    <name>governikus::DiagnosisController</name>
    <message>
        <source>unknown</source>
        <translation>unbekannt</translation>
    </message>
    <message>
        <source>not available</source>
        <translation>nicht verfügbar</translation>
    </message>
</context>
<context>
    <name>governikus::DiagnosisModel</name>
    <message>
        <source>Time of diagnosis</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Diagnosezeitpunkt</translation>
    </message>
    <message>
        <source>Initial diagnosis running, please wait.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Führe initiale Diagnose durch, bitte warten.</translation>
    </message>
    <message>
        <source>PC/SC driver information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>PC/SC Treiber-Informationen</translation>
    </message>
    <message>
        <source>Diagnosis is running...</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Diagnose läuft...</translation>
    </message>
    <message>
        <source>Card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kartenleser</translation>
    </message>
    <message>
        <source>Paired smartphones</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Gekoppelte Smartphones</translation>
    </message>
    <message>
        <source>Antivirus information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Antivirus-Informationen</translation>
    </message>
    <message>
        <source>No Antivirus information available on this platform.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Auf dieser Plattform sind keine Antivirus-Informationen verfügbar.</translation>
    </message>
    <message>
        <source>Firewall information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Firewallinformationen</translation>
    </message>
    <message>
        <source>No Firewall information available on this platform.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Auf dieser Plattform sind keine Firewall-Informationen verfügbar.</translation>
    </message>
    <message>
        <source>Yes</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Ja</translation>
    </message>
    <message>
        <source>No</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Nein</translation>
    </message>
    <message>
        <source>Failed to retrieve date &amp; time</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Datum &amp; Uhrzeit konnten nicht ermittelt werden</translation>
    </message>
    <message>
        <source>d. MMMM yyyy, hh:mm:ss AP</source>
        <extracomment>DESKTOP Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>d. MMMM yyyy, HH:mm:ss &apos;Uhr&apos;</translation>
    </message>
    <message>
        <source>&lt;Not set&gt;</source>
        <extracomment>DESKTOP Interface has no hardware address set</extracomment>
        <translation>&lt;Nicht gesetzt&gt;</translation>
    </message>
    <message>
        <source>Hardware address: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Hardwareadresse: %1</translation>
    </message>
    <message>
        <source>No IP addresses assigned</source>
        <extracomment>DESKTOP Interface has no IP addresses assigned</extracomment>
        <translation>Keine IP-Adressen zugewiesen</translation>
    </message>
    <message>
        <source>IPv4 address: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>IPv4-Adresse: %1</translation>
    </message>
    <message>
        <source>IPv6 address: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>IPv6-Adresse: %1</translation>
    </message>
    <message>
        <source>Unknown address: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Unbekannte Adresse: %1</translation>
    </message>
    <message>
        <source>Hostname: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Hostname: %1</translation>
    </message>
    <message>
        <source>Port: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Port: %1</translation>
    </message>
    <message>
        <source>Type: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Typ: %1</translation>
    </message>
    <message>
        <source>Capabilities: %1</source>
        <extracomment>DESKTOP list of the capabitlities of the proxy connection</extracomment>
        <translation>Fähigkeiten: %1</translation>
    </message>
    <message>
        <source>Ping test to proxy: Successful</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Proxy-Pingtest: Erfolgreich</translation>
    </message>
    <message>
        <source>Ping test to proxy: Failed</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Proxy-Pingtest: Fehlgeschlagen</translation>
    </message>
    <message>
        <source>Connection test with proxy: Successful</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Proxy-Verbindungstest: Erfolgreich</translation>
    </message>
    <message>
        <source>Connection test with proxy: Failed</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Proxy-Verbindungstest: Fehlgeschlagen</translation>
    </message>
    <message>
        <source>No proxy found</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Kein Proxy erkannt</translation>
    </message>
    <message>
        <source>Connection test without proxy: Successful</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Verbindungstest ohne Proxy: Erfolgreich</translation>
    </message>
    <message>
        <source>Connection test without proxy: Failed</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Verbindungstest ohne Proxy: Fehlgeschlagen</translation>
    </message>
    <message>
        <source>Proxy information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Proxyinformationen</translation>
    </message>
    <message>
        <source>No Antivirus software detected.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Es konnte keine Antivirus-Software erkannt werden.</translation>
    </message>
    <message>
        <source>Last updated: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Zuletzt aktualisiert: %1</translation>
    </message>
    <message>
        <source>Executable path: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Anwendungspfad: %1</translation>
    </message>
    <message>
        <source>Antivirus detection failed.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Antivirus-Erkennung fehlgeschlagen.</translation>
    </message>
    <message>
        <source>No 3rd party firewalls detected</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Keine Drittanbieter-Firewall erkannt</translation>
    </message>
    <message>
        <source>Enabled: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Aktiviert: %1</translation>
    </message>
    <message>
        <source>Up to date: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Auf dem aktuellen Stand: %1</translation>
    </message>
    <message>
        <source>Firewalls from 3rd party vendors</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Firewalls von Drittanbietern</translation>
    </message>
    <message>
        <source>Outgoing %1 rule</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Ausgehende %1-Regel</translation>
    </message>
    <message>
        <source>Exists: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Existiert: %1</translation>
    </message>
    <message>
        <source>Incoming %1 rule</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Eingehende %1-Regel</translation>
    </message>
    <message>
        <source>Windows firewall rules</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Windows-Firewall-Regeln</translation>
    </message>
    <message>
        <source>Windows firewall profiles</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Windows Firewall-Profile</translation>
    </message>
    <message>
        <source>An error occurred while trying to gather firewall information. Please check the log for more information.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Beim Sammeln von Firewall-Informationen ist ein Fehler aufgetreten. Bitte konsultieren Sie das Protokoll für weitere Informationen.</translation>
    </message>
    <message>
        <source>PC/SC information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>PC/SC Informationen</translation>
    </message>
    <message>
        <source>Version</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Version</translation>
    </message>
    <message>
        <source>Vendor: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Hersteller: %1</translation>
    </message>
    <message>
        <source>Version: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Version: %1</translation>
    </message>
    <message>
        <source>File path: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Dateipfad: %1</translation>
    </message>
    <message>
        <source>Components</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Komponenten</translation>
    </message>
    <message>
        <source>Driver</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Treiber</translation>
    </message>
    <message>
        <source>No devices paired.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Keine Geräte gekoppelt.</translation>
    </message>
    <message>
        <source>dd.MM.yyyy, hh:mm:ss</source>
        <extracomment>DESKTOP Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>dd.MM.yyyy, hh:mm:ss</translation>
    </message>
    <message>
        <source>Last connection: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Zuletzt verbunden: %1</translation>
    </message>
    <message>
        <source>No information found for this certificate.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Für dieses Zertifikat konnten keine Informationen ermittelt werden.</translation>
    </message>
    <message>
        <source>Connected Card readers</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Verbundene Kartenleser</translation>
    </message>
    <message>
        <source>No supported reader found.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Keine unterstützten Leser erkannt.</translation>
    </message>
    <message>
        <source>Basic card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Basis-Kartenleser</translation>
    </message>
    <message>
        <source>Standard / comfort card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Standard- / Komfortleser</translation>
    </message>
    <message>
        <source>Card: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Karte: %1</translation>
    </message>
    <message>
        <source>Retry counter: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Verbleibende Versuche: %1</translation>
    </message>
    <message>
        <source>No driver installed</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Treiber nicht installiert</translation>
    </message>
    <message>
        <source>Network</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Netzwerk</translation>
    </message>
    <message>
        <source>Antivirus and firewall</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Antivirus und Firewall</translation>
    </message>
    <message>
        <source>Service addresses</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Dienstadressen</translation>
    </message>
    <message>
        <source>Not bound</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Nicht gebunden</translation>
    </message>
    <message>
        <source>Encountered error: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Aufgetretener Fehler: %1</translation>
    </message>
</context>
<context>
    <name>governikus::ECardApiResult</name>
    <message>
        <source>An unexpected error has occurred during processing.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Während der Verarbeitung ist ein unerwarteter Fehler aufgetreten.</translation>
    </message>
    <message>
        <source>Use of the function by the client application is not permitted.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Für die Operation fehlen die benötigten Rechte.</translation>
    </message>
    <message>
        <source>An internal error has occurred during processing.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Während der Verarbeitung ist ein interner Fehler aufgetreten.</translation>
    </message>
    <message>
        <source>There was some problem with a provided or omitted parameter.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ein fehlerhafter Parameter wurde übermittelt.</translation>
    </message>
    <message>
        <source>The API function is unknown.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Schnittstellenfunktion ist unbekannt.</translation>
    </message>
    <message>
        <source>The framework or layer has not been initialized.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Schnittstelle ist nicht initialisiert.</translation>
    </message>
    <message>
        <source>The active session has been terminated.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die aktuelle Sitzung wurde beendet.</translation>
    </message>
    <message>
        <source>A Communication error occurred during processing.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Während der Verarbeitung ist ein Verbindungsfehler aufgetreten.</translation>
    </message>
    <message>
        <source>The operation was terminated as the set time was exceeded.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der Vorgang wurde wegen einer Zeitüberschreitung abgebrochen.</translation>
    </message>
    <message>
        <source>The operation was aborted as an invalid channel handle was used.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der Vorgang wurde wegen einer ungültigen Verbindung abgebrochen.</translation>
    </message>
    <message>
        <source>A trusted channel could not be opened.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Es konnte keine sichere Verbindung hergestellt werden.</translation>
    </message>
    <message>
        <source>The operation was aborted as an unknown protocol was used.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der Vorgang wurde wegen der Verwendung eines unbekannten Protokolls abgebrochen.</translation>
    </message>
    <message>
        <source>The operation was aborted as an unknown cipher suite was used.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der Vorgang wurde wegen der Verwendung eines unbekannten Verschlüsselungsalgorithmus abgebrochen.</translation>
    </message>
    <message>
        <source>The operation was aborted as an unknown web service binding was used.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der Vorgang wurde wegen der Verwendung eines unbekannten Webservice-Bindings abgebrochen.</translation>
    </message>
    <message>
        <source>The card is missing or was removed.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Es liegt keine Karte auf oder sie wurde entfernt.</translation>
    </message>
    <message>
        <source>The new PIN and the confirmation do not match.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die neue PIN und ihre Wiederholung stimmen nicht überein.</translation>
    </message>
    <message>
        <source>The format of the PIN is wrong.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Zeichenanzahl der PIN ist inkorrekt.</translation>
    </message>
    <message>
        <source>Signature certificate key generation is not possible.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Erzeugung eines Schlüssels für das Signaturzertifikat war nicht möglich.</translation>
    </message>
    <message>
        <source>The process has been cancelled.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der Vorgang wurde abgebrochen.</translation>
    </message>
    <message>
        <source>One or more certificate checks failed. The operation will be aborted due to security reasons.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Eine oder mehrere Zertifikatsprüfungen schlugen fehl. Der Vorgang wird aus Sicherheitsgründen abgebrochen.</translation>
    </message>
    <message>
        <source>This action cannot be performed. The eID function of your ID card is not activated.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Diese Aktion kann leider nicht durchgeführt werden. Die Online-Ausweisfunktion Ihres Ausweises ist nicht aktiviert.</translation>
    </message>
    <message>
        <source>The authenticity of your ID card could not be verified. Please make sure that you are using a genuine ID card. Please note that test applications require the use of a test ID card.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Echtheit Ihres Ausweises konnte nicht überprüft werden. Bitte stellen Sie sicher, dass Sie einen echten Ausweis verwenden. Bitte beachten Sie, dass Sie bei Testanwendungen einen Testausweis verwenden müssen.</translation>
    </message>
    <message>
        <source>The age verification failed.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Altersverifikation war nicht erfolgreich.</translation>
    </message>
    <message>
        <source>The community verification failed.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Wohnortverifikation war nicht erfolgreich.</translation>
    </message>
    <message>
        <source>The ID card is invalid or disabled.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der Ausweis ist ungültig oder gesperrt.</translation>
    </message>
</context>
<context>
    <name>governikus::GlobalStatus</name>
    <message>
        <source>A secure connection to the provider could not be established. Check the network connection and try again later. If this doesn&apos;t help, contact our %1support%2.</source>
        <extracomment>ALL_PLATFORMS Technical error description. Generic message with link to support section of the homepage. %1: HTML hyperlink start tag %2: HTML hyperlink end tag</extracomment>
        <translation>Es konnte keine sichere Verbindung zum Anbieter aufgebaut werden. Überprüfen Sie die Netzwerkverbindung und probieren Sie es später erneut. Wenn dies nicht hilft, kontaktieren Sie unseren %1Support%2.</translation>
    </message>
    <message>
        <source>No error occurred.</source>
        <extracomment>ALL_PLATFORMS No actual error occurred, required to provide a message for status code No_Error.</extracomment>
        <translation>Es ist kein Fehler aufgetreten.</translation>
    </message>
    <message>
        <source>An unexpected error has occurred during processing.</source>
        <extracomment>ALL_PLATFORMS An unknown error from any of the subcomponents (PCSC, EcardAPI, QtNetwork, ...) occurred.</extracomment>
        <translation>Während der Verarbeitung ist ein unerwarteter Fehler aufgetreten.</translation>
    </message>
    <message>
        <source>Cannot start authentication. An operation is already active.</source>
        <extracomment>ALL_PLATFORMS An ActivationHandler is requested even though an operation is currently still running.</extracomment>
        <translation>Der Ausweisvorgang kann nicht gestartet werden. Ein anderer Vorgang ist bereits aktiv.</translation>
    </message>
    <message>
        <source>Restart the authentication process and make sure that the position of the ID card does not change during the reading process.</source>
        <extracomment>ALL_PLATFORMS The card was removed after the PACE channel was established.</extracomment>
        <translation>Starten Sie den Authentisierungsvorgang erneut und stellen Sie sicher, dass sich die Position des Ausweises während des Auslesevorgangs nicht verändert.</translation>
    </message>
    <message>
        <source>The authenticity of your ID card could not be confirmed.</source>
        <extracomment>ALL_PLATFORMS The certificates supplied by the card did not pass the authenticity check, further operation is not allowed.</extracomment>
        <translation>Die Echtheit Ihres Ausweises konnte nicht bestätigt werden.</translation>
    </message>
    <message>
        <source>The program received an unknown message from the server.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Die Anwendung hat eine unbekannte Nachricht vom Server erhalten.</translation>
    </message>
    <message>
        <source>The program received an unexpected message from the server.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Die Anwendung hat eine nicht erwartete Nachricht vom Server erhalten.</translation>
    </message>
    <message>
        <source>Using the developer mode is only allowed in a test environment.</source>
        <extracomment>ALL_PLATFORMS The developer mode is enabled but a productive environment was detected.</extracomment>
        <translation>Der Entwicklermodus darf nur in einer Testumgebung verwendet werden.</translation>
    </message>
    <message>
        <source>Pre-verification failed.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Eine oder mehrere Zertifikatsprüfungen schlugen fehl.</translation>
    </message>
    <message>
        <source>No unique AT CVC</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Kein eindeutiges AT CVC</translation>
    </message>
    <message>
        <source>No unique DV CVC</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Kein eindeutiges DV CVC</translation>
    </message>
    <message>
        <source>Authentication failed.</source>
        <extracomment>ALL_PLATFORMS DidAuthenticateEAC2, AA or the ID card declined the certificates.</extracomment>
        <translation>Die Authentisierung ist fehlgeschlagen.</translation>
    </message>
    <message>
        <source>The length of the data sent to the ID card was not accepted. Either the data is faulty or your card reader does not support Extended Length communication and cannot be used to read the ID card. Unfortunately, the %1 has no influence on this restriction.</source>
        <extracomment>ALL_PLATFORMS A card command failed because the data length was wrong or the card reader does not support Extended Length.</extracomment>
        <translation>Die Länge der an den Ausweis gesendeten Daten wurde nicht akzeptiert. Entweder sind die Daten fehlerhaft oder Ihr Kartenleser unterstützt keine Extended Length Kommunikation und kann nicht zum Auslesen des Ausweises genutzt werden. Auf diese Einschränkung hat die %1 leider keinen Einfluss.</translation>
    </message>
    <message>
        <source>No certificate description available.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Keine Zertifikatsbeschreibung vorhanden.</translation>
    </message>
    <message>
        <source>No subject url available in certificate description.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Es konnte keine URL in der Zertifikatsbeschreibung gefunden werden.</translation>
    </message>
    <message>
        <source>The certificate description does not match the certificate.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Die Zertifikatsbeschreibung passt nicht zum Zertifikat.</translation>
    </message>
    <message>
        <source>The subject URL in the certificate description and the TCToken URL do not satisfy the same origin policy.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Die URL in der Zertifikatsbeschreibung und die TCToken-URL erfüllen die Same-Origin-Policy nicht.</translation>
    </message>
    <message>
        <source>The program received an error from the server.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Die Anwendung hat einen Fehler vom Server erhalten.</translation>
    </message>
    <message>
        <source>Hash of TLS certificate not in certificate description (issuer: %1). This indicates a misconfiguration or manipulation of the certificate. Please check that your antivirus software and firewall are not interfering with TLS traffic.</source>
        <extracomment>ALL_PLATFORMS The TLS certificate was not folded with the Authorization Certificate, thus violating the security requirements. Might also be caused by a firewall and/or the antivirus software.</extracomment>
        <translation>Der Hashwert des TLS-Zertifikats ist nicht in der Zertifikatsbeschreibung vorhanden (Aussteller: %1). Dies deutet auf eine Fehlkonfiguration oder Manipulation des Zertifikats hin. Bitte überprüfen Sie, dass weder eine Antivirus-Software noch eine Firewall in den TLS-Verkehr eingreifen.</translation>
    </message>
    <message>
        <source>The TCToken contained no data.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Das TCToken enthielt keine Daten.</translation>
    </message>
    <message>
        <source>The service is temporarily not available. Please try again later.</source>
        <extracomment>ALL_PLATFORMS A server has responded with an HTTP error code 503.</extracomment>
        <translation>Der Dienst ist vorübergehend nicht verfügbar, bitte versuchen Sie es zu einem späteren Zeitpunkt erneut.</translation>
    </message>
    <message>
        <source>Establishing a connection is taking too long.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Der Verbindungsaufbau dauert zu lange.</translation>
    </message>
    <message>
        <source>Establishing a connection via the proxy did not succeed.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Der Verbindungsaufbau über den Proxy war nicht erfolgreich.</translation>
    </message>
    <message>
        <source>It wasn&apos;t possible to connect to the server: the server sent a non-standard response.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Die Verbindung zum Server ist fehlgeschlagen: Der Server hat eine nicht-standard Antwort gesendet.</translation>
    </message>
    <message>
        <source>It wasn&apos;t possible to connect to the server: a secure connection could not be established.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Die Verbindung zum Server ist fehlgeschlagen: Es konnte keine sichere Verbindung aufgebaut werden.</translation>
    </message>
    <message>
        <source>Application was invoked with wrong parameters.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Die Anwendung wurde mit den falschen Parametern aufgerufen.</translation>
    </message>
    <message>
        <source>An unknown network error occurred. Check your network connection and try to restart the app.</source>
        <extracomment>ALL_PLATFORMS Other critical network error by Qt.</extracomment>
        <translation>Ein unbekannter Netzwerkfehler ist aufgetreten. Überprüfen Sie Ihre Netzwerkverbindung und versuchen Sie, die App neu zu starten.</translation>
    </message>
    <message>
        <source>The selected card reader cannot be accessed anymore.</source>
        <extracomment>ALL_PLATFORMS The card reader was removed after the PACE channel was established.</extracomment>
        <translation>Der ausgewählte Kartenleser kann nicht mehr angesprochen werden.</translation>
    </message>
    <message>
        <source>The server provided no or incomplete information. Your personal data could not be read out.</source>
        <extracomment>ALL_PLATFORMS The result of the self authentication could not be retrieved from the eService.</extracomment>
        <translation>Der Server lieferte keine oder nur unvollständige Informationen. Ihre persönlichen Daten konnten nicht ausgelesen werden.</translation>
    </message>
    <message>
        <source>Error while connecting to the provider. The TLS connection uses an unsupported key algorithm or length.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Fehler bei der Verbindung mit dem Anbieter. Der Verschlüsselungsalgorithmus oder die Länge des Schlüssels der TLS-Verbindung wird nicht unterstützt.</translation>
    </message>
    <message>
        <source>Error while connecting to the server. The TLS certificate uses an unsupported key algorithm or length. Certificate issuer: %1</source>
        <extracomment>ALL_PLATFORMS Received a TLS certificate that uses an invalid algorithm or key length.</extracomment>
        <translation>Fehler bei der Verbindung zum Server. Der Verschlüsselungsalgorithmus oder die Länge des Schlüssels im TLS-Zertifikat des Servers wird nicht unterstützt. Zertifikatsaussteller: %1</translation>
    </message>
    <message>
        <source>Empty redirect URL</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Leere Redirect-URL</translation>
    </message>
    <message>
        <source>Expected redirect, got %1</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Erwartete HTTP-redirect, tatsächlich erhalten: %1</translation>
    </message>
    <message>
        <source>Invalid scheme: %1</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Ungültiges URL-Schema: %1</translation>
    </message>
    <message>
        <source>Malformed redirect URL: %1</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Nicht wohlgeformte Redirect-URL: %1</translation>
    </message>
    <message>
        <source>You have canceled the process.</source>
        <extracomment>ALL_PLATFORMS The user cancelled the authentication in the UI.</extracomment>
        <translation>Sie haben den Vorgang abgebrochen.</translation>
    </message>
    <message>
        <source>The process has been aborted because of an internal error.</source>
        <extracomment>ALL_PLATFORMS An internal error occurred before fetching TcToken.</extracomment>
        <translation>Der Vorgang wurde aufgrund eines internen Fehlers abgebrochen.</translation>
    </message>
    <message>
        <source>The process has been cancelled by the card reader.</source>
        <extracomment>ALL_PLATFORMS The user cancelled the authentication on his card reader.</extracomment>
        <translation>Der Vorgang wurde durch den Kartenleser abgebrochen.</translation>
    </message>
    <message>
        <source>The maximum time was exceeded during input process.</source>
        <extracomment>ALL_PLATFORMS The card reader signalled that it timed out.</extracomment>
        <translation>Bei der Eingabe wurde die maximale Zeit überschritten.</translation>
    </message>
    <message>
        <source>Card does not exist</source>
        <extracomment>ALL_PLATFORMS Internal error, either PCSC, SaK or card reader could not find the ID card.</extracomment>
        <translation>Karte nicht vorhanden</translation>
    </message>
    <message>
        <source>The eID function of your ID card is not yet activated.</source>
        <extracomment>ALL_PLATFORMS The eID function of the ID card is not active.</extracomment>
        <translation>Die Online-Ausweisfunktion Ihres Ausweises ist noch nicht aktiviert.</translation>
    </message>
    <message>
        <source>An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.</source>
        <extracomment>ALL_PLATFORMS Communication with the card failed due to the specification of the TR (Technische Richtlinie),</extracomment>
        <translation>Bei der Kommunikation mit dem Ausweis ist ein Fehler aufgetreten. Bitte überprüfen Sie, dass der Ausweis korrekt aufgelegt ist und versuchen Sie es erneut.</translation>
    </message>
    <message>
        <source>A protocol error occurred. Please make sure that your ID card is placed correctly on the card reader and try again. If the problem occurs again, please contact our support at %1.</source>
        <extracomment>ALL_PLATFORMS Communication with the card failed due to the specification of the TR (Technische Richtlinie). The protocol was faulty or invalid values were requested/received. %1 is a html link to the support.</extracomment>
        <translation>Ein Protokollfehler ist aufgetreten. Bitte überprüfen Sie, dass der Ausweis korrekt aufgelegt ist und versuchen Sie es erneut. Wenn das Problem wieder auftritt kontaktieren Sie bitte unseren Support unter %1.</translation>
    </message>
    <message>
        <source>The given PIN is not correct.</source>
        <extracomment>ALL_PLATFORMS The ID card declined the PIN.</extracomment>
        <translation>Die eingegebene PIN ist nicht korrekt.</translation>
    </message>
    <message>
        <source>The given Card Access Number (CAN) is not correct.</source>
        <extracomment>ALL_PLATFORMS The ID card declined the CAN.</extracomment>
        <translation>Die eingegebene Zugangsnummer (CAN) ist nicht korrekt.</translation>
    </message>
    <message>
        <source>The given PUK is not correct.</source>
        <extracomment>ALL_PLATFORMS The ID card declined the PUK.</extracomment>
        <translation>Die eingegebene PUK ist nicht korrekt.</translation>
    </message>
    <message>
        <source>The PIN is not blocked.</source>
        <extracomment>ALL_PLATFORMS It was attempted to unlock the ID card via PUK even though it was not locked in the first place. This scenario is avoided in the UI by hiding the respective UI elements.</extracomment>
        <translation>Die PIN ist nicht gesperrt.</translation>
    </message>
    <message>
        <source>The entered PUK has already been used 10 times and can no longer be used to unblock the PIN.</source>
        <extracomment>ALL_PLATFORMS The card declined the PUK since it was entered wrongfully 10 times.</extracomment>
        <translation>Die eingegebene PUK wurde bereits 10 Mal verwendet und kann nicht mehr zur Entsperrung der PIN genutzt werden.</translation>
    </message>
    <message>
        <source>The new PIN and the confirmation do not match.</source>
        <extracomment>ALL_PLATFORMS The card reader signalled that the new PIN was not confirmed correctly.</extracomment>
        <translation>Die neue PIN und ihre Wiederholung stimmen nicht überein.</translation>
    </message>
    <message>
        <source>The length of the new PIN is not valid.</source>
        <extracomment>ALL_PLATFORMS The card reader declined the new PIN since its length was invalid.</extracomment>
        <translation>Die neue PIN hat eine ungültige Länge.</translation>
    </message>
    <message>
        <source>The validity verification of the card failed.</source>
        <extracomment>ALL_PLATFORMS The validity verification of the card failed.</extracomment>
        <translation>Die Gültigkeitsprüfung der Karte ist fehlgeschlagen.</translation>
    </message>
    <message>
        <source>The smartphone as card reader (SaC) connection was aborted.</source>
        <extracomment>ALL_PLATFORMS The connection to the smartphone card reader (SaK) was lost.</extracomment>
        <translation>Die Verbindung zum Smartphone als Kartenleser wurde abgebrochen.</translation>
    </message>
    <message>
        <source>Smartphone as card reader (SaC) connection request was invalid.</source>
        <extracomment>ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) was invalid (missing device ID).</extracomment>
        <translation>Die Verbindungsanforderung zum Smartphone als Kartenleser (SaK) war fehlerhaft.</translation>
    </message>
    <message>
        <source>Your smartphone as card reader (SaC) version is incompatible with the local version. Please install the latest %1 version on both your smartphone and your computer.</source>
        <extracomment>ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) was invalid (API mismatch).</extracomment>
        <translation>Die Version Ihres Smartphones als Kartenleser (SaK) ist inkompatibel. Installieren Sie bitte auf Ihrem Smartphone und Ihrem Computer die aktuellste %1 Version.</translation>
    </message>
    <message>
        <source>An error occurred while trying to establish a connection to the smartphone as card reader (SaC).</source>
        <extracomment>ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) failed due to network errors (Host not found, OS error, ...)</extracomment>
        <translation>Bei der Verbindung zum Smartphone als Kartenleser (SaK) ist ein Fehler aufgetreten.</translation>
    </message>
    <message>
        <source>The smartphone to be paired has rejected the connection. Please check the pairing code.</source>
        <extracomment>ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) was rejected by the device.</extracomment>
        <translation>Das zu koppelnde Smartphone hat die Verbindung verweigert. Überprüfen Sie bitte den Kopplungscode.</translation>
    </message>
    <message>
        <source>File not found.</source>
        <extracomment>ALL_PLATFORMS Download of the file failed with HTTP error code 404.</extracomment>
        <translation>Datei nicht gefunden.</translation>
    </message>
    <message>
        <source>Cannot save file.</source>
        <extracomment>ALL_PLATFORMS Saving the downloaded file on the local disk failed.</extracomment>
        <translation>Speichern der Datei nicht möglich.</translation>
    </message>
    <message>
        <source>Received data were corrupted.</source>
        <extracomment>ALL_PLATFORMS The downloaded file contained unsupported/invalid data.</extracomment>
        <translation>Die empfangenen Daten waren beschädigt.</translation>
    </message>
    <message>
        <source>Received data does not contain data for the current platform.</source>
        <extracomment>ALL_PLATFORMS The downloaded file does not contain data for the current platform.</extracomment>
        <translation>Die empfangenen Daten enthalten keine Informationen für die verwendete Plattform.</translation>
    </message>
    <message>
        <source>Download aborted.</source>
        <extracomment>ALL_PLATFORMS The download is aborted.</extracomment>
        <translation>Herunterladen abgebrochen.</translation>
    </message>
    <message>
        <source>A new process to start the update could not be launched.</source>
        <extracomment>ALL_PLATFORMS Starting the update failed.</extracomment>
        <translation>Ein neuer Prozess zur Durchführung der Aktualisierung konnte nicht gestartet werden.</translation>
    </message>
    <message>
        <source>The program did not receive a StartPaosResponse message from the server.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Die Anwendung hat keine StartPaosResponse Nachricht vom Server erhalten.</translation>
    </message>
    <message>
        <source>The server could not process the client request.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Der Server konnte die Anfrage des Clients nicht verarbeiten.</translation>
    </message>
    <message>
        <source>The service encountered an internal error while processing a request.</source>
        <extracomment>ALL_PLATFORMS A server has responded with an HTTP error code 5xx.</extracomment>
        <translation>Bei der Bearbeitung der Anfrage ist beim Anbieter ein Fehler aufgetreten.</translation>
    </message>
    <message>
        <source>The service reported an error while processing a client request.</source>
        <extracomment>ALL_PLATFORMS A server has responded with an HTTP error code 4xx.</extracomment>
        <translation>Der Anbieter hat bei der Verarbeitung der Anfrage Fehler festgestellt.</translation>
    </message>
    <message>
        <source>%1 Support</source>
        <extracomment>ALL_PLATFORMS Link text to the app support. %1 is the app name.</extracomment>
        <translation>%1 Support</translation>
    </message>
</context>
<context>
    <name>governikus::HttpHandler</name>
    <message>
        <source>Invalid request (%1)</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Ungültige Anfrage (%1)</translation>
    </message>
    <message>
        <source>Your browser sent a request that couldn&apos;t be interpreted.</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Ihr Browser hat eine Anfrage gesendet, die nicht interpretiert werden konnte.</translation>
    </message>
    <message>
        <source>Error message:</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Fehlermeldung:</translation>
    </message>
    <message>
        <source>Unknown request: %1</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Unbekannte Anfrage: %1</translation>
    </message>
    <message>
        <source>Would you like to report this error?</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Möchten Sie diesen Fehler melden?</translation>
    </message>
    <message>
        <source>Report now</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Jetzt melden</translation>
    </message>
</context>
<context>
    <name>governikus::LogFilesModel</name>
    <message>
        <source>Current log</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Aktuelles Protokoll</translation>
    </message>
    <message>
        <source>dd.MM.yyyy hh:mm:ss</source>
        <extracomment>ALL_PLATFORMS Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>dd.MM.yyyy hh:mm:ss</translation>
    </message>
</context>
<context>
    <name>governikus::LogModel</name>
    <message>
        <source>The logfile is disabled.</source>
        <translation>Die Protokolldatei ist deaktiviert.</translation>
    </message>
    <message>
        <source>Error while saving logfile to &quot;%1&quot;</source>
        <translation>Fehler beim Speichern der Protokolldatei unter &quot;%1&quot;</translation>
    </message>
    <message>
        <source>Mobile logfile</source>
        <translation>Mobile Protokolldatei</translation>
    </message>
    <message>
        <source>&lt;Please describe the error&gt;</source>
        <translation>&lt;Bitte beschreiben Sie den Fehler&gt;</translation>
    </message>
    <message>
        <source>Send application log per email...</source>
        <extracomment>ANDROID</extracomment>
        <translation>Anwendungsprotokoll per E-Mail senden...</translation>
    </message>
    <message>
        <source>Share application log...</source>
        <extracomment>ANDROID</extracomment>
        <translation>Anwendungsprotokoll teilen...</translation>
    </message>
    <message>
        <source>Could not mail log! Please configure your mail client first.</source>
        <translation>Log konnte nicht versendet werden! Bitte konfigurieren Sie Ihr E-Mail-Programm.</translation>
    </message>
    <message>
        <source>Could not share log! Connection to OS failed.</source>
        <translation>Log konnte nicht geteilt werden! Verbindung zum Betriebssystem fehlgeschlagen.</translation>
    </message>
    <message>
        <source>Could not mail log! Connection to OS failed.</source>
        <translation>Log konnte nicht versendet werden! Verbindung zum Betriebssystem fehlgeschlagen.</translation>
    </message>
</context>
<context>
    <name>governikus::NfcReader</name>
    <message>
        <source>ID card detected. Please do not move the device!</source>
        <extracomment>IOS Feedback when a new ID card has been detected</extracomment>
        <translation>Ein Ausweis wurde erkannt. Bitte bewegen Sie das Gerät nicht!</translation>
    </message>
    <message>
        <source>Please place your ID card on the top of the device&apos;s back side.</source>
        <extracomment>IOS The ID card may be inserted, the authentication process may be started.</extracomment>
        <translation>Bitte platzieren Sie Ihren Ausweis oben an der Geräterückseite.</translation>
    </message>
    <message>
        <source>Scanning process has been finished successfully.</source>
        <extracomment>IOS The current session was stopped without errors.</extracomment>
        <translation>Scannen erfolgreich abgeschlossen.</translation>
    </message>
</context>
<context>
    <name>governikus::NotificationModel</name>
    <message>
        <source>hh:mm:ss</source>
        <extracomment>ALL_PLATFORMS Time format according to https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>HH:mm:ss</translation>
    </message>
</context>
<context>
    <name>governikus::NumberModel</name>
    <message>
        <source>You have entered an incorrect, 5-digit Transport PIN 3 times, your %1Transport PIN is now blocked%2. To remove the block, the%1 10-digit PUK%2 must be entered first.</source>
        <extracomment>ALL_PLATFORMS The Transport PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK. %1 + %2 are used to emphasize.</extracomment>
        <translation>Sie haben 3 Mal eine falsche, 5-stellige Transport-PIN eingegeben, Ihre %1Transport-PIN ist nun gesperrt%2. Um die Sperre aufzuheben, muss zunächst die%1 10-stellige PUK%2 eingegeben werden.</translation>
    </message>
    <message>
        <source>You have entered an incorrect, 10-digit PUK. Please try again.</source>
        <extracomment>ALL_PLATFORMS The PUK entered wrongfully and needs to be supplied again.</extracomment>
        <translation>Sie haben eine falsche, 10-stellige PUK eingegeben. Bitte versuchen Sie es erneut.</translation>
    </message>
    <message>
        <source>The input does not match. Please choose a new ID card PIN.</source>
        <extracomment>ALL_PLATFORMS Error message if the new pin confirmation mismatches.</extracomment>
        <translation>Die Eingaben stimmen nicht überein. Bitte wählen Sie eine neue Karten-PIN.</translation>
    </message>
    <message>
        <source>You have entered an incorrect, 6-digit ID card PIN.</source>
        <extracomment>ALL_PLATFORMS The wrong ID card PIN was entered on the first attempt. Part 1/2</extracomment>
        <translation>Sie haben eine falsche 6-stellige Karten-PIN eingegeben.</translation>
    </message>
    <message>
        <source>You have%1 2 further attempts%2 to enter the correct ID card PIN.</source>
        <extracomment>ALL_PLATFORMS The wrong ID card PIN was entered on the first attempt. %1 + %2 are used to emphasize. Part 2/2</extracomment>
        <translation>Sie haben%1 2 weitere Versuche%2, die richtige Karten-PIN einzugegeben.</translation>
    </message>
    <message>
        <source>You have entered an %1incorrect, 6-digit ID card PIN 2 times%2.</source>
        <extracomment>ALL_PLATFORMS The wrong ID card PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 1/2</extracomment>
        <translation>Sie haben zum%1 2. Mal eine falsche 6-stellige Karten-PIN%2 eingegeben.</translation>
    </message>
    <message>
        <source>For a 3rd attempt, the%1 6-digit Card Access Number (CAN)%2 must be entered first. You can find your CAN in the %1bottom right on the front of your ID card%2.</source>
        <extracomment>ALL_PLATFORMS The wrong Transport PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 2/2
----------
ALL_PLATFORMS The wrong ID card PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 2/2</extracomment>
        <translation>Für den 3. Versuch muss zunächst die%1 6-stellige Zugangsnummer (CAN)%2 eingegeben werden. Sie finden Ihre CAN %1unten rechts auf der Vorderseite Ihres Ausweises%2.</translation>
    </message>
    <message>
        <source>You have entered an incorrect, 6-digit ID card PIN 3 times. Your %1ID card PIN is now blocked%2.</source>
        <extracomment>ALL_PLATFORMS The ID card PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK. Part 1/2</extracomment>
        <translation>Sie haben 3 Mal eine falsche, 6-stellige Karten-PIN eingegeben. Ihre %1Karten-PIN ist nun gesperrt%2.</translation>
    </message>
    <message>
        <source>To remove the block, the%1 10-digit PUK%2 must be entered first. You can find the PUK in the bottom %1right next%2 to the Transport PIN in the %1authority&apos;s letter%2.</source>
        <extracomment>ALL_PLATFORMS The ID card PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK. Part 2/2</extracomment>
        <translation>Um die Sperre aufzuheben, muss zunächst die%1 10-stellige PUK%2 eingegeben werden. Sie finden die PUK %1im Brief der Ausweisbehörde unten rechts%2 neben der Transport-PIN.</translation>
    </message>
    <message>
        <source>You have entered an %1incorrect Card Access Number (CAN)%2. Please try again. You can find your CAN in the %1bottom right on the front of your ID card%2.</source>
        <extracomment>ALL_PLATFORMS The CAN was entered wrongfully and needs to be supplied again. %1 + %2 are used to emphasize.</extracomment>
        <translation>Sie haben eine %1falsche Zugangsnummer (CAN)%2 eingegeben. Bitte versuchen Sie es erneut. Sie finden Ihre CAN %1unten rechts auf der Vorderseite Ihres Ausweises%2.</translation>
    </message>
    <message>
        <source>You have entered an incorrect, 5-digit Transport PIN.</source>
        <extracomment>ALL_PLATFORMS The wrong Transport PIN was entered on the first attempt. Part 1/2</extracomment>
        <translation>Sie haben eine falsche, 5-stellige Transport-PIN eingegeben.</translation>
    </message>
    <message>
        <source>You have%1 2 further attempts%2 to enter the correct Transport PIN. The 5-digit Transport PIN may be found on the %1bottom left of your PIN letter%2.</source>
        <extracomment>ALL_PLATFORMS The wrong Transport PIN was entered on the first attempt. %1 + %2 are used to emphasize. Part 2/2</extracomment>
        <translation>Sie haben%1 2 weitere Versuche%2, die korrekte Transport-PIN einzugeben. Die 5-stellige Transport-PIN befindet sich %1unten links in Ihrem PIN-Brief%2.</translation>
    </message>
    <message>
        <source>You have entered an %1incorrect, 5-digit Transport PIN 2 times%2.</source>
        <extracomment>ALL_PLATFORMS The wrong Transport PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 1/2</extracomment>
        <translation>Sie haben%1 2 Mal eine falsche, 5-stellige Transport-PIN%2 eingegeben.</translation>
    </message>
    <message>
        <source>%1An incorrect PIN has been entered 2 times%2 at the last use of your ID card.</source>
        <extracomment>ALL_PLATFORMS Once per workflow info text shown when an ID card with one PIN attempt left has been detected. %1 + %2 are used to emphasize. Part 1/2</extracomment>
        <translation>Bei der letzten Nutzung Ihres Online-Ausweises wurde%1 2 Mal eine falsche Karten-PIN%2 eingegeben.</translation>
    </message>
    <message>
        <source>For a 3rd attempt, the%1 6-digit Card Access Number (CAN)%2 must be entered first. You can find your CAN %1in the bottom right on the front of your ID card%2.</source>
        <extracomment>ALL_PLATFORMS Once per workflow info text shown when an ID card with one PIN attempt left has been detected. %1 + %2 are used to emphasize. Part 2/2</extracomment>
        <translation>Für einen 3. Versuch muss die%1 6-stellige Zugangsnummer (CAN)%2 eingegeben werden. Sie finden die CAN %1unten rechts auf der Vorderseite Ihres Ausweises%2.</translation>
    </message>
    <message>
        <source>%1An incorrect PIN has been entered 3 times%2 at the last use of your ID card.</source>
        <extracomment>ALL_PLATFORMS Once per workflow info text shown when a blocked ID card has been detected. %1 + %2 are used to emphasize. Part 1/2</extracomment>
        <translation>Bei der letzten Nutzung Ihres Online-Ausweises wurde%1 3 Mal eine falsche Karten-PIN%2 eingegeben.</translation>
    </message>
    <message>
        <source>Therefore you have to enter the %1PUK%2 first to %1unlock the ID card PIN%2.</source>
        <extracomment>ALL_PLATFORMS Once per workflow info text shown when a blocked ID card has been detected. %1 + %2 are used to emphasize. Part 2/2</extracomment>
        <translation>Deshalb müssen Sie zunächst die %1PUK%2 eingeben um die %1Karten-PIN zu entsperren%2.</translation>
    </message>
</context>
<context>
    <name>governikus::PinResetInformationModel</name>
    <message>
        <source>You may turn to the competent authority and reset the ID card PIN there.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Sie können Ihre PIN direkt bei Ihrer zuständigen Ausweisbehörde zurücksetzen.</translation>
    </message>
    <message>
        <source>Find competent authority</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausweisbehörde finden</translation>
    </message>
    <message>
        <source>You may turn to your competent authority to activate the eID function.</source>
        <extracomment>ALL_PLATFORMS Hint when a workflow failed because the eID function was not activated</extracomment>
        <translation>Sie können die Online-Ausweisfunktion direkt bei Ihrer zuständigen Ausweisbehörde aktivieren.</translation>
    </message>
    <message>
        <source>Request PIN Reset Letter</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>PIN-Rücksetzbrief bestellen</translation>
    </message>
    <message>
        <source>Online via PIN Reset Service</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Online per PIN-Rücksetzdienst</translation>
    </message>
    <message>
        <source>At your competent authority</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Bei Ihrer Ausweisbehörde</translation>
    </message>
    <message>
        <source>You may request a PIN Reset Letter with a new PIN and it&apos;s according activation code on the following website.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Auf der nachfolgenden Webseite können Sie einen PIN-Rücksetzbrief mit neuer PIN und zugehörigem Aktivierungscode bestellen.</translation>
    </message>
    <message>
        <source>You may request a PIN Reset Letter with a new PIN and it&apos;s according activation code on the following website to activate the eID function.</source>
        <extracomment>ALL_PLATFORMS Hint when a workflow failed because the eID function was not activated</extracomment>
        <translation>Auf der nachfolgenden Webseite können Sie einen PIN-Rücksetzbrief mit einer PIN und zugehörigem Aktivierungscode bestellen, um die Online-Ausweisfunktion zu aktivieren.</translation>
    </message>
</context>
<context>
    <name>governikus::ReaderModel</name>
    <message>
        <source>Driver installed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Treiber installiert</translation>
    </message>
    <message>
        <source>The smartcard service of your system is not reachable.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der Smartcard-Dienst Ihres Systems ist nicht erreichbar.</translation>
    </message>
    <message>
        <source>No driver installed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Treiber nicht installiert</translation>
    </message>
    <message>
        <source>Please download and install the driver you can find at: %1</source>
        <extracomment>ALL_PLATFORMS The driver for card reader needs to be installed, the download link is provided in the message.</extracomment>
        <translation>Bitte installieren Sie den Treiber: %1</translation>
    </message>
    <message>
        <source>This card reader is not officially supported and may not work as expected.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der Kartenleser wird nicht offiziell unterstützt und funktioniert möglicherweise nicht richtig.</translation>
    </message>
    <message>
        <source>hh:mm:ss AP</source>
        <extracomment>ALL_PLATFORMS Time format according to https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>HH:mm:ss</translation>
    </message>
    <message>
        <source>The list of card readers was last updated at %1.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Die Liste der Kartenleser wurde zuletzt um %1 aktualisiert.</translation>
    </message>
</context>
<context>
    <name>governikus::RedirectRequest</name>
    <message>
        <source>Cannot reach local %1</source>
        <extracomment>ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.</extracomment>
        <translation>Die lokale %1 ist nicht erreichbar</translation>
    </message>
    <message>
        <source>Your local %1 is not running. Please start your local %1 and try again.</source>
        <extracomment>ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.</extracomment>
        <translation>Ihre lokale %1 läuft nicht. Bitte starten Sie Ihre lokale %1 und versuchen Sie es erneut.</translation>
    </message>
    <message>
        <source>Would you like to try again?</source>
        <extracomment>ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.</extracomment>
        <translation>Möchten Sie es erneut versuchen?</translation>
    </message>
    <message>
        <source>Try again</source>
        <extracomment>ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.</extracomment>
        <translation>Erneut versuchen</translation>
    </message>
</context>
<context>
    <name>governikus::ReleaseInformationModel</name>
    <message>
        <source>No release information present, make sure you are connected to the internet.</source>
        <translation>Es wurden keine Release-Informationen gefunden, bitte stellen Sie sicher, dass Sie mit dem Internet verbunden sind.</translation>
    </message>
</context>
<context>
    <name>governikus::RemoteDeviceModel</name>
    <message>
        <source>Not connected</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Nicht verbunden</translation>
    </message>
    <message>
        <source>Available</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Verfügbar</translation>
    </message>
    <message>
        <source>Paired, but unsupported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Gekoppelt, aber nicht unterstützt</translation>
    </message>
    <message>
        <source>Unsupported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Nicht unterstützt</translation>
    </message>
    <message>
        <source>Not paired</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Nicht gekoppelt</translation>
    </message>
    <message>
        <source>dd.MM.yyyy hh:mm AP</source>
        <extracomment>ALL_PLATFORMS Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>dd.MM.yyyy HH:mm</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Nicht verfügbar</translation>
    </message>
    <message>
        <source>Tap to pair</source>
        <extracomment>MOBILE</extracomment>
        <translation>Tippen zum Koppeln</translation>
    </message>
    <message>
        <source>was</source>
        <extracomment>ALL_PLATFORMS Describes the former name of the device and is shown as: &quot;New_Name (was Old_Name)&quot;</extracomment>
        <translation>ehem.</translation>
    </message>
</context>
<context>
    <name>governikus::RemoteServiceModel</name>
    <message>
        <source>Please pay attention to the display on your other device &quot;%1&quot;.</source>
        <extracomment>MOBILE The smartphone is connected as card reader (SaK) and currently processing an authentication request. The user is asked to pay attention the its screen.</extracomment>
        <translation>Bitte beachten Sie die Anzeige auf Ihrem anderen Gerät &quot;%1&quot;.</translation>
    </message>
    <message>
        <source>Please connect your WiFi to use your smartphone as a card reader (SaC).</source>
        <extracomment>ALL_PLATFORMS The WiFi feature is not enabled but required to use the smartphone as a card reader (SaK).</extracomment>
        <translation>Bitte verbinden Sie sich mit Ihrem WLAN, um Ihr Smartphone als Kartenleser (SaK) benutzen zu können.</translation>
    </message>
    <message>
        <source>The NFC radio standard is required for communication with the ID card.

Please enable NFC to use your smartphone as a card reader (SaC).</source>
        <extracomment>ALL_PLATFORMS NFC is available but not active.</extracomment>
        <translation>Für die Kommunikation mit dem Ausweis wird der Funkstandard NFC benötigt.

Bitte aktivieren Sie NFC, um Ihr Smartphone als Kartenleser (SaK) benutzen zu können.</translation>
    </message>
</context>
<context>
    <name>governikus::RemoteServiceSettings</name>
    <message>
        <source>Smartphone as card reader (SaC)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Smartphone als Kartenleser (SaK)</translation>
    </message>
</context>
<context>
    <name>governikus::SelfData</name>
    <message>
        <source>This data has not been stored in this chip generation.</source>
        <extracomment>ALL_PLATFORMS The requested data is not stored on this chip&apos;s generation.</extracomment>
        <translation>Diese Datengruppe wurde in dieser Chip-Generation nicht gespeichert.</translation>
    </message>
    <message>
        <source>Address</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Adresse</translation>
    </message>
    <message>
        <source>dd.MM.yyyy</source>
        <extracomment>ALL_PLATFORMS Date format according to https://doc.qt.io/qt/qdate.html#toString</extracomment>
        <translation>dd.MM.yyyy</translation>
    </message>
    <message>
        <source>xx.MM.yyyy</source>
        <extracomment>ALL_PLATFORMS Date format according to https://doc.qt.io/qt/qdate.html#toString with unknown day
----------
ALL_PLATFORMS Additional date format with unknown day</extracomment>
        <translation>xx.MM.yyyy</translation>
    </message>
    <message>
        <source>xx.xx.yyyy</source>
        <extracomment>ALL_PLATFORMS Date format according to https://doc.qt.io/qt/qdate.html#toString with unknown day and month</extracomment>
        <translation>xx.xx.yyyy</translation>
    </message>
    <message>
        <source>Family name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Familienname</translation>
    </message>
    <message>
        <source>Birth name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Geburtsname</translation>
    </message>
    <message>
        <source>Given name(s)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Vorname(n)</translation>
    </message>
    <message>
        <source>Doctoral degree</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Doktorgrad</translation>
    </message>
    <message>
        <source>Date of birth</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Geburtsdatum</translation>
    </message>
    <message>
        <source>Place of birth</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Geburtsort</translation>
    </message>
    <message>
        <source>Document type</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Dokumentenart</translation>
    </message>
    <message>
        <source>Nationality</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Staatsangehörigkeit</translation>
    </message>
    <message>
        <source>Religious / artistic name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ordens- / Künstlername</translation>
    </message>
    <message>
        <source>Issuing country</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ausstellender Staat</translation>
    </message>
    <message>
        <source>Residence permit I</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Nebenbestimmungen I (nur eAT)</translation>
    </message>
    <message>
        <source>Valid until</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Gültig bis</translation>
    </message>
</context>
<context>
    <name>governikus::StateChangePin</name>
    <message>
        <source>You have successfully changed your ID card PIN.</source>
        <extracomment>ALL_PLATFORMS The ID card PIN was changed successfully (1/2).</extracomment>
        <translation>Sie haben Ihre Karten-PIN erfolgreich geändert.</translation>
    </message>
    <message>
        <source>You may now remove your ID card from the device.</source>
        <translation>Sie können nun Ihren Ausweis vom Gerät entfernen.</translation>
    </message>
</context>
<context>
    <name>governikus::StateCheckRefreshAddress</name>
    <message>
        <source>Sending data to provider</source>
        <extracomment>ALL_PLATFORMS Status message after the communication between card and server is completed, the result is being forwarded to the provider.</extracomment>
        <translation>Daten werden an den Anbieter gesendet</translation>
    </message>
</context>
<context>
    <name>governikus::StateConnectCard</name>
    <message>
        <source>The used ID card type is not accepted by the server.</source>
        <extracomment>IOS</extracomment>
        <translation>Der verwendete Ausweistyp wird vom Server nicht akzeptiert.</translation>
    </message>
</context>
<context>
    <name>governikus::StateDidAuthenticateEac1</name>
    <message>
        <source>Provider is being verified</source>
        <extracomment>ALL_PLATFORMS Status message after the PIN was entered, Terminal Authentication.</extracomment>
        <translation>Anbieter wird geprüft</translation>
    </message>
</context>
<context>
    <name>governikus::StateDidAuthenticateEac2</name>
    <message>
        <source>eID is being verified</source>
        <extracomment>ALL_PLATFORMS Status message after the PIN was entered, Card Authentication.</extracomment>
        <translation>eID wird geprüft</translation>
    </message>
</context>
<context>
    <name>governikus::StateEnterPacePassword</name>
    <message>
        <source>Access denied.</source>
        <extracomment>IOS The current session was interrupted because of a wrong password.</extracomment>
        <translation>Zugriff verweigert.</translation>
    </message>
</context>
<context>
    <name>governikus::StateEstablishPaceChannel</name>
    <message>
        <source>The secure channel is opened</source>
        <extracomment>ALL_PLATFORMS First status message after the PIN was entered.</extracomment>
        <translation>Der sichere Kanal wird geöffnet</translation>
    </message>
</context>
<context>
    <name>governikus::StateGetSelfAuthenticationData</name>
    <message>
        <source>Getting results from server</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ergebnisse vom Server abrufen</translation>
    </message>
</context>
<context>
    <name>governikus::StateGetTcToken</name>
    <message>
        <source>Fetch TCToken</source>
        <translation>TCToken abrufen</translation>
    </message>
</context>
<context>
    <name>governikus::StateTransmit</name>
    <message>
        <source>Reading data</source>
        <extracomment>ALL_PLATFORMS Status message after the PIN was entered, communication between eID server and card is running.</extracomment>
        <translation>Daten werden gelesen</translation>
    </message>
</context>
<context>
    <name>governikus::StateUnfortunateCardPosition</name>
    <message>
        <source>Weak NFC signal</source>
        <extracomment>The NFC signal is weak or unstable. The scan is stopped with this information in the iOS dialog.</extracomment>
        <translation>Schwacher NFC-Empfang</translation>
    </message>
</context>
<context>
    <name>governikus::Survey</name>
    <message>
        <source>Vendor</source>
        <translation>Hersteller</translation>
    </message>
    <message>
        <source>Model Name</source>
        <translation>Modellname</translation>
    </message>
    <message>
        <source>Model Number</source>
        <translation>Modellnummer</translation>
    </message>
    <message>
        <source>Build Number</source>
        <translation>Versionsnummer</translation>
    </message>
    <message>
        <source>OS version</source>
        <translation>Betriebssystemversion</translation>
    </message>
    <message>
        <source>Kernel version</source>
        <translation>Kernel-Version</translation>
    </message>
    <message>
        <source>Max. NFC Packet Length</source>
        <translation>Max. NFC Paketlänge</translation>
    </message>
    <message>
        <source>%1 Version</source>
        <translation>%1-Version</translation>
    </message>
    <message>
        <source>NFC Tag Type</source>
        <translation>NFC-Tagtyp</translation>
    </message>
</context>
<context>
    <name>governikus::TrayIcon</name>
    <message>
        <source>Application was started.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Die Anwendung wurde gestartet.</translation>
    </message>
    <message>
        <source>Open</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Öffnen</translation>
    </message>
    <message>
        <source>Quit %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>%1 beenden</translation>
    </message>
</context>
<context>
    <name>governikus::UiPluginProxy</name>
    <message>
        <source>Reverse proxy plugin is enabled</source>
        <translation>Reverse-Proxy-Plugin ist aktiviert</translation>
    </message>
</context>
<context>
    <name>governikus::UiPluginQml</name>
    <message>
        <source>Information</source>
        <translation>Information</translation>
    </message>
</context>
<context>
    <name>governikus::UiPluginWebService</name>
    <message>
        <source>An unknown program uses the required port (%1). Please exit the other program and try again!</source>
        <extracomment>ALL_PLATFORMS An unknown program is using the local port on which the AA listens.</extracomment>
        <translation>Eine unbekannte Anwendung verwendet den benötigten Port (%1) bereits. Bitte beenden Sie zuerst die andere Anwendung und versuchen Sie es anschließend erneut!</translation>
    </message>
    <message>
        <source>The program (%1) uses the required port (%2). Please close %1 and try again!</source>
        <extracomment>ALL_PLATFORMS A known program is using the local port on which the AA listens.</extracomment>
        <translation>Die Anwendung &quot;%1&quot; verwendet den benötigten Port (%2) bereits. Bitte beenden Sie &quot;%1&quot; und versuchen Sie es anschließend erneut!</translation>
    </message>
    <message>
        <source>You tried to start a newer version (%1) of currently running %2. Please stop the current version (%3) and start again!</source>
        <extracomment>ALL_PLATFORMS The external request to show the UI requested a newer version than the one currently installed.</extracomment>
        <translation>Sie versuchen eine neuere Version (%1) der aktuell laufenden %2 zu starten. Bitte beenden Sie zuerst die andere Version (%3) und versuchen Sie es anschließend erneut!</translation>
    </message>
    <message>
        <source>You tried to start an older version (%1) of currently running %2. Please open the currently running version (%3)!</source>
        <extracomment>ALL_PLATFORMS The external request to show the UI requested an older version than the one currently installed.</extracomment>
        <translation>Sie versuchen eine ältere Version (%1) der aktuell laufenden %2 zu starten. Bitte öffnen Sie die aktuell laufende Version (%3)!</translation>
    </message>
    <message>
        <source>Reverse-Proxy of %1 is started and this instance cannot rebind port. Please ask your administrator!</source>
        <translation>Der Reverse-Proxy der %1 ist gestartet und diese Instanz konnte nicht auf einen anderen Port wechseln. Bitte wenden Sie sich an Ihren Administrator!</translation>
    </message>
    <message>
        <source>Cannot start authentication</source>
        <extracomment>ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.</extracomment>
        <translation>Ausweisvorgang kann nicht gestartet werden</translation>
    </message>
    <message>
        <source>An operation is already in progress.</source>
        <extracomment>ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.</extracomment>
        <translation>Ein Vorgang ist bereits in Arbeit.</translation>
    </message>
    <message>
        <source>Would you like to try again?</source>
        <extracomment>ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.</extracomment>
        <translation>Möchten Sie es erneut versuchen?</translation>
    </message>
    <message>
        <source>Try again</source>
        <extracomment>ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.</extracomment>
        <translation>Erneut versuchen</translation>
    </message>
    <message>
        <source>The browser connection was lost.</source>
        <extracomment>ALL_PLATFORMS No HTTP connection present.</extracomment>
        <translation>Die Verbindung zum Browser ging verloren.</translation>
    </message>
    <message>
        <source>Invalid request (%1)</source>
        <extracomment>ALL_PLATFORMS Invalid request by the browser, part of an HTML error page</extracomment>
        <translation>Ungültige Anfrage (%1)</translation>
    </message>
    <message>
        <source>Your browser sent a request that couldn&apos;t be interpreted.</source>
        <extracomment>ALL_PLATFORMS Invalid request by the browser, part of an HTML error page</extracomment>
        <translation>Ihr Browser hat eine Anfrage gesendet, die nicht interpretiert werden konnte.</translation>
    </message>
    <message>
        <source>Error message:</source>
        <extracomment>ALL_PLATFORMS Invalid request by the browser, part of an HTML error page</extracomment>
        <translation>Fehlermeldung:</translation>
    </message>
    <message>
        <source>Would you like to report this error?</source>
        <extracomment>ALL_PLATFORMS Invalid request by the browser, part of an HTML error page</extracomment>
        <translation>Möchten Sie diesen Fehler melden?</translation>
    </message>
    <message>
        <source>Report now</source>
        <extracomment>ALL_PLATFORMS Invalid request by the browser, part of an HTML error page</extracomment>
        <translation>Jetzt melden</translation>
    </message>
    <message>
        <source>The connection to the browser was lost while redirecting to the provider (%1). No automatic redirection could be performed.</source>
        <extracomment>ALL_PLATFORMS The connection to the browser was lost/timed out..</extracomment>
        <translation>Bei der Weiterleitung zum Anbieter (%1) ging die Verbindung zum Browser verloren. Es konnte keine automatische Weiterleitung erfolgen.</translation>
    </message>
</context>
<context>
    <name>governikus::WorkflowModel</name>
    <message>
        <source>%1 error report - %2</source>
        <translation>%1 Fehlerbericht - %2</translation>
    </message>
    <message>
        <source>Contact your local citizens&apos; office (Bürgeramt) to apply for a new ID card or to unblock the ID card.</source>
        <translation>Bitte wenden Sie sich an das Bürgeramt, um ein neues Ausweisdokument zu beantragen oder die Sperrung des Ausweises aufzuheben.</translation>
    </message>
    <message>
        <source>The used ID card is not accepted by the server. Please remove the ID card from your device or card reader and use a suitable ID card or contact the service provider.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Der verwendete Ausweistyp wird vom Server nicht akzeptiert. Bitte entfernen Sie den Ausweis vom Gerät oder Kartenleser und verwenden Sie einen geeigneten Ausweis oder kontaktieren Sie den Anbieter.</translation>
    </message>
    <message>
        <source>Error code: %1</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Fehlercode: %1</translation>
    </message>
</context>
</TS>
