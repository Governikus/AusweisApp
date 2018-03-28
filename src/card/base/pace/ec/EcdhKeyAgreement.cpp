/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/KnownOIDs.h"
#include "asn1/PACEInfo.h"
#include "Commands.h"
#include "pace/CipherMac.h"
#include "pace/ec/EcdhGenericMapping.h"
#include "pace/ec/EcdhKeyAgreement.h"
#include "pace/ec/EcUtil.h"
#include "pace/ec/EllipticCurveFactory.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(secure)


QByteArray EcdhKeyAgreement::encodeUncompressedPublicKey(const QSharedPointer<const PACEInfo>& pPaceInfo, const QSharedPointer<const EC_GROUP>& pCurve, const QSharedPointer<const EC_POINT>& pPoint)
{
	const QByteArray& publicKeyData =
			Asn1Util::encode(char(0x06), pPaceInfo->getProtocolValueBytes()) +
			Asn1Util::encode(char(0x86), EcUtil::point2oct(pCurve, pPoint.data()));

	const QByteArray& publicKey =
			QByteArray(1, char(0x7f)) +
			Asn1Util::encode(char(0x49), publicKeyData);

	return publicKey;
}


QByteArray EcdhKeyAgreement::encodeCompressedPublicKey(const QSharedPointer<const EC_GROUP>& pCurve, const QSharedPointer<const EC_POINT>& pPoint)
{
	QByteArray uncompressedPointBytes = EcUtil::point2oct(pCurve, pPoint.data());
	return uncompressedPointBytes.mid(1, (uncompressedPointBytes.size() - 1) / 2);
}


