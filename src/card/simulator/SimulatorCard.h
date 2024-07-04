/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "SimulatorFileSystem.h"
#include "asn1/AuthenticatedAuxiliaryData.h"
#include "asn1/Chat.h"
#include "asn1/Oid.h"
#include "pace/SecureMessaging.h"

#include <memory>
#include <openssl/ec.h>


namespace governikus
{


class SimulatorCard
	: public Card
{
	Q_OBJECT

	private:
		bool mConnected;
		SimulatorFileSystem mFileSystem;
		std::unique_ptr<SecureMessaging> mSecureMessaging;
		std::unique_ptr<SecureMessaging> mNewSecureMessaging;
		Oid mSelectedProtocol;
		int mChainingStep;
		int mPaceKeyId;
		QSharedPointer<CHAT> mPaceChat;
		QByteArray mPaceNonce;
		QByteArray mPaceTerminalKey;
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
		QSharedPointer<EVP_PKEY> mCardKey;
#else
		QSharedPointer<EC_KEY> mCardKey;
#endif
		QSharedPointer<AuthenticatedAuxiliaryData> mTaAuxData;

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
		QByteArray ecMultiplication(const QByteArray& pPoint) const;
		QByteArray generateAuthenticationToken(const QByteArray& pPublicKey, const QByteArray& pNonce = QByteArray());
		QByteArray generateRestrictedId(const QByteArray& pPublicKey) const;
		StatusCode verifyAuxiliaryData(const QByteArray& pASN1Struct);
};

} // namespace governikus
