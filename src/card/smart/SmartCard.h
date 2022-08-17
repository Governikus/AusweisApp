/*!
 * \brief Implementation of Smart-eID card
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "SmartManager.h"

#include <string>


namespace governikus
{


class SmartCard
	: public Card
{
	Q_OBJECT

	private:
		QSharedPointer<SmartManager> mSmartManager;

	public:
		SmartCard();

		CardReturnCode establishConnection() override;
		CardReturnCode releaseConnection() override;
		bool isConnected() const override;
		ResponseApduResult transmit(const CommandApdu& pCmd) override;

		EstablishPaceChannelOutput prepareIdentification(const QByteArray& pChat) override;
		ResponseApduResult getChallenge() override;
		TerminalAndChipAuthenticationResult performTAandCA(
			const CVCertificateChain& pTerminalCvcChain,
			const QByteArray& pAuxiliaryData,
			const QByteArray& pSignature,
			const QByteArray& pPin,
			const QByteArray& pEphemeralPublicKey) override;
};

} // namespace governikus
