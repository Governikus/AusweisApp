Desktop
=======
This chapter deals with the desktop specific properties of the AusweisApp2 SDK.
The AusweisApp2 core is reachable over a **WebSocket** which is running by
default since AusweisApp2 1.16.0. Subsequent sections deal with the SDK
interface itself and explain which steps are necessary in order to communicate
with the AusweisApp2 SDK.



.. _websocket:

WebSocket
---------
The AusweisApp2 uses the same default port as defined in TR-03124-1.
Your application can connect to ``ws://localhost:24727/eID-Kernel`` to
establish a bidirectional connection.

You can check the version of AusweisApp2 by the ``Server`` header of the HTTP
response or by an additional query to get the :ref:`client_status`.

If the WebSocket handshake was successful your application can send :doc:`commands`
and receive :doc:`messages`.
The AusweisApp2 will send an HTTP error 503 "Service Unavailable" if the WebSocket
is disabled.

.. seealso::

  https://tools.ietf.org/html/rfc6455



User installed
^^^^^^^^^^^^^^
Your application can connect to a user installed AusweisApp2. If the
user already has an active workflow your request will be refused by
an HTTP error ``409 Conflict``. Also it is not possible to connect
multiple times to the WebSocket as only one connection is allowed and
will be refused by an HTTP error ``429 Too Many Requests``. Once an
application is connected to the WebSocket the graphical user interface
of the AusweisApp2 will be blocked and shows a hint that another
application uses the AusweisApp2.

.. important::

  Please provide a ``User-Agent`` in your HTTP upgrade request! The AusweisApp2
  will show the content to the user as a hint which application uses the AusweisApp2.



Integrated
^^^^^^^^^^
You can deliver separate AusweisApp2 binaries inside your own application or
start an already installed AusweisApp2.
If your application spawns a separate process you should provide the commandline
parameter ``--port 0`` to avoid conflicts with a user started AusweisApp2 and
other processes that uses a specified port.

The AusweisApp2 will create a text file in the system temporary directory to provide
the selected port. The port filename contains the PID of the running process to allow
multiple instances at the same time.

Example: **$TMPDIR/AusweisApp2.12345.port**

Your application can avoid the graphical interface of AusweisApp2 by providing the
commandline parameter ``--ui websocket``.



.. _automatic:

Automatic
---------
.. versionadded:: 1.24.0
   Mode "automatic" added.

You can enable an automatic authentication with the automatic plugin by providing the
commandline parameter ``--ui automatic``.

It uses the :doc:`simulator` by default with default :ref:`filesystem` if there is no
available card. Otherwise it uses the already inserted card. If you use an inserted
card (e.g. via PCSC) you can provide additional environment variables for PIN, CAN and PUK
on start up.

* ``AUSWEISAPP2_AUTOMATIC_PIN``
* ``AUSWEISAPP2_AUTOMATIC_CAN``
* ``AUSWEISAPP2_AUTOMATIC_PUK``

The default value for the PIN is **123456**. If a value is not defined or the card
refuses a PIN, CAN or PUK the AusweisApp2 will cancel the whole workflow.
Also the workflow will be canceled if the card reader is not a basic reader as it
is not possible to automatically enter the values.


.. note::
  It is possible to pass multiple plugins to the AusweisApp2, e.g.: ``--ui websocket --ui automatic``.

.. seealso::
  The :doc:`container` SDK is designed for scripted and automatic workflows and enables
  the automatic mode by default.



.. _client_status:

Status
------
TR-03124-1 defined a query for status information. This is useful to fetch current
version of installed AusweisApp2 to check if the version supports the WebSocket-API.

You can get this by a HTTP GET query to ``http://localhost:24727/eID-Client?Status``.
If you prefer the JSON syntax you can add it to the parameter ``?Status=json`` to get
the following information.

.. code-block:: json

  {
    "Implementation-Title": "AusweisApp2",
    "Implementation-Vendor": "Governikus GmbH & Co. KG",
    "Implementation-Version": "1.16.0",
    "Name": "AusweisApp2",
    "Specification-Title": "TR-03124",
    "Specification-Vendor": "Federal Office for Information Security",
    "Specification-Version": "1.3"
  }


.. seealso::

  The AusweisApp2 SDK provides a :ref:`get_info` command and an :ref:`info` message
  to fetch the same information to check the compatibility of used AusweisApp2.



Reader
------
The AusweisApp2 SDK uses PC/SC and paired Smartphones as card reader. If the
user wants to use the "smartphone as card reader" feature it is necessary
to pair the devices by the graphical interface of AusweisApp2. The AusweisApp2 SDK
provides no API to pair those devices.
