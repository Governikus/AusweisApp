/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.LabeledIntent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.net.Uri;

import androidx.core.content.FileProvider;


public final class ShareUtil
{
	private ShareUtil()
	{
	}


	public static void mailLog(Activity activity, final String email, final String subject, final String msg, final String logFilePath, final String chooserTitle)
	{
		try
		{
			Intent emailIntent = new Intent(Intent.ACTION_SENDTO);
			emailIntent.setData(Uri.parse("mailto:"));

			PackageManager packageManager = activity.getPackageManager();
			List<ResolveInfo> emailApps = packageManager.queryIntentActivities(emailIntent, PackageManager.MATCH_DEFAULT_ONLY);
			List<LabeledIntent> launcherIntents = new ArrayList<>();
			for (ResolveInfo resolveInfo : emailApps)
			{
				String packageName = resolveInfo.activityInfo.packageName;

				Intent shareData = new Intent(Intent.ACTION_SEND);
				shareData.setComponent(new ComponentName(packageName, resolveInfo.activityInfo.name));
				shareData.setType("text/plain");
				shareData.putExtra(Intent.EXTRA_EMAIL, new String[] {email});
				shareData.putExtra(Intent.EXTRA_SUBJECT, subject);
				shareData.putExtra(Intent.EXTRA_TEXT, msg);
				shareData.putExtra(Intent.EXTRA_STREAM, FileProvider.getUriForFile(activity, activity.getPackageName(), new File(logFilePath)));
				shareData.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);

				launcherIntents.add(new LabeledIntent(shareData, packageName, resolveInfo.loadLabel(packageManager), resolveInfo.icon));
			}

			Intent chooserIntent = Intent.createChooser(new Intent(), chooserTitle);
			chooserIntent.putExtra(Intent.EXTRA_INITIAL_INTENTS, launcherIntents.toArray(new LabeledIntent[launcherIntents.size()]));
			activity.startActivity(chooserIntent);
		}
		catch (Exception e)
		{
			LogHandler.getLogger().log(Level.SEVERE, "Error sharing logfile", e);
		}
	}


	public static void shareLog(Activity activity, final String logFilePath, final String chooserTitle)
	{
		try
		{
			Intent shareData = new Intent(Intent.ACTION_SEND);
			shareData.setType("text/plain");
			shareData.putExtra(Intent.EXTRA_STREAM, FileProvider.getUriForFile(activity, activity.getPackageName(), new File(logFilePath)));
			shareData.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
			activity.startActivity(Intent.createChooser(shareData, chooserTitle));
		}
		catch (Exception e)
		{
			LogHandler.getLogger().log(Level.SEVERE, "Error sharing logfile", e);
		}
	}


}
