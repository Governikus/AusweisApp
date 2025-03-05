/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "EcdhGenericMapping.h"

#include "EcUtil.h"

#include <QLoggingCategory>
#include <QScopeGuard>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)


EcdhGenericMapping::EcdhGenericMapping(const QSharedPointer<EC_GROUP>& pCurve)
	: mCurve(pCurve)
	, mLocalKey()
{
}


const QSharedPointer<EC_GROUP>& EcdhGenericMapping::getCurve() const
{
	return mCurve;
}


QByteArray EcdhGenericMapping::generateLocalMappingData()
{
	if (!mCurve)
	{
		qCCritical(card) << "No curve defined";
		return QByteArray();
	}

	mLocalKey = EcUtil::generateKey(mCurve);
	return EcUtil::getEncodedPublicKey(mLocalKey);
}


bool EcdhGenericMapping::generateEphemeralDomainParameters(const QByteArray& pRemoteMappingData,
		const QByteArray& pNonce)
{
	QSharedPointer<EC_POINT> remotePubKey = EcUtil::oct2point(mCurve, pRemoteMappingData);
	if (remotePubKey == nullptr)
	{
		return false;
	}

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	const QSharedPointer<const EC_POINT> localPubKeyPtr = EcUtil::oct2point(mCurve, EcUtil::getEncodedPublicKey(mLocalKey));
	const EC_POINT* localPubKey = localPubKeyPtr.data();
#else
	const EC_POINT* localPubKey = EC_KEY_get0_public_key(mLocalKey.data());
#endif
	if (!EC_POINT_cmp(mCurve.data(), localPubKey, remotePubKey.data(), nullptr))
	{
		qCCritical(card) << "The exchanged public keys are equal.";
		return false;
	}

	QSharedPointer<BIGNUM> s = EcUtil::create(BN_new());
	if (!BN_bin2bn(reinterpret_cast<const uchar*>(pNonce.constData()), static_cast<int>(pNonce.size()), s.data()))
	{
		qCCritical(card) << "Cannot convert nonce to BIGNUM";
		return false;
	}

	return setGenerator(createNewGenerator(remotePubKey, s));
}


QSharedPointer<EC_POINT> EcdhGenericMapping::createNewGenerator(const QSharedPointer<const EC_POINT>& pRemotePubKey, const QSharedPointer<const BIGNUM>& pS)
{
	const auto& privKey = EcUtil::getPrivateKey(mLocalKey);
	if (!privKey)
	{
		qCCritical(card) << "Cannot fetch private key";
		return QSharedPointer<EC_POINT>();
	}

	QSharedPointer<EC_POINT> H = EcUtil::create(EC_POINT_new(mCurve.data()));
	if (!EC_POINT_mul(mCurve.data(), H.data(), nullptr, pRemotePubKey.data(), privKey.data(), nullptr))
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


bool EcdhGenericMapping::setGenerator(const QSharedPointer<const EC_POINT>& pNewGenerator) const
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
