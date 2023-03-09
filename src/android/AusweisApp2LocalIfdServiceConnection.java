/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.IBinder;

public class AusweisApp2LocalIfdServiceConnection implements ServiceConnection
{
	// Native methods provided by LocalIfdClient
	public static native void notifyLocalIfdServiceConnected();
	public static native void notifyLocalIfdServiceDisconnected();

	@Override
	public void onServiceConnected(ComponentName name, IBinder service)
	{
		notifyLocalIfdServiceConnected();
	}


	@Override
	public void onServiceDisconnected(ComponentName name)
	{
		notifyLocalIfdServiceDisconnected();
	}


	@Override
	public void onBindingDied(ComponentName name)
	{
		notifyLocalIfdServiceDisconnected();
	}


	@Override
	public void onNullBinding(ComponentName name)
	{
		notifyLocalIfdServiceDisconnected();
	}


}
