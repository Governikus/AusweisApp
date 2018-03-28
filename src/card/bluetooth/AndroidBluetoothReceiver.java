/*
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;


public final class AndroidBluetoothReceiver extends BroadcastReceiver
{

	public enum BluetoothAdapterState
	{
		STATE_UNKNOWN(-1),
		STATE_OFF(BluetoothAdapter.STATE_OFF),
		STATE_ON(BluetoothAdapter.STATE_ON),
		STATE_TURNING_OFF(BluetoothAdapter.STATE_TURNING_OFF),
		STATE_TURNING_ON(BluetoothAdapter.STATE_TURNING_ON);

		private final int mValue;

		BluetoothAdapterState(int pValue)
		{
			mValue = pValue;
		}

		public static BluetoothAdapterState forInt(int pValue)
		{
			for (BluetoothAdapterState state : BluetoothAdapterState.values())
			{
				if (state.getValue() == pValue)
				{
					return state;
				}
			}

			Log.e(AusweisApp2Service.LOG_TAG, "Unknown state " + pValue);
			return STATE_UNKNOWN;
		}


		int getValue()
		{
			return mValue;
		}


	}

	private static final String LOG_TAG = AusweisApp2Service.LOG_TAG;

	private static final class InstanceHolder
	{
		static final BroadcastReceiver INSTANCE = new AndroidBluetoothReceiver();
	}

	private AndroidBluetoothReceiver()
	{
		super();
	}


	public static void register(Context context)
	{
		try
		{
			context.registerReceiver(InstanceHolder.INSTANCE, new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED));
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
			context.unregisterReceiver(InstanceHolder.INSTANCE);
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
		bluetoothAdapterStateChanged(previousState.getValue(), currentState.getValue());
	}


	private native void bluetoothAdapterStateChanged(int previous, int current);
}
