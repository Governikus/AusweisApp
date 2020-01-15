/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.io.File;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class UpdateReceiver extends BroadcastReceiver
{
	private static final String LOG_TAG = AusweisApp2Service.LOG_TAG;


	@Override
	public void onReceive(Context context, Intent intent)
	{
		if (!intent.getAction().equals(Intent.ACTION_MY_PACKAGE_REPLACED))
		{
			return;
		}

		Log.d(LOG_TAG, "New app version received. Clear app cache.");

		try
		{
			deleteDir(context.getCacheDir());
		}
		catch (Exception e)
		{
			Log.e(LOG_TAG, e.toString());
		}
	}


	private static boolean deleteDir(File file)
	{
		if (file == null)
		{
			return false;
		}

		if (file.isFile())
		{
			return file.delete();
		}

		if (file.isDirectory())
		{
			for (File child : file.listFiles())
			{
				if (!deleteDir(child))
				{
					Log.d(LOG_TAG, "Failed to delete " + child);
					return false;
				}
			}
			return file.delete();
		}

		return false;
	}


}
