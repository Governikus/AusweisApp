Android
=======
This chapter deals with the Android specific properties of the AusweisApp2 SDK.
The AusweisApp2 core is encapsulated into an **Android service** which is
running in the background without a user interface. This service is interfaced
via an Android specific interprocess communication (IPC) mechanism. The basics
of this mechanism - the **Android Interface Definition Language** (AIDL) -
are introduced in the following section. Subsequent sections deal with the
SDK interface itself and explain which steps are necessary in order to talk
to the AusweisApp2 SDK.

The AusweisApp2 is available as an integrated and as an external variant.
The integrated version is provided as an AAR package that can automatically
be fetched by Android's default build system **gradle**. The external variant
is available as an APK in Google's PlayStore. It is required that the user
has manually installed the AusweisApp2 like any other app to connect to the
external variant.

.. important::
   The integrated variant is available in jcenter for free.
   If you need enterprise support feel free to contact us.



Integrated
----------
The integrated SDK is distributed as an AAR package that contains
native **arm64-v8a** libraries only.
The AAR package is available in the default repository of Android.
The following listing shows the required **jcenter** in **build.gradle**.

.. code-block:: groovy

  buildscript {
      repositories {
          jcenter()
      }
  }


The integrated AusweisApp2 will be fetched automatically as a dependency by
your **app/build.gradle** file.
It is recommended to always use the latest version by **1.+** of AusweisApp2.
But you are free to add a concrete version like **1.20.0**.

.. code-block:: groovy

  dependencies {
      implementation 'com.governikus:ausweisapp:1.20.+'
  }



.. seealso::
  The AAR package provides an **AndroidManifest.xml** to register required
  permissions and the background service. If your own AndroidManifest has
  conflicts with our provided file you can add some attributes to resolve
  those conflicts.

  https://developer.android.com/studio/build/manifest-merge.html



External
--------
The APK is available in Google's PlayStore and needs to be installed by
the user. The external SDK is distributed as 32-bit and 64-bit.


Security
^^^^^^^^
The following listing provides information about the solution to provide a
secure connection to AusweisApp2.

  - Data between two apps connected via AIDL as a bound service cannot be
    grabbed by an attacker. Android will send the data to the corresponding
    app directly. There is no broadcast like an implicit intent.

  - An attacker cannot bind to an already bound service as AusweisApp2 will
    accept only one connection at the same time.

  - An attacker cannot resume a connection after the previous app disconnects
    because AusweisApp2 will reset the internal state if an app connects
    with another session ID.

  - An attacker cannot grab the session ID of the previous app because
    AusweisApp2 uses multiple sources of secure random number generator.

  - An attacker cannot fake AusweisApp2 for other apps because the connection
    via AIDL is bound with package name "com.governikus.ausweisapp2". Google
    ensures that there is no other app in Google Play Store with that package
    name. Also the client app can check the fingerprint of signature certificate
    used for that package name.



Verify the authenticity of AusweisApp2
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The following section deals with the cryptographic verification of the SDK's
authenticity. This step is necessary to ensure that the SDK has not been
modified in a malicious way.


