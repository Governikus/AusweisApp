/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "CardConnectionWorker.h"
#include "asn1/SecurityInfo.h"
#include "pace/DomainParameterMapping.h"
#include "pace/KeyAgreement.h"

#include <QSharedPointer>
#include <openssl/ec.h>

class test_EcdhKeyAgreement;

namespace governikus
{

class EcdhKeyAgreement
	: public KeyAgreement
{
	private:
		friend class::test_EcdhKeyAgreement;

		QSharedPointer<DomainParameterMapping<EC_GROUP> > mMapping;
		QSharedPointer<EC_GROUP> mEphemeralCurve;
		QSharedPointer<EC_POINT> mTerminalPublicKey;
		QSharedPointer<EC_POINT> mCardPublicKey;

		QSharedPointer<EC_GROUP> determineEphemeralDomainParameters(const QByteArray& pNonce);
		QSharedPointer<EC_POINT> performKeyExchange(const QSharedPointer<const EC_GROUP>& pCurve);

		static QByteArray encodeUncompressedPublicKey(const QSharedPointer<const PACEInfo>& pPaceInfo, const QSharedPointer<const EC_GROUP>& pCurve, const QSharedPointer<const EC_POINT>& pPoint);
		static QByteArray encodeCompressedPublicKey(const QSharedPointer<const EC_GROUP>& pCurve, const QSharedPointer<const EC_POINT>& pPoint);

		QByteArray determineSharedSecret(const QByteArray& pNonce) override;
		QByteArray getUncompressedTerminalPublicKey() override;
		QByteArray getUncompressedCardPublicKey() override;
		QByteArray getCompressedCardPublicKey() override;

		EcdhKeyAgreement(const QSharedPointer<const PACEInfo>& pPaceInfo, const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

	public:
		static QSharedPointer<KeyAgreement> create(const QSharedPointer<const PACEInfo>& pPaceInfo,
				const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

		virtual ~EcdhKeyAgreement();
};

} /* namespace governikus */
