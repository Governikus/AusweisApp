/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import android.content.Context;
import android.net.wifi.WifiManager;

public final class MulticastLockJniBridgeUtil
{
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
		LogHandler.getLogger().info("Multicast lock: " + cLock.toString());
	}


	public static synchronized void release(Context pContext)
	{
		cLock.release();
		LogHandler.getLogger().info("Multicast lock released.");
	}


}