Fingerprint
"""""""""""
In order to verify that the AusweisApp2 SDK is authentic and has not been
modified in a malicious way, it is required to verify its authenticity
before establishing a connection with it.
Each Android application is signed by a distribution certificate which
ensures its authenticity. During the installation of an application
Android verifies that it has been correctly signed with the supplied
distribution certificate.
So everything the client has to do in order to verify the authenticity
is to verify that the correct certificate has been used. The **SHA256**
fingerprint of the authentic SDK certificate is the following:

.. code-block:: text

  B0 2A C7 6B 50 A4 97 AE 81 0A EA C2 25 98 18 7B 3D 42 90 27 7D 08 51 A7 FA 8E 1A EA 5A 97 98 70



Example
"""""""
The following example code demonstrates how the certificate
hash value of a signed application on Android can be verified.

.. code-block:: java

  import android.content.pm.PackageInfo;
  import android.content.pm.PackageManager;
  import android.content.pm.Signature;

  public class AusweisApp2Validator
  {
    private static final String PACKAGE = "com.governikus.ausweisapp2";
    private static final String FINGERPRINT = "..."; // see above

    public boolean isValid()
    {
      final PackageManager m = getPackageManager();
      PackageInfo info;
      try {
        info = m.getPackageInfo(PACKAGE, PackageManager.GET_SIGNATURES);
      } catch (PackageManager.NameNotFoundException e) {
        return false;
      }

      // What the API names signatures are actually the signing certificates.
      Signature certificates[] =  info.signatures;
      String computed = computeHashHexString("SHA256", certificates[0]);

      return FINGERPRINT.equalsIgnoreCase(computed);
    }
  }


.. _android_import_aidl:

Import the AIDL files
^^^^^^^^^^^^^^^^^^^^^
Android provides an interprocess communication (IPC) mechanism which is based on
messages consisting of primitive types.
In order to abstract from this very basic mechanism, there is the Android
Interface Definition Language (AIDL).
It allows the definition of Java like interfaces.
The Android SDK generates the necessary interface implementations from supplied
AIDL files in order to perform IPC, as if this function had been called directly
in the current process.

In order to interact with the AusweisApp2 SDK there are two AIDL interfaces.
The first one is given to the client application by the SDK and allows the
client to establish a session with the SDK,
to send JSON commands to the SDK and to pass discovered NFC tags to the SDK.

The second AIDL interface is given to the SDK by the client application. It
enables the client to receive the intial session parameters as well as
JSON messages from the SDK. Furthermore it has a function which is called
when an existing connection with the SDK is dropped by the SDK. Both interfaces
are listed below and you need to import them into your build environment.

.. important::
  It is required that you place the AIDL files under subdirectory
  "aidl/com.governikus.ausweisapp2". Also the interface methods
  names must be exactly the same.

.. seealso::

  https://developer.android.com/guide/components/aidl.html

.. note::
  If you implement the integrated variant beside the external variant you do
  **not** need to manually add AIDL files as the AAR package already provides
  those interfaces.


Interface
"""""""""

.. code-block:: java

  package com.governikus.ausweisapp2;

  import com.governikus.ausweisapp2.IAusweisApp2SdkCallback;
  import android.nfc.Tag;

  interface IAusweisApp2Sdk
  {
      boolean connectSdk(IAusweisApp2SdkCallback pCallback);
      boolean send(String pSessionId, String pMessageFromClient);
      boolean updateNfcTag(String pSessionId, in Tag pTag);
  }



Callback
""""""""

.. code-block:: java

  package com.governikus.ausweisapp2;

  interface IAusweisApp2SdkCallback
  {
      void sessionIdGenerated(String pSessionId, boolean pIsSecureSessionId);
      void receive(String pJson);
      void sdkDisconnected();
  }




Background service
------------------
The integrated and external variants use the same method to establish
a connection to the AusweisApp2 SDK. The AusweisApp2 SDK is a background
service in the external AusweisApp2 or an embedded background service
in your own application.


.. _android_binding_service:

Binding to the service
^^^^^^^^^^^^^^^^^^^^^^
In order to start the AusweisApp2 SDK it is necessary to bind to the
Android service supplied by the SDK.
This binding fulfils two purposes:

  - First it starts the SDK.

  - Second it enables the client to establish an IPC connection as
    mentioned above.


Due to the nature of an Android service, there can be only one instance of
the SDK running. If multiple clients bind to the service, they are interacting
with the same instance of the service.
The service is terminated once all previously bound clients are unbound.

To differentiate between different connected clients, virtual sessions are used
once the binding is completed. These sessions are discussed in a separate
section, section :ref:`android_create_session`.



