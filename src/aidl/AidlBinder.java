package com.governikus.ausweisapp2;

import android.nfc.Tag;
import android.os.DeadObjectException;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import java.util.HashMap;
import java.util.Map;

class AidlBinder extends IAusweisApp2Sdk.Stub
{
	public final static String LOG_TAG = AusweisApp2Service.LOG_TAG;
	private final AusweisApp2Service mService;
	private IAusweisApp2SdkCallback mCallback = null;
	private String mCallbackSessionId = null;

	public AidlBinder(AusweisApp2Service pService)
	{
		mService = pService;
	}


	private void cleanUpDeadCallback()
	{
		if (mCallback == null)
		{
			return;
		}

		IBinder binder = mCallback.asBinder();
		if (!binder.isBinderAlive() || !binder.pingBinder())
		{
			Log.i(LOG_TAG, "Android service: Removing dead callback.");
			mCallback = null;
		}
	}


	private void handleRemoteException(RemoteException pException)
	{
		Log.w(LOG_TAG, "Android service: Connected client send an exception. Dropping client.", pException);
		mCallback = null;
	}


	public synchronized boolean connectSdk(IAusweisApp2SdkCallback pCallback)
	{
		if (pCallback == null)
		{
			Log.w(LOG_TAG, "Android service: Supplied callback is null.");
			return false;
		}

		cleanUpDeadCallback();
		if (mCallback != null)
		{
			Log.w(LOG_TAG, "Android service: A client is already connected. Ignoring newly supplied callback.");
			return false;
		}


		mCallback = pCallback;
		mCallbackSessionId = resetValidSessionID("");
		final boolean secureSessionId = isSecureRandomPsk();
		Log.i(LOG_TAG, "Android service: Callback connected.");

		try
		{
			mCallback.sessionIdGenerated(secureSessionId ? mCallbackSessionId : null, secureSessionId);
		}
		catch (RemoteException e)
		{
			handleRemoteException(e);
		}
		return true;
	}


	private boolean isValidSessionId(String pSessionId)
	{
		if (mCallback == null || pSessionId.compareTo(mCallbackSessionId) != 0)
		{
			Log.w(LOG_TAG, "Android service: Invalid sessiond ID!");
			return false;
		}
		return true;
	}


	public synchronized boolean send(String pSessionId, String pMessageFromClient)
	{
		Log.d(LOG_TAG, "Android service: Received JSON from client");

		if (!isValidSessionId(mCallbackSessionId))
		{
			return false;
		}

		aidlSend(pMessageFromClient);
		return true;
	}


	public synchronized boolean updateNfcTag(String pSessionId, Tag pTag)
	{
		if (!isValidSessionId(mCallbackSessionId))
		{
			return false;
		}

		mService.getNfcConnector().updateNfcTag(pTag);
		return true;
	}


	// FIXME This function is meant to return void istead of java.lang.Object but JNI does not like void
	public synchronized Object aidlReceive(String pMessageToClient)
	{
		Log.d(LOG_TAG, "Android service: Passing JSON to client");

		if (mCallback == null)
		{
			Log.d(LOG_TAG, "Android service: Callback not connected.");
			return null;
		}

		try
		{
			mCallback.receive(pMessageToClient);
		}
		catch (DeadObjectException e)
		{
			Log.w(LOG_TAG, "Android service: Connected client is already dead.");
			mCallback = null;
		}
		catch (RemoteException e)
		{
			handleRemoteException(e);
		}

		return null;
	}


	private native String resetValidSessionID(String pPsk);
	private native boolean isSecureRandomPsk();
	private native void aidlSend(String pMessageFromClient);
}
