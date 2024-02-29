Messages
--------
The |AppName| (server) will send some proper
messages (**msg**) to your application (client)
during the whole workflow or as an answer to
your :doc:`commands`.




.. _access_rights:

ACCESS_RIGHTS
^^^^^^^^^^^^^
This message will be sent by |AppName| once the authentication is started
by :ref:`run_auth` and the |AppName| got the certificate from the service.

If your application receives this message you can call :ref:`set_access_rights`
to change some optional access rights or you can call :ref:`get_access_rights`
to get this message again.

Also you can call :ref:`get_certificate` to get the certificate to show this
to your user.

The workflow will continue if you call :ref:`accept` to indicate that the
user accepted the requested access rights or call :ref:`cancel` to abort
the whole workflow.


  - **error**: This optional parameter indicates an error of a :ref:`set_access_rights` call
    if the command contained invalid data.

  - **chat**: Access rights of the provider.

    - **effective**: Indicates the enabled access rights of **optional** and **required**.

    - **optional**: These rights are optional and can be enabled or disabled by :ref:`set_access_rights`.

    - **required**: These rights are mandatory and cannot be disabled.

  - **transactionInfo**: Optional transaction information.

  - **aux**: Optional auxiliary data of the provider.

    - **ageVerificationDate**: Optional required date of birth for AgeVerification as ISO 8601.

    - **requiredAge**: Optional required age for AgeVerification. It is calculated
      by |AppName| on the basis of ageVerificationDate and current date.

    - **validityDate**: Optional validity date as ISO 8601.

    - **communityId**: Optional id of community.



.. code-block:: json

 {
   "msg": "ACCESS_RIGHTS",
   "error": "some optional error message",
   "chat":
         {
          "effective": ["Address", "FamilyName", "GivenNames", "AgeVerification", "CanAllowed"],
          "optional": ["GivenNames", "AgeVerification", "CanAllowed"],
          "required": ["Address", "FamilyName"]
         },
   "transactionInfo": "this is an example",
   "aux":
        {
         "ageVerificationDate": "1999-07-20",
         "requiredAge": "18",
         "validityDate": "2017-07-20",
         "communityId": "02760400110000"
        }
 }


.. code-block:: json

 {
   "msg": "ACCESS_RIGHTS",
   "chat":
         {
          "effective": ["Address", "FamilyName", "GivenNames", "AgeVerification"],
          "optional": ["GivenNames", "AgeVerification"],
          "required": ["Address", "FamilyName"]
         }
 }


Values
""""""
.. versionadded:: 1.22.0
   The following access rights are possible now:

    - CanAllowed
    - PinManagement


.. versionadded:: 1.20.0
   The following write access rights are possible now:

    - WriteAddress
    - WriteCommunityID
    - WriteResidencePermitI
    - WriteResidencePermitII


The following access rights are possible:

  - Address
  - BirthName
  - FamilyName
  - GivenNames
  - PlaceOfBirth
  - DateOfBirth
  - DoctoralDegree
  - ArtisticName
  - Pseudonym
  - ValidUntil
  - Nationality
  - IssuingCountry
  - DocumentType
  - ResidencePermitI
  - ResidencePermitII
  - CommunityID
  - AddressVerification
  - AgeVerification
  - WriteAddress
  - WriteCommunityID
  - WriteResidencePermitI
  - WriteResidencePermitII
  - CanAllowed
  - PinManagement


.. seealso::

  `TR-03110`_, part 4, chapter 2.2.3

  `TR-03127`_, chapter 3.2.2

.. _TR-03110: https://www.bsi.bund.de/EN/Publications/TechnicalGuidelines/TR03110/BSITR03110.html
.. _TR-03127: https://www.bsi.bund.de/DE/Publikationen/TechnischeRichtlinien/tr03127/tr-03127.html




.. _api_level:

API_LEVEL
^^^^^^^^^
This message will be sent if :ref:`get_api_level` or :ref:`set_api_level` is called.

