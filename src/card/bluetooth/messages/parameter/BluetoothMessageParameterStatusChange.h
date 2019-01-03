/*!
 * \brief Implements special BluetoothMessageParameter for StatusChange.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
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
		using Ptr = QSharedPointer<const BluetoothMessageParameterStatusChange>;

		BluetoothMessageParameterStatusChange(const QByteArray& pValue);
		virtual ~BluetoothMessageParameterStatusChange() override;

		BluetoothStatusChange getStatusChange() const;
		virtual QString toStringValue() const override;
};

} // namespace governikus
