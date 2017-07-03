/*!
 * BluetoothMessageParameterResultCode.h
 *
 * \brief Implements special BluetoothMessageParameter for ResultCode.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
		virtual ~BluetoothMessageParameterResultCode();

		BluetoothResultCode getResultCode() const;
		virtual QString toStringValue() const override;
};

} /* namespace governikus */