It lists all **available** API levels that can be used and set by :ref:`set_api_level`.
Also it indicates the **current** selected API level.

.. versionadded:: 1.24.0
   Level **2** added.


  - **error**: Optional error message if :ref:`SET_API_LEVEL` failed.

  - **available**: List of supported API level by this version.

  - **current**: Currently selected API level.

.. code-block:: json

  {
    "msg": "API_LEVEL",
    "error": "optional error message like an invalid level",
    "available": [1,2,3,4],
    "current": 4
  }

Your application should always set the compatible API level. The |AppName|
will support multiple API levels to give you enough time to add support
for the new API.

Even if you added support for the new API, your application should still support
the old API level in case the user updates your application but
does not update the |AppName|. Otherwise you need to show a message to the user
that they need to update the |AppName|.

The API level will be increased for **incompatible** changes only. If we can add
additional commands, messages or information without breaking the previous API
you can check the application version with :ref:`get_info` to know if the
current version supports your requirements.

This documentation will mark every API change with a flag like the following:

  - New in version 1.10.0.

  - Changed in version 1.10.0.

  - Deprecated since version 1.10.0.




.. _auth:

AUTH
^^^^
This message will be sent by |AppName| if an authentication
is initially started. The next message should be :ref:`access_rights`
or :ref:`auth` again if the authentication immediately results
in an error.

If you receive an :ref:`auth` message with a parameter **error**
your command :ref:`run_auth` was invalid and the workflow was not
started at all.


  - **error**: Optional error message if :ref:`run_auth` failed.

.. code-block:: json

  {
    "msg": "AUTH",
    "error": "error message if RUN_AUTH failed"
  }



If the workflow is finished the |AppName| will send a message with
a result and an url parameter to indicate the end of an authentication.

.. versionadded:: 1.26.3
   Parameter **reason** added.


  - **result**: The final result of authentication.

    - **major**: Major "error" code.

    - **minor**: Minor error code.

    - **language**: Language of description and message. Language "en"
      is supported only at the moment.

    - **description**: Description of the error message.

    - **message**: The error message.

    - **reason**: Unique :doc:`failurecodes`.

  - **url**: Refresh url or an optional communication error address.

.. code-block:: json

  {
    "msg": "AUTH",
    "result":
             {
              "major": "http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"
             },
    "url": "https://test.governikus-eid.de/gov_autent/async?refID=_123456789"
  }


.. code-block:: json

  {
    "msg": "AUTH",
    "result":
             {
              "major": "http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error",
              "minor": "http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#internalError",
              "language": "en",
              "description": "An internal error has occurred during processing.",
              "message": "The connection to the ID card has been lost. The process was aborted.",
              "reason": "Card_Removed"
             },
    "url": "https://test.governikus-eid.de/gov_autent/async?refID=_abcdefgh"
  }




.. _bad_state:

BAD_STATE
^^^^^^^^^
Indicates that your previous command was send in an invalid state.

Some commands can be send to the server only if certain "state"
is reached in the workflow to obtain the corresponding result.
Otherwise the command will fail with :ref:`bad_state`.

For example, you cannot send :ref:`get_certificate` if there is no
authentication in progress.


  - **error**: Name of the received command that is invalid in this state.

.. code-block:: json

  {
    "msg": "BAD_STATE",
    "error": "NAME_OF_YOUR_COMMAND"
  }




.. _certificate:

CERTIFICATE
^^^^^^^^^^^
Provides information about the used certificate.

  - **description**: Detailed description of the certificate.

    - **issuerName**: Name of the certificate issuer.

    - **issuerUrl**: URL of the certificate issuer.

    - **subjectName**: Name of the certificate subject.

    - **subjectUrl**: URL of the certificate subject.

    - **termsOfUsage**: Raw certificate information about
      the terms of usage.

    - **purpose**: Parsed purpose of the terms of usage.

  - **validity**: Validity dates of the certificate in UTC.

    - **effectiveDate**: Certificate is valid since this date.

    - **expirationDate**: Certificate is invalid after this date.

