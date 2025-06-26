/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "PortWrapper.h"


using namespace governikus;


bool PortWrapper::isEmpty() const
{
	return mPorts.isEmpty();
}


void PortWrapper::invalidate()
{
	if (!isEmpty())
	{
		mPorts.removeFirst();
	}
}


quint16 PortWrapper::fetchPort() const
{
	if (isEmpty())
	{
		return 0;
	}

	return mPorts.constFirst();
}
