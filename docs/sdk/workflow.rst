Workflow
--------
This section shows some possible workflows as an example
communication between your application and the AusweisApp2.

The JSON structure can be identified by parameter **cmd**
or parameter **msg** as described in section :doc:`commands`
and section :doc:`messages`.

- **cmd**: Commands are sent by your application.

- **msg**: Messages are sent by the AusweisApp2.



Minimal successful authentication
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The following messages and commands are the minimal
iterations of a successful authentication.

We assume that the user already inserted a card
into the connected card reader.


.. code-block:: json

  {"cmd": "RUN_AUTH", "tcTokenURL": "https://test.governikus-eid.de/DEMO"}

  {"msg": "AUTH"}

  {"msg": "ACCESS_RIGHTS", "raw": {"effective":[24,15,0],"optional":[15],"required":[24,0]}}

  {"cmd": "ACCEPT"}

  {"msg": "ENTER_PIN", "reader": {"attached":true,"card":{"deactivated":false,"inserted":true,"retryCounter":3},"name":"NFC"}}

  {"cmd": "SET_PIN", "pin": "123456"}

  {"msg": "AUTH","result": {"major":"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"},"url":"https://test.governikus-eid.de/DEMO/?refID=123456"}



Successful authentication with CAN
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The following messages and commands show possible
iterations if the user enters an incorrect PIN and CAN twice
before entering the correct CAN and PIN.

We assume that the user didn't insert a card
into the connected card reader.


.. code-block:: json

  {"cmd": "RUN_AUTH", "tcTokenURL": "https://test.governikus-eid.de/DEMO"}

  {"msg": "AUTH"}

  {"msg": "ACCESS_RIGHTS", "raw": {"effective":[0],"optional":[],"required":[0]}}

  {"cmd": "ACCEPT"}

  {"msg": "INSERT_CARD"}

  {"msg": "ENTER_PIN", "reader": {"attached":true,"card":{"deactivated":false,"inserted":true,"retryCounter":3},"name":"NFC"}}

  {"cmd": "SET_PIN", "pin": "000000"}

  {"msg": "ENTER_PIN", "reader": {"attached":true,"card":{"deactivated":false,"inserted":true,"retryCounter":2},"name":"NFC"}}

  {"cmd": "SET_PIN", "pin": "000001"}

  {"msg": "ENTER_CAN", "reader": {"attached":true,"card":{"deactivated":false,"inserted":true,"retryCounter":1},"name":"NFC"}}

  {"cmd": "SET_CAN", "can": "000000"}

  {"msg": "ENTER_CAN", "reader": {"attached":true,"card":{"deactivated":false,"inserted":true,"retryCounter":1},"name":"NFC"}}

  {"cmd": "SET_CAN", "can": "654321"}

  {"msg": "ENTER_PIN", "reader": {"attached":true,"card":{"deactivated":false,"inserted":true,"retryCounter":1},"name":"NFC"}}

  {"cmd": "SET_PIN", "pin": "123456"}

  {"msg": "AUTH","result": {"major":"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"},"url":"https://test.governikus-eid.de/DEMO/?refID=123456"}



Cancelled authentication
^^^^^^^^^^^^^^^^^^^^^^^^
The following messages and commands show possible
iterations if the user cancels the authentication.

We assume that the user didn't connect the card reader.


.. code-block:: json

  {"cmd": "RUN_AUTH", "tcTokenURL": "https://test.governikus-eid.de/DEMO"}

  {"msg": "AUTH"}

  {"msg": "ACCESS_RIGHTS", "raw": {"effective":[0],"optional":[],"required":[0]}}

  {"cmd": "CANCEL"}

  {"msg": "AUTH", "result": {"description":"The operation was aborted due to cancellation by user.","language":"en","major":"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error","message":"The process was cancelled by the user.","minor":"http://www.bsi.bund.de/ecard/api/1.1/resultminor/sal#cancellationByUser"},"url":"https://test.governikus-eid.de/DEMO/?errID=123456"}


