package com.governikus.ausweisapp2;

import android.nfc.Tag;
import android.os.DeadObjectException;
import android.os.IBinder;
import android.util.Log;

import java.lang.Throwable;
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


	private void handleClientException(Throwable pException)
	{
		Log.w(LOG_TAG, "Android service: Connected client sent an exception. Dropping client.", pException);
		mCallback = null;
	}


	public synchronized boolean connectSdk(IAusweisApp2SdkCallback pCallback)
	{
		if (pCallback == null)
		{
			Log.w(LOG_TAG, "Android service: Supplied callback is null.");
			return false;
		}

		if (pCallback == mCallback)
		{
			Log.i(LOG_TAG, "Android service: Supplied callback is already in use.");
			return true;
		}

		cleanUpDeadCallback();
		if (mCallback != null)
		{
			Log.i(LOG_TAG, "Android service: A client is already connected. Dropping previous callback.");
			try
			{
				mCallbackSessionId = null;
				mCallback.sdkDisconnected();
			}
			catch (Throwable t)
			{
				handleClientException(t);
			}
		}


		mCallbackSessionId = resetValidSessionID();
		if (mCallbackSessionId.isEmpty())
		{
			return false;
		}

		mCallback = pCallback;
		final boolean sessionIdIsSecure = isSecureRandomPsk();
		Log.i(LOG_TAG, "Android service: Callback connected.");

		try
		{
			mCallback.sessionIdGenerated(sessionIdIsSecure ? mCallbackSessionId : null, sessionIdIsSecure);
		}
		catch (Throwable t)
		{
			handleClientException(t);
		}
		return sessionIdIsSecure;
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


	public synchronized void aidlReceive(String pMessageToClient)
	{
		Log.d(LOG_TAG, "Android service: Passing JSON to client");

		if (mCallback == null)
		{
			Log.d(LOG_TAG, "Android service: Callback not connected.");
			return;
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
		catch (Throwable t)
		{
			handleClientException(t);
		}
	}


	private native String resetValidSessionID();
	private native boolean isSecureRandomPsk();
	private native void aidlSend(String pMessageFromClient);
}
