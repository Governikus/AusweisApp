/*!
 * BluetoothMessageParameterConnectionStatus.h
 *
 * \brief Implements special BluetoothMessageParameter for ConnectionStatus.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "BluetoothMessageParameter.h"

namespace governikus
{

class BluetoothMessageParameterConnectionStatus
	: public BluetoothMessageParameter
{
	private:
		BluetoothConnectionStatus mConnectionStatus;

	public:
		BluetoothMessageParameterConnectionStatus(const QByteArray& pValue);
		virtual ~BluetoothMessageParameterConnectionStatus();

		BluetoothConnectionStatus getResultCode() const;
		virtual QString toStringValue() const override;
};

} /* namespace governikus */
