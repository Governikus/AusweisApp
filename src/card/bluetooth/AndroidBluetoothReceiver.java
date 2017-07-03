package com.governikus.ausweisapp2;

import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;


public class AndroidBluetoothReceiver extends BroadcastReceiver
{

	public enum BluetoothAdapterState
	{
		STATE_UNKNOWN(-1),
		STATE_OFF(BluetoothAdapter.STATE_OFF),
		STATE_ON(BluetoothAdapter.STATE_ON),
		STATE_TURNING_OFF(BluetoothAdapter.STATE_TURNING_OFF),
		STATE_TURNING_ON(BluetoothAdapter.STATE_TURNING_ON);

		public int value;

		private BluetoothAdapterState(int value)
		{
			this.value = value;
		}

		public static BluetoothAdapterState forInt(int value)
		{
			for (BluetoothAdapterState state : BluetoothAdapterState.values())
			{
				if (state.value == value)
				{
					return state;
				}
			}
			Log.e(LOG_TAG, "Unknown state " + value);
			return STATE_UNKNOWN;
		}


	}

	private static final String LOG_TAG = "AusweisApp2";

	private static final BroadcastReceiver singleInstance = new AndroidBluetoothReceiver();


	private AndroidBluetoothReceiver()
	{
		super();
	}


	public static void register(Context context)
	{
		try
		{
			context.registerReceiver(AndroidBluetoothReceiver.singleInstance, new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED));
		}
		catch (Throwable t)
		{
			Log.e(LOG_TAG, "Cannot register ", t);
		}
	}


	public static void unregister(Context context)
	{
		try
		{
			context.unregisterReceiver(AndroidBluetoothReceiver.singleInstance);
		}
		catch (Throwable t)
		{
			Log.e(LOG_TAG, "Cannot unregister ", t);
		}
	}


	public void onReceive(Context context, Intent intent)
	{
		BluetoothAdapterState previousState = BluetoothAdapterState.forInt(intent.getIntExtra(BluetoothAdapter.EXTRA_PREVIOUS_STATE, -1));
		BluetoothAdapterState currentState = BluetoothAdapterState.forInt(intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, -1));

		Log.d(LOG_TAG, "state changed " + previousState + " -> " + currentState);
		bluetoothAdapterStateChanged(previousState.value, currentState.value);
	}


	private native void bluetoothAdapterStateChanged(int previous, int current);
}