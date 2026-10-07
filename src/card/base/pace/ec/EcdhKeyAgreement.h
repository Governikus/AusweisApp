/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
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
		EcdhGenericMapping mMapping;
		QByteArray mTerminalPublicKey;
		QByteArray mCardPublicKey;

		CardReturnCode determineEphemeralDomainParameters(const QByteArray& pNonce);
		CardResult performKeyExchange();

		KeyAgreement::CardResult determineSharedSecret(const QByteArray& pNonce) override;
		QByteArray getUncompressedTerminalPublicKey() override;
		QByteArray getUncompressedCardPublicKey() override;
		QByteArray getCompressedCardPublicKey() override;

		explicit EcdhKeyAgreement(const QSharedPointer<const PaceInfo>& pPaceInfo,
				const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker,
				const QSharedPointer<EC_GROUP>& pCurve);

	public:
		static QSharedPointer<EcdhKeyAgreement> create(const QSharedPointer<const PaceInfo>& pPaceInfo,
				const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

		static QByteArray encodeUncompressedPublicKey(const Oid& pOid, const QByteArray& pKey);
};

} // namespace governikus
