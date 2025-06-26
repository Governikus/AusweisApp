Desktop
=======
This chapter deals with the desktop specific properties of the |AppName| SDK.
The |AppName| core is reachable over a **WebSocket** which is running by
default since |AppName| 1.16.0. Subsequent sections deal with the SDK
interface itself and explain which steps are necessary in order to communicate
with the |AppName| SDK.



.. _websocket:

WebSocket
---------
The |AppName| uses the same default port as defined in TR-03124-1.
Your application can connect to ``ws://localhost:24727/eID-Kernel`` to
establish a bidirectional connection.

You can check the version of |AppName| by the ``Server`` header of the HTTP
response or by an additional query to get the :ref:`client_status`.

If the WebSocket handshake was successful your application can send :doc:`commands`
and receive :doc:`messages`.
The |AppName| will send an HTTP error 503 "Service Unavailable" if the WebSocket
is disabled.

.. seealso::

  https://tools.ietf.org/html/rfc6455



User installed
^^^^^^^^^^^^^^
Your application can connect to a user installed |AppName|. If the
user already has an active workflow your request will be refused by
an HTTP error ``409 Conflict``. Also it is not possible to connect
multiple times to the WebSocket as only one connection is allowed and
will be refused by an HTTP error ``429 Too Many Requests``. Once an
application is connected to the WebSocket the graphical user interface
of the |AppName| will be blocked and shows a hint that another
application uses the |AppName|.

.. important::

  Please provide a ``User-Agent`` in your HTTP upgrade request! The |AppName|
  will show the content to the user as a hint which application uses the |AppName|.



Integrated
^^^^^^^^^^
You can start an already installed |AppName| for your application.
If your application spawns a separate process you should provide the commandline
parameter ``--port 0`` to avoid conflicts with a user started |AppName| and
other processes that uses a specified port.

The |AppName| will create a text file in the system temporary directory to provide
the selected port. The port filename contains the PID of the running process to allow
multiple instances at the same time.

Example: **$TMPDIR/AusweisApp.12345.port**

Your application can avoid the graphical interface of |AppName| by providing the
commandline parameter ``--ui websocket``.

.. important::
  If your application changes the used port the "smartphone as card reader"
  is not possible.


.. _automatic:

Automatic
---------
.. versionadded:: 2.3.0
   Variable "AUSWEISAPP_AUTOMATIC_SIMULATOR" added.

   Variables "AUSWEISAPP2_AUTOMATIC_*" renamed to "AUSWEISAPP_AUTOMATIC_*".

.. versionadded:: 1.26.3
   Variable "AUSWEISAPP2_AUTOMATIC_DEVELOPERMODE" added.

.. versionadded:: 1.24.0
   Mode "automatic" added.

You can enable an automatic authentication with the automatic plugin by providing the
commandline parameter ``--ui automatic``.

It uses the :doc:`simulator` by default with default :ref:`filesystem` if there is no
available card. Otherwise it uses the already inserted card. If you use an inserted
card (e.g. via PCSC) you can provide additional environment variables for PIN, CAN and PUK
on start up.

* ``AUSWEISAPP_AUTOMATIC_PIN``
* ``AUSWEISAPP_AUTOMATIC_CAN``
* ``AUSWEISAPP_AUTOMATIC_PUK``

The default value for the PIN is **123456**. If a value is not defined or the card
refuses a PIN, CAN or PUK the |AppName| will cancel the whole workflow.
Also the workflow will be canceled if the card reader is not a basic reader as it
is not possible to automatically enter the values.

The **developerMode** (like in :ref:`run_auth`) can be enabled with the environment
variable ``AUSWEISAPP_AUTOMATIC_DEVELOPERMODE``.
This will be evaluated if the automatic plugin takes control over the workflow.

If you want to provide another :ref:`filesystem` to the simulator you can use the
environment variable ``AUSWEISAPP_AUTOMATIC_SIMULATOR``. This can be a file or the
JSON content itself.


.. note::
  It is possible to pass multiple plugins to the |AppName|, e.g.: ``--ui webservice --ui automatic``.

.. seealso::
  The :doc:`container` SDK is designed for scripted and automatic workflows and enables
  the automatic mode by default.



.. _client_status:

Status
------
TR-03124-1 defined a query for status information. This is useful to fetch current
version of installed |AppName| to check if the version supports the WebSocket-API.

You can get this by a HTTP GET query to ``http://localhost:24727/eID-Client?Status``.
If you prefer the JSON syntax you can add it to the parameter ``?Status=json`` to get
the following information.

.. code-block:: json

  {
    "Name": "AusweisApp2",
    "Implementation-Title": "AusweisApp2",
    "Implementation-Vendor": "Governikus GmbH & Co. KG",
    "Implementation-Version": "2.0.0",
    "Specification-Title": "TR-03124-1",
    "Specification-Vendor": "Federal Office for Information Security",
    "Specification-Version": "1.4"
  }


.. seealso::

  The |AppName| SDK provides a :ref:`get_info` command and an :ref:`info` message
  to fetch the same information to check the compatibility of used |AppName|.



Reader
------
The |AppName| SDK uses PC/SC and paired Smartphones as card reader. If the
user wants to use the "smartphone as card reader" feature it is necessary
to pair the devices by the graphical interface of |AppName|. The |AppName| SDK
provides no API to pair those devices.
