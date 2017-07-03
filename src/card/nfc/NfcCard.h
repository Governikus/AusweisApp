/*!
 * \brief Implementation of \ref Card for NFC.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "Card.h"

namespace governikus
{
class NfcCard
	: public Card
{
	Q_OBJECT

	private:
		bool mConnected;

	Q_SIGNALS:
		void fireCardRemoved();

	public:
		NfcCard();
		virtual ~NfcCard();

		virtual ReturnCode connect() override;
		virtual ReturnCode disconnect() override;
		virtual bool isConnected() override;

		virtual ReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;

		virtual ReturnCode establishPaceChannel(PACE_PIN_ID pPinId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPACEChannelOutput& pChannelOutput, int pTimeoutSeconds) override;

		virtual ReturnCode setEidPin(unsigned int pTimeoutSeconds) override;
};

} /* namespace governikus */
