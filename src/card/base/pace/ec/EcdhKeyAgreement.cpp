/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "pace/ec/EcdhKeyAgreement.h"

#include "asn1/PaceInfo.h"
#include "pace/ec/EcUtil.h"

#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(secure)


QByteArray EcdhKeyAgreement::encodeUncompressedPublicKey(const QSharedPointer<const PaceInfo>& pPaceInfo, const QSharedPointer<const EC_GROUP>& pCurve, const QSharedPointer<const EC_POINT>& pPoint)
{
	const QByteArray& publicKeyData =
			Asn1Util::encode(V_ASN1_UNIVERSAL, 6, pPaceInfo->getOid().getData()) +
			Asn1Util::encode(V_ASN1_CONTEXT_SPECIFIC, 6, EcUtil::point2oct(pCurve, pPoint.data()));
	const QByteArray publicKey = Asn1Util::encode(V_ASN1_APPLICATION, 73, publicKeyData, true);

	return publicKey;
}


EcdhKeyAgreement::EcdhKeyAgreement(const QSharedPointer<const PaceInfo>& pPaceInfo,
		const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker,
		const QSharedPointer<EcdhGenericMapping>& pMapping)
	: KeyAgreement(pPaceInfo, pCardConnectionWorker)
	, mMapping(pMapping)
	, mTerminalPublicKey()
	, mCardPublicKey()
{
}


QSharedPointer<EcdhKeyAgreement> EcdhKeyAgreement::create(const QSharedPointer<const PaceInfo>& pPaceInfo,
		const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
{
	if (!pPaceInfo->isStandardizedDomainParameters())
	{
		/*
		 * To support creation of elliptic curves with explicit domain parameters,
		 * parse the PACEDomainParameterInfo object from EF.CardAccess and use the
		 * parameters to create the curve.
		 */
		qCCritical(card) << "Creation of elliptic curves by explicit domain parameters not supported";
		return nullptr;
	}

	if (pPaceInfo->getProtocol().getMapping() != MappingType::GM)
	{
		qCCritical(card) << "Currently only generic mapping supported";
		return nullptr;
	}

	const auto curve = EcUtil::createCurve(pPaceInfo->getParameterIdAsNid());
	if (curve.isNull())
	{
		qCCritical(card) << "Creation of elliptic curve failed";
		return nullptr;
	}

	return QSharedPointer<EcdhKeyAgreement>(new EcdhKeyAgreement(pPaceInfo, pCardConnectionWorker, QSharedPointer<EcdhGenericMapping>::create(curve)));
}


KeyAgreement::CardResult EcdhKeyAgreement::determineSharedSecret(const QByteArray& pNonce)
{
	if (const auto& ephemeralCurveResultCode = determineEphemeralDomainParameters(pNonce);
			ephemeralCurveResultCode != CardReturnCode::OK)
	{
		return {ephemeralCurveResultCode};
	}

	return performKeyExchange();
}


CardReturnCode EcdhKeyAgreement::determineEphemeralDomainParameters(const QByteArray& pNonce)
{
	QByteArray terminalMappingData = mMapping->generateTerminalMappingData();
	auto [resultCode, cardMappingData] = transmitGAMappingData(terminalMappingData);
	if (resultCode != CardReturnCode::OK)
	{
		return resultCode;
	}

	return mMapping->generateEphemeralDomainParameters(cardMappingData, pNonce) ? CardReturnCode::OK : CardReturnCode::PROTOCOL_ERROR;
}


KeyAgreement::CardResult EcdhKeyAgreement::performKeyExchange()
{
	const auto& curve = mMapping->getCurve();

	const auto terminalEphemeralKey = EcUtil::generateKey(curve);
	if (terminalEphemeralKey.isNull())
	{
		return {CardReturnCode::PROTOCOL_ERROR};
	}

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	const QByteArray terminalEphemeralPublicKeyBytes = EcUtil::getEncodedPublicKey(terminalEphemeralKey);

	const auto& privKeyPtr = EcUtil::getPrivateKey(terminalEphemeralKey);
	const BIGNUM* terminalEphemeralPrivateKey = privKeyPtr.data();
#else
	const QByteArray terminalEphemeralPublicKeyBytes = EcUtil::point2oct(curve, EC_KEY_get0_public_key(terminalEphemeralKey.data()));
	const BIGNUM* const terminalEphemeralPrivateKey = EC_KEY_get0_private_key(terminalEphemeralKey.data());
#endif

	// Make a copy of the terminal public key for later mutual authentication.
	mTerminalPublicKey = EcUtil::oct2point(curve, terminalEphemeralPublicKeyBytes);

	auto [resultCode, cardEphemeralPublicKeyBytes] = transmitGAEphemeralPublicKey(terminalEphemeralPublicKeyBytes);
	if (resultCode != CardReturnCode::OK)
	{
		return {resultCode};
	}
	qCDebug(secure) << "uncompressedCardEphemeralPublicKey:" << cardEphemeralPublicKeyBytes.toHex();

	mCardPublicKey = EcUtil::oct2point(curve, cardEphemeralPublicKeyBytes);
	if (!mCardPublicKey)
	{
		qCCritical(card) << "Cannot encode card ephemeral public key";
		return {CardReturnCode::PROTOCOL_ERROR};
	}

	if (!EC_POINT_cmp(curve.data(), mTerminalPublicKey.data(), mCardPublicKey.data(), nullptr))
	{
		qCCritical(card) << "The exchanged public keys are equal";
		return {CardReturnCode::PROTOCOL_ERROR};
	}

	QSharedPointer<EC_POINT> mutualPoint = EcUtil::create(EC_POINT_new(curve.data()));
	if (!EC_POINT_mul(curve.data(), mutualPoint.data(), nullptr, mCardPublicKey.data(), terminalEphemeralPrivateKey, nullptr))
	{
		qCCritical(card) << "Calculation of elliptic curve point (shared secret) failed";
		return {CardReturnCode::PROTOCOL_ERROR};
	}

	const QByteArray sharedSecret = EcUtil::point2oct(mMapping->getCurve(), mutualPoint.data(), true);
	return {CardReturnCode::OK, sharedSecret};
}


QByteArray EcdhKeyAgreement::getUncompressedTerminalPublicKey()
{
	return encodeUncompressedPublicKey(getPaceInfo(), mMapping->getCurve(), mTerminalPublicKey);
}


QByteArray EcdhKeyAgreement::getUncompressedCardPublicKey()
{
	return encodeUncompressedPublicKey(getPaceInfo(), mMapping->getCurve(), mCardPublicKey);
}


QByteArray EcdhKeyAgreement::getCompressedCardPublicKey()
{
	return EcUtil::point2oct(mMapping->getCurve(), mCardPublicKey.data(), true);
}
