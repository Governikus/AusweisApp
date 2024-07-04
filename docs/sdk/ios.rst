iOS
===
This chapter deals with the iOS specific properties of the |AppName| SDK.
The |AppName| core is encapsulated into an **XCFramework** which needs to
be linked into your application.

Subsequent sections deal with the SDK interface itself and explain which
steps are necessary in order to communicate with the |AppName| SDK.

.. seealso::
   For iOS there is also the
   `AusweisApp SDK Wrapper <https://www.ausweisapp.bund.de/sdkwrapper/>`_
   which is a software library that offers a high-level interface to the |AppName| SDK.

.. important::
   Apple released the necessary NFC API with iOS 13.0!
   Be aware that it is not possible to support older versions.

Use XCFramework
---------------
The interface ``AusweisApp2.h`` of the SDK for iOS is provided as **C-Header**
that you need to import/include into your application. It grants access to
start and shutdown a separate background thread with the integrated
|AppName| core.

After you established a connection to the |AppName| SDK your application
can send :doc:`commands` and receive :doc:`messages`.


Clone
^^^^^
The XCFramework is provided in our github repository. It is recommended
to use at least Xcode 12 and the dependency handling of at
least Swift 5.3 (Swift Package Manager).

  https://github.com/Governikus/AusweisApp2-SDK-iOS

You can clone and add that repository with Xcode and import it into your
project under the following menu.

  :menuselection:`File --> Swift Packages --> Add Package Dependency`


Import
^^^^^^
After you added the repository to your Xcode project you can import the
module via ``import AusweisApp2`` in Swift classes or ``@import AusweisApp2;``
in Objective-C classes and call the functions of the ``AusweisApp2.h`` header.

.. code-block:: c

  typedef void (* AusweisApp2Callback)(const char* pMsg);
  bool ausweisapp2_init(AusweisApp2Callback pCallback, const char* pCmdline);
  void ausweisapp2_shutdown(void);
  bool ausweisapp2_is_running(void);
  void ausweisapp2_send(const char* pCmd);

.. versionchanged:: 1.24.0
   Added optional parameter ``pCmdline`` to function ``ausweisapp2_init``.


First, you need to define a callback function that will be called by the |AppName|
to request or provide additional information. If your application initializes the
SDK you must pass that callback to ``ausweisapp2_init``. That function will return
``false`` if the callback is ``NULL`` or the SDK is already running.
The Parameter ``pCmdline`` is optional and can be ``NULL``. This allows your application
to provide additional commandline arguments like ``--no-loghandler``.

After you called that function the |AppName| SDK will start up. If the
initialization is finished the SDK calls your callback function once with
``NULL`` as parameter to indicate that it is ready to accept :doc:`commands`.
Do not call ``ausweisapp2_send`` until your callback received that message, otherwise
that command will be ignored.

Once the SDK is ready to go you can send :doc:`commands` by ``ausweisapp2_send``.
Your callback will receive the :doc:`messages`.

If you call ``ausweisapp2_shutdown`` the |AppName| SDK will be terminated. This
function joins the thread of the |AppName| and blocks until the |AppName| is
finished. You should not call this function in your callback as it is called
by the |AppName| thread. In that case ``ausweisapp2_shutdown`` cannot be a
blocking call to avoid a deadlock.
If you call this function while a workflow is running the workflow will be
canceled automatically before the shutdown.


.. important::
   Your callback will be called by the separate |AppName| thread. Do **not**
   make long running or blocking calls! It is recommended to use an async dispatcher.

   Also, you should not call ``ausweisapp2_send`` or ``ausweisapp2_shutdown`` within
   your callback function.



Info.plist
----------
You need to enable the card identifier in your applications ``Info.plist`` like this,
otherwise iOS will not recognize any identity cards. Also, it is necessary to provide
a message why your application needs access to the NFC hardware.

.. code-block:: xml

  <key>com.apple.developer.nfc.readersession.iso7816.select-identifiers</key>
  <array>
    <string>E80704007F00070302</string>
  </array>

  <key>NFCReaderUsageDescription</key>
  <string>AusweisApp needs NFC to access the ID card.</string>


.. seealso::

  * https://developer.apple.com/documentation/bundleresources/information_property_list/select-identifiers
  * https://developer.apple.com/documentation/bundleresources/information_property_list/nfcreaderusagedescription



Entitlements
------------
Your application needs to provide an entitlement file to request the format
of reader sessions.

.. code-block:: xml

  <?xml version="1.0" encoding="UTF-8"?>
  <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
  <plist version="1.0">
    <dict>
      <key>com.apple.developer.nfc.readersession.formats</key>
      <array>
        <string>TAG</string>
      </array>
    </dict>
  </plist>

.. seealso::

  https://developer.apple.com/documentation/bundleresources/entitlements/com_apple_developer_nfc_readersession_formats



Logging
-------

The |AppName| uses default logging of iOS and has its own log file.
It is **recommended** to collect that log file if an error occurs in
your application to receive better support.

The log file is in your application path:

.. code-block:: text

    NSTemporaryDirectory() + /AusweisApp.XXXXXX.log

The *XXXXXX* characters will be replaced by an automatically generated
portion of the filename to avoid conflicts with previous instances.

A new log file will be created for each new instance of the |AppName| and
will be deleted after a correct shutdown.
In case of old or multiple log files, it is highly probable that the
previous instance crashed.

The |AppName| deletes any log files that are older than 14 days.
