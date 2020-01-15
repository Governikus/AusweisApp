/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.util.Log;


public final class MulticastLockJniBridgeUtil
{
	private static final String LOG_TAG = AusweisApp2Service.LOG_TAG;

	private static WifiManager.MulticastLock cLock;


	private MulticastLockJniBridgeUtil()
	{
	}


	public static synchronized void acquire(Context pContext)
	{
		if (cLock == null)
		{
			WifiManager wifi = (WifiManager) pContext.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
			cLock = wifi.createMulticastLock("AusweisApp2");
			cLock.setReferenceCounted(true);
		}

		cLock.acquire();
		Log.d(LOG_TAG, "Multicast lock: " + cLock.toString());
	}


	public static synchronized void release(Context pContext)
	{
		cLock.release();
		Log.d(LOG_TAG, "Multicast lock released.");
	}


}
