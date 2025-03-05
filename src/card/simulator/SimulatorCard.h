/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "SimulatorFileSystem.h"
#include "SmartCardDefinitions.h"
#include "asn1/AccessRoleAndRight.h"
#include "asn1/CVCertificate.h"
#include "asn1/Oid.h"
#include "pace/SecureMessaging.h"

#include <QSet>

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
		QSet<AccessRight> mAccessRights;
		PacePasswordId mPacePassword;
		int mPaceKeyId;
		QByteArray mPaceNonce;
		QByteArray mPaceTerminalKey;
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
		QSharedPointer<EVP_PKEY> mCardKey;
#else
		QSharedPointer<EC_KEY> mCardKey;
#endif
		QSharedPointer<const CVCertificate> mTaCertificate;
		QByteArray mTaSigningData;
		QByteArray mTaAuxData;

	public:
		explicit SimulatorCard(const SimulatorFileSystem& pFileSystem);

		CardReturnCode establishConnection() override;
		CardReturnCode releaseConnection() override;
		bool isConnected() const override;

		ResponseApduResult transmit(const CommandApdu& pCmd) override;

		EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription) override;

		CardReturnCode destroyPaceChannel() override;

		ResponseApduResult setEidPin(quint8 pTimeoutSeconds) override;

	private:
		ResponseApdu executeCommand(const CommandApdu& pCmd);
		ResponseApdu executeFileCommand(const CommandApdu& pCmd);
		ResponseApdu executeMseSetAt(const CommandApdu& pCmd);
		ResponseApdu executeMseSetDst(const QByteArray& pData) const;
		ResponseApdu executeGeneralAuthenticate(const CommandApdu& pCmd);
		ResponseApdu executePsoVerify(const QByteArray& pData);
		ResponseApdu executeExternalAuthenticate(const QByteArray& pSignature);
		ResponseApdu executePinManagement(const CommandApdu& pCmd) const;
		ResponseApdu executeResetRetryCounter(const CommandApdu& pCmd) const;
		QByteArray ecMultiplication(const QByteArray& pPoint) const;
		QByteArray generateAuthenticationToken(const QByteArray& pPublicKey, const QByteArray& pNonce, const QByteArray& pVerify = QByteArray());
		QByteArray generateRestrictedId(const QByteArray& pPublicKey) const;
		StatusCode verifyAuxiliaryData(const QByteArray& pASN1Struct) const;
};

} // namespace governikus
