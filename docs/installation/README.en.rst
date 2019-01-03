English
=======

Windows
-------

Start the installer of AusweisApp2 using the command line to configure the
installation process and preset system-wide default settings. In addition to the
usual arguments [1]_, the following command contains all supported arguments,
which are explained below.

.. code-block:: winbatch

  msiexec /i AusweisApp2-X.YY.Z.msi /quiet INSTALL_ROOT="C:\AusweisApp2" SYSTEMSETTINGS=false DESKTOPSHORTCUT=false AUTOSTART=false AUTOHIDE=false REMINDTOCLOSE=false ASSISTANT=false TRANSPORTPINREMINDER=false UPDATECHECK=false ONSCREENKEYBOARD=true HISTORY=false

INSTALL_ROOT
  States the installation directory. If not specified, the folder
  "C:\\Program Files (x86)\\AusweisApp2 X.YY.Z" is used.

SYSTEMSETTINGS
  Concerns the settings of firewall rules of the Windows Firewall. When not
  specifying the argument, firewall rules are created (true). By indicating
  SYSTEMSETTINGS=false, no firewall rules are created.

DESKTOPSHORTCUT
  By specifying DESKTOPSHORTCUT=false, no desktop shortcut is created. Without
  specifying the argument, the desktop shortcut is created for all users (true).

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
  changed their transport PIN. Setting TRANSPORTPINREMINDER=false deactivates this
  reminder. Not specified, the reminder is activated (true).

UPDATECHECK
  Upon opening the user interface of the AusweisApp2, an update check is started,
  provided that at least 24 hours have elapsed since the last update check. If a
  newer version is available, the user is notified accordingly. Setting
  UPDATECHECK to false or true deactivates or activates the update check
  respectively. Users are unable to change this setting in the AusweisApp2. Not
  specified, the update check is activated, but users can adjust the settings.

ONSCREENKEYBOARD
  An on-screen keyboard is available to enter PIN, CAN or PUK. It is deactivated or
  activated by setting ONSCREENKEYBOARD to false or true. Users are able to adjust
  the settings.

HISTORY
  Each authentication is saved in the history. No personal data is saved, only the
  time of authentication, the service provider and the selected fields (without
  content). Indicating HISTORY as false or true, the history function is
  deactivated or activated. Users are able to adjust the settings.

Alternatively, Orca [2]_ can be used to create an MST file that defines the
above arguments. The arguments are available in the "Directory" and "Property"
tables. The MST file can be transferred with the following command:

.. code-block:: winbatch

  msiexec /i AusweisApp2-X.YY.Z.msi /quiet TRANSFORMS=file.mst

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
    <key>common.autoUpdateCheck</key>
    <false/>
    <key>common.keylessPassword</key>
    <true/>
    <key>history.enable</key>
    <false/>
  </dict>
  </plist>

The description for each value is applicable for both Windows and macOS,
although the naming of the attributes differs, as shown in the following table:

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

.. [1] https://docs.microsoft.com/en-us/windows/desktop/msi/standard-installer-command-line-options
.. [2] https://docs.microsoft.com/en-us/windows/desktop/Msi/orca-exe
