/*!
 * \brief Implements special BluetoothMessage for StatusInd.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BluetoothMessage.h"
#include "parameter/BluetoothMessageParameterStatusChange.h"


namespace governikus
{

class BluetoothMessageStatusInd
	: public BluetoothMessage
{
	private:
		Q_DISABLE_COPY(BluetoothMessageStatusInd)

	public:
		BluetoothMessageStatusInd();
		virtual ~BluetoothMessageStatusInd();

		BluetoothStatusChange getStatusChange() const;
};

} // namespace governikus
