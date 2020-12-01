/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CommandApdu.h"

#include <QByteArray>


namespace governikus
{

class PinModify
{
	private:
		quint8 mTimeoutSeconds;

	public:
		explicit PinModify(quint8 pTimeoutSeconds);
		explicit PinModify(const QByteArray& pRemoteInputData);

		quint8 getTimeoutSeconds() const;
		QByteArray createCcid() const;
};

} // namespace governikus
