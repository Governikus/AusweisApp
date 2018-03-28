/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Apdu.h"

#include <QByteArray>


namespace governikus
{

class PinModify
{
	private:
		enum class ProtocolType
		{
			PCSC,
			BLUETOOTH
		};

		quint8 mTimeoutSeconds;

		QByteArray createPinModificationDataStructure(ProtocolType pType) const;

	public:
		PinModify(quint8 pTimeoutSeconds);
		PinModify(const QByteArray& pRemoteInputData);

		quint8 getTimeoutSeconds() const;

		QByteArray createCcidForPcsc() const;
		QByteArray createCcidForRemote() const;
		CommandApdu createCcidForBluetooth() const;
};

} /* namespace governikus */
