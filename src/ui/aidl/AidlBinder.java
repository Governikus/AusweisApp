/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.util.logging.Level;

import android.content.Intent;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.os.DeadObjectException;
import android.os.IBinder;

import org.qtproject.qt.android.QtNative;

class AidlBinder extends IAusweisApp2Sdk.Stub
{
	private IAusweisApp2SdkCallback mCallback;
	private String mCallbackSessionId;

	private void cleanUpDeadCallback()
	{
		if (mCallback == null)
		{
			return;
		}

		IBinder binder = mCallback.asBinder();
		if (!binder.isBinderAlive() || !binder.pingBinder())
		{
			LogHandler.getLogger().info("Android service: Removing dead callback.");
			mCallback = null;
		}
	}


	private void handleClientException(Throwable pException)
	{
		LogHandler.getLogger().log(Level.WARNING, "Android service: Connected client sent an exception. Dropping client.", pException);
		mCallback = null;
	}


	public synchronized boolean connectSdk(IAusweisApp2SdkCallback pCallback)
	{
		if (pCallback == null)
		{
			LogHandler.getLogger().warning("Android service: Supplied callback is null.");
			return false;
		}

		cleanUpDeadCallback();
		if (mCallback != null)
		{
			LogHandler.getLogger().info("Android service: A client is already connected. Dropping previous callback.");
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
		LogHandler.getLogger().info("Android service: Callback connected.");

		try
		{
			mCallback.sessionIdGenerated(sessionIdIsSecure ? mCallbackSessionId : null, sessionIdIsSecure);
		}
		catch (Throwable t)
		{
			handleClientException(t);
		}

		return startReaderManagerScans() && sessionIdIsSecure;
	}


	private boolean isValidSessionId(String pSessionId)
	{
		if (mCallback == null || pSessionId.compareTo(mCallbackSessionId) != 0)
		{
			LogHandler.getLogger().warning("Android service: Invalid sessiond ID!");
			return false;
		}
		return true;
	}


	public synchronized boolean send(String pSessionId, String pMessageFromClient)
	{
		LogHandler.getLogger().info("Android service: Received JSON from client");

		if (!isValidSessionId(pSessionId))
		{
			return false;
		}

		aidlSend(pMessageFromClient);
		return true;
	}


	public synchronized boolean updateNfcTag(String pSessionId, Tag pTag)
	{
		LogHandler.getLogger().info("Android service: Received nfc tag from client");

		if (!isValidSessionId(pSessionId))
		{
			return false;
		}

		Intent newIntent = new Intent();
		newIntent.putExtra(NfcAdapter.EXTRA_TAG, pTag);
		QtNative.onNewIntent(newIntent);

		return true;
	}


	public synchronized void aidlReceive(String pMessageToClient)
	{
		LogHandler.getLogger().info("Android service: Passing JSON to client");

		if (mCallback == null)
		{
			LogHandler.getLogger().info("Android service: Callback not connected.");
			return;
		}

		try
		{
			mCallback.receive(pMessageToClient);
		}
		catch (DeadObjectException e)
		{
			LogHandler.getLogger().warning("Android service: Connected client is already dead.");
			mCallback = null;
		}
		catch (Throwable t)
		{
			handleClientException(t);
		}
	}


	private native String resetValidSessionID();
	private native boolean isSecureRandomPsk();
	private native boolean startReaderManagerScans();
	private native void aidlSend(String pMessageFromClient);
}
