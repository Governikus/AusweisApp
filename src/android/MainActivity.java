/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
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
import android.content.IntentSender;
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

import androidx.core.view.WindowCompat;

import org.qtproject.qt.android.QtNative;
import org.qtproject.qt.android.bindings.QtActivity;

import com.google.android.gms.tasks.Task;
import com.google.android.play.core.appupdate.AppUpdateInfo;
import com.google.android.play.core.appupdate.AppUpdateManager;
import com.google.android.play.core.appupdate.AppUpdateManagerFactory;
import com.google.android.play.core.appupdate.AppUpdateOptions;
import com.google.android.play.core.install.InstallStateUpdatedListener;
import com.google.android.play.core.install.model.AppUpdateType;
import com.google.android.play.core.install.model.InstallErrorCode;
import com.google.android.play.core.install.model.InstallStatus;
import com.google.android.play.core.install.model.UpdateAvailability;
import com.google.android.play.core.review.ReviewException;
import com.google.android.play.core.review.ReviewInfo;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;
import com.google.android.play.core.review.model.ReviewErrorCode;


public class MainActivity extends QtActivity
{
	private static Intent cIntent;

	private AppUpdateManager mAppUpdateManager;
	private AppUpdateInfo mAppUpdateInfo;
	private InstallStateUpdatedListener mInstallListener;
	private static final int IN_APP_FLEXIBLE_UPDATE_REQUEST_CODE = 24_727;
	private static final int IN_APP_IMMEDIATE_UPDATE_REQUEST_CODE = 24_728;
	private boolean mUpdateCanceledInSession;

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
	// Native methods provided by AppUpdateDataModel
	public static native void notifyUpdateFound(int pVersionCode, int pStalenessDays, int pPriority);
	public static native void notifyUpdateReadyForInstallation();
	public static native void notifyImmediateUpdateCanceled();

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
		mAppUpdateManager = AppUpdateManagerFactory.create(this);

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
		handleAppUpdateInfo();
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


	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		super.onActivityResult(requestCode, resultCode, data);

		if (requestCode == IN_APP_FLEXIBLE_UPDATE_REQUEST_CODE || requestCode == IN_APP_IMMEDIATE_UPDATE_REQUEST_CODE)
		{
			if (resultCode == RESULT_OK)
			{
				LogHandler.getLogger().log(Level.INFO, () -> "Update accepted/completed");
			}
			else if (resultCode == RESULT_CANCELED)
			{
				if (requestCode == IN_APP_IMMEDIATE_UPDATE_REQUEST_CODE)
				{
					mUpdateCanceledInSession = true;
					notifyImmediateUpdateCanceled();
				}
				LogHandler.getLogger().log(Level.INFO, () -> "User canceled the update");
			}
			else if (resultCode == com.google.android.play.core.install.model.ActivityResult.RESULT_IN_APP_UPDATE_FAILED)
			{
				LogHandler.getLogger().log(Level.INFO, () -> "Update failed");
			}
		}
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


	public void launchReviewFlow()
	{
		ReviewManager manager = ReviewManagerFactory.create(this);
		Task<ReviewInfo> request = manager.requestReviewFlow();
		request.addOnCompleteListener(task -> {
					if (task.isSuccessful())
					{
						LogHandler.getLogger().log(Level.INFO, () -> "In-app review flow requested");

						ReviewInfo reviewInfo = task.getResult();
						Task<Void> flow = manager.launchReviewFlow(this, reviewInfo);
						flow.addOnCompleteListener(flowTask -> {
							LogHandler.getLogger().log(Level.INFO, () -> "In-app review flow finished");
						});
					}
					else
					{
						@ReviewErrorCode int reviewErrorCode = ((ReviewException) task.getException()).getErrorCode();
						LogHandler.getLogger().log(Level.WARNING, () -> "Couldn't launch in-app review flow " + reviewErrorCode);
					}
				});

	}


	private void handleAppUpdateInfo()
	{
		Task<AppUpdateInfo> appUpdateInfoTask = mAppUpdateManager.getAppUpdateInfo();
		appUpdateInfoTask.addOnSuccessListener(appUpdateInfo -> {
					if (appUpdateInfo.updateAvailability() != UpdateAvailability.UPDATE_AVAILABLE && appUpdateInfo.updateAvailability() != UpdateAvailability.DEVELOPER_TRIGGERED_UPDATE_IN_PROGRESS)
					{
						return;
					}

					mAppUpdateInfo = appUpdateInfo;
					if (appUpdateInfo.installStatus() == InstallStatus.DOWNLOADED)
					{
						LogHandler.getLogger().log(Level.INFO, () -> "Found a downloaded but not installed flexible update");
						notifyUpdateReadyForInstallation();
					}
					else if (appUpdateInfo.updateAvailability() == UpdateAvailability.DEVELOPER_TRIGGERED_UPDATE_IN_PROGRESS)
					{
						LogHandler.getLogger().log(Level.INFO, () -> "Found an interrupted immediate update, continuing");
						startUpdateFlow(true);
					}
					else if (!mUpdateCanceledInSession)
					{
						final Integer stalenessDays = appUpdateInfo.clientVersionStalenessDays();
						notifyUpdateFound(appUpdateInfo.availableVersionCode(), stalenessDays != null ? stalenessDays : -1, appUpdateInfo.updatePriority());
					}
				});

		appUpdateInfoTask.addOnFailureListener(error -> {
					LogHandler.getLogger().log(Level.WARNING, () -> "Failed to check for update: " + error);
				});
	}


	public void startUpdateFlow(boolean pImmediate)
	{
		if (mAppUpdateInfo == null)
		{
			LogHandler.getLogger().log(Level.WARNING, () -> "No AppUpdateInfo present for update");
			return;
		}

		final int updateType = pImmediate ? AppUpdateType.IMMEDIATE : AppUpdateType.FLEXIBLE;
		if (!mAppUpdateInfo.isUpdateTypeAllowed(updateType))
		{
			LogHandler.getLogger().log(Level.WARNING, () -> "AppUpdateInfo does not allow update type " + updateType);
			return;
		}

		if (!pImmediate)
		{
			mInstallListener = state -> {
				if (state.installErrorCode() != InstallErrorCode.NO_ERROR)
				{
					LogHandler.getLogger().log(Level.WARNING, () -> "In-app update installation failed" + state.installErrorCode());
				}
				if (state.installStatus() == InstallStatus.DOWNLOADED)
				{
					LogHandler.getLogger().log(Level.INFO, () -> "Flexible update successfully downloaded, notifying user");
					notifyUpdateReadyForInstallation();
				}
			};
			mAppUpdateManager.registerListener(mInstallListener);
		}

		try
		{
			final boolean startResult = mAppUpdateManager.startUpdateFlowForResult(
					mAppUpdateInfo,
					this,
					AppUpdateOptions.newBuilder(updateType).build(),
					pImmediate ? IN_APP_IMMEDIATE_UPDATE_REQUEST_CODE : IN_APP_FLEXIBLE_UPDATE_REQUEST_CODE
					);
			LogHandler.getLogger().log(Level.INFO, () -> "Update flow started with result: " + startResult);
		}
		catch (IntentSender.SendIntentException e)
		{
			LogHandler.getLogger().log(Level.WARNING, () -> "Start of update flow failed: " + e.toString());
		}
		mAppUpdateInfo = null;
	}


	public void completeUpdate()
	{
		mAppUpdateManager.completeUpdate();
		if (mInstallListener != null)
		{
			mAppUpdateManager.unregisterListener(mInstallListener);
		}
	}


}
