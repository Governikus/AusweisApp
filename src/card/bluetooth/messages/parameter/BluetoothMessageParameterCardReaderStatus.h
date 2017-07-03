/*
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "messages/parameter/BluetoothMessageParameter.h"

namespace governikus
{

class BluetoothMessageParameterCardReaderStatus
	: public BluetoothMessageParameter
{
	private:
		BluetoothCardReaderStatus mCardReaderStatus;

	public:
		typedef QSharedPointer<const BluetoothMessageParameterCardReaderStatus> Ptr;

		BluetoothMessageParameterCardReaderStatus(const QByteArray& pValue);
		virtual ~BluetoothMessageParameterCardReaderStatus();

		BluetoothStatusChange getStatusChange() const;
		BluetoothCardReaderStatus getCardReaderStatus() const;
		virtual QString toStringValue() const override;
};

} /* namespace governikus */
