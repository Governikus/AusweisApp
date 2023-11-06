Commands
--------
Your application (client) can send some commands (**cmd**) to
control the |AppName|. The |AppName| (server) will send
some proper :doc:`messages` during the whole workflow or as an
answer to your command.




.. _get_info:

GET_INFO
^^^^^^^^
Returns information about the current installation of |AppName|.

The |AppName| will send an :ref:`info` message as an answer.


.. code-block:: json

  {"cmd": "GET_INFO"}




.. _get_status:

GET_STATUS
^^^^^^^^^^
Returns information about the current workflow and state of |AppName|.

The |AppName| will send a :ref:`status` message as an answer.

.. versionadded:: 1.24.0
   Support of GET_STATUS command in :ref:`api_level` **2**.


.. code-block:: json

  {"cmd": "GET_STATUS"}




.. _get_api_level:

GET_API_LEVEL
^^^^^^^^^^^^^
Returns information about the available and current API level.

The |AppName| will send an :ref:`api_level` message as an answer.


.. code-block:: json

  {"cmd": "GET_API_LEVEL"}




.. _set_api_level:

SET_API_LEVEL
^^^^^^^^^^^^^
Set supported API level of your application.

If you initially develop your application against the
|AppName| SDK you should check with :ref:`get_api_level`
the highest supported level and set this value with this command
if you connect to the SDK. This will set the SDK to act with
the defined level even if a newer level is available.

The |AppName| will send an :ref:`api_level` message as an answer.


  - **level**:
    Supported API level of your app.

.. code-block:: json

  {
    "cmd": "SET_API_LEVEL",
    "level": 1
  }




.. _get_reader:

GET_READER
^^^^^^^^^^
Returns information about the requested reader.

If you explicitly want to ask for information of a known
reader name you can request it with this command.

The |AppName| will send a :ref:`reader` message as an answer.


  - **name**:
    Name of the reader.

.. code-block:: json

  {
    "cmd": "GET_READER",
    "name": "NAME OF THE READER"
  }




.. _get_reader_list:

GET_READER_LIST
^^^^^^^^^^^^^^^
Returns information about all connected readers.

If you explicitly want to ask for information of all connected
readers you can request it with this command.

The |AppName| will send a :ref:`reader_list` message as an answer.


.. code-block:: json

  {"cmd": "GET_READER_LIST"}




.. _run_auth:

RUN_AUTH
^^^^^^^^
Starts an authentication.

The |AppName| will send an :ref:`auth` message when the authentication is started.

The system dialog on iOS can be customized by **messages**.
This dialog won't be stopped by default after an :ref:`enter_pin`, :ref:`enter_can`
and :ref:`enter_puk`.
Command :ref:`interrupt` allows to stop the dialog manually, if needed.


.. versionchanged:: 1.24.0
   Parameter **handleInterrupt** removed with :ref:`api_level` v2 and defaults to ``false``.

.. versionadded:: 1.24.0
   Parameter **status** added.

.. versionadded:: 1.22.3
   Parameter **developerMode** added.

.. versionadded:: 1.22.1
   Parameter **handleInterrupt** and **messages** added.

