/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.io.File;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class UpdateReceiver extends BroadcastReceiver
{
	@Override
	public void onReceive(Context context, Intent intent)
	{
		if (!intent.getAction().equals(Intent.ACTION_MY_PACKAGE_REPLACED))
		{
			return;
		}

		LogHandler.getLogger().info("New app version received. Clear app cache.");

		try
		{
			deleteDir(context.getCacheDir());
			deleteDir(context.getExternalCacheDir());
		}
		catch (Exception e)
		{
			LogHandler.getLogger().severe(e.toString());
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
					LogHandler.getLogger().info("Failed to delete " + child);
					return false;
				}
			}
			return file.delete();
		}

		return false;
	}


}
