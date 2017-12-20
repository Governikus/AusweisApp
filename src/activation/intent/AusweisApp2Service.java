/*
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import android.content.Intent;
import android.nfc.Tag;
import android.os.BadParcelableException;
import android.os.Binder;
import android.os.IBinder;
import android.os.Parcel;
import android.os.RemoteException;
import android.util.Log;
import java.util.HashMap;
import java.util.Map;

import org.qtproject.qt5.android.bindings.QtService;


public class AusweisApp2Service
	extends QtService
{
	public final static String LOG_TAG = "AusweisApp2";
	private final AidlBinder mBinder = new AidlBinder(this);

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

		// register the broadcast receiver after loading the C++ library in super.onCreate()
		AndroidBluetoothReceiver.register(this);
	}


	@Override
	public void onDestroy()
	{
		Log.d(LOG_TAG, "Android service destroyed.");

		// unregister the broadcast receiver before unloading the C++ library in super.onDestroy()
		AndroidBluetoothReceiver.unregister(this);

		super.onDestroy();

		// Workaround. When bound & unbound the QtService is in a funny state causing
		// "WARNING: QApplication was not created in the main() thread." and a crash on
		// first rebind.
		// This workaround is inspired by https://bugreports.qt.io/browse/QTBUG-54012 .
		System.exit(0);
	}


}
