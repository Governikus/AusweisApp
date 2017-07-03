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




Recommended
-----------
The SDK uses JSON as the communication protocol.
It is recommended to use an existing library for this.

  - **Java**: `Google GSON <https://github.com/google/gson>`_

  - **C++**: `JsonCPP <https://github.com/open-source-parsers/jsoncpp>`_
