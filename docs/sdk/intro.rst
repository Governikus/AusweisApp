Introduction
============
This documentation will explain how to initialize and start up
the AusweisApp2 as an additional service. It distinguishes between
a connection to the application and the communication between your
application and AusweisApp2.

The section "Connection" will show you what you need to do to
set up a connection to AusweisApp2. Once you have established
a connection you can send and receive JSON documents in a
bi-directional manner.
There are different commands and messages. These are listed
and described in the section "Protocol". The protocol
is split up in :doc:`commands` and :doc:`messages`. Commands
will be sent by your application to control AusweisApp2.
Messages contain additional information to your command
or will be sent as an event.

Also this documentation provides some example workflows to
show a possible communication.


.. important::
   The AusweisApp2 does **not** provide any personal data to
   your client application directly as AusweisApp2 does not
   have access to this data for security reasons.
   AusweisApp2 facilitates a secure connection between the
   eID server and the ID card, enabling the eID server to get
   those data from the card.

   This way your backend receives high level trust data.
   Since your client application runs in a user's environment,
   you could not be sure about the integrity of the data
   if your client application were to receive high sensitive
   data from the AusweisApp2 directly as your backend does
   not have any possibility to verify the source of the data.

   Also this approach, recommended for compliance reasons by the
   Federal Office for Information Security, spares your client
   application the necessity of encypting these high sensitive
   data.

   In case your client application requires data input from the
   ID card, you need to get this from the backend system
   (e.g. the eID server) after a succesfull authentication.


   .. seealso::
     `TR-03124`_, part 1: Specifications

   .. _TR-03124: https://www.bsi.bund.de/SharedDocs/Downloads/DE/BSI/Publikationen/TechnischeRichtlinien/TR03124/TR-03124-1.pdf


Recommended
-----------
The SDK uses JSON as the communication protocol.
It is recommended to use an existing library for this.

  - **Java**: `Google GSON <https://github.com/google/gson>`_

  - **C++**: `JsonCPP <https://github.com/open-source-parsers/jsoncpp>`_
