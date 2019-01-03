/*
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.nfc.NfcAdapter;
import android.nfc.tech.IsoDep;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import org.qtproject.qt5.android.bindings.QtActivity;

public class MainActivity extends QtActivity
{
	private static final String LOG_TAG = AusweisApp2Service.LOG_TAG;

	private static Intent cIntent;

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
			if (mAdapter != null)
			{
				mAdapter.enableForegroundDispatch(mActivity, mPendingIntent, mFilters, mTechLists);
			}
		}


		void disable()
		{
			if (mAdapter != null)
			{
				mAdapter.disableForegroundDispatch(mActivity);
			}
		}


	}


	// required by IntentActivationHandler -> MainActivityAccessor
	public static String getStoredIntent()
	{
		if (cIntent == null)
		{
			Log.e(LOG_TAG, "No stored intent available, returning null");
			return null;
		}

		return cIntent.getDataString();
	}


	public static boolean isStartedByAuth()
	{
		if (cIntent == null)
		{
			return false;
		}
		return cIntent.getAction().equals("android.intent.action.VIEW");
	}


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		Log.d(LOG_TAG, "onCreate (initial invocation of application): " + getIntent());
		super.onCreate(savedInstanceState);
		cIntent = getIntent();

		// register the broadcast receiver after loading the C++ library in super.onCreate()
		AndroidBluetoothReceiver.register(this);

		mNfcForegroundDispatcher = new NfcForegroundDispatcher(this);

		setRequestedOrientation(isTablet() ? ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE : ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
	}


	@Override
	protected void onNewIntent(Intent newIntent)
	{
		Log.d(LOG_TAG, "onNewIntent (subsequent invocation of application): " + newIntent);
		super.onNewIntent(newIntent);

		triggerActivation(newIntent.getDataString());
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
		final int screenLayout = context.getResources().getConfiguration().screenLayout;
		final boolean xlarge = (screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_XLARGE;
		final boolean large = (screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_LARGE;

		return xlarge || large;
	}


	private native void triggerActivation(String lastIntent);
}
