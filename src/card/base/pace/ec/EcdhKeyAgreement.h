/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/SecurityInfo.h"
#include "CardConnectionWorker.h"
#include "pace/DomainParameterMapping.h"
#include "pace/KeyAgreement.h"

#include <openssl/ec.h>
#include <QPair>
#include <QSharedPointer>

class test_EcdhKeyAgreement;

namespace governikus
{

class EcdhKeyAgreement
	: public KeyAgreement
{
	private:
		friend class ::test_EcdhKeyAgreement;

		QSharedPointer<DomainParameterMapping<EC_GROUP>> mMapping;
		QSharedPointer<EC_GROUP> mEphemeralCurve;
		QSharedPointer<EC_POINT> mTerminalPublicKey;
		QSharedPointer<const EC_POINT> mCardPublicKey;

		QPair<CardReturnCode, QSharedPointer<EC_GROUP>> determineEphemeralDomainParameters(const QByteArray& pNonce);
		QPair<CardReturnCode, QSharedPointer<EC_POINT>> performKeyExchange(const QSharedPointer<const EC_GROUP>& pCurve);

		static QByteArray encodeUncompressedPublicKey(const QSharedPointer<const PaceInfo>& pPaceInfo, const QSharedPointer<const EC_GROUP>& pCurve, const QSharedPointer<const EC_POINT>& pPoint);
		static QByteArray encodeCompressedPublicKey(const QSharedPointer<const EC_GROUP>& pCurve, const QSharedPointer<const EC_POINT>& pPoint);

		KeyAgreement::CardResult determineSharedSecret(const QByteArray& pNonce) override;
		QByteArray getUncompressedTerminalPublicKey() override;
		QByteArray getUncompressedCardPublicKey() override;
		QByteArray getCompressedCardPublicKey() override;

		EcdhKeyAgreement(const QSharedPointer<const PaceInfo>& pPaceInfo, const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

	public:
		static QSharedPointer<EcdhKeyAgreement> create(const QSharedPointer<const PaceInfo>& pPaceInfo,
				const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

		~EcdhKeyAgreement() override = default;
};

} // namespace governikus