.. code-block:: json

  {
    "msg": "CERTIFICATE",
    "description":
                  {
                   "issuerName": "Governikus Test DVCA",
                   "issuerUrl": "http://www.governikus.de",
                   "subjectName": "Governikus GmbH & Co. KG",
                   "subjectUrl": "https://test.governikus-eid.de",
                   "termsOfUsage": "Anschrift:\t\r\nGovernikus GmbH & Co. KG\r\nAm Fallturm 9\r\n28359 Bremen\t\r\n\r\nE-Mail-Adresse:\thb@bos-bremen.de\t\r\n\r\nZweck des Auslesevorgangs:\tDemonstration des eID-Service\t\r\n\r\nZuständige Datenschutzaufsicht:\t\r\nDie Landesbeauftragte für Datenschutz und Informationsfreiheit der Freien Hansestadt Bremen\r\nArndtstraße 1\r\n27570 Bremerhaven",
                   "purpose": "Demonstration des eID-Service"
                  },
    "validity":
               {
                "effectiveDate": "2016-07-31",
                "expirationDate": "2016-08-30"
               }
  }




.. _change_pin:

CHANGE_PIN
^^^^^^^^^^
This message will be sent by |AppName| if a change PIN workflow
is initially started.

If you receive a :ref:`change_pin` message with a parameter **success**
the workflow is finished. This could happen after a :ref:`set_pin`
command if the connection to the card failed. Also the parameter
**success** is false after a :ref:`cancel` command.


.. versionadded:: 1.22.0
   Support of CHANGE_PIN message.


.. versionadded:: 1.26.4
   Parameter **reason** added.


  - **success**: Indicates with true that the PIN was successfully
    changed, otherwise false.

  - **reason**: Unique :doc:`failurecodes`.

.. code-block:: json

  {
    "msg": "CHANGE_PIN",
    "success": true
  }


.. code-block:: json

  {
    "msg": "CHANGE_PIN",
    "success": false,
    "reason": "Card_Removed"
  }




.. _enter_can:

ENTER_CAN
^^^^^^^^^
Indicates that a CAN is required to continue workflow.

If the |AppName| sends this message, you will have to
provide the CAN of the inserted card with :ref:`set_can`.

The CAN is required to enable the last attempt of PIN input if
the retryCounter is **1**. The workflow continues automatically with
the correct CAN and the |AppName| will send an :ref:`enter_pin` message.
Despite the correct CAN being entered, the retryCounter remains at **1**.

The CAN is also required, if the authentication terminal has an approved
"CAN allowed right". This allows the workflow to continue without
an additional PIN.

If your application provides an invalid :ref:`set_can` command
the |AppName| will send an :ref:`enter_can` message with an error
parameter.

If your application provides a valid :ref:`set_can` command
and the CAN was incorrect the |AppName| will send :ref:`enter_can`
again but without an error parameter.

.. versionadded:: 1.14.2
   Support of "CAN allowed right".


  - **error**: Optional error message if your command :ref:`set_can`
    was invalid.

  - **reader**: Information about the used card and card reader.
    Please see message :ref:`reader` for details.

.. code-block:: json

  {
    "msg": "ENTER_CAN",
    "error": "You must provide 6 digits",
    "reader":
             {
              "name": "NFC",
              "insertable": false,
              "attached": true,
              "keypad": false,
              "card":
                     {
                      "inoperative": false,
                      "deactivated": false,
                      "retryCounter": 1
                     }
             }
  }

.. note::
  There is no retry limit for an incorrect CAN.




.. _enter_pin:

ENTER_PIN
^^^^^^^^^
Indicates that a PIN is required to continue the workflow.

If the |AppName| sends this message, you will have to
provide the PIN of the inserted card with :ref:`set_pin`.

