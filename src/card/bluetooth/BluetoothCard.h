/*!
 * \brief Implementation of Card object for Bluetooth
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "Card.h"
#include "CyberJackWaveDevice.h"


namespace governikus
{

class BluetoothCard
	: public Card
{
	Q_OBJECT

	private:
		bool mConnected;
		QSharedPointer<CyberJackWaveDevice> mDevice;

		CardReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes, quint8 pTimeoutSeconds);

	public:
		BluetoothCard(QSharedPointer<CyberJackWaveDevice> pDevice);

		CardReturnCode connect() override;
		CardReturnCode disconnect() override;
		bool isConnected() override;

		CardReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;

		CardReturnCode establishPaceChannel(PACE_PASSWORD_ID pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPACEChannelOutput& pChannelOutput, quint8 pTimeoutSeconds) override;

		CardReturnCode destroyPaceChannel() override;

		CardReturnCode setEidPin(quint8 pTimeoutSeconds) override;
};

} /* namespace governikus */