EcdhKeyAgreement::EcdhKeyAgreement(const QSharedPointer<const PACEInfo>& pPaceInfo, const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
	: KeyAgreement(pPaceInfo, pCardConnectionWorker)
	, mMapping()
	, mEphemeralCurve()
	, mTerminalPublicKey()
	, mCardPublicKey()
{
}


QSharedPointer<KeyAgreement> EcdhKeyAgreement::create(const QSharedPointer<const PACEInfo>& pPaceInfo,
		const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
{
	QSharedPointer<EcdhKeyAgreement> keyAgreement(new EcdhKeyAgreement(pPaceInfo, pCardConnectionWorker));

	QSharedPointer<EC_GROUP> ecGroup = EllipticCurveFactory::create(pPaceInfo);
	if (ecGroup == nullptr)
	{
		qCCritical(card) << "Creation of elliptic curve failed";
		return QSharedPointer<EcdhKeyAgreement>();
	}

	if (pPaceInfo->getMappingType() != MappingType::GM)
	{
		qCCritical(card) << "Currently only generic mapping supported";
		return QSharedPointer<EcdhKeyAgreement>();
	}
	keyAgreement->mMapping.reset(new EcdhGenericMapping(ecGroup));

	return keyAgreement;
}


EcdhKeyAgreement::~EcdhKeyAgreement()
{
}


CardOperationResult<QByteArray> EcdhKeyAgreement::determineSharedSecret(const QByteArray& pNonce)
{
	CardOperationResult<QSharedPointer<EC_GROUP> > ephemeralCurveResult = determineEphemeralDomainParameters(pNonce);
	CardReturnCode ephemeralCurveResultCode = ephemeralCurveResult.getReturnCode();
	mEphemeralCurve = ephemeralCurveResult.getPayload();
	if (ephemeralCurveResultCode != CardReturnCode::OK)
	{
		return CardOperationResult<QByteArray>(ephemeralCurveResultCode, QByteArray());
	}

	CardOperationResult<QSharedPointer<EC_POINT> > mutualPointResult = performKeyExchange(mEphemeralCurve);
	CardReturnCode mutualPointResultCode = mutualPointResult.getReturnCode();
	QSharedPointer<EC_POINT> mutualPoint = mutualPointResult.getPayload();
	if (mutualPointResultCode != CardReturnCode::OK)
	{
		return CardOperationResult<QByteArray>(mutualPointResultCode, QByteArray());
	}

	QByteArray sharedSecret = EcUtil::point2oct(mEphemeralCurve, mutualPoint.data());
	sharedSecret = sharedSecret.mid(1, (sharedSecret.size() - 1) / 2);

	return CardOperationResult<QByteArray>(CardReturnCode::OK, sharedSecret);
}


CardOperationResult<QSharedPointer<EC_GROUP> > EcdhKeyAgreement::determineEphemeralDomainParameters(const QByteArray& pNonce)
{
	QByteArray terminalMappingData = mMapping->generateTerminalMappingData();
	CardOperationResult<QByteArray> result = transmitGAMappingData(terminalMappingData);
	CardReturnCode resultCode = result.getReturnCode();
	if (resultCode != CardReturnCode::OK)
	{
		return CardOperationResult<QSharedPointer<EC_GROUP> >(resultCode, QSharedPointer<EC_GROUP>());
	}

	QByteArray cardMappingData = result.getPayload();
	return CardOperationResult<QSharedPointer<EC_GROUP> >(CardReturnCode::OK, mMapping->generateEphemeralDomainParameters(cardMappingData, pNonce));
}


CardOperationResult<QSharedPointer<EC_POINT> > EcdhKeyAgreement::performKeyExchange(const QSharedPointer<const EC_GROUP>& pCurve)
{
	QSharedPointer<EC_KEY> terminalEphemeralKey = EcUtil::create(EC_KEY_new());
	if (!EC_KEY_set_group(terminalEphemeralKey.data(), pCurve.data()))
	{
		qCCritical(card) << "Error EC_KEY_set_group";
		return CardOperationResult<QSharedPointer<EC_POINT> >(CardReturnCode::PROTOCOL_ERROR, QSharedPointer<EC_POINT>());
	}
	if (!EC_KEY_generate_key(terminalEphemeralKey.data()))
	{
		qCCritical(card) << "Error EC_KEY_generate_key";
		return CardOperationResult<QSharedPointer<EC_POINT> >(CardReturnCode::PROTOCOL_ERROR, QSharedPointer<EC_POINT>());
	}
	// Make a copy of the terminal public key for later mutual authentication.
	mTerminalPublicKey = EcUtil::create(EC_POINT_dup(EC_KEY_get0_public_key(terminalEphemeralKey.data()), pCurve.data()));
	QByteArray terminalEphemeralPublicKeyBytes = EcUtil::point2oct(pCurve, mTerminalPublicKey.data());
	const BIGNUM* terminalPrivateKey = EC_KEY_get0_private_key(terminalEphemeralKey.data());

	CardOperationResult<QByteArray> result = transmitGAEphemeralPublicKey(terminalEphemeralPublicKeyBytes);
	CardReturnCode resultCode = result.getReturnCode();
	if (resultCode != CardReturnCode::OK)
	{
		return CardOperationResult<QSharedPointer<EC_POINT> >(resultCode, QSharedPointer<EC_POINT>());
	}
	QByteArray cardEphemeralPublicKeyBytes = result.getPayload();
	qCDebug(secure) << "uncompressedCardEphemeralPublicKey: " << cardEphemeralPublicKeyBytes.toHex();

	mCardPublicKey = EcUtil::oct2point(pCurve, cardEphemeralPublicKeyBytes);
	if (!mCardPublicKey)
	{
		qCCritical(card) << "Cannot encode card ephemeral public key";
		return CardOperationResult<QSharedPointer<EC_POINT> >(CardReturnCode::PROTOCOL_ERROR, QSharedPointer<EC_POINT>());
	}

	if (!EC_POINT_cmp(pCurve.data(), mTerminalPublicKey.data(), mCardPublicKey.data(), nullptr))
	{
		qCCritical(card) << "The exchanged public keys are equal";
		return CardOperationResult<QSharedPointer<EC_POINT> >(CardReturnCode::PROTOCOL_ERROR, QSharedPointer<EC_POINT>());
	}

	QSharedPointer<EC_POINT> mutualPoint = EcUtil::create(EC_POINT_new(pCurve.data()));
	if (!EC_POINT_mul(pCurve.data(), mutualPoint.data(), nullptr, mCardPublicKey.data(), terminalPrivateKey, nullptr))
	{
		qCCritical(card) << "Calculation of elliptic curve point H failed";
		return CardOperationResult<QSharedPointer<EC_POINT> >(CardReturnCode::PROTOCOL_ERROR, QSharedPointer<EC_POINT>());
	}
	return CardOperationResult<QSharedPointer<EC_POINT> >(CardReturnCode::OK, mutualPoint);
}


QByteArray EcdhKeyAgreement::getUncompressedTerminalPublicKey()
{
	return encodeUncompressedPublicKey(mPaceInfo, mEphemeralCurve, mTerminalPublicKey);
}


QByteArray EcdhKeyAgreement::getUncompressedCardPublicKey()
{
	return encodeUncompressedPublicKey(mPaceInfo, mEphemeralCurve, mCardPublicKey);
}


QByteArray EcdhKeyAgreement::getCompressedCardPublicKey()
{
	return encodeCompressedPublicKey(mEphemeralCurve, mCardPublicKey);
}
