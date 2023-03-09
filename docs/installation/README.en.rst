English
=======

Windows
-------

Start the installer of AusweisApp2 using the command line to configure the
installation process and preset system-wide default settings.
The return value of msiexec indicates the result of the installation [#msiexecreturnvalues]_.
In addition to the usual arguments [#standardarguments]_, the following command
contains all supported arguments, which are explained below.

.. code-block:: winbatch

  msiexec /i AusweisApp2-X.YY.Z.msi /quiet INSTALLDIR="C:\AusweisApp2" SYSTEMSETTINGS=false DESKTOPSHORTCUT=false PROXYSERVICE=false AUTOSTART=false AUTOHIDE=false REMINDTOCLOSE=false ASSISTANT=false TRANSPORTPINREMINDER=false CUSTOMPROXYTYPE="HTTP" CUSTOMPROXYHOST="proxy.example.org" CUSTOMPROXYPORT=1337 UPDATECHECK=false ONSCREENKEYBOARD=true SHUFFLESCREENKEYBOARD=true SECURESCREENKEYBOARD=true HISTORY=false ENABLECANALLOWED=true SKIPRIGHTSONCANALLOWED=true LAUNCH=true

INSTALLDIR
  States the installation directory. If not specified, the folder
  "C:\\Program Files\\AusweisApp2" is used.

SYSTEMSETTINGS
  Concerns the settings of firewall rules of the Windows Firewall. When not
  specifying the argument, firewall rules are created (true). By indicating
  SYSTEMSETTINGS=false, no firewall rules are created.

DESKTOPSHORTCUT
  By specifying DESKTOPSHORTCUT=false, no desktop shortcut is created. Without
  specifying the argument, the desktop shortcut is created for all users (true).

PROXYSERVICE
  The proxy service is required to enable the parallel operation of several
  entities of AusweisApp2. The proxy service monitors port 24727 (defined in
  BSI TR-03124-1) and forwards requests to the local AusweisApp2 instances.
  The Discovery messages (amendment to BSI TR-03112-6 - IFD Service - Chapter
  3) are not forwarded, so that SaC devices cannot be recognized or used in
  this operating mode. Not specified, the proxy service will be installed
  automatically if Terminal Services is installed and the system is running
  in application server mode.

AUTOSTART
  Setting AUTOSTART=true creates autostart entry for all users. Users are unable
  to deactivate the autostart function in the AusweisApp2. Not specified, no
  autostart entry is created (false). In that case, users are able to activate the
  autostart function in the AusweisApp2.

AUTOHIDE
  Concerns the automatic minimization after a successful authentication. Not
  specified, it is activated (true). Setting AUTOHIDE=false, it is deactivated.
  Users can adjust this setting to their preferences.

REMINDTOCLOSE
  Closing the AusweisApp2 by clicking on the X, the user is notified that only the
  user interface is closed and that the AusweisApp2 is still available in the info
  tray. At this point, it is possible to prevent future notifications. Setting
  REMINDTOCLOSE=false deactivates this notification from the outset. Not
  specified, it is activated (true).

ASSISTANT
  Starting the AusweisApp2 for the first time, the user interface is displayed and
  the installation wizard is shown. With each subsequent start, the AusweisApp2
  is started in the background, without the installation wizard being shown. By
  indicating ASSISTANT=false, the AusweisApp2 is started in the background without
  the installation wizard from the outset. Not specified, the installation
  wizard is activated (true).

TRANSPORTPINREMINDER
  Prior to the first authentication, the user is asked once whether they have
  changed their Transport PIN. Setting TRANSPORTPINREMINDER=false deactivates this
  reminder. Not specified, the reminder is activated (true).

CUSTOMPROXYTYPE
  Part of a proxy configuration. Valid values are SOCKS5 and HTTP.
  All proxy parameters have to be set to use the proxy (see
  CUSTOMPROXYHOST and CUSTOMPROXYPORT). You can disable the proxy after installation
  with a checkbox in the settings.

CUSTOMPROXYHOST
  Part of a proxy configuration. Sets the Host of the proxy. All proxy parameters have
  to be set to use the proxy (see CUSTOMPROXYTYPE and CUSTOMPROXYPORT).
  You can disable the proxy after installation with a checkbox in the settings.

CUSTOMPROXYPORT
  Part of a proxy configuration. Sets the port of the proxy. Only values between
  1 and 65536 are valid. All proxy parameters have to be set to use the proxy (see
  CUSTOMPROXYTYPE and CUSTOMPROXYHOST). You can disable the proxy after installation
  with a checkbox in the settings.

UPDATECHECK
  Upon opening the user interface of the AusweisApp2, an update check is started,
  provided that at least 24 hours have elapsed since the last update check. If a
  newer version is available, the user is notified accordingly. Setting
  UPDATECHECK to false or true deactivates or activates the update check
  respectively. Users are unable to change this setting in the AusweisApp2. Not
  specified, the update check is activated, but users can adjust the settings.
  The UPDATECHECK parameter affects neither updates of the service
  provider list nor updates of card reader information.

ONSCREENKEYBOARD
  An on-screen keyboard is available to enter PIN, CAN or PUK. It is deactivated or
  activated by setting ONSCREENKEYBOARD to false or true. Users are able to adjust
  the setting.

SHUFFLESCREENKEYBOARD
  If the on-screen keyboard is activated, the number keys can be arranged at random.
  By setting SHUFFLESCREENKEYBOARD to false or true, the random arrangement can be
  deactivated or activated. Users are able to adjust the setting.

SECURESCREENKEYBOARD
  If the on-screen keyboard is activated, the animation of the number keys can be
  disabled. By setting SECURESCREENKEYBOARD to false or true, the animation can be
  activated or deactivated. Users are able to adjust the setting.

HISTORY
  Each authentication is saved in the history. No personal data is saved, only the
  time of authentication, the provider and the selected fields (without
  content). Indicating HISTORY as false or true, the history function is
  deactivated or activated. Users are able to adjust the settings.

ENABLECANALLOWED
  Enables support for the CAN allowed mode. If the provider got issued a corresponding authorization
  certificate the ID card can be read by entering the CAN instead of the PIN.

SKIPRIGHTSONCANALLOWED
  Skips the page with the authorization certificate in the CAN allowed mode and asks directly for
  the CAN.

LAUNCH
  Starts the AusweisApp2 after the installation has finished.

Alternatively, Orca [#orca]_ can be used to create an MST file that defines the
above arguments. The arguments are available in the "Directory" and "Property"
tables. The MST file can be transferred with the following command:

.. code-block:: winbatch

  msiexec /i AusweisApp2-X.YY.Z.msi /quiet TRANSFORMS=file.mst

In order to optimize the start of the AusweisApp2 on systems with no graphics
acceleration, the system variable "QT_QUICK_BACKEND" can be set to the value
"software". In this case, the AusweisApp2 does not attempt to use graphics
acceleration and starts directly with the alternative software renderer.

macOS
-----

MacOS does not provide a command line installation. However, some of the above
settings can be specified system-wide by a plist file in the
/Library/Preferences directory. This plist file must be manually stored by the
administrator of the system and will be used by all (future) installations of
AusweisApp2. All not mentioned settings are not supported on macOS. The name of
the file must be "com.governikus.AusweisApp2.plist". The content is shown below:

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
    <key>visualPrivacy</key>
    <true/>
    <key>history.enable</key>
    <false/>
    <key>enableCanAllowed</key>
    <true/>
    <key>skipRightsOnCanAllowed</key>
    <true/>
  </dict>
  </plist>

The description for each value is applicable for both Windows and macOS,
although the naming of the attributes differs, as shown in the following table:

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
visualPrivacy           SECURESCREENKEYBOARD
history.enable          HISTORY
enableCanAllowed        ENABLECANALLOWED
skipRightsOnCanAllowed  SKIPRIGHTSONCANALLOWED
======================= =======================

It might be necessary to force a reload of the data cached by the operating
system: :code:`killall -u $USER cfprefsd`

.. [#msiexecreturnvalues] https://docs.microsoft.com/en-us/windows/desktop/msi/error-codes
.. [#standardarguments] https://docs.microsoft.com/en-us/windows/desktop/msi/standard-installer-command-line-options
.. [#orca] https://docs.microsoft.com/en-us/windows/desktop/Msi/orca-exe


Operational Environment Requirements
------------------------------------

Required authorization for installation and execution
'''''''''''''''''''''''''''''''''''''''''''''''''''''

Administrator privileges are required to install the AusweisApp2.

The execution of the AusweisApp2 does not require administrator privileges.

Used network ports
''''''''''''''''''

All network ports used by the AusweisApp2 are listed in :numref:`porttable_en`.
:numref:`communicationmodel_en` shows a schematic representation of the
individual connections made by the AusweisApp2.

The AusweisApp2 starts a HTTP-Server on port 24727.
The server binds only to the localhost network interface.
The availability of the local server is necessary for the online eID function,
because providers will redirect the user with a HTTP redirect to the
local server to continue the authentication process in the AusweisApp2 (eID1).
The server is also used to offer other local applications to use the
AusweisApp2 via a websocket interface (SDK function, eID-SDK).
Therefore local incoming network connections to TCP Port 24727 must be
permitted.

If the proxy service is activated, the AusweisApp2 proxy takes over the server
functions of AusweisApp2 on port 24727. The entities of AusweisApp2 recognize
the proxy and use a free random port in this case to which the proxy forwards
the requests.

Broadcast on UDP port 24727 in the local subnet have to be receivable by the
AusweisApp2 to use the "Smartphone as Card Reader" functionality.
It may be necessary to deactive AP isolation on your router.

.. _communicationmodel_en:
.. figure:: CommunicationModel_en.pdf

    Communication model of the AusweisApp2

The installer of the AusweisApp2 provides an option to register all needed
firewall rules in the Windows Firewall.
If the rules are not registered, the user will be prompted by the Windows
Firewall to allow the outgoing connections once the AusweisApp2 tries to
connect to a server.
These prompts are suppressed by registering the firewall rules during
installation.
No rules have to be added to the Windows Firewall for the local connections
eID1 and eID-SDK  (when using the standard settings).

In table :numref:`firewalltable_en` all firewall rules registered by the
installer are listed.

TLS connections
'''''''''''''''

Transmitted TLS certificates are solely validated via the interlacing with
the authorization certificate issued by the german eID PKI.
CA certificates in the Windows truststore are thus ignored.
It is therefore generally not possible to use the AusweisApp2 behind a
TLS termination proxy.

.. raw:: latex

    \begin{landscape}

.. _porttable_en:
.. csv-table:: Network connections of the AusweisApp2
   :header: "Reference", "Protocol", "Port", "Direction", "Optional", "Purpose", "Note"
   :widths: 8, 8, 8, 8, 8, 35, 25

   "eID1",    TCP, 24727 [#aa2proxy]_,  "incoming", "no",  "Online eID function, eID activation [#TR-03124]_",                                                                                "Only accessible from localhost [#TR-03124]_"
   "eID2",    TCP, 443 [#eidports]_,    "outgoing", "no",  "Online eID function, connection to the provider, TLS-1-2 channel [#TR-03124]_",                                                   "TLS certificates interlaced with authorization certificate [#TR-03124]_"
   "eID3",    TCP, 443 [#eidports]_,    "outgoing", "no",  "Online eID function, connection to eID-Server, TLS-2 channel [#TR-03124]_",                                                       "TLS certificates interlaced with authorization certificate [#TR-03124]_"
   "eID-SDK", TCP, 24727 [#aa2proxy]_,  "incoming", "no",  "Usage of the SDK functionality",                                                                                                  "Only accessible from localhost [#TR-03124]_"
   "SaC1",    UDP, 24727 [#aa2proxy]_,  "incoming", "yes", "Smartphone as Card Reader, detection [#TR-03112]_",                                                                               "Broadcasts"
   "SaC2",    TCP, ,                    "outgoing", "yes", "Smartphone as Card Reader, usage [#TR-03112]_",                                                                                   "Connection in local subnet"
   "Update",  TCP, 443,                 "outgoing", "yes", "Updates [#govurl]_ of provider and card reader information as well as information on new AusweisApp2 versions [#updatecheck]_ .", "TLS certificates will be validated against CA certificates included in the AusweisApp2. CA certificates provided by the OS are ignored."

.. [#aa2proxy] Or a random port when using AusweisApp2 proxy.
.. [#TR-03124] See TR-03124 specification from the BSI
.. [#eidports] Port 443 is used for the initial contact with the provider or
   eID server. Due to configuration of the service on the service provider's
   behalf, any other port might be used by forwarding.
.. [#TR-03112] See TR-03112-6 specifiaction from the BSI
.. [#govurl] All updates are based on the URL https://appl.governikus-asp.de/ausweisapp2/
.. [#updatecheck] Automatic checks for new AusweisApp2 versions can be deactivated, see commandline parameter
    UPDATECHECK.

.. _firewalltable_en:
.. csv-table:: Firewall rules of the AusweisApp2
   :header: "Name", "Protocol", "Port", "Direction", "Connection reference"
   :widths: 25, 15, 15, 15, 30
   :align: left

   "AusweisApp2-Firewall-Rule", TCP, \*, "outgoing", "eID2, eID3, SaC2, Update"
   "AusweisApp2-SaC", UDP, 24727, "incoming", "SaC1"

.. raw:: latex

    \end{landscape}
