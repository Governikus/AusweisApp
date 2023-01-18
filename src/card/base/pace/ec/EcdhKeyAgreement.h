/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnectionWorker.h"
#include "asn1/SecurityInfo.h"
#include "pace/KeyAgreement.h"
#include "pace/ec/EcdhGenericMapping.h"

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

		static QByteArray encodeUncompressedPublicKey(const QSharedPointer<const PaceInfo>& pPaceInfo, const QSharedPointer<const EC_GROUP>& pCurve, const QSharedPointer<const EC_POINT>& pPoint);

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
};

} // namespace governikus
