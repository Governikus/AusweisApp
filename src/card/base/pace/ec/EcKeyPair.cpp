/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "EcKeyPair.h"

#include "EcUtil.h"
#include "asn1/ASN1TemplateUtil.h"

#include <QLoggingCategory>

#include <openssl/err.h>
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	#include <openssl/core_names.h>
#endif


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


#if OPENSSL_VERSION_NUMBER >= 0x30000000L
QByteArray EcKeyPair::multiply(const QByteArray& pPublicKey, const QSharedPointer<EVP_PKEY>& pKey)
#else
QByteArray EcKeyPair::multiply(const QByteArray& pPublicKey, const QSharedPointer<EC_KEY>& pKey)
#endif
{
	if (pKey.isNull())
	{
		qCCritical(card) << "Missing private key";
		return QByteArray();
	}

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	size_t len = 0;
	if (!EVP_PKEY_get_utf8_string_param(pKey.data(), OSSL_PKEY_PARAM_GROUP_NAME, nullptr, 0, &len) || len <= 0)
	{
		qCCritical(card) << "Missing group name";
		return QByteArray();
	}
	QByteArray groupName(static_cast<qsizetype>(len), Qt::Uninitialized);
	EVP_PKEY_get_utf8_string_param(pKey.data(), OSSL_PKEY_PARAM_GROUP_NAME, groupName.data(), len, &len);
	const auto nid = OBJ_sn2nid(groupName.data());
	const auto& curve = EcUtil::create(EC_GROUP_new_by_curve_name(nid));
#else
	const auto& curve = EcUtil::create(EC_GROUP_dup(EC_KEY_get0_group(pKey.data())));
#endif
	if (!curve)
	{
		qCCritical(card) << "Failed to create curve";
		return QByteArray();
	}

	auto point = EcUtil::oct2point(curve, pPublicKey);
	if (!point)
	{
		qCCritical(card) << "Interpreting the EC point failed";
		return QByteArray();
	}

	const auto& result = EcUtil::create(EC_POINT_new(curve.data()));
	const auto& privateKey = EcUtil::getPrivateKey(pKey);
	if (!EC_POINT_mul(curve.data(), result.data(), nullptr, point.data(), privateKey.data(), nullptr))
	{
		qCCritical(card) << "EC multiplication failed";
		return QByteArray();
	}

	return EcUtil::point2oct(curve, result.data());
}


EcKeyPair::EcKeyPair()
	: mPrivateKey()
	, mPublicKey()
{
}


EcKeyPair::EcKeyPair(const EcdhGenericMapping& pMapping)
	: mPrivateKey(EcUtil::generateKey(pMapping.getNid()))
	, mPublicKey()
{
	if (mPrivateKey)
	{
		mPublicKey = multiply(pMapping.getGenerator(), mPrivateKey);
	}
}


EcKeyPair::EcKeyPair(const QByteArray& pPrivateKey)
	: mPrivateKey()
	, mPublicKey()
{
	if (pPrivateKey.isEmpty())
	{
		return;
	}

	const auto* dataPointer = reinterpret_cast<const unsigned char*>(pPrivateKey.constData());
	const auto& key = EcUtil::create(d2i_PrivateKey(EVP_PKEY_EC, nullptr, &dataPointer, static_cast<long>(pPrivateKey.length())));
	if (key.isNull())
	{
		qCCritical(card) << "Interpreting private key failed:" << getOpenSslError();
		return;
	}

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	mPrivateKey = key;
#else
	mPrivateKey = EcUtil::create(EVP_PKEY_get1_EC_KEY(key.data()));
#endif

	if (mPrivateKey)
	{
		mPublicKey = EcUtil::getEncodedPublicKey(mPrivateKey);
	}
}


QByteArray EcKeyPair::getPublicKey(bool pCompressed) const
{
	return pCompressed ? EcUtil::compressPoint(mPublicKey) : mPublicKey;
}


QByteArray EcKeyPair::getSharedSecret(const QByteArray& pRemoteKey) const
{
	return EcUtil::compressPoint(multiply(pRemoteKey, mPrivateKey));
}