Create connection
"""""""""""""""""
First of all, in order to bind to the service, one needs to instantiate an
Android ServiceConnection.
Subsequently, the object is passed to the Android API and the contained
methods are invoked by Android on service connection and disconnection.


.. code-block:: java

  import android.content.ServiceConnection;

  // [...]

  ServiceConnection mConnection = new ServiceConnection()
  {
    @Override
    public void onServiceConnected(ComponentName className, IBinder service)
    {
        // ... details below
    }

    @Override
    public void onServiceDisconnected(ComponentName className)
    {
        // ... details below
    }
  }



.. _android_raw_connection:

Bind service to raw connection
""""""""""""""""""""""""""""""
In order to perform the actual binding a directed Intent, which identifies
the AusweisApp2 SDK, is created.
This Intent is sent to the Android API along with the ServiceConnection
created above. This API call either starts up the SDK if it is the
first client, or connects to the running SDK instance if there is already
another client bound.

If you use the external variant of AusweisApp2 you need to pass the package
name of Governikus. Otherwise you need to pass your own package name
as the integrated variant is a background service of your application.


.. code-block:: java

  import android.app.Activity;
  import android.content.Context;
  import android.content.Intent;

  // [...]

  String pkg = "com.governikus.ausweisapp2";

  boolean useIntegrated = true; // use external or integrated
  if (useIntegrated)
    pkg = getApplicationContext().getPackageName();

  String name = "com.governikus.ausweisapp2.START_SERVICE";
  Intent serviceIntent = new Intent(name);
  serviceIntent.setPackage(pkg);
  bindService(serviceIntent, mConnection, Context.BIND_AUTO_CREATE);

.. seealso::

  https://developer.android.com/guide/components/bound-services.html

  https://developer.android.com/reference/android/app/Activity.html



Redirect to Play Store
""""""""""""""""""""""
It is necessary that AusweisApp2 is installed in order to use the external SDK.
It is recommended to check and display a message in case the user needs
to install AusweisApp2 first. Also, the user should be redirected to
the Play Store entry to find the app.


.. code-block:: java

  import android.content.ActivityNotFoundException;
  import android.content.pm.ResolveInfo;
  import android.net.Uri;
  import java.util.List;

  PackageManager m = getPackageManager();
  List<ResolveInfo> list = m.queryIntentServices(serviceIntent, PackageManager.MATCH_ALL);

  if (list == null || list.isEmpty())
  {
    final String name = "com.governikus.ausweisapp2";
    try {
      startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=" + name)));
    } catch (ActivityNotFoundException e) {
      // Use the browser if Play Store is not installed, too!
      startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://play.google.com/store/apps/details?id=" + name)));
    }
  }

.. note::
  This is not necessary if you use the integrated variant.



.. _android_init_aidl:

Initializing the AIDL connection
""""""""""""""""""""""""""""""""
Once the Android service of the AusweisApp2 SDK is successfully started
and bound to by the client,
the Android system calls the onServiceConnected method of the ServiceConnection
created and supplied above.
This method receives an instance of the IBinder Android service interface.

The IBinder is then used by the client application to initialize the auto
generated AIDL stub
in order to use the AIDL IPC mechanism.
The used stub is supposed to be auto generated by the Android SDK if you have
properly configured your build environment.

The stub initialization returns an instance of **IAusweisApp2Sdk** which is used
to interact with the SDK.
The example below stores this instance in the member variable mSdk.

.. code-block:: java

  import android.content.ComponentName;
  import android.content.ServiceConnection;
  import android.os.IBinder;

  import com.governikus.ausweisapp2.IAusweisApp2Sdk;

  // [...]

  IAusweisApp2Sdk mSdk;

  ServiceConnection mConnection = new ServiceConnection(){
    @Override
    public void onServiceConnected(ComponentName className, IBinder service)
    {
        try {
            mSdk = IAusweisApp2Sdk.Stub.asInterface(service);
        } catch (ClassCastException|RemoteException e) {
            // ...
        }
    }

    @Override
    public void onServiceDisconnected(ComponentName className)
    {
        mSdk = null;
    }
  }

.. seealso::

  :ref:`android_import_aidl`



.. _android_create_session:

Create session to AusweisApp2
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Once your client is bound to the AusweisApp2 SDK service and you have initialized
the AIDL IPC mechanism, you are ready to use the actual SDK API.

Since the Android system does not allow to limit the number of clients which
can connect to a service, the SDK API uses custom **sessions** to manage the
connected clients. There is a maximum of one established session at a time.

In order to open a session with the SDK you need to pass an
instance of **IAusweisApp2SdkCallback** to the **connectSdk** function of your
previously acquired instance of **IAusweisApp2Sdk**. If your callback is accepted,
the function returns true. Otherwise there is a problem with your supplied callback.
Sessions will be disconnected once the IBinder instance of the connected client is
invalidated, another communication error occurs or another Client connects. Please see
:ref:`android_disconnect_sdk` for instructions to gracefully disconnect from the SDK.

As mentioned above: If there already is a connected client and a second client attempts
to connect, the first client is disconnected and the second client is granted exclusive
access to the SDK. The first client is informed via its callback by **sdkDisconnected**.
The second client is presented a fresh environment and it has no access to any data of
the first client.