The workflow will automatically continue if the PIN was correct.
Otherwise you will receive another message :ref:`enter_pin`.
If the correct PIN is entered the retryCounter will be set to **3**.

If your application provides an invalid :ref:`set_pin` command
the |AppName| will send an :ref:`enter_pin` message with an error
parameter and the retryCounter of the card is **not** decreased.

If your application provides a valid :ref:`set_pin` command
and the PIN was incorrect the |AppName| will send :ref:`enter_pin`
again with a decreased retryCounter but without an error parameter.

If the value of retryCounter is **1** the |AppName| will initially send an
:ref:`enter_can` message. Once your application provides a correct CAN the
|AppName| will send an :ref:`enter_pin` again with a retryCounter of **1**.

If the value of retryCounter is **0** the |AppName| will initially send an
:ref:`enter_puk` message. Once your application provides a correct PUK the
|AppName| will send an :ref:`enter_pin` again with a retryCounter of **3**.


  - **error**: Optional error message if your command :ref:`set_pin`
    was invalid.

  - **reader**: Information about the used card and card reader.
    Please see message :ref:`reader` for details.

.. code-block:: json

  {
    "msg": "ENTER_PIN",
    "error": "You must provide 6 digits",
    "reader":
             {
              "name": "NFC",
              "insertable": false,
              "attached": true,
              "keypad": false,
              "card":
                     {
                      "inoperative": false,
                      "deactivated": false,
                      "retryCounter": 3
                     }
             }
  }





.. _enter_new_pin:

ENTER_NEW_PIN
^^^^^^^^^^^^^
Indicates that a new PIN is required to continue the workflow.

If the |AppName| sends this message, you will have to
provide the new PIN of the inserted card with :ref:`set_new_pin`.


.. versionadded:: 1.22.0
   Support of ENTER_NEW_PIN message.


  - **error**: Optional error message if your command :ref:`set_new_pin`
    was invalid.

  - **reader**: Information about the used card and card reader.
    Please see message :ref:`reader` for details.

.. code-block:: json

  {
    "msg": "ENTER_NEW_PIN",
    "error": "You must provide 6 digits",
    "reader":
             {
              "name": "NFC",
              "insertable": false,
              "attached": true,
              "keypad": false,
              "card":
                     {
                      "inoperative": false,
                      "deactivated": false,
                      "retryCounter": 3
                     }
             }
  }





.. _enter_puk:

ENTER_PUK
^^^^^^^^^
Indicates that a PUK is required to continue the workflow.

If the |AppName| sends this message, you will have to
provide the PUK of the inserted card with :ref:`set_puk`.

The workflow will automatically continue if the PUK was correct
and the |AppName| will send an :ref:`enter_pin` message.
Otherwise you will receive another message :ref:`enter_puk`.
If the correct PUK is entered the retryCounter will be set to **3**.

If your application provides an invalid :ref:`set_puk` command
the |AppName| will send an :ref:`enter_puk` message with an error
parameter.

If your application provides a valid :ref:`set_puk` command
and the PUK was incorrect the |AppName| will send :ref:`enter_puk`
again but without an error parameter.

If |AppName| sends :ref:`enter_puk` with field "inoperative" of embedded
:ref:`reader` message set true it is not possible to unblock the PIN.
You will have to show a message to the user that the card is inoperative
and the user should contact the authority responsible for issuing the
identification card to unblock the PIN.
You need to send a :ref:`cancel` to abort the workflow if card is operative.
Please see the note for more information.


  - **error**: Optional error message if your command :ref:`set_puk`
    was invalid.

  - **reader**: Information about the used card and card reader.
    Please see message :ref:`reader` for details.

.. code-block:: json

  {
    "msg": "ENTER_PUK",
    "error": "You must provide 10 digits",
    "reader":
             {
              "name": "NFC",
              "insertable": false,
              "attached": true,
              "keypad": false,
              "card":
                     {
                      "inoperative": false,
                      "deactivated": false,
                      "retryCounter": 0
                     }
             }
  }

