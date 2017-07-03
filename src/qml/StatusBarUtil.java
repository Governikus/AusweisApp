package com.governikus.ausweisapp2;

import android.app.Activity;
import android.os.Build.VERSION;
import android.os.Build.VERSION_CODES;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager.LayoutParams;
import com.governikus.ausweisapp2.R;

public class StatusBarUtil
{
	public static void setStatusBarColor(final Activity activity, final String color)
	{
		activity.runOnUiThread(new Runnable(){
			@Override
			public void run(){
				if (VERSION.SDK_INT >= VERSION_CODES.KITKAT)
				{
					Window window = activity.getWindow();
					window.clearFlags(LayoutParams.FLAG_TRANSLUCENT_STATUS);
					window.addFlags(LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
					int colorValue;
					if (color.equals("CITIZEN"))
					{
						colorValue = R.color.statusbar_citizen;
					}
					else if (color.equals("INSURANCE"))
					{
						colorValue = R.color.statusbar_insurance;
					}
					else if (color.equals("FINANCE"))
					{
						colorValue = R.color.statusbar_finance;
					}
					else if (color.equals("OTHER"))
					{
						colorValue = R.color.statusbar_other;
					}
					else
					{
						colorValue = R.color.statusbar_default;
					}
					window.setStatusBarColor(activity.getResources().getColor(colorValue));
				}
			}
		});
	}


}
