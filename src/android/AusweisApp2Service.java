/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import android.content.Intent;
import android.os.IBinder;

import org.qtproject.qt.android.bindings.QtService;

public class AusweisApp2Service
	extends QtService
{
	private final AidlBinder mBinder = new AidlBinder();

	public AidlBinder getAidlBinder()
	{
		return mBinder;
	}


	@Override
	public IBinder onBind(Intent intent)
	{
		LogHandler.getLogger().info("Android service bound.");
		return mBinder;
	}


	@Override
	public boolean onUnbind(Intent intent)
	{
		LogHandler.getLogger().info("Android service unbound.");
		return super.onUnbind(intent);
	}


	@Override
	public void onCreate()
	{
		LogHandler.getLogger().info("Android service created.");
		super.onCreate();
	}


	@Override
	public void onDestroy()
	{
		LogHandler.getLogger().info("Android service destroyed.");
		BootstrapHelper.triggerShutdown();
		super.onDestroy();
	}


}
