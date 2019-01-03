/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include <openssl/ec.h>

#include "pace/ec/EcdhGenericMapping.h"
#include "pace/ec/EcUtil.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


EcdhGenericMapping::EcdhGenericMapping(const QSharedPointer<EC_GROUP>& pCurve)
	: DomainParameterMapping()
	, mCurve(pCurve)
	, mTerminalKey()
{
}


QByteArray EcdhGenericMapping::generateTerminalMappingData()
{
	Q_ASSERT(!mCurve.isNull());
	mTerminalKey = EcUtil::create(EC_KEY_new());
	if (!EC_KEY_set_group(mTerminalKey.data(), mCurve.data()))
	{
		qCCritical(card) << "Error EC_KEY_set_group";
	}

	if (!EC_KEY_generate_key(mTerminalKey.data()))
	{
		qCCritical(card) << "Error EC_KEY_generate_key";
	}

	return EcUtil::point2oct(mCurve, EC_KEY_get0_public_key(mTerminalKey.data()));
}


QSharedPointer<EC_GROUP> EcdhGenericMapping::generateEphemeralDomainParameters(const QByteArray& pCardMappingData,
		const QByteArray& pNonce)
{
	QSharedPointer<EC_POINT> cardPubKey = EcUtil::oct2point(mCurve, pCardMappingData);
	if (cardPubKey == nullptr)
	{
		return QSharedPointer<EC_GROUP>();
	}
	if (!EC_POINT_cmp(mCurve.data(), EC_KEY_get0_public_key(mTerminalKey.data()), cardPubKey.data(), nullptr))
	{
		qCCritical(card) << "The exchanged public keys are equal.";
		return QSharedPointer<EC_GROUP>();
	}
	QSharedPointer<BIGNUM> s = EcUtil::create(BN_new());
	if (!BN_bin2bn(reinterpret_cast<const uchar*>(pNonce.constData()), pNonce.size(), s.data()))
	{
		qCCritical(card) << "Cannot convert nonce to BIGNUM";
		return QSharedPointer<EC_GROUP>();
	}

	QSharedPointer<EC_POINT> newGenerator = createNewGenerator(cardPubKey, s);
	setGenerator(newGenerator);
	return mCurve;
}


QSharedPointer<EC_POINT> EcdhGenericMapping::createNewGenerator(const QSharedPointer<const EC_POINT>& pCardPubKey, const QSharedPointer<const BIGNUM>& pS)
{
	QSharedPointer<EC_POINT> H = EcUtil::create(EC_POINT_new(mCurve.data()));
	if (!EC_POINT_mul(mCurve.data(), H.data(), nullptr, pCardPubKey.data(), EC_KEY_get0_private_key(mTerminalKey.data()), nullptr))
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


void EcdhGenericMapping::setGenerator(const QSharedPointer<const EC_POINT>& pNewGenerator)
{
	QSharedPointer<BIGNUM> curveOrder = EcUtil::create(BN_new());
	if (!EC_GROUP_get_order(mCurve.data(), curveOrder.data(), nullptr))
	{
		qCCritical(card) << "Calculation of curveOrder failed";
		return;
	}
	QSharedPointer<BIGNUM> curveCofactor = EcUtil::create(BN_new());
	if (!EC_GROUP_get_cofactor(mCurve.data(), curveCofactor.data(), nullptr))
	{
		qCCritical(card) << "Calculation of curveCofactor failed";
		return;
	}
	// Da Die Kurve Primordnung hat, entspricht die Ordnung des neuen Erzeugers der, des alten Erzeugers
	if (!EC_GROUP_set_generator(mCurve.data(), pNewGenerator.data(), curveOrder.data(), curveCofactor.data()))
	{
		qCCritical(card) << "Error EC_GROUP_set_generator";
		return;
	}
	if (!EC_GROUP_check(mCurve.data(), nullptr))
	{
		qCCritical(card) << "Error EC_GROUP_check";
		return;
	}
}
