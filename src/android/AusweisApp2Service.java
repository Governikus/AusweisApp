/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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

		super.onDestroy();

		// Workaround. When bound & unbound the QtService is in a funny state causing
		// "WARNING: QApplication was not created in the main() thread." and a crash on
		// first rebind.
		// This workaround is inspired by https://bugreports.qt.io/browse/QTBUG-54012 .
		System.exit(0); // NOPMD see comment above
	}


}
