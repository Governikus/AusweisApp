/*!
 * BluetoothMessageParameterStatusChange.h
 *
 * \brief Implements special BluetoothMessageParameter for StatusChange.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "BluetoothMessageParameter.h"

namespace governikus
{

class BluetoothMessageParameterStatusChange
	: public BluetoothMessageParameter
{
	private:
		BluetoothStatusChange mStatusChange;

	public:
		typedef QSharedPointer<const BluetoothMessageParameterStatusChange> Ptr;

		BluetoothMessageParameterStatusChange(const QByteArray& pValue);
		virtual ~BluetoothMessageParameterStatusChange();

		BluetoothStatusChange getStatusChange() const;
		virtual QString toStringValue() const override;
};

} /* namespace governikus */
