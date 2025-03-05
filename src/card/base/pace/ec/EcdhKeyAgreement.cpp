/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "EcdhKeyAgreement.h"

#include "EcUtil.h"
#include "asn1/ASN1Struct.h"
#include "asn1/ASN1Util.h"
#include "asn1/PaceInfo.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(secure)


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
	QByteArray terminalMappingData = mMapping->generateLocalMappingData();
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

	const QByteArray terminalEphemeralPublicKeyBytes = EcUtil::getEncodedPublicKey(terminalEphemeralKey);
	const auto& terminalEphemeralPrivateKey = EcUtil::getPrivateKey(terminalEphemeralKey);

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
	if (!EC_POINT_mul(curve.data(), mutualPoint.data(), nullptr, mCardPublicKey.data(), terminalEphemeralPrivateKey.data(), nullptr))
	{
		qCCritical(card) << "Calculation of elliptic curve point (shared secret) failed";
		return {CardReturnCode::PROTOCOL_ERROR};
	}

	const QByteArray sharedSecret = EcUtil::point2oct(mMapping->getCurve(), mutualPoint.data(), true);
	return {CardReturnCode::OK, sharedSecret};
}


QByteArray EcdhKeyAgreement::getUncompressedTerminalPublicKey()
{
	return encodeUncompressedPublicKey(getPaceInfo()->getOid(), EcUtil::point2oct(mMapping->getCurve(), mTerminalPublicKey.data()));
}


QByteArray EcdhKeyAgreement::getUncompressedCardPublicKey()
{
	return encodeUncompressedPublicKey(getPaceInfo()->getOid(), EcUtil::point2oct(mMapping->getCurve(), mCardPublicKey.data()));
}


QByteArray EcdhKeyAgreement::getCompressedCardPublicKey()
{
	return EcUtil::point2oct(mMapping->getCurve(), mCardPublicKey.data(), true);
}


QByteArray EcdhKeyAgreement::encodeUncompressedPublicKey(const Oid& pOid, const QByteArray& pKey)
{
	const QByteArray oID = Asn1Util::encode(V_ASN1_UNIVERSAL, ASN1Struct::UNI_OBJECT_IDENTIFIER, QByteArray(pOid));
	const QByteArray publicPoint = Asn1Util::encode(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::EC_PUBLIC_POINT, pKey);
	return Asn1Util::encode(V_ASN1_APPLICATION, ASN1Struct::PUBLIC_KEY, oID + publicPoint, true);
}
