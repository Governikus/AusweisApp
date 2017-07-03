/*!
 * BluetoothMessageStatusInd.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BluetoothMessageStatusInd.h"

using namespace governikus;

BluetoothMessageStatusInd::BluetoothMessageStatusInd()
	: BluetoothMessage(BluetoothMsgId::StatusInd)
{
}


BluetoothMessageStatusInd::~BluetoothMessageStatusInd()
{
}


BluetoothMessageParameterStatusChange::Ptr BluetoothMessageStatusInd::getParamStatusChange() const
{
	return getParameter<BluetoothMessageParameterStatusChange>();
}


BluetoothStatusChange BluetoothMessageStatusInd::getStatusChange() const
{
	auto ptr = getParamStatusChange();
	if (ptr)
	{
		return ptr->getStatusChange();
	}

	Q_ASSERT(false); // check for isValid before you call this
	return BluetoothStatusChange::Unknown;
}
