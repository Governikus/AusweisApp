/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "Commands.h"
#include "asn1/KnownOIDs.h"
#include "asn1/PACEInfo.h"
#include "pace/CipherMac.h"
#include "pace/ec/EcUtil.h"
#include "pace/ec/EcdhGenericMapping.h"
#include "pace/ec/EcdhKeyAgreement.h"
#include "pace/ec/EllipticCurveFactory.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(secure)


QByteArray EcdhKeyAgreement::encodeUncompressedPublicKey(QSharedPointer<PACEInfo> pPaceInfo, QSharedPointer<EC_GROUP> pCurve, QSharedPointer<EC_POINT> pPoint)
{
	QByteArray pointBytes = EcUtil::point2oct(pCurve, pPoint.data());

	QByteArray publicKeyData;
	publicKeyData.append(0x06);
	publicKeyData.append(pPaceInfo->getProtocolValueBytes().size());
	publicKeyData.append(pPaceInfo->getProtocolValueBytes());
	publicKeyData.append(char(0x86));
	publicKeyData.append(pointBytes.size());
	publicKeyData.append(pointBytes);

	QByteArray publicKey;
	publicKey.append("\x7f\x49");
	publicKey.append(publicKeyData.size());
	publicKey.append(publicKeyData);

	return publicKey;
}


QByteArray EcdhKeyAgreement::encodeCompressedPublicKey(QSharedPointer<EC_GROUP> pCurve, QSharedPointer<EC_POINT> pPoint)
{
	QByteArray uncompressedPointBytes = EcUtil::point2oct(pCurve, pPoint.data());
	return uncompressedPointBytes.mid(1, (uncompressedPointBytes.size() - 1) / 2);
}


EcdhKeyAgreement::EcdhKeyAgreement(QSharedPointer<PACEInfo> pPaceInfo, QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
	: KeyAgreement(pPaceInfo, pCardConnectionWorker)
	, mMapping()
	, mEphemeralCurve()
	, mTerminalPublicKey()
	, mCardPublicKey()
{
}


QSharedPointer<KeyAgreement> EcdhKeyAgreement::create(QSharedPointer<PACEInfo> pPaceInfo,
		QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
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


QByteArray EcdhKeyAgreement::determineSharedSecret(const QByteArray& pNonce)
{
	mEphemeralCurve = determineEphemeralDomainParameters(pNonce);
	if (!mEphemeralCurve)
	{
		return QByteArray();
	}

	QSharedPointer<EC_POINT> mutualPoint = performKeyExchange(mEphemeralCurve);
	if (!mutualPoint)
	{
		return QByteArray();
	}

	QByteArray sharedSecret = EcUtil::point2oct(mEphemeralCurve, mutualPoint.data());
	sharedSecret = sharedSecret.mid(1, (sharedSecret.size() - 1) / 2);

	return sharedSecret;
}


QSharedPointer<EC_GROUP> EcdhKeyAgreement::determineEphemeralDomainParameters(const QByteArray& pNonce)
{
	QByteArray terminalMappingData = mMapping->generateTerminalMappingData();
	QByteArray cardMappingData = transmitGAMappingData(terminalMappingData);
	if (cardMappingData.isNull())
	{
		return QSharedPointer<EC_GROUP>();
	}
	return mMapping->generateEphemeralDomainParameters(cardMappingData, pNonce);
}


QSharedPointer<EC_POINT> EcdhKeyAgreement::performKeyExchange(QSharedPointer<EC_GROUP> pCurve)
{
	QSharedPointer<EC_KEY> terminalEphemeralKey = EcUtil::create(EC_KEY_new());
	if (!EC_KEY_set_group(terminalEphemeralKey.data(), pCurve.data()))
	{
		qCCritical(card) << "Error EC_KEY_set_group";
		return QSharedPointer<EC_POINT>();
	}
	if (!EC_KEY_generate_key(terminalEphemeralKey.data()))
	{
		qCCritical(card) << "Error EC_KEY_generate_key";
		return QSharedPointer<EC_POINT>();
	}
	// Make a copy of the terminal public key for later mutual authentication.
	mTerminalPublicKey = EcUtil::create(EC_POINT_dup(EC_KEY_get0_public_key(terminalEphemeralKey.data()), pCurve.data()));
	QByteArray terminalEphemeralPublicKeyBytes = EcUtil::point2oct(pCurve, mTerminalPublicKey.data());
	const BIGNUM* terminalPrivateKey = EC_KEY_get0_private_key(terminalEphemeralKey.data());

	QByteArray cardEphemeralPublicKeyBytes = transmitGAEphemeralPublicKey(terminalEphemeralPublicKeyBytes);
	if (cardEphemeralPublicKeyBytes.isNull())
	{
		return QSharedPointer<EC_POINT>();
	}
	qCDebug(secure) << "uncompressedCardEphemeralPublicKey: " << cardEphemeralPublicKeyBytes.toHex();

	mCardPublicKey = EcUtil::oct2point(pCurve, cardEphemeralPublicKeyBytes);
	if (!mCardPublicKey)
	{
		qCCritical(card) << "Cannot encode card ephemeral public key";
		return QSharedPointer<EC_POINT>();
	}

	if (!EC_POINT_cmp(pCurve.data(), mTerminalPublicKey.data(), mCardPublicKey.data(), nullptr))
	{
		qCCritical(card) << "The exchanged public keys are equal";
		return QSharedPointer<EC_POINT>();
	}

	QSharedPointer<EC_POINT> mutualPoint = EcUtil::create(EC_POINT_new(pCurve.data()));
	if (!EC_POINT_mul(pCurve.data(), mutualPoint.data(), nullptr, mCardPublicKey.data(), terminalPrivateKey, nullptr))
	{
		qCCritical(card) << "Calculation of elliptic curve point H failed";
		return QSharedPointer<EC_POINT>();
	}
	return mutualPoint;
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
