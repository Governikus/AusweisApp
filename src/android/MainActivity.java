/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import java.util.Arrays;
import java.util.List;
import java.util.Locale;
import java.util.function.Consumer;
import java.util.logging.Level;

import android.accessibilityservice.AccessibilityServiceInfo;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.net.Uri;
import android.nfc.NfcAdapter;
import android.nfc.tech.IsoDep;
import android.os.Build;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.os.VibratorManager;
import android.view.View;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.view.accessibility.AccessibilityManager;

import org.qtproject.qt.android.QtNative;
import org.qtproject.qt.android.bindings.QtActivity;

import androidx.core.view.WindowCompat;


public class MainActivity extends QtActivity
{
	private static Intent cIntent;


	private NfcReaderMode mNfcReaderMode;
	private boolean mIsResumed;
	private boolean mScreenRecordingRunning;

	private boolean mIsScreenReaderRunning;

	private final Consumer<Integer> mScreenRecordingCallback = state -> {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.VANILLA_ICE_CREAM) // API 35, Android 15
		{
			mScreenRecordingRunning = state == WindowManager.SCREEN_RECORDING_STATE_VISIBLE;
			notifyScreenRecordingChanged();
		}
	};

	// Native methods provided by UiPluginQml
	public static native void notifyConfigurationChanged();
	// Native methods provided by ApplicationModel
	public static native void notifyScreenReaderRunningChanged();
	public static native void notifyScreenRecordingChanged();

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


		@SuppressWarnings("deprecation")
		Vibrator vibrator()
		{
			return (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
		}


		void vibrate()
		{
			Vibrator v;
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) // API 31, Android 12
			{
				VibratorManager vibratorManager = (VibratorManager) getSystemService(Context.VIBRATOR_MANAGER_SERVICE);
				v = vibratorManager.getDefaultVibrator();
			}
			else
			{
				v = vibrator();
			}
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
			LogHandler.getLogger().log(Level.INFO, () -> "Convert Intent action " + pIntent.getAction() + " to " + Intent.ACTION_VIEW);
			pIntent.setAction(Intent.ACTION_VIEW);
		}
	}


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		setTheme(R.style.AppTheme);

		convertChromeOsIntent(getIntent());
		LogHandler.getLogger().log(Level.INFO, () -> "onCreate: " + getIntent());
		super.onCreate(savedInstanceState);
		WindowCompat.enableEdgeToEdge(getWindow());

		cIntent = getIntent();

		mNfcReaderMode = new NfcReaderMode();

		AccessibilityManager accessibilityManager = (AccessibilityManager) getSystemService(ACCESSIBILITY_SERVICE);
		refreshAccessibilityState(accessibilityManager);
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) // API 33, Android 13
		{
			accessibilityManager.addAccessibilityServicesStateChangeListener(new AccessibilityManager.AccessibilityServicesStateChangeListener()
					{
						@Override
						public void onAccessibilityServicesStateChanged(AccessibilityManager accessibilityManager)
						{
							MainActivity.this.refreshAccessibilityState(accessibilityManager);
						}
					});
		}
		else
		{
			accessibilityManager.addAccessibilityStateChangeListener(new AccessibilityManager.AccessibilityStateChangeListener()
					{
						@Override
						public void onAccessibilityStateChanged(boolean enabled)
						{
							AccessibilityManager accessibilityManager = (AccessibilityManager) getSystemService(ACCESSIBILITY_SERVICE);
							MainActivity.this.refreshAccessibilityState(accessibilityManager);
						}
					});
		}

	}


	@Override
	protected void onNewIntent(Intent newIntent)
	{
		convertChromeOsIntent(newIntent);
		cIntent = newIntent;
		setIntent(newIntent);
		LogHandler.getLogger().log(Level.INFO, () -> "onNewIntent: " + newIntent);
		super.onNewIntent(newIntent);
	}


	private native void setReaderModeNative(boolean pEnabled);

	@Override
	protected void onStart()
	{
		super.onStart();
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.VANILLA_ICE_CREAM) // API 35, Android 15
		{
			WindowManager windowManager = (WindowManager) getSystemService(WINDOW_SERVICE);
			int initialState = windowManager.addScreenRecordingCallback(getMainExecutor(), mScreenRecordingCallback);
			mScreenRecordingCallback.accept(initialState);
		}
	}


	@Override
	protected void onStop()
	{
		super.onStop();
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.VANILLA_ICE_CREAM) // API 35, Android 15
		{
			WindowManager windowManager = (WindowManager) getSystemService(WINDOW_SERVICE);
			windowManager.removeScreenRecordingCallback(mScreenRecordingCallback);
		}
	}


	@Override
	public void onResume()
	{
		super.onResume();
		mIsResumed = true;

		setReaderModeNative(true);
	}


	@Override
	public void onPause()
	{
		setReaderModeNative(false);

		mIsResumed = false;
		super.onPause();
	}


	@Override
	protected void onDestroy()
	{
		LogHandler.getLogger().log(Level.INFO, () -> "onDestroy");
		super.onDestroy();
	}


	// used by NfcReaderManagerPlugin
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


	// used by NfcReaderManagerPlugin
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
		LogHandler.getLogger().log(Level.INFO, () -> "Keep screen on: " + pActivate);
		if (pActivate)
		{
			getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		}
		else
		{
			getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		}
	}


	public void preventScreenshot(boolean pPrevent)
	{
		LogHandler.getLogger().log(Level.INFO, () -> "Prevent screenshot: " + pPrevent);
		if (pPrevent)
		{
			getWindow().addFlags(WindowManager.LayoutParams.FLAG_SECURE);
		}
		else
		{
			getWindow().clearFlags(WindowManager.LayoutParams.FLAG_SECURE);
		}
	}


	public boolean isScreenReaderRunning()
	{
		return mIsScreenReaderRunning;
	}


	public boolean isScreenRecordingRunning()
	{
		return mScreenRecordingRunning;
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
			LogHandler.getLogger().log(Level.INFO, () -> "Started Intent in browser with id " + pReferrer);
		}
		catch (ActivityNotFoundException e)
		{
			LogHandler.getLogger().log(Level.WARNING, () -> "Couldn't open URL in browser with id " + pReferrer);
			return false;
		}
		return true;
	}


	public boolean isChromeOS()
	{
		PackageManager packageManager = getPackageManager();
		return packageManager.hasSystemFeature("org.chromium.arc") || packageManager.hasSystemFeature("org.chromium.arc.device_management");
	}


	@SuppressWarnings("deprecation")
	public void setAppearanceLightStatusBarsDeprecated(boolean enable)
	{
		View rootView = getWindow().getDecorView().findViewById(android.R.id.content);
		int currentVisibility = rootView.getSystemUiVisibility();
		if (enable)
		{
			rootView.setSystemUiVisibility(currentVisibility | View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
		}
		else
		{
			rootView.setSystemUiVisibility(currentVisibility & ~View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
		}
	}


	public void setAppearanceLightStatusBars(boolean enable)
	{
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.R) // API 30, Android 11
		{
			setAppearanceLightStatusBarsDeprecated(enable);
			return;
		}

		WindowInsetsController controller = getWindow().getInsetsController();
		controller.setSystemBarsAppearance(enable ? WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS : 0, WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS);
		controller.setSystemBarsAppearance(enable ? WindowInsetsController.APPEARANCE_LIGHT_NAVIGATION_BARS : 0, WindowInsetsController.APPEARANCE_LIGHT_NAVIGATION_BARS);
	}


	@Override
	public void onConfigurationChanged(Configuration newConfig)
	{
		super.onConfigurationChanged(newConfig);
		notifyConfigurationChanged();
	}


	public void refreshAccessibilityState(AccessibilityManager accessibilityManager)
	{
		List<AccessibilityServiceInfo> services = accessibilityManager.getEnabledAccessibilityServiceList(AccessibilityServiceInfo.FEEDBACK_SPOKEN);
		boolean isRunning = !services.isEmpty();

		if (mIsScreenReaderRunning != isRunning)
		{
			mIsScreenReaderRunning = isRunning;
			notifyScreenReaderRunningChanged();
		}
	}


	public void resetStoredIntent()
	{
		cIntent = null;
	}


}