.. deprecated:: 1.22.1
   Parameter **handleInterrupt** has been removed with :ref:`api_level` v2
   and defaults to ``false``.


  - **tcTokenURL**:
    URL to the TcToken. This is equal to the desktop style activation URL.
    *(http://127.0.0.1:24727/eID-Client?tcTokenURL=)*

  - **developerMode**: True to enable "Developer Mode" for test cards and disable some security
    checks according to BSI TR-03124-1, otherwise false. (optional, default: false)

  - **handleInterrupt**: True to automatically handle system dialog on iOS, otherwise false.
    :ref:`api_level` v1 only. (optional, default: false)

  - **status**: True to enable automatic :ref:`status` messages, otherwise false.
    :ref:`api_level` v2 only. (optional, default: true)

  - **messages**: Messages for the system dialog on iOS. (optional, default: empty)

    - **sessionStarted**: Shown if scanning is started.

    - **sessionFailed**: Shown if communication was stopped with an error.

    - **sessionSucceeded**: Shown if communication was stopped successfully.

    - **sessionInProgress**: Shown if communication is in progress. This message
      will be appended with current percentage level.

.. code-block:: json

  {
    "cmd": "RUN_AUTH",
    "tcTokenURL": "https://test.governikus-eid.de/AusweisAuskunft/WebServiceRequesterServlet",
    "developerMode": false,
    "handleInterrupt": false,
    "status": true,
    "messages":
        {
         "sessionStarted": "Please place your ID card on the top of the device's back side.",
         "sessionFailed": "Scanning process failed.",
         "sessionSucceeded": "Scanning process has been finished successfully.",
         "sessionInProgress": "Scanning process is in progress."
        }
  }

.. note::
  This command is allowed only if the |AppName| has no running
  authentication or other workflow. Otherwise you will get
  a :ref:`bad_state` message as an answer.




.. _run_change_pin:

RUN_CHANGE_PIN
^^^^^^^^^^^^^^
Starts a change PIN workflow.

The |AppName| will send a :ref:`change_pin` message when the workflow is started.

The system dialog on iOS can be customized by **messages**.
This dialog won't be stopped by default after an :ref:`enter_pin`, :ref:`enter_can`,
:ref:`enter_new_pin` and :ref:`enter_puk`.
Command :ref:`interrupt` allows to stop the dialog manually, if needed.


.. versionchanged:: 1.24.0
   Parameter **handleInterrupt** removed with :ref:`api_level` v2 and defaults to ``false``.

.. versionadded:: 1.24.0
   Parameter **status** added.

.. versionadded:: 1.22.1
   Parameter **handleInterrupt** and **messages** added.

.. deprecated:: 1.22.1
   Parameter **handleInterrupt** has been removed with :ref:`api_level` v2
   and defaults to ``false``.

.. versionadded:: 1.22.0
   Support of RUN_CHANGE_PIN command.


  - **handleInterrupt**: True to automatically handle system dialog on iOS, otherwise false.
    :ref:`api_level` v1 only. (optional, default: false)

  - **status**: True to enable automatic :ref:`status` messages, otherwise false.
    :ref:`api_level` v2 only. (optional, default: true)

  - **messages**: Messages for the system dialog on iOS. (optional, default: empty)

    - **sessionStarted**: Shown if scanning is started.

    - **sessionFailed**: Shown if communication was stopped with an error.

    - **sessionSucceeded**: Shown if communication was stopped successfully.

    - **sessionInProgress**: Shown if communication is in progress. This message
      will be appended with current percentage level.

.. code-block:: json

  {
    "cmd": "RUN_CHANGE_PIN",
    "handleInterrupt": false,
    "status": true,
    "messages":
        {
         "sessionStarted": "Please place your ID card on the top of the device's back side.",
         "sessionFailed": "Scanning process failed.",
         "sessionSucceeded": "Scanning process has been finished successfully.",
         "sessionInProgress": "Scanning process is in progress."
        }
  }

.. note::
  This command is allowed only if the |AppName| has no running
  authentication or other workflow. Otherwise you will get
  a :ref:`bad_state` message as an answer.




.. _get_access_rights:

GET_ACCESS_RIGHTS
^^^^^^^^^^^^^^^^^
Returns information about the requested access rights.

The |AppName| will send an :ref:`access_rights` message as an answer.


.. code-block:: json

  {"cmd": "GET_ACCESS_RIGHTS"}

.. note::
  This command is allowed only if the |AppName| sends an initial
  :ref:`access_rights` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _set_access_rights:

SET_ACCESS_RIGHTS
^^^^^^^^^^^^^^^^^
Set effective access rights.

By default the **effective** access rights are **optional** + **required**.
If you want to enable or disable some **optional** access rights you can
send this command to modify the **effective** access rights.

The |AppName| will send an :ref:`access_rights` message as an answer.


  - **chat**:
    List of enabled **optional** access rights. If you send an empty **[]**
    all **optional** access rights are disabled.

.. code-block:: json

  {
    "cmd": "SET_ACCESS_RIGHTS",
    "chat": []
  }

.. code-block:: json

  {
    "cmd": "SET_ACCESS_RIGHTS",
    "chat": ["FamilyName"]
  }

.. note::
  This command is allowed only if the |AppName| sends an initial
  :ref:`access_rights` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.

.. seealso::
  List of possible access rights are listed in :ref:`access_rights`.




.. _set_card:

SET_CARD
^^^^^^^^
Insert "virtual" card.

Since :ref:`api_level` **2** it is possible to provide a "virtual"
card. The information whether this is possible will be indicated
in a :ref:`reader` message.


.. versionadded:: 1.24.0
   This command was introduced in :ref:`api_level` **2**.

.. versionadded:: 1.26.4
   Parameter **keys** added.


- **name**: Name of the :ref:`reader`.

- **simulator**: Specific data for :doc:`simulator`. (optional)

  - **files**: Content of card :ref:`filesystem`.

  - **keys**: Keys of card :ref:`filesystem`.

.. code-block:: json

  {
    "cmd": "SET_CARD",
    "name": "reader name",
    "simulator":
    {
       "files": [],
       "keys": []
    }
  }

.. note::
  This command is allowed only if the |AppName| sends an initial
  :ref:`insert_card` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _get_certificate:

GET_CERTIFICATE
^^^^^^^^^^^^^^^
Returns the certificate of current authentication.

The |AppName| will send a :ref:`certificate` message as an answer.


.. code-block:: json

  {"cmd": "GET_CERTIFICATE"}

.. note::
  This command is allowed only if the |AppName| sends an initial
  :ref:`access_rights` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _cancel:

CANCEL
^^^^^^
Cancel the whole workflow.

If your application sends this command the |AppName| will cancel the
workflow. You can send this command in any state of a running workflow
to abort it.


.. code-block:: json

  {"cmd": "CANCEL"}

.. note::
  This command is allowed only if the |AppName| started an authentication or
  the :ref:`change_pin` workflow.
  Otherwise you will get a :ref:`bad_state` message as an answer.




.. _accept:

ACCEPT
^^^^^^
Accept the current state.

If the |AppName| will send the message :ref:`access_rights` the user
needs to **accept** or **cancel**. So the workflow is paused until
your application sends this command to accept the requested information.

If the user does not accept the requested information your application
needs to send the command :ref:`cancel` to abort the whole workflow.

This command will be used later for additional requested information
if the |AppName| needs to pause the workflow. In :ref:`api_level` v1
only :ref:`access_rights` needs to be accepted.


.. code-block:: json

  {"cmd": "ACCEPT"}

.. note::
  This command is allowed only if the |AppName| sends an initial
  :ref:`access_rights` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _interrupt:

INTERRUPT
^^^^^^^^^
Interrupts current system dialog on iOS.

If your application provides **false** to parameter **handleInterrupt** in
:ref:`run_auth` or :ref:`run_change_pin` and you need to request more information
from the user, you need to interrupt the system dialog manually.

This command will be used later for additional information if your
application needs to interrupt the workflow. Currently only
the iOS system dialog can be interrupted.

.. versionadded:: 1.22.1
   Support of INTERRUPT command.


.. code-block:: json

  {"cmd": "INTERRUPT"}

.. note::
  This command is allowed only if the |AppName| sends a :ref:`enter_pin`,
  :ref:`enter_can`, :ref:`enter_new_pin` or :ref:`enter_puk` message.
  Otherwise you will get a :ref:`bad_state` message as an answer.




.. _set_pin:

SET_PIN
^^^^^^^
Set PIN of inserted card.

If the |AppName| sends message :ref:`enter_pin` you need
to send this command to unblock the card with the PIN.

The |AppName| will send an :ref:`enter_pin` message on error
or message :ref:`enter_can` if the retryCounter of the card
is decreased to **1**.
For detailed information see message :ref:`enter_pin`.

If the PIN was correct, the workflow will continue.

If the last attempt to enter the PIN failed, |AppName|
will send the message :ref:`enter_puk` as the retryCounter
is decreased to **0**.

.. versionchanged:: 1.16.0
   The parameter "value" must be omitted if the used :ref:`reader` has a
   keypad.


- **value**: The Personal Identification Number (PIN) of the card.
  This must be 6 digits in an :ref:`auth` workflow. If a
  :ref:`change_pin` workflow is in progress the value must
  be 5 or 6 digits because of a possible transport PIN.
  If the :ref:`reader` has a keypad this parameter must be omitted.

.. code-block:: json

  {
    "cmd": "SET_PIN",
    "value": "123456"
  }

.. note::
  This command is allowed only if the |AppName| sends an initial
  :ref:`enter_pin` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _set_new_pin:

SET_NEW_PIN
^^^^^^^^^^^
Set new PIN of inserted card.

If the |AppName| sends message :ref:`enter_new_pin` you need
to send this command to set the new PIN of the card.

.. versionadded:: 1.22.0
   Support of SET_NEW_PIN command.


- **value**: The new personal identification number (PIN) of the card.
  This must be 6 digits if the :ref:`reader` has no keypad, otherwise
  this parameter must be omitted.

.. code-block:: json

  {
    "cmd": "SET_NEW_PIN",
    "value": "123456"
  }

.. note::
  This command is allowed only if the |AppName| sends an initial
  :ref:`enter_new_pin` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _set_can:

SET_CAN
^^^^^^^
Set CAN of inserted card.

If the |AppName| sends message :ref:`enter_can` you need
to send this command to unblock the last retry of :ref:`set_pin`.

The |AppName| will send an :ref:`enter_can` message on error.
Otherwise the workflow will continue with :ref:`enter_pin`.

.. versionchanged:: 1.16.0
   The parameter "value" must be omitted if the used :ref:`reader` has a
   keypad.


- **value**: The Card Access Number (CAN) of the card.
  This must be 6 digits if the :ref:`reader` has no keypad, otherwise
  this parameter must be omitted.

.. code-block:: json

  {
    "cmd": "SET_CAN",
    "value": "123456"
  }

.. note::
  This command is allowed only if the |AppName| sends an initial
  :ref:`enter_can` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _set_puk:

SET_PUK
^^^^^^^
Set PUK of inserted card.

If the |AppName| sends message :ref:`enter_puk` you need
to send this command to unblock :ref:`set_pin`.

The |AppName| will send an :ref:`enter_puk` message on error
or if the PUK is operative.
Otherwise the workflow will continue with :ref:`enter_pin`.
For detailed information see message :ref:`enter_puk`.

.. versionchanged:: 1.16.0
   The parameter "value" must be omitted if the used :ref:`reader` has a
   keypad.


- **value**: The Personal Unblocking Key (PUK) of the card.
  This must be 10 digits if the :ref:`reader` has no keypad, otherwise
  this parameter must be omitted.

.. code-block:: json

  {
    "cmd": "SET_PUK",
    "value": "1234567890"
  }

.. note::
  This command is allowed only if the |AppName| sends an initial
  :ref:`enter_puk` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.
