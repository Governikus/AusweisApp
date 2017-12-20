/*
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkInfo;
import android.os.Build;
import android.util.Log;

public class WifiInfo
{
	private static final String TAG = "AusweisApp2";

	public static boolean wifiEnabled(Context context)
	{
		ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);

		if (android.os.Build.VERSION.SDK_INT < 21)
		{
			for (NetworkInfo networkInfo : connectivityManager.getAllNetworkInfo())
			{
				if (networkInfo.getType() == ConnectivityManager.TYPE_WIFI && networkInfo.getState().equals(NetworkInfo.State.CONNECTED))
				{
					return true;
				}
			}
		}
		else
		{
			for (Network mNetwork : connectivityManager.getAllNetworks())
			{
				NetworkInfo networkInfo = connectivityManager.getNetworkInfo(mNetwork);
				if (networkInfo.getType() == ConnectivityManager.TYPE_WIFI && networkInfo.getState().equals(NetworkInfo.State.CONNECTED))
				{
					return true;
				}
			}
		}
		return false;
	}


}
