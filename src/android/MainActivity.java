/*
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.util.Arrays;
import java.util.List;
import java.util.Locale;

import android.accessibilityservice.AccessibilityServiceInfo;
import android.app.PendingIntent;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.net.Uri;
import android.nfc.NfcAdapter;
import android.nfc.tech.IsoDep;
import android.os.Build;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.util.Log;
import android.view.accessibility.AccessibilityManager;
import android.view.View;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.Window;
import android.view.WindowManager;

import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.QtNative;

import androidx.core.view.ViewCompat;


public class MainActivity extends QtActivity
{
	private static final String LOG_TAG = AusweisApp2Service.LOG_TAG;

	private static Intent cIntent;
	private static Uri cReferrer;
	private static boolean cStartedByAuth;

	private final MarginLayoutParams windowInsets = new MarginLayoutParams(0, 0);

	private NfcForegroundDispatcher mNfcForegroundDispatcher;
	private NfcReaderMode mNfcReaderMode;
	private boolean mReaderModeRequested;
	private boolean mIsResumed;

	// Native method provided by UIPlugInQml
	public static native void notifySafeAreaMarginsChanged();

	private class NfcForegroundDispatcher
	{
		private final IntentFilter[] mFilters;
		private final String[][] mTechLists;
		private final PendingIntent mPendingIntent;

		NfcForegroundDispatcher()
		{
			mFilters = new IntentFilter[] {
				new IntentFilter(NfcAdapter.ACTION_TECH_DISCOVERED)
			};
			mTechLists = new String[][] {
				new String[] {
					IsoDep.class.getName()
				}
			};
			mPendingIntent = PendingIntent.getActivity(MainActivity.this, 0, new Intent(), 0);
		}


		void enable()
		{
			NfcAdapter adapter = NfcAdapter.getDefaultAdapter(MainActivity.this);
			if (adapter != null)
			{
				adapter.enableForegroundDispatch(MainActivity.this, mPendingIntent, mFilters, mTechLists);
			}
		}


		void disable()
		{
			NfcAdapter adapter = NfcAdapter.getDefaultAdapter(MainActivity.this);
			if (adapter != null)
			{
				adapter.disableForegroundDispatch(MainActivity.this);
			}
		}


	}


	private class NfcReaderMode
	{
		private final int mFlags;
		private final NfcAdapter.ReaderCallback mCallback;
		private boolean mEnabled;

		NfcReaderMode()
		{
			mFlags = NfcAdapter.FLAG_READER_NFC_A
					| NfcAdapter.FLAG_READER_NFC_B
					| NfcAdapter.FLAG_READER_SKIP_NDEF_CHECK
					| NfcAdapter.FLAG_READER_NO_PLATFORM_SOUNDS;
			mCallback = pTag ->
			{
				if (Arrays.asList(pTag.getTechList()).contains(IsoDep.class.getName()))
				{
					vibrate();

					Intent nfcIntent = new Intent();
					nfcIntent.putExtra(NfcAdapter.EXTRA_TAG, pTag);
					QtNative.onNewIntent(nfcIntent);
				}
			};
		}


		void enable()
		{
			NfcAdapter adapter = NfcAdapter.getDefaultAdapter(MainActivity.this);
			if (adapter != null && !mEnabled)
			{
				mEnabled = true;
				adapter.enableReaderMode(MainActivity.this, mCallback, mFlags, null);
			}

		}


		void disable()
		{
			NfcAdapter adapter = NfcAdapter.getDefaultAdapter(MainActivity.this);
			if (adapter != null && mEnabled)
			{
				adapter.disableReaderMode(MainActivity.this);
			}
			mEnabled = false;
		}


		void vibrate()
		{
			Vibrator v = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);

			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) // API 26, Android 8.0
			{
				v.vibrate(VibrationEffect.createOneShot(250, VibrationEffect.DEFAULT_AMPLITUDE));
				return;
			}

			v.vibrate(250);
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
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP_MR1) // API 22, Android 5.1
			{
				Log.e(LOG_TAG, "No stored referrer available, returning null");
			}
			return null;
		}

		String ref = cReferrer.toString();
		cReferrer = null;
		return ref;
	}


	public MainActivity()
	{
		QT_ANDROID_THEMES = new String[] {
			"AppTheme"
		};
		QT_ANDROID_DEFAULT_THEME = "AppTheme";
	}


	public static boolean isStartedByAuth()
	{
		return cStartedByAuth;
	}


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		setTheme(R.style.AppTheme);

		Log.d(LOG_TAG, "onCreate: " + getIntent());
		super.onCreate(savedInstanceState);

		onNewIntent(getIntent());

		mNfcForegroundDispatcher = new NfcForegroundDispatcher();
		mNfcReaderMode = new NfcReaderMode();

		// Set statusBar/navigationBar color and handle systemWindowInsets
		Window window = getWindow();
		View rootView = window.getDecorView().findViewById(android.R.id.content);
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) // API 29, Android 10
		{
			rootView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR);
			window.setNavigationBarColor(Color.TRANSPARENT);
		}
		else
		{
			rootView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
		}
		window.setStatusBarColor(Color.TRANSPARENT);
		ViewCompat.setOnApplyWindowInsetsListener(rootView, (v, insets) ->
				{
					windowInsets.topMargin = insets.getSystemWindowInsetTop();
					windowInsets.leftMargin = insets.getSystemWindowInsetLeft();
					windowInsets.rightMargin = insets.getSystemWindowInsetRight();
					windowInsets.bottomMargin = insets.getSystemWindowInsetBottom();

					notifySafeAreaMarginsChanged();

					return insets;
				});
	}


	@Override
	protected void onNewIntent(Intent newIntent)
	{
		Log.d(LOG_TAG, "onNewIntent: " + newIntent);
		super.onNewIntent(newIntent);

		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP_MR1) // API 22, Android 5.1
		{
			cReferrer = getReferrer();
		}

		cIntent = newIntent;
		Uri data = cIntent.getData();
		if (data == null)
		{
			cStartedByAuth = false;
		}
		else
		{
			cStartedByAuth = cIntent.getAction().equals(Intent.ACTION_VIEW)
					&& data.getQuery().toLowerCase(Locale.GERMAN).contains("tctokenurl");
		}
	}


	@Override
	public void onResume()
	{
		super.onResume();
		mIsResumed = true;

		mNfcForegroundDispatcher.enable();
		if (mReaderModeRequested)
		{
			mNfcReaderMode.enable();
		}
	}


	@Override
	public void onPause()
	{
		mNfcReaderMode.disable();
		mNfcForegroundDispatcher.disable();
		mIsResumed = false;
		super.onPause();
	}


	@Override
	protected void onDestroy()
	{
		Log.d(LOG_TAG, "onDestroy");
		super.onDestroy();
	}


	// used by NfcReader
	public void enableNfcReaderMode()
	{
		mReaderModeRequested = true;
		if (mIsResumed)
		{
			mNfcReaderMode.enable();
		}
	}


	// used by NfcReader
	public void disableNfcReaderMode()
	{
		mReaderModeRequested = false;
		mNfcReaderMode.disable();
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


	public MarginLayoutParams getWindowInsets()
	{
		return windowInsets;
	}


	public boolean openUrl(String pUrl, String pReferrer)
	{
		String packageName = pReferrer == null ? null : pReferrer.replace("android-app://", "");
		Intent intent = new Intent(Intent.ACTION_VIEW)
				.setData(Uri.parse(pUrl))
				.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
				.setPackage(packageName);
		try
		{
			startActivity(intent);
			Log.d(LOG_TAG, "Started Intent in browser with id " + packageName);
		}
		catch (ActivityNotFoundException e)
		{
			Log.e(LOG_TAG, "Couldn't open URL in browser with id " + packageName);
			return false;
		}
		return true;
	}


}
