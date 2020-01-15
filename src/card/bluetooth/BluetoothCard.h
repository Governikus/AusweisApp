/*!
 * \brief Implementation of Card object for Bluetooth
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
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

		ResponseApduResult transmit(const CommandApdu& pCmd, quint8 pTimeoutSeconds);

	public:
		explicit BluetoothCard(QSharedPointer<CyberJackWaveDevice> pDevice);

		CardReturnCode connect() override;
		CardReturnCode disconnect() override;
		bool isConnected() override;

		ResponseApduResult transmit(const CommandApdu& pCmd) override;

		EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, quint8 pTimeoutSeconds) override;

		CardReturnCode destroyPaceChannel() override;

		ResponseApduResult setEidPin(quint8 pTimeoutSeconds) override;
};

} // namespace governikus
