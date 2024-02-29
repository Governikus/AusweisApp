/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.util.Arrays;
import java.util.List;
import java.util.Locale;

import android.accessibilityservice.AccessibilityServiceInfo;
import android.annotation.SuppressLint;
import android.app.PendingIntent;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.graphics.Color;
import android.net.Uri;
import android.nfc.NfcAdapter;
import android.nfc.tech.IsoDep;
import android.os.Build;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.view.View;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.view.accessibility.AccessibilityManager;

import org.qtproject.qt.android.QtActivityDelegate;
import org.qtproject.qt.android.QtNative;
import org.qtproject.qt.android.bindings.QtActivity;

import androidx.core.view.ViewCompat;


public class MainActivity extends QtActivity
{
	private static Intent cIntent;

	private final MarginLayoutParams windowInsets = new MarginLayoutParams(0, 0);

	private NfcForegroundDispatcher mNfcForegroundDispatcher;
	private NfcReaderMode mNfcReaderMode;
	private boolean mIsResumed;

	// Native methods provided by UIPlugInQml
	public static native void notifySafeAreaMarginsChanged();
	public static native void notifyConfigurationChanged();

	private class NfcForegroundDispatcher
	{
		private final IntentFilter[] mFilters;
		private final String[][] mTechLists;
		private final PendingIntent mPendingIntent;

		@SuppressLint("UnspecifiedImmutableFlag")
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

			mPendingIntent = PendingIntent.getActivity(MainActivity.this, 0, new Intent(), PendingIntent.FLAG_IMMUTABLE);
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


		boolean isEnabled()
		{
			return mEnabled;
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
			v.vibrate(VibrationEffect.createOneShot(250, VibrationEffect.DEFAULT_AMPLITUDE));
		}


	}


	// required by IntentActivationHandler -> MainActivityAccessor
	public static String fetchStoredReferrer()
	{
		if (cIntent == null || cIntent.getExtras() == null)
		{
			return "";
		}

		return cIntent.getExtras().getString(EXTRA_SOURCE_INFO);
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
		if (cIntent == null || cIntent.getData() == null)
		{
			return false;
		}

		return cIntent.getAction().equals(Intent.ACTION_VIEW)
			   && cIntent.getData().getQuery().toLowerCase(Locale.GERMAN).contains("tctokenurl");
	}


	private void convertChromeOsIntent(Intent pIntent)
	{
		if (pIntent != null && "org.chromium.arc.intent.action.VIEW".equals(pIntent.getAction()))
		{
			LogHandler.getLogger().info("Convert Intent action " + pIntent.getAction() + " to " + Intent.ACTION_VIEW);
			pIntent.setAction(Intent.ACTION_VIEW);
		}
	}


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		setTheme(R.style.AppTheme);

		convertChromeOsIntent(getIntent());
		LogHandler.getLogger().info("onCreate: " + getIntent());
		super.onCreate(savedInstanceState);

		cIntent = getIntent();

		mNfcForegroundDispatcher = new NfcForegroundDispatcher();
		mNfcReaderMode = new NfcReaderMode();

		// Handle systemWindowInsets
		Window window = getWindow();
		View rootView = window.getDecorView().findViewById(android.R.id.content);
		ViewCompat.setOnApplyWindowInsetsListener(rootView, (v, insets) ->
				{
					windowInsets.topMargin = insets.getSystemWindowInsetTop();
					windowInsets.leftMargin = insets.getSystemWindowInsetLeft();
					windowInsets.rightMargin = insets.getSystemWindowInsetRight();
					windowInsets.bottomMargin = insets.getSystemWindowInsetBottom();

					notifySafeAreaMarginsChanged();

					return insets;
				});

		// Set Qt flags first so we can overwrite with our own values.
		setSystemUiVisibility(QtActivityDelegate.SYSTEM_UI_VISIBILITY_TRANSLUCENT);
		window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION | WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);

		// Make statusbar and navigation bar transparent
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
	}


	@Override
	protected void onNewIntent(Intent newIntent)
	{
		convertChromeOsIntent(newIntent);
		cIntent = newIntent;
		setIntent(newIntent);
		LogHandler.getLogger().info("onNewIntent: " + newIntent);
		super.onNewIntent(newIntent);
	}


	private native void setReaderModeNative(boolean pEnabled);


	@Override
	public void onResume()
	{
		super.onResume();
		mIsResumed = true;

		mNfcForegroundDispatcher.enable();
		setReaderModeNative(true);
	}


	@Override
	public void onPause()
	{
		setReaderModeNative(false);
		mNfcForegroundDispatcher.disable();

		mIsResumed = false;
		super.onPause();
	}


	@Override
	protected void onDestroy()
	{
		LogHandler.getLogger().info("onDestroy");
		super.onDestroy();
	}


	// used by NfcReaderManagerPlugIn
	public void setReaderMode(boolean pEnabled)
	{
		if (pEnabled)
		{
			mNfcReaderMode.enable();
		}
		else
		{
			mNfcReaderMode.disable();
		}
	}


	// used by NfcReaderManagerPlugIn
	public void resetNfcReaderMode()
	{
		if (mIsResumed && mNfcReaderMode.isEnabled())
		{
			mNfcReaderMode.disable();
			mNfcReaderMode.enable();
		}
	}


	public void keepScreenOn(boolean pActivate)
	{
		LogHandler.getLogger().info("Keep screen on: " + pActivate);
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
		Intent intent = new Intent(Intent.ACTION_VIEW)
				.setData(Uri.parse(pUrl))
				.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
				.setPackage(pReferrer);
		try
		{
			startActivity(intent);
			LogHandler.getLogger().info("Started Intent in browser with id " + pReferrer);
		}
		catch (ActivityNotFoundException e)
		{
			LogHandler.getLogger().warning("Couldn't open URL in browser with id " + pReferrer);
			return false;
		}
		return true;
	}


	@Override
	public void onConfigurationChanged(Configuration newConfig)
	{
		super.onConfigurationChanged(newConfig);
		notifyConfigurationChanged();
	}


}
