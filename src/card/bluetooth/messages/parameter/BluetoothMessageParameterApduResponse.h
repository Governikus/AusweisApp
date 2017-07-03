/*!
 * BluetoothMessageParameterApduResponse.h
 *
 * \brief Implements special BluetoothMessageParameter for TransferApduResponse.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "BluetoothMessageParameter.h"

namespace governikus
{

class BluetoothMessageParameterApduResponse
	: public BluetoothMessageParameter
{
	public:
		BluetoothMessageParameterApduResponse(const QByteArray& pApdu);
		virtual ~BluetoothMessageParameterApduResponse();

		const QByteArray& getResponseApdu() const;
		virtual QString toStringValue() const override;
};

} /* namespace governikus */
