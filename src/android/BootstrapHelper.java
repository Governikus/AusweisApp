/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

package com.governikus.ausweisapp2;

public final class BootstrapHelper
{
	public static native void triggerShutdown();

	private BootstrapHelper()
	{
		throw new IllegalStateException();
	}


}
