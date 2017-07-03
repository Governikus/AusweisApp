package com.governikus.ausweisapp2;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.NfcAdapter;
import android.util.Log;


enum NfcAdapterState
{
	STATE_UNKNOWN(-1),
	STATE_OFF(NfcAdapter.STATE_OFF),
	STATE_TURNING_ON(NfcAdapter.STATE_TURNING_ON),
	STATE_ON(NfcAdapter.STATE_ON),
	STATE_TURNING_OFF(NfcAdapter.STATE_TURNING_OFF);


	public int value;

	private NfcAdapterState(int value)
	{
		this.value = value;
	}

	public static NfcAdapterState forInt(int value)
	{
		for (NfcAdapterState state : NfcAdapterState.values())
		{
			if (state.value == value)
			{
				return state;
			}
		}
		Log.e(NfcAdapterStateChangeReceiver.LOG_TAG, "Unknown state " + value);
		return STATE_UNKNOWN;
	}


}


public class NfcAdapterStateChangeReceiver extends BroadcastReceiver
{
	static final String LOG_TAG = "AusweisApp2";

	private static NfcAdapterStateChangeReceiver singleInstance = new NfcAdapterStateChangeReceiver();


	private NfcAdapterStateChangeReceiver()
	{
		super();
	}


	@Override
	public void onReceive(Context context, Intent intent)
	{
		NfcAdapterState state = NfcAdapterState.forInt(intent.getIntExtra(NfcAdapter.EXTRA_ADAPTER_STATE, -1));
		Log.d(LOG_TAG, "nfc state changed to " + state);
		nfcAdapterStateChanged(state.value);
	}


	private native void nfcAdapterStateChanged(int newState);


	public static void register(Context context)
	{
		try
		{
			context.registerReceiver(NfcAdapterStateChangeReceiver.singleInstance, new IntentFilter(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED));
		}
		catch (Throwable t)
		{
			Log.e(LOG_TAG, "Cannot register", t);
		}
	}


	public static void unregister(Context context)
	{
		try
		{
			context.unregisterReceiver(NfcAdapterStateChangeReceiver.singleInstance);
		}
		catch (Throwable t)
		{
			Log.e(LOG_TAG, "Cannot unregister", t);
		}
	}


}
