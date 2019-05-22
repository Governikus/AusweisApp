/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import android.content.Context;
import android.net.wifi.WifiManager;

public final class WifiInfo
{
	private WifiInfo()
	{
	}


	public static boolean wifiEnabled(Context pContext)
	{
		WifiManager wifi;
		wifi = (WifiManager) pContext.getSystemService(Context.WIFI_SERVICE);

		switch (wifi.getWifiState())
		{
			case WifiManager.WIFI_STATE_ENABLED:
			case WifiManager.WIFI_STATE_ENABLING:
				return true;

			default:
				return false;
		}
	}


	public static boolean enableWifi(Context pContext)
	{
		WifiManager wifi;
		wifi = (WifiManager) pContext.getSystemService(Context.WIFI_SERVICE);

		wifi.setWifiEnabled(true);
		return true;
	}


}
