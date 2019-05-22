/*!
 * \brief Implements special BluetoothMessageParameter for TransferApduResponse.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
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
		virtual ~BluetoothMessageParameterApduResponse() override;

		const QByteArray& getResponseApdu() const;
		virtual QString toStringValue() const override;
};

} // namespace governikus
