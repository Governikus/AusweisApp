/*!
 * \brief Implements special BluetoothMessageParameter for ResultCode.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BluetoothMessageParameter.h"

namespace governikus
{

class BluetoothMessageParameterResultCode
	: public BluetoothMessageParameter
{
	private:
		BluetoothResultCode mResultCode;

	public:
		BluetoothMessageParameterResultCode(const QByteArray& pValue);
		virtual ~BluetoothMessageParameterResultCode() override;

		BluetoothResultCode getResultCode() const;
		virtual QString toStringValue() const override;
};

} /* namespace governikus */