If you have successfully established a session, the **sessionIdGenerated** function
of your callback is invoked. With this invocation you receive two arguments.
**pIsSecureSessionId** is true if the SDK was able to gather enough entropy in
order to generate a secure random session ID. If it is false, there is no
session ID passed. There is nothing you can do about such an error. It results from
a problem with the random number generator, which in turn is very likely the result of
an ongoing local attack. The device should be considered manipulated and the user should be
informed.

On success **pSessionId** holds the actual session ID generated by the SDK.
This ID is used to identify your session and you need to pass it to all future SDK
function invocations of this session.

The listing below shows an example for an instantiation of IAusweisApp2SdkCallback
and establishing a session.




.. code-block:: java

  import com.governikus.ausweisapp2.IAusweisApp2Sdk;
  import com.governikus.ausweisapp2.IAusweisApp2SdkCallback;

  // [...]

  LocalCallback mCallback = new LocalCallback();
  class LocalCallback extends IAusweisApp2SdkCallback.Stub
  {
    public String mSessionID = null;

    @Override
    public void sessionIdGenerated(
      String pSessionId, boolean pIsSecureSessionId) throws RemoteException
    {
        mSessionID = pSessionId;
    }

    @Override
    public void receive(String pJson) throws RemoteException
    {
        // handle message from SDK
    }
  }

  // [...]

  try
  {
    if (!mSdk.connectSdk(mCallback))
    {
        // already connected? Handle error...
    }
  }
  catch (RemoteException e)
  {
      // handle exception
  }

.. seealso::

  :ref:`android_init_aidl`
  :ref:`android_disconnect_sdk`



Send command
""""""""""""
In order to send a JSON command to the AusweisApp2 SDK, you need to invoke
the **send** function of your instance of **IAusweisApp2Sdk**. For this command
to be processed by the SDK you need to supply the session ID which you have
previously received. The listing below shows an example.



.. code-block:: java

  String cmd = "{\"cmd\": \"GET_INFO\"}";
  try
  {
    if (!mSdk.send(mCallback.mSessionID, cmd))
    {
        // disconnected? Handle error...
    }
  }
  catch (RemoteException e)
  {
      // handle exception
  }




Receive message
"""""""""""""""
Messages from the AusweisApp2 SDK are passed to you via the same instance of
**IAusweisApp2SdkCallback** in which you have received the session ID.
The **receive** method is called each time the SDK sends a message.

.. seealso::

  :ref:`android_create_session`


.. _android_disconnect_sdk:

Disconnect from SDK
^^^^^^^^^^^^^^^^^^^
In order to disconnect from the AusweisApp2 SDK you need to invalidate your
instance of **IBinder**. There are two possibilities to do this. The first
one is to unbind from the SDK Android service to undo your binding, like
shown in the code listing below. The second one is to return false in the
**pingBinder** function of your IBinder instance.

.. code-block:: java

  unbindService(mConnection);

.. seealso::

  :ref:`android_binding_service`

  https://developer.android.com/reference/android/os/IBinder.html




.. _android_nfc_tags:

Passing NFC tags to the SDK
^^^^^^^^^^^^^^^^^^^^^^^^^^^
NFC tags can only be detected by applications which have a foreground window
on the Android platform. A common workaround for this problem is
to equip background services with a transparent window which is shown
to dispatch NFC tags.

However, if there are multiple applications installed, which are capable
of dispatching NFC tags, the Android system will display an **App Chooser**
for each discovered tag enabling the user to select the appropriate application
to handle the NFC tag. To have such a chooser
display the name and image of the client application instead of the SDK,
the client application is required to dispatch discovered NFC tags and to
pass them to the SDK.

Furthermore, this interface design enables the client application to do
**foreground dispatching** of NFC tags. If the active application registers itself for
foreground dispatching, it receives discovered NFC tags directly without
Android displaying an App Chooser.


Permissions in AndroidManifest.xml
""""""""""""""""""""""""""""""""""
The client applications needs to register the NFC permission as shown in the
listing below in order to access the NFC reader hardware.

.. code-block:: xml

  <uses-permission android:name="android.permission.NFC"/>


.. seealso::

  https://developer.android.com/guide/topics/security/permissions.html

.. note::
  The integrated variant already provides an **AndroidManifest.xml** with
  prepared permissions.



