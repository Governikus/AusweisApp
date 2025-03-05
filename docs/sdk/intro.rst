Introduction
============
This documentation will explain how to initialize and start up
the |AppName| as an additional service. It distinguishes between
a connection to the application and the communication between your
application and |AppName|.

The section :ref:`connection` will show you what you need to do to
set up a connection to |AppName|. Once you have established
a connection you can send and receive JSON documents in a
bi-directional manner.
There are different commands and messages. These are listed
and described in the section :ref:`protocol`. The protocol
is split up in :doc:`commands` and :doc:`messages`. Commands
will be sent by your application to control |AppName|.
Messages contain additional information to your command
or will be sent as an event.

Also this documentation provides some example workflows to
show a possible communication.

.. seealso::
   For Android and iOS there is also the
   `AusweisApp SDK Wrapper <https://www.ausweisapp.bund.de/sdkwrapper/>`_
   which is a software library that offers a high-level interface to the |AppName| SDK.

.. important::
   The |AppName| does **not** provide any personal data to
   your client application directly as |AppName| does not
   have access to this data for security reasons.
   |AppName| facilitates a secure connection between the
   eID server and the ID card, enabling the eID server to get
   those data from the card.

   This way your backend receives high level trust data.
   Since your client application runs in a user's environment,
   you could not be sure about the integrity of the data
   if your client application were to receive high sensitive
   data from the |AppName| directly as your backend does
   not have any possibility to verify the source of the data.

   Also this approach, recommended for compliance reasons by the
   Federal Office for Information Security, spares your client
   application the necessity of encrypting these high sensitive
   data.

   In case your client application requires data input from the
   ID card, you need to get this from the backend system
   (e.g. the eID server) after a successful authentication.


   .. seealso::
     `TR-03124`_, part 1: Specifications

   .. _TR-03124: https://www.bsi.bund.de/SharedDocs/Downloads/DE/BSI/Publikationen/TechnischeRichtlinien/TR03124/TR-03124-1.pdf
