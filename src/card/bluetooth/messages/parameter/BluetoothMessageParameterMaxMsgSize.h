/*!
 * BluetoothMessageParameterMaxMsgSize.h
 *
 * \brief Implements special BluetoothMessageParameter for MaxMsgSize.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "BluetoothMessageParameter.h"

namespace governikus
{

class BluetoothMessageParameterMaxMsgSize
	: public BluetoothMessageParameter
{
	private:
		unsigned int mMaxMsgSize;

	public:
		BluetoothMessageParameterMaxMsgSize(const QByteArray& pValue);
		BluetoothMessageParameterMaxMsgSize(int pMaxMsgSize);
		virtual ~BluetoothMessageParameterMaxMsgSize();

		unsigned int getMaxMsgSize() const;
		virtual QString toStringValue() const override;
};

} /* namespace governikus */
