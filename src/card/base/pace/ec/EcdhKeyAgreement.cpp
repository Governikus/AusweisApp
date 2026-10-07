/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "EcdhKeyAgreement.h"

#include "EcKeyPair.h"
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
		const QSharedPointer<EC_GROUP>& pCurve)
	: KeyAgreement(pPaceInfo, pCardConnectionWorker)
	, mMapping(pCurve)
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

	return QSharedPointer<EcdhKeyAgreement>(new EcdhKeyAgreement(pPaceInfo, pCardConnectionWorker, curve));
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
	QByteArray terminalMappingData = mMapping.generateLocalMappingData();
	auto [resultCode, cardMappingData] = transmitGAMappingData(terminalMappingData);
	if (resultCode != CardReturnCode::OK)
	{
		return resultCode;
	}

	return mMapping.generateEphemeralDomainParameters(cardMappingData, pNonce) ? CardReturnCode::OK : CardReturnCode::PROTOCOL_ERROR;
}


KeyAgreement::CardResult EcdhKeyAgreement::performKeyExchange()
{
	EcKeyPair keyPair(mMapping);
	mTerminalPublicKey = keyPair.getPublicKey();
	if (mTerminalPublicKey.isEmpty())
	{
		return {CardReturnCode::PROTOCOL_ERROR};
	}

	auto [resultCode, cardEphemeralPublicKeyBytes] = transmitGAEphemeralPublicKey(mTerminalPublicKey);
	if (resultCode != CardReturnCode::OK)
	{
		return {resultCode};
	}

	if (cardEphemeralPublicKeyBytes.isEmpty())
	{
		qCCritical(card) << "Missing card ephemeral public key";
		return {CardReturnCode::PROTOCOL_ERROR};
	}

	qCDebug(secure) << "uncompressedCardEphemeralPublicKey:" << cardEphemeralPublicKeyBytes.toHex();
	mCardPublicKey = cardEphemeralPublicKeyBytes;

	if (mTerminalPublicKey == mCardPublicKey)
	{
		qCCritical(card) << "The exchanged public keys are equal";
		return {CardReturnCode::PROTOCOL_ERROR};
	}

	const QByteArray sharedSecret = keyPair.getSharedSecret(mCardPublicKey);
	return {CardReturnCode::OK, sharedSecret};
}


QByteArray EcdhKeyAgreement::getUncompressedTerminalPublicKey()
{
	return encodeUncompressedPublicKey(getPaceInfo()->getOid(), mTerminalPublicKey);
}


QByteArray EcdhKeyAgreement::getUncompressedCardPublicKey()
{
	return encodeUncompressedPublicKey(getPaceInfo()->getOid(), mCardPublicKey);
}


QByteArray EcdhKeyAgreement::getCompressedCardPublicKey()
{
	return EcUtil::compressPoint(mCardPublicKey);
}


QByteArray EcdhKeyAgreement::encodeUncompressedPublicKey(const Oid& pOid, const QByteArray& pKey)
{
	const QByteArray oID = Asn1Util::encode(V_ASN1_UNIVERSAL, ASN1Struct::UNI_OBJECT_IDENTIFIER, QByteArray(pOid));
	const QByteArray publicPoint = Asn1Util::encode(V_ASN1_CONTEXT_SPECIFIC, ASN1Struct::EC_PUBLIC_POINT, pKey);
	return Asn1Util::encode(V_ASN1_APPLICATION, ASN1Struct::PUBLIC_KEY, oID + publicPoint, true);
}
