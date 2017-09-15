Commands
--------
Your application (client) can send some commands (**cmd**) to
control the AusweisApp2. The AusweisApp2 (server) will send
some proper :doc:`messages` during the whole workflow or as an
answer to your command.




.. _get_info:

GET_INFO
^^^^^^^^
Returns information about the current installation of AusweisApp2.

The AusweisApp2 will send an :ref:`info` message as an answer.


.. code-block:: json

  {"cmd": "GET_INFO"}




.. _get_api_level:

GET_API_LEVEL
^^^^^^^^^^^^^
Returns information about the available and current API level.

The AusweisApp2 will send an :ref:`api_level` message as an answer.


.. code-block:: json

  {"cmd": "GET_API_LEVEL"}




.. _set_api_level:

SET_API_LEVEL
^^^^^^^^^^^^^
Set supported API level of your application.

If you initially develop your application against the
AusweisApp2 SDK you should check with :ref:`get_api_level`
the highest supported level and set this value with this command
if you connect to the SDK. This will set the SDK to act with
the defined level even if a newer level is available.

The AusweisApp2 will send an :ref:`api_level` message as an answer.


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

The AusweisApp2 will send a :ref:`reader` message as an answer.


  - **name**:
    Name of the reader.

.. code-block:: json

  {
    "cmd": "GET_READER",
    "name": "NAME OF THE READER"
  }




GET_READER_LIST
^^^^^^^^^^^^^^^
Returns information about all connected readers.

If you explicitly want to ask for information of all connected
readers you can request it with this command.

The AusweisApp2 will send a :ref:`reader_list` message as an answer.


.. code-block:: json

  {"cmd": "GET_READER_LIST"}





.. _run_auth:

RUN_AUTH
^^^^^^^^
Starts an authentication.

The AusweisApp2 will send an :ref:`auth` message when the authentication is started.


  - **tcTokenURL**:
    URL to the TcToken. This is equal to the desktop style activation URL.
    *(http://127.0.0.1:24727/eID-Client?tcTokenURL=)*

.. code-block:: json

  {
    "cmd": "RUN_AUTH",
    "tcTokenURL": "https://test.governikus-eid.de/Autent-DemoApplication/RequestServlet?provider=demo_epa_20&redirect=true"
  }

.. note::
  This command is allowed only if the AusweisApp2 has no running
  authentication. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _get_access_rights:

GET_ACCESS_RIGHTS
^^^^^^^^^^^^^^^^^
Returns information about the requested access rights.

The AusweisApp2 will send an :ref:`access_rights` message as an answer.


.. code-block:: json

  {"cmd": "GET_ACCESS_RIGHTS"}

.. note::
  This command is allowed only if the AusweisApp2 sends an initial
  :ref:`access_rights` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _set_access_rights:

SET_ACCESS_RIGHTS
^^^^^^^^^^^^^^^^^
Set effective access rights.

By default the **effective** access rights are **optional** + **required**.
If you want to enable or disable some **optional** access rights you can
send this command to modify the **effective** access rights.

The AusweisApp2 will send an :ref:`access_rights` message as an answer.


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
  This command is allowed only if the AusweisApp2 sends an initial
  :ref:`access_rights` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.

.. seealso::
  List of possible access rights are listed in :ref:`access_rights`.



.. _get_certificate:

GET_CERTIFICATE
^^^^^^^^^^^^^^^
Returns the certificate of current authentication.

The AusweisApp2 will send a :ref:`certificate` message as an answer.


.. code-block:: json

  {"cmd": "GET_CERTIFICATE"}

.. note::
  This command is allowed only if the AusweisApp2 sends an initial
  :ref:`access_rights` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _cancel:

CANCEL
^^^^^^
Cancel the whole workflow.

If your application sends this command the AusweisApp2 will cancel the
workflow. You can send this command in any state of a running workflow
to abort it.


.. code-block:: json

  {"cmd": "CANCEL"}

.. note::
  This command is allowed only if the AusweisApp2 started an authentication.
  Otherwise you will get a :ref:`bad_state` message as an answer.




.. _accept:

ACCEPT
^^^^^^
Accept the current state.

If the AusweisApp2 will send the message :ref:`access_rights` the user
needs to **accept** or **deny**. So the workflow is paused until
your application sends this command to accept the requested information.

If the user does not accept the requested information your application
needs to send the command :ref:`cancel` to abort the whole workflow.

This command will be used later for additional requested information
if the AusweisApp2 needs to pause the workflow. In :ref:`api_level` v1
only :ref:`access_rights` needs to be accepted.


.. code-block:: json

  {"cmd": "ACCEPT"}

.. note::
  This command is allowed only if the AusweisApp2 sends an initial
  :ref:`access_rights` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _set_pin:

SET_PIN
^^^^^^^
Set PIN of inserted card.

If the AusweisApp2 sends message :ref:`enter_pin` you need
to send this command to unblock the card with the PIN.

The AusweisApp2 will send an :ref:`enter_pin` message on error
or message :ref:`enter_can` if the retryCounter of the card
is decreased to **1**.
For detailed information see message :ref:`enter_pin`.

If the PIN was correct, the workflow will continue.

If the last attempt to enter the PIN failed, AusweisApp2
will send the message :ref:`enter_puk` as the retryCounter
is decreased to **0**.


- **value**: The personal identification number (PIN) of the card.
  This must be 6 digits.

.. code-block:: json

  {
    "cmd": "SET_PIN",
    "value": "123456"
  }

.. note::
  This command is allowed only if the AusweisApp2 sends an initial
  :ref:`enter_pin` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _set_can:

SET_CAN
^^^^^^^
Set CAN of inserted card.

If the AusweisApp2 sends message :ref:`enter_can` you need
to send this command to unblock the last retry of :ref:`set_pin`.

The AusweisApp2 will send an :ref:`enter_can` message on error.
Otherwise the workflow will continue with :ref:`enter_pin`.


- **value**: The card access number (CAN) of the card.
  This must be 6 digits.

.. code-block:: json

  {
    "cmd": "SET_CAN",
    "value": "123456"
  }

.. note::
  This command is allowed only if the AusweisApp2 sends an initial
  :ref:`enter_can` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.




.. _set_puk:

SET_PUK
^^^^^^^
Set PUK of inserted card.

If the AusweisApp2 sends message :ref:`enter_puk` you need
to send this command to unblock :ref:`set_pin`.

The AusweisApp2 will send an :ref:`enter_puk` message on error
or if the PUK is operative.
Otherwise the workflow will continue with :ref:`enter_pin`.
For detailed information see message :ref:`enter_puk`.

- **value**: The personal unblocking key (PUK) of the card.
  This must be 10 digits.

.. code-block:: json

  {
    "cmd": "SET_PUK",
    "value": "1234567890"
  }

.. note::
  This command is allowed only if the AusweisApp2 sends an initial
  :ref:`enter_puk` message. Otherwise you will get a :ref:`bad_state`
  message as an answer.
