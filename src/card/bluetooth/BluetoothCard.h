/*!
 * BluetoothCard.h
 *
 * \brief Implementation of Card object for Bluetooth
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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

		ReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes, unsigned int pTimeoutSeconds);

	public:
		BluetoothCard(QSharedPointer<CyberJackWaveDevice> pDevice);

		ReturnCode connect() override;
		ReturnCode disconnect() override;
		bool isConnected() override;

		ReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;

		ReturnCode establishPaceChannel(PACE_PIN_ID pPinId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPACEChannelOutput& pChannelOutput, int pTimeoutSeconds) override;

		ReturnCode destroyPaceChannel() override;

		ReturnCode setEidPin(unsigned int pTimeoutSeconds) override;
};

} /* namespace governikus */
