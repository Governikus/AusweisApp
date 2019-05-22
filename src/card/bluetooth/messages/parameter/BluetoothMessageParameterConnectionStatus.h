/*!
 * \brief Implements special BluetoothMessageParameter for ConnectionStatus.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
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
		virtual ~BluetoothMessageParameterConnectionStatus() override;

		BluetoothConnectionStatus getResultCode() const;
		virtual QString toStringValue() const override;
};

} // namespace governikus
