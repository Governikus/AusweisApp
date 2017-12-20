/*
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
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
		using Ptr = QSharedPointer<const BluetoothMessageParameterCardReaderStatus>;

		BluetoothMessageParameterCardReaderStatus(const QByteArray& pValue);
		virtual ~BluetoothMessageParameterCardReaderStatus() override;

		BluetoothStatusChange getStatusChange() const;
		BluetoothCardReaderStatus getCardReaderStatus() const;
		virtual QString toStringValue() const override;
};

} /* namespace governikus */
