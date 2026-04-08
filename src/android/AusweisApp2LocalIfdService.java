/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.nio.charset.StandardCharsets;
import java.util.logging.Level;

import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.ResultReceiver;

import org.qtproject.qt.android.bindings.QtService;

public final class AusweisApp2LocalIfdService extends QtService
{
	private static final String ACTION_REMOTE_IFD = AusweisApp2LocalIfdService.class.getCanonicalName() + ".remoteIfd";
	public static final String PARAM_TLS_WEBSOCKET_PSK = "PSK";
	public static final String PARAM_SERVICE_TOKEN = "SERVICE_TOKEN";
	public static final String PARAM_ENABLED = "ENABLED";
	public static final String PARAM_RECEIVER = "RECEIVER";

	private final IBinder mBinder = new AusweisApp2LocalIfdServiceBinder();

	// Native methods provided by UiPluginLocalIfd
	public static native boolean notifyStartWorkflow(String pPsk);
	public static native void notifyAbortWorkflow();
	public static native void notifyEnableLocalIfdDetection(boolean pEnable);
	public static native boolean verifyServiceToken(String pToken);

	// Native method provided by RemoteIfdClient
	public static native void notifyLocalIfdDisabled();

	public class AusweisApp2LocalIfdServiceBinder extends Binder
	{
		AusweisApp2LocalIfdService getService()
		{
			return AusweisApp2LocalIfdService.this;
		}


	}


	public static void enableLocalIfdDetection(Context context, String serviceToken, boolean enable)
	{
		Intent serviceIntent = new Intent(context, AusweisApp2LocalIfdService.class);
		serviceIntent.setAction(ACTION_REMOTE_IFD);
		serviceIntent.putExtra(PARAM_SERVICE_TOKEN, serviceToken);
		serviceIntent.putExtra(PARAM_ENABLED, enable);

		if (!enable)
		{
			// We only need to listen if the detection was stopped to be sure to be able to bind the port.
			ResultReceiver receiver = new ResultReceiver(new Handler(Looper.getMainLooper()))
			{
				@Override
				protected void onReceiveResult(int resultCode, Bundle resultData)
				{
					notifyLocalIfdDisabled();
				}

			};
			serviceIntent.putExtra(PARAM_RECEIVER, receiver);
		}

		try
		{
			context.startService(serviceIntent);
		}
		catch (Exception e)
		{
			LogHandler.getLogger().log(Level.SEVERE, "Could not change Local IFD detection", e);
		}
	}


	@SuppressWarnings("deprecation")
	private ResultReceiver getReceiver(Intent intent)
	{
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU) // API 33, Android 13
		{
			return intent.getParcelableExtra(PARAM_RECEIVER);
		}

		return intent.getParcelableExtra(PARAM_RECEIVER, ResultReceiver.class);
	}


	@Override
	public int onStartCommand(Intent intent, int flags, int startId)
	{
		if (intent == null)
		{
			return START_NOT_STICKY;
		}

		if (!intent.hasExtra(PARAM_SERVICE_TOKEN))
		{
			LogHandler.getLogger().severe("Missing required parameter " + PARAM_SERVICE_TOKEN);
			return START_NOT_STICKY;
		}

		String serviceToken = intent.getStringExtra(PARAM_SERVICE_TOKEN);
		if (!verifyServiceToken(serviceToken))
		{
			LogHandler.getLogger().severe("Command not allowed");
			return START_NOT_STICKY;
		}

		if (ACTION_REMOTE_IFD.equals(intent.getAction()))
		{
			if (!intent.hasExtra(PARAM_ENABLED))
			{
				LogHandler.getLogger().severe("Missing required parameter " + PARAM_ENABLED);
				return START_NOT_STICKY;
			}

			boolean enabled = intent.getBooleanExtra(PARAM_ENABLED, false);
			notifyEnableLocalIfdDetection(enabled);

			ResultReceiver receiver = getReceiver(intent);
			if (receiver != null)
			{
				receiver.send(0, Bundle.EMPTY);
			}
		}

		return START_NOT_STICKY;
	}


	@Override
	public IBinder onBind(Intent intent)
	{
		if (!intent.hasExtra(PARAM_TLS_WEBSOCKET_PSK))
		{
			LogHandler.getLogger().severe("Missing required parameter " + PARAM_TLS_WEBSOCKET_PSK);
			return null;
		}

		String psk = intent.getStringExtra(PARAM_TLS_WEBSOCKET_PSK);
		if (psk != null)
		{
			LogHandler.getLogger().info("Received string parameter");
		}
		else
		{
			byte[] bytes = intent.getByteArrayExtra(PARAM_TLS_WEBSOCKET_PSK);
			if (bytes != null)
			{
				psk = new String(bytes, StandardCharsets.UTF_8);
				LogHandler.getLogger().info("Received byte[] parameter");
			}
		}

		if (psk == null || psk.isEmpty())
		{
			LogHandler.getLogger().severe("Parameter " + PARAM_TLS_WEBSOCKET_PSK + " must not be empty");
			return null;
		}

		if (!notifyStartWorkflow(psk))
		{
			LogHandler.getLogger().severe("Could not start Local IFD Service");
			return null;
		}

		LogHandler.getLogger().info("Local IFD Service bound");

		return mBinder;
	}


	@Override
	public boolean onUnbind(Intent intent)
	{
		LogHandler.getLogger().info("Local IFD Service unbound");
		notifyAbortWorkflow();
		stopSelf();
		return false;
	}


	@Override
	public void onCreate()
	{
		super.onCreate();
		LogHandler.getLogger().info("Local IFD Service created");
	}


	@Override
	public void onDestroy()
	{
		LogHandler.getLogger().info("Local IFD Service destroyed");
		BootstrapHelper.triggerShutdown();
		super.onDestroy();
	}


}