.. note::
  There is no retry limit for an incorrect PUK. But
  be aware that the PUK can only be used 10 times to
  unblock the PIN. There is no readable counter for this.
  The |AppName| is not able to provide any counter information
  of PUK usage.
  If the PUK is used 10 times it is not possible to unblock
  the PIN anymore and the card will remain in PUK state.
  Also it is not possible to indicate this state before the
  user enters the correct PUK once. This information will be
  provided as field "inoperative" of :ref:`reader` message.




.. _info:

INFO
^^^^
Provides information about the |AppName|.

Especially if you want to get a specific **Implementation-Version**
to check if the current installation supports some additional
:doc:`commands` or :doc:`messages`.

Also you should check the :ref:`api_level` as it will be
increased for **incompatible** changes.


  - **VersionInfo**: Structure of version information.

    - **Name**: Application name.

    - **Implementation-Title**: Title of implementation.

    - **Implementation-Vendor**: Vendor of implementation.

    - **Implementation-Version**: Version of implementation.

    - **Specification-Title**: Title of specification.

    - **Specification-Vendor**: Vendor of specification.

    - **Specification-Version**: Version of specification.

  - **AusweisApp**: Indicates the state of the connection to
    the AusweisApp for integrated SDK (**Android only**).
    The following states are possible.

    - **CONNECTED**: The SDK is connected.

    - **DISCONNECTED**: The SDK is not connected.

    - **INVALID_CERTIFICATE**: The certificate of LocalIfd is not valid.

    - **INCOMPATIBLE_VERSION**: The version of LocalIfd is not compatible.

    - **UNKNOWN**: The state could not be recognized.

.. code-block:: json

  {
    "msg": "INFO",
    "VersionInfo":
                  {
                   "Name": "AusweisApp2",
                   "Implementation-Title": "AusweisApp2",
                   "Implementation-Vendor": "Governikus GmbH & Co. KG",
                   "Implementation-Version": "2.0.0",
                   "Specification-Title": "TR-03124-1",
                   "Specification-Vendor": "Federal Office for Information Security",
                   "Specification-Version": "1.4"
                  },
     "AusweisApp": "CONNECTED"
  }




.. _insert_card:

INSERT_CARD
^^^^^^^^^^^
Indicates that the |AppName| requires a card to continue.

If the |AppName| needs a card to continue the workflow
this message will be sent as a notification.
If your application receives this message it should
show a hint to the user.

The user must provide a physical card or your application needs to
to provide a "virtual" card by calling :ref:`set_card`.

After the user or your application inserted a card, the workflow will
continue automatically, unless both the eID function and
CAN allowed mode are disabled.
CAN allowed mode is enabled if the |AppName| is used as SDK and the
certificate contains the CAN allowed right.
In this case, the workflow will be paused until another card is inserted.
If the user already inserted a card this message will not be sent at all.

This message will also be sent if there is no connected card reader.


  - **error**: Optional detailed error message.

.. code-block:: json

  {
    "msg": "INSERT_CARD",
    "error": "Name cannot be undefined"
  }




.. _internal_error:

INTERNAL_ERROR
^^^^^^^^^^^^^^
Indicates an internal error.

If your application receives this message you found
a bug. Please report this issue to our support!


  - **error**: Optional detailed error message.

.. code-block:: json

  {
    "msg": "INTERNAL_ERROR",
    "error": "Unexpected condition"
  }




.. _invalid:

INVALID
^^^^^^^
Indicates a broken JSON message.

If your application receives this message you
passed a broken JSON structure to the |AppName|.

Please fix your JSON document and send it again!


  - **error**: Detailed error message.

.. code-block:: json

  {
    "msg": "INVALID",
    "error": "unterminated string (offset: 12)"
  }





.. _reader:

