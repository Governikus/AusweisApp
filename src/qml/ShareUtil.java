package com.governikus.ausweisapp2;

import android.app.Activity;
import android.content.*;
import android.Manifest;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;

import java.io.*;
import java.nio.channels.FileChannel;
import java.text.*;
import java.util.*;


public class ShareUtil
{
	private static final String TAG = "AusweisApp2";


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
			File logFile = new File(logFilePath);

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
			Log.e(TAG, "Error sharing log file", e);
		}
	}


}
