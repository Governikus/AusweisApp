/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "pace/ec/EcdhGenericMapping.h"

#include "pace/ec/EcUtil.h"

#include <QLoggingCategory>
#include <QScopeGuard>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)


EcdhGenericMapping::EcdhGenericMapping(const QSharedPointer<EC_GROUP>& pCurve)
	: mCurve(pCurve)
	, mTerminalKey()
{
}


const QSharedPointer<EC_GROUP>& EcdhGenericMapping::getCurve() const
{
	return mCurve;
}


QByteArray EcdhGenericMapping::generateTerminalMappingData()
{
	if (!mCurve)
	{
		qCCritical(card) << "No curve defined";
		return QByteArray();
	}

	mTerminalKey = EcUtil::generateKey(mCurve);

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	return EcUtil::getEncodedPublicKey(mTerminalKey);

#else
	return EcUtil::point2oct(mCurve, EC_KEY_get0_public_key(mTerminalKey.data()));

#endif
}


bool EcdhGenericMapping::generateEphemeralDomainParameters(const QByteArray& pCardMappingData,
		const QByteArray& pNonce)
{
	QSharedPointer<EC_POINT> cardPubKey = EcUtil::oct2point(mCurve, pCardMappingData);
	if (cardPubKey == nullptr)
	{
		return false;
	}

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	const QSharedPointer<const EC_POINT> terminalPubKeyPtr = EcUtil::oct2point(mCurve, EcUtil::getEncodedPublicKey(mTerminalKey));
	const EC_POINT* terminalPubKey = terminalPubKeyPtr.data();
#else
	const EC_POINT* terminalPubKey = EC_KEY_get0_public_key(mTerminalKey.data());
#endif
	if (!EC_POINT_cmp(mCurve.data(), terminalPubKey, cardPubKey.data(), nullptr))
	{
		qCCritical(card) << "The exchanged public keys are equal.";
		return false;
	}

	QSharedPointer<BIGNUM> s = EcUtil::create(BN_new());
	if (!BN_bin2bn(reinterpret_cast<const uchar*>(pNonce.constData()), pNonce.size(), s.data()))
	{
		qCCritical(card) << "Cannot convert nonce to BIGNUM";
		return false;
	}

	return setGenerator(createNewGenerator(cardPubKey, s));
}


QSharedPointer<EC_POINT> EcdhGenericMapping::createNewGenerator(const QSharedPointer<const EC_POINT>& pCardPubKey, const QSharedPointer<const BIGNUM>& pS)
{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	const auto& privKeyPtr = EcUtil::getPrivateKey(mTerminalKey);
	const BIGNUM* privKey = privKeyPtr.data();
#else
	const BIGNUM* privKey = EC_KEY_get0_private_key(mTerminalKey.data());
#endif

	if (!privKey)
	{
		qCCritical(card) << "Cannot fetch private key";
		return QSharedPointer<EC_POINT>();
	}

	QSharedPointer<EC_POINT> H = EcUtil::create(EC_POINT_new(mCurve.data()));
	if (!EC_POINT_mul(mCurve.data(), H.data(), nullptr, pCardPubKey.data(), privKey, nullptr))
	{
		qCCritical(card) << "Calculation of elliptic curve point H failed";
		return QSharedPointer<EC_POINT>();
	}

	QSharedPointer<EC_POINT> G_tilde = EcUtil::create(EC_POINT_new(mCurve.data()));
	if (!EC_POINT_mul(mCurve.data(), G_tilde.data(), pS.data(), H.data(), BN_value_one(), nullptr))
	{
		qCCritical(card) << "Calculation of elliptic curve point G_tilde failed";
		return QSharedPointer<EC_POINT>();
	}
	return G_tilde;
}


bool EcdhGenericMapping::setGenerator(const QSharedPointer<const EC_POINT>& pNewGenerator)
{
	QSharedPointer<BIGNUM> curveOrder = EcUtil::create(BN_new());
	if (!EC_GROUP_get_order(mCurve.data(), curveOrder.data(), nullptr))
	{
		qCCritical(card) << "Calculation of curveOrder failed";
		return false;
	}

	QSharedPointer<BIGNUM> curveCofactor = EcUtil::create(BN_new());
	if (!EC_GROUP_get_cofactor(mCurve.data(), curveCofactor.data(), nullptr))
	{
		qCCritical(card) << "Calculation of curveCofactor failed";
		return false;
	}

	// Da Die Kurve Primordnung hat, entspricht die Ordnung des neuen Erzeugers der, des alten Erzeugers
	if (!EC_GROUP_set_generator(mCurve.data(), pNewGenerator.data(), curveOrder.data(), curveCofactor.data()))
	{
		qCCritical(card) << "Error EC_GROUP_set_generator";
		return false;
	}

	if (!EC_GROUP_check(mCurve.data(), nullptr))
	{
		qCCritical(card) << "Error EC_GROUP_check";
		return false;
	}

	return true;
}