READER
^^^^^^
Provides information about a connected or disconnected card reader.

This message will be sent by the |AppName| if a card reader was added
or removed to the operating system. Also if a card was inserted into a
card reader or removed from a card reader.

Your application can explicitly check for card reader with :ref:`get_reader`.

If a workflow is in progress and a card with disabled eID function was
inserted, this message will still be sent, but the workflow will be paused
until a card with enabled eID function is inserted.

.. versionadded:: 1.24.0
   Parameter **insertable** added.

.. versionadded:: 1.16.0
   Parameter **keypad** added.


  - **name**: Identifier of card reader.

  - **insertable**: Indicates whether a card can be inserted via :ref:`set_card`.

  - **attached**: Indicates whether a card reader is connected or disconnected.

  - **keypad**: Indicates whether a card reader has a keypad. The parameter
    is only shown when a reader is attached.

  - **card**: Provides information about inserted card, otherwise null.

    - **inoperative**: True if PUK is inoperative and cannot unblock PIN,
      otherwise false. This can be recognized if user enters a correct
      PUK only. It is not possbible to read this data before a user tries
      to unblock the PIN.

    - **deactivated**: True if eID function is deactivated, otherwise false.
      The scan dialog on iOS won't be closed if this is True. You need to
      send :ref:`interrupt` yourself to show an error message.

    - **retryCounter**: Count of possible retries for the PIN. If you enter a PIN
      with command :ref:`set_pin` it will be decreased if PIN was incorrect.

.. code-block:: json

  {
    "msg": "READER",
    "name": "NFC",
    "insertable": false,
    "attached": true,
    "keypad": false,
    "card":
           {
            "inoperative": false,
            "deactivated": false,
            "retryCounter": 3
           }
  }




.. _reader_list:

READER_LIST
^^^^^^^^^^^
Provides information about all connected card readers.

.. versionchanged:: 1.24.0
   Parameter **reader** was renamed to **readers** with :ref:`api_level` **2**.


  - **readers**: A list of all connected card readers. Please
    see message :ref:`reader` for details.

.. code-block:: json

  {
    "msg": "READER_LIST",
    "readers":
             [
               {
                "name": "Example reader 1 [SmartCard] (1234567) 01 00",
                "insertable": false,
                "attached": true,
                "keypad": true,
                "card": null
               },

               {
                "name": "NFC",
                "insertable": false,
                "attached": true,
                "keypad": false,
                "card":
                       {
                        "inoperative": false,
                        "deactivated": false,
                        "retryCounter": 3
                       }
               }
             ]
  }




.. _status:

STATUS
^^^^^^
Provides information about the current workflow and state.
This message indicates if a workflow is in progress or the
workflow is paused. This can occur if the |AppName| needs
additional data like :ref:`access_rights` or :ref:`insert_card`.

The messages will be sent by default if not disabled in :ref:`run_auth`
or :ref:`run_change_pin`.

.. versionadded:: 1.24.0
   Support of STATUS message in :ref:`api_level` **2**.


  - **workflow**: Name of the current workflow.
    If there is no workflow in progress this will be null.

  - **progress**: Percentage of workflow progress.
    If there is no workflow in progress this will be null.

  - **state**: Name of the current state if paused.
    If there is no workflow in progress or the workflow is not paused
    this will be null.

.. code-block:: json

  {
    "msg": "STATUS",
    "workflow": "AUTH",
    "progress": 25,
    "state": "ACCESS_RIGHTS"
  }




.. _unknown_command:

UNKNOWN_COMMAND
^^^^^^^^^^^^^^^
Indicates that the command type is unknown.

If your application receives this message you
passed a wrong command to the |AppName|.

Please fix your command and send it again!

Be aware of case sensitive names in :doc:`commands`.


  - **error**: Name of the unknown command.

.. code-block:: json

  {
    "msg": "UNKNOWN_COMMAND",
    "error": "get_INFo"
  }

