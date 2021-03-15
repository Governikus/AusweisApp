/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import org.qtproject.qt5.android.bindings.QtService;

import android.content.Intent;
import android.os.IBinder;
import android.util.Log;


public class AusweisApp2Service
	extends QtService
{
	public static final String LOG_TAG = "AusweisApp2";
	private final AidlBinder mBinder = new AidlBinder();

	public AidlBinder getAidlBinder()
	{
		return mBinder;
	}


	@Override
	public IBinder onBind(Intent intent)
	{
		Log.d(LOG_TAG, "Android service bound.");
		return mBinder;
	}


	@Override
	public boolean onUnbind(Intent intent)
	{
		Log.d(LOG_TAG, "Android service unbound.");
		return super.onUnbind(intent);
	}


	@Override
	public void onCreate()
	{
		Log.d(LOG_TAG, "Android service created.");
		super.onCreate();
	}


	@Override
	public void onDestroy()
	{
		Log.d(LOG_TAG, "Android service destroyed.");

		super.onDestroy();

		// Workaround. When bound & unbound the QtService is in a funny state causing
		// "WARNING: QApplication was not created in the main() thread." and a crash on
		// first rebind.
		// This workaround is inspired by https://bugreports.qt.io/browse/QTBUG-54012 .
		System.exit(0); // NOPMD see comment above
	}


}
