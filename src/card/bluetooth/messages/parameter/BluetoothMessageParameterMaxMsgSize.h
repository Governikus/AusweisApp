/*!
 * \brief Implements special BluetoothMessageParameter for MaxMsgSize.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BluetoothMessageParameter.h"

namespace governikus
{

class BluetoothMessageParameterMaxMsgSize
	: public BluetoothMessageParameter
{
	private:
		uint mMaxMsgSize;

	public:
		BluetoothMessageParameterMaxMsgSize(const QByteArray& pValue);
		BluetoothMessageParameterMaxMsgSize(uint pMaxMsgSize);
		virtual ~BluetoothMessageParameterMaxMsgSize() override;

		unsigned int getMaxMsgSize() const;
		virtual QString toStringValue() const override;
};

} // namespace governikus
