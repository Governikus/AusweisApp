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

	Q_SIGNALS:
		void fireCardRemoved();

	public:
		NfcCard();
		virtual ~NfcCard();

		virtual CardReturnCode connect() override;
		virtual CardReturnCode disconnect() override;
		virtual bool isConnected() override;

		virtual CardReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;
};

} /* namespace governikus */
