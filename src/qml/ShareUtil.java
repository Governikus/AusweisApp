/*
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.io.File;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;


public final class ShareUtil
{
	private static final String LOG_TAG = AusweisApp2Service.LOG_TAG;

	private ShareUtil()
	{
	}


	public static void shareText(Context ctx, final String text, final String chooserTitle)
	{
		Intent shareData = new Intent();
		shareData.setType("text/plain");
		shareData.setAction(Intent.ACTION_SEND);
		shareData.putExtra(Intent.EXTRA_TEXT, text);
		ctx.startActivity(Intent.createChooser(shareData, chooserTitle));
	}


	public static void shareLog(Activity activity, final String email, final String subject, final String msg, final String logFilePath, final String chooserTitle)
	{
		try
		{
			Intent shareData = new Intent();
			shareData.setType("message/rfc822");
			shareData.setAction(Intent.ACTION_SEND);
			shareData.putExtra(Intent.EXTRA_EMAIL, new String[] {email});
			shareData.putExtra(Intent.EXTRA_SUBJECT, subject);
			shareData.putExtra(Intent.EXTRA_TEXT, msg);
			shareData.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(new File(logFilePath)));
			activity.startActivity(Intent.createChooser(shareData, chooserTitle));
		}
		catch (Exception e)
		{
			Log.e(LOG_TAG, "Error sharing log file", e);
		}
	}


}
