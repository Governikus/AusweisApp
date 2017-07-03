package com.governikus.ausweisapp2;

import android.content.Context;
import android.content.Intent;


public class ShareUtil
{
	public static void shareText(Context ctx, String text, String chooserTitle)
	{
		Intent shareData = new Intent();
		shareData.setType("text/plain");
		shareData.setAction(Intent.ACTION_SEND);
		shareData.putExtra(Intent.EXTRA_TEXT, text);
		ctx.startActivity(Intent.createChooser(shareData, chooserTitle));
	}


}
