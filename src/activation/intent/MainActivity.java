/*
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import android.accessibilityservice.AccessibilityServiceInfo;
import android.app.Activity;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.net.Uri;
import android.nfc.NfcAdapter;
import android.nfc.tech.IsoDep;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.accessibility.AccessibilityManager;
import android.view.Window;
import android.view.WindowManager;
import java.util.List;

import org.qtproject.qt5.android.bindings.QtActivity;

public class MainActivity extends QtActivity
{
	private static final String LOG_TAG = AusweisApp2Service.LOG_TAG;

	private static Intent cIntent;
	private static Uri cReferrer;
	private static boolean cStartedByAuth;

	private NfcForegroundDispatcher mNfcForegroundDispatcher;
	private static class NfcForegroundDispatcher
	{
		private final Activity mActivity;
		private final NfcAdapter mAdapter;
		private final PendingIntent mPendingIntent;
		private final IntentFilter[] mFilters;
		private final String[][] mTechLists;

		NfcForegroundDispatcher(Activity pActivity)
		{
			mActivity = pActivity;
			mAdapter = NfcAdapter.getDefaultAdapter(mActivity);
			mPendingIntent = PendingIntent.getActivity(
					mActivity, 0, new Intent(mActivity, mActivity.getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP), 0);

			mFilters = new IntentFilter[] {
				new IntentFilter(NfcAdapter.ACTION_TECH_DISCOVERED)
			};
			mTechLists = new String[][] {
				new String[] {
					IsoDep.class.getName()
				}
			};
		}


		void enable()
		{
			if (mAdapter != null && mActivity.getPackageManager().hasSystemFeature(PackageManager.FEATURE_NFC))
			{
				mAdapter.enableForegroundDispatch(mActivity, mPendingIntent, mFilters, mTechLists);
			}
		}


		void disable()
		{
			if (mAdapter != null && mActivity.getPackageManager().hasSystemFeature(PackageManager.FEATURE_NFC))
			{
				mAdapter.disableForegroundDispatch(mActivity);
			}
		}


	}


	// required by IntentActivationHandler -> MainActivityAccessor
	public static String fetchStoredIntent()
	{
		if (cIntent == null)
		{
			Log.e(LOG_TAG, "No stored intent available, returning null");
			return null;
		}

		String url = cIntent.getDataString();
		cIntent = null;
		return url;
	}


	// required by IntentActivationHandler -> MainActivityAccessor
	public static String fetchStoredReferrer()
	{
		if (cReferrer == null)
		{
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP_MR1) // API 22
			{
				Log.e(LOG_TAG, "No stored referrer available, returning null");
			}
			return null;
		}

		String ref = cReferrer.toString();
		cReferrer = null;
		return ref;
	}


	public static boolean isStartedByAuth()
	{
		return cStartedByAuth;
	}


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		Log.d(LOG_TAG, "onCreate: " + getIntent());
		super.onCreate(savedInstanceState);

		// Make statusbar transparent
		Window window = getWindow();
		window.setFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS, WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);
		window.setFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS, WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
		window.setStatusBarColor(Color.TRANSPARENT);

		onNewIntent(getIntent());
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP_MR1) // API 22
		{
			cReferrer = getReferrer();
		}

		// register the broadcast receiver after loading the C++ library in super.onCreate()
		AndroidBluetoothReceiver.register(this);

		mNfcForegroundDispatcher = new NfcForegroundDispatcher(this);

		setRequestedOrientation(isTablet() ? ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE : ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
	}


	@Override
	protected void onNewIntent(Intent newIntent)
	{
		Log.d(LOG_TAG, "onNewIntent: " + newIntent);
		super.onNewIntent(newIntent);
		cIntent = newIntent;
		cStartedByAuth = "android.intent.action.VIEW".equals(cIntent.getAction());
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP_MR1) // API 22
		{
			cReferrer = getReferrer();
		}
	}


	@Override
	public void onResume()
	{
		mNfcForegroundDispatcher.enable();
		super.onResume();
	}


	@Override
	public void onPause()
	{
		mNfcForegroundDispatcher.disable();
		super.onPause();
	}


	@Override
	protected void onDestroy()
	{
		Log.d(LOG_TAG, "onDestroy");

		// unregister the broadcast receiver before unloading the C++ library in super.onDestroy()
		AndroidBluetoothReceiver.unregister(this);

		super.onDestroy();
	}


	public void keepScreenOn(boolean pActivate)
	{
		Log.d(LOG_TAG, "Keep screen on: " + pActivate);
		if (pActivate)
		{
			getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		}
		else
		{
			getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		}
	}


	// required by UIPlugInQml::getPlatformSelectors()
	public boolean isTablet()
	{
		final Context context = getBaseContext();
		// https://developer.android.com/training/multiscreen/screensizes.html#TaskUseSWQuali
		return context.getResources().getConfiguration().smallestScreenWidthDp >= 600;
	}


	public boolean isScreenReaderRunning()
	{
		AccessibilityManager accessibilityManager = (AccessibilityManager) getSystemService(ACCESSIBILITY_SERVICE);
		List<AccessibilityServiceInfo> services = accessibilityManager.getEnabledAccessibilityServiceList(AccessibilityServiceInfo.FEEDBACK_SPOKEN);
		return !services.isEmpty();
	}


	public int getStatusBarHeight()
	{
		return getDimension("status_bar_height");
	}


	public int getNavigationBarHeight()
	{
		return getDimension("navigation_bar_height");
	}


	private int getDimension(String dimensionName)
	{
		int result = 0;
		int resourceId = getResources().getIdentifier(dimensionName, "dimen", "android");
		if (resourceId > 0)
		{
			result = getResources().getDimensionPixelSize(resourceId);
		}
		return result;
	}


}
