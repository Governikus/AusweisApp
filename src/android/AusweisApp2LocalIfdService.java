/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.nio.charset.StandardCharsets;
import java.util.logging.Level;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;

import org.qtproject.qt.android.bindings.QtService;

public final class AusweisApp2LocalIfdService extends QtService
{
	private static final String ACTION_ABORT_WORKFLOW = AusweisApp2LocalIfdService.class.getCanonicalName() + ".abort";
	public static final String PARAM_TLS_WEBSOCKET_PSK = "PSK";
	public static final String PARAM_SERVICE_TOKEN = "SERVICE_TOKEN";

	private final IBinder mBinder = new AusweisApp2LocalIfdServiceBinder();

	// Native method provided by UiPluginLocalIfd
	public static native boolean notifyStartWorkflow(String pPsk);
	public static native void notifyAbortWorkflow();
	public static native boolean verifyServiceToken(String token);

	public class AusweisApp2LocalIfdServiceBinder extends Binder
	{
		AusweisApp2LocalIfdService getService()
		{
			return AusweisApp2LocalIfdService.this;
		}


	}

	private static boolean isServiceRunning(Context context)
	{
		final String serviceName = AusweisApp2LocalIfdService.class.getName();
		ActivityManager manager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
		for (RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE))
		{
			if (serviceName.equals(service.service.getClassName()))
			{
				return true;
			}
		}
		return false;
	}


	public static void abortWorkflow(Context context, String serviceToken)
	{
		if (!isServiceRunning(context))
		{
			return;
		}

		Intent abortWorkflowIntent = new Intent(context, AusweisApp2LocalIfdService.class);
		abortWorkflowIntent.setAction(ACTION_ABORT_WORKFLOW);
		abortWorkflowIntent.putExtra(PARAM_SERVICE_TOKEN, serviceToken);

		try
		{
			LogHandler.getLogger().info("Aborting LocalIfdService workflow");
			context.startService(abortWorkflowIntent);
		}
		catch (Exception e)
		{
			LogHandler.getLogger().log(Level.SEVERE, "Could not abort LocalIfdService workflow", e);
		}
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

		if (ACTION_ABORT_WORKFLOW.equals(intent.getAction()))
		{
			abortWorkflow();
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

		String psk;
		try
		{
			if (intent.getExtras().get(PARAM_TLS_WEBSOCKET_PSK).getClass() == String.class)
			{
				psk = intent.getStringExtra(PARAM_TLS_WEBSOCKET_PSK);
				LogHandler.getLogger().info("Received string parameter");
			}
			else
			{
				psk = new String(intent.getByteArrayExtra(PARAM_TLS_WEBSOCKET_PSK), StandardCharsets.UTF_8);
				LogHandler.getLogger().info("Received byte[] parameter");
			}
		}
		catch (Exception e)
		{
			LogHandler.getLogger().severe("Could not get string parameter " + PARAM_TLS_WEBSOCKET_PSK + " from Intent");
			return null;
		}

		if (psk == null || psk.isEmpty())
		{
			LogHandler.getLogger().severe("Parameter " + PARAM_TLS_WEBSOCKET_PSK + " must not be empty");
			return null;
		}

		if (!notifyStartWorkflow(psk))
		{
			LogHandler.getLogger().severe("Could not start LocalIfdService");
			return null;
		}

		LogHandler.getLogger().info("LocalIfdService bound");

		return mBinder;
	}


	@Override
	public boolean onUnbind(Intent intent)
	{
		LogHandler.getLogger().info("LocalIfdService unbound");
		abortWorkflow();
		return false;
	}


	@Override
	public void onCreate()
	{
		super.onCreate();
		LogHandler.getLogger().info("LocalIfdService created");
	}


	@Override
	public void onDestroy()
	{
		LogHandler.getLogger().info("LocalIfdService destroyed");
		BootstrapHelper.triggerShutdown();
		super.onDestroy();
	}


	private void abortWorkflow()
	{
		notifyAbortWorkflow();
		stopSelf();
	}


}
