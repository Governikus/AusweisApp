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


BluetoothStatusChange BluetoothMessageStatusInd::getStatusChange() const
{
	const auto& param = getParameter(BluetoothParamId::StatusChange);
	if (param)
	{
		return param.staticCast<const BluetoothMessageParameterStatusChange>()->getStatusChange();
	}

	Q_ASSERT(false); // check for isValid before you call this
	return BluetoothStatusChange::Unknown;
}
