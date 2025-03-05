Changelog
=========

Version 2.3.0
^^^^^^^^^^^^^
* Removed support for iOS 14 and 15.
* Updated Android NDK to r27c (27.2.12479018).
* Renamed functions from ``ausweisapp2_`` to ``ausweisapp_`` on iOS.
* Renamed header from ``AusweisApp2.h`` to ``AusweisApp.h`` on iOS.
* Added environment variable ``AUSWEISAPP_AUTOMATIC_SIMULATOR``.
* Renamed environment variables ``AUSWEISAPP2_AUTOMATIC_*`` to ``AUSWEISAPP_AUTOMATIC_*``.


Version 2.2.2
^^^^^^^^^^^^^
* Added Android ABIs armeabi-v7a and x86_64 in addition to arm64-v8a.


Version 2.2.1
^^^^^^^^^^^^^
* Stabilized fast SDK restarts on iOS.
* Support 16 KB page sizes on Android.
* Updated Android NDK to r27b (27.1.12297006).


Version 2.2.0
^^^^^^^^^^^^^
* Removed support for Android 8.
* Introduced the following additions in :ref:`api_level` **3**:

  * The command :ref:`continue_cmd`.

  * The message :ref:`pause_message`.

  * Extended parameter **card** of the message :ref:`reader` to signal an unknown card.

* Added parameter **content** to the :ref:`filesystem` of the Simulator.
* Declared parameter **private** of the Simulator's :ref:`filesystem` deprecated.


Version 1.26.4
^^^^^^^^^^^^^^
* Added parameter **keys** to the command :ref:`set_card`.
* Added parameter **reason** to the message :ref:`change_pin`.


Version 1.26.3
^^^^^^^^^^^^^^
* Added variable "AUSWEISAPP2_AUTOMATIC_DEVELOPERMODE" to the mode :ref:`automatic`.
* Added parameter **reason** to message :ref:`auth`.


Version 1.24.0
^^^^^^^^^^^^^^
* Introduced the following changes in :ref:`api_level` **2**:

  * The commands :ref:`get_status` and :ref:`set_card`.

  * Removed the parameter **handleInterrupt** of the commands :ref:`run_auth` and :ref:`run_change_pin`.

  * Added the parameter **status** to the commands :ref:`run_auth` and :ref:`run_change_pin`.

  * Renamed parameter **reader** to **readers** in message :ref:`reader`

  * Added message :ref:`status`.

* Added mode :ref:`automatic` for Desktop Systems.
* Added parameter **insertable** to message :ref:`reader`.


Version 1.22.3
^^^^^^^^^^^^^^
 * Added parameter **developerMode** to the command :ref:`run_auth`.


Version 1.22.1
^^^^^^^^^^^^^^
* Added parameter **handleInterrupt** and **messages** to the commands :ref:`run_auth` and :ref:`run_change_pin`
  for :ref:`api_level` **1**.
* Added the command :ref:`interrupt`.


Version 1.22.0
^^^^^^^^^^^^^^
* Added the commands :ref:`run_change_pin` and :ref:`set_new_pin`.
* Extended the message :ref:`access_rights` for "CAN allowed right" and "PIN management right".
* Added messages :ref:`change_pin` and :ref:`enter_new_pin`.


Version 1.20.0
^^^^^^^^^^^^^^
* Extended :ref:`access_rights` for write access.


Version 1.16.0
^^^^^^^^^^^^^^
* Changed parameter **value** for the commands :ref:`set_pin`, :ref:`set_can`, and :ref:`set_puk` for readers with
  a keypad.
* Added parameter **keypad** to message :ref:`reader`.


Version 1.14.2
^^^^^^^^^^^^^^
* The message :ref:`set_can` may now be used with the "CAN allowed right".