Intent-Filter in AndroidManifest.xml
""""""""""""""""""""""""""""""""""""
In order to be informed about attached NFC tags by Android, the client
application is required to register an intent filter. The appropriate
filter is shown in the listing below.

.. code-block:: xml

  <intent-filter>
    <action android:name="android.nfc.action.TECH_DISCOVERED" />
  </intent-filter>
  <meta-data android:name="android.nfc.action.TECH_DISCOVERED" android:resource="@xml/nfc_tech_filter" />

.. seealso::

  https://developer.android.com/guide/components/intents-filters.html


NFC Technology Filter
"""""""""""""""""""""
Since there are many different kinds of NFC tags, Android requires the
application to register a technology filter for the kind of tags the application
wants to receive. The proper filter for the German eID card is shown
in the listing below.

.. code-block:: xml

  <resources xmlns:xliff="urn:oasis:names:tc:xliff:document:1.2">
    <tech-list>
      <tech>android.nfc.tech.IsoDep</tech>
    </tech-list>
  </resources>



Implementation
""""""""""""""
As it is common on the Android platform, information is sent to applications
encapsulated in instances of the **Intent** class. In order to process newly
discovered NFC tags, Intents which are given to the application need to be
checked for the parcelable NFC extra as shown in the code listing below.
Subsequently the client is required to send them to the AusweisApp2 SDK by
calling the **updateNfcTag** method (of) the previously acquired **IAusweisApp2Sdk**
instance.
The listing below shows an example for the described process.


.. code-block:: java

  import android.content.Intent;
  import android.nfc.NfcAdapter;
  import android.nfc.Tag;

  import com.governikus.ausweisapp2.IAusweisApp2Sdk;
  import com.governikus.ausweisapp2.IAusweisApp2SdkCallback;

  // [...]

  void handleIntent(Intent intent)
  {
    final Tag tag = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG);
    if (tag != null)
    {
      try {
        mSdk.updateNfcTag(mCallback.mSessionID, tag);
      } catch (RemoteException e) {
        // ...
      }
    }
  }






Dispatching NFC tags in foreground
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
As already mentioned under :ref:`android_nfc_tags`, an App Chooser is displayed
for discovered NFC tags by Android if multiple applications which are able to
dispatch NFC tags are installed. An application can suppress this App Chooser
if it registers itself for **foreground dispatching** at runtime. This way NFC
tags are send directly to the registered application without a chooser being
displayed. An example implementation of the required steps in order to register
are shown in code listing below.


.. code-block:: java

  import android.app.Activity;
  import android.nfc.NfcAdapter;
  import android.content.Intent;
  import android.app.PendingIntent;
  import android.content.IntentFilter;
  import android.nfc.tech.IsoDep;

  class ForegroundDispatcher
  {
    private final Activity mActivity;
    private final NfcAdapter mAdapter;
    private final PendingIntent mPendingIntent;
    private final IntentFilter[] mFilters;
    private final String[][] mTechLists;

    ForegroundDispatcher(Activity pActivity)
    {
      mActivity = pActivity;
      mAdapter = NfcAdapter.getDefaultAdapter(mActivity);
      Intent intent = new Intent(mActivity, mActivity.getClass()).
        addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
      mPendingIntent = PendingIntent.getActivity(mActivity, 0, intent, 0);

      mFilters = new IntentFilter[] {
              new IntentFilter(NfcAdapter.ACTION_TECH_DISCOVERED)
      };
      mTechLists = new String[][] { new String[] {
              IsoDep.class.getName()
      } };
    }

    void enable()
    {
      if (mAdapter != null)
        mAdapter.enableForegroundDispatch(mActivity,
                                          mPendingIntent,
                                          mFilters,
                                          mTechLists);
    }

    void disable()
    {
      if (mAdapter != null)
        mAdapter.disableForegroundDispatch(mActivity);
    }
  }

  // [...]

  ForegroundDispatcher mDispatcher = new ForegroundDispatcher(this);



The example implementation from above needs to be invoked when the application
is brought to foreground and when it looses focus. An example for appropriate
places are the **onResume** and the **onPause** methods of Activities as shown
in the code listing below.

.. code-block:: java

  @Override
  public void onResume()
  {
    super.onResume();
    mDispatcher.enable();
  }

  @Override
  public void onPause()
  {
    super.onPause();
    mDispatcher.disable();
  }


.. seealso::

  https://developer.android.com/reference/android/app/Activity.html#ActivityLifecycle
