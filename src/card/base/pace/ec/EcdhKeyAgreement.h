/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnectionWorker.h"
#include "EcdhGenericMapping.h"
#include "pace/KeyAgreement.h"

#include <QSharedPointer>
#include <openssl/ec.h>

class test_EcdhKeyAgreement;

namespace governikus
{

class EcdhKeyAgreement
	: public KeyAgreement
{
	friend class ::test_EcdhKeyAgreement;

	private:
		QSharedPointer<EcdhGenericMapping> mMapping;
		QSharedPointer<EC_POINT> mTerminalPublicKey;
		QSharedPointer<const EC_POINT> mCardPublicKey;

		CardReturnCode determineEphemeralDomainParameters(const QByteArray& pNonce);
		CardResult performKeyExchange();

		KeyAgreement::CardResult determineSharedSecret(const QByteArray& pNonce) override;
		QByteArray getUncompressedTerminalPublicKey() override;
		QByteArray getUncompressedCardPublicKey() override;
		QByteArray getCompressedCardPublicKey() override;

		explicit EcdhKeyAgreement(const QSharedPointer<const PaceInfo>& pPaceInfo,
				const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker,
				const QSharedPointer<EcdhGenericMapping>& pMapping);

	public:
		static QSharedPointer<EcdhKeyAgreement> create(const QSharedPointer<const PaceInfo>& pPaceInfo,
				const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

		static QByteArray encodeUncompressedPublicKey(const Oid& pOid, const QByteArray& pKey);
};

} // namespace governikus
