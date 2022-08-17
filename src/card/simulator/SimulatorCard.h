/*!
 * \brief Implementation of Simulator.
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "SimulatorFileSystem.h"
#include "asn1/AuthenticatedAuxiliaryData.h"
#include "pace/SecureMessaging.h"

#include <memory>

namespace governikus
{


class SimulatorCard
	: public Card
{
	Q_OBJECT

	private:
		bool mConnected;
		SimulatorFileSystem mFileSystem;
		QSharedPointer<AuthenticatedAuxiliaryData> mAuxiliaryData;
		std::unique_ptr<SecureMessaging> mSecureMessaging;
		std::unique_ptr<SecureMessaging> mNewSecureMessaging;
		int mRiKeyId;

	public:
		explicit SimulatorCard(const SimulatorFileSystem& pFileSystem);

		CardReturnCode establishConnection() override;
		CardReturnCode releaseConnection() override;
		bool isConnected() const override;

		ResponseApduResult transmit(const CommandApdu& pCmd) override;

		EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription, quint8 pTimeoutSeconds) override;

		CardReturnCode destroyPaceChannel() override;

		ResponseApduResult setEidPin(quint8 pTimeoutSeconds) override;

	private:
		QByteArray brainpoolP256r1Multiplication(const QByteArray& pPoint, const QByteArray& pScalar);
		QByteArray generateAuthenticationToken(const QByteArray& pPublicKey, const QByteArray& pNonce);
		QByteArray generateRestrictedId(const QByteArray& pPublicKey);
		StatusCode verifyAuxiliaryData(const QByteArray& pCommandData);
};

} // namespace governikus
