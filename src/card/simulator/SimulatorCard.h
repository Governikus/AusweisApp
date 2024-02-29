/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of Simulator.
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
		int mCaKeyId;
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
		ResponseApduResult executeFileCommand(const CommandApdu& pCmd);
		ResponseApduResult executeMseSetAt(const CommandApdu& pCmd);
		ResponseApduResult executeGeneralAuthenticate(const CommandApdu& pCmd);
		QByteArray brainpoolP256r1Multiplication(const QByteArray& pPoint, const QByteArray& pScalar) const;
		QByteArray generateAuthenticationToken(const QByteArray& pPublicKey, const QByteArray& pNonce);
		QByteArray generateRestrictedId(const QByteArray& pPublicKey) const;
		StatusCode verifyAuxiliaryData(const QByteArray& pASN1Struct);
};

} // namespace governikus
