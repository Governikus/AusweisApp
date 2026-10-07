/**
 * Copyright (c) 2021-2026 Governikus Service GmbH, Germany
 */

#include "EcUtil.h"
#include "asn1/ASN1TemplateUtil.h"

#include <QLoggingCategory>
#include <QScopeGuard>

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	#include <openssl/param_build.h>
#endif

#include <openssl/err.h>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;


QSharedPointer<EC_GROUP> EcUtil::createCurve(int pNid)
{
	qCDebug(card) << "Create elliptic curve:" << OBJ_nid2sn(pNid);
	EC_GROUP* ecGroup = EC_GROUP_new_by_curve_name(pNid);
	if (ecGroup == nullptr)
	{
		qCCritical(card) << "Error on EC_GROUP_new_by_curve_name, curve is unknown:" << pNid;
	}
	return EcUtil::create(ecGroup);
}


QByteArray EcUtil::compressPoint(const QByteArray& pPoint)
{
	if (pPoint.size() % 2 == 0 || pPoint.at(0) != 0x04)
	{
		qCCritical(card) << "Unable to apply compression on point:" << pPoint.toHex();
		return pPoint;
	}

	// Compression as defined in TR 03110-3 A.2.2.3 without a leading
	// byte in front of the x coordinate indicating the compression type,
	// i.e. not the typical SECG SEC1 v1.0 format used by OpenSSL.
	return pPoint.mid(1, (pPoint.size() - 1) / 2);
}


QByteArray EcUtil::point2oct(const QSharedPointer<const EC_GROUP>& pCurve, const EC_POINT* pPoint, bool pCompressed)
{
	if (pCurve.isNull() || pPoint == nullptr)
	{
		qCCritical(card) << "Invalid input data, cannot encode elliptic curve point";
		return QByteArray();
	}

	const point_conversion_form_t form = pCompressed ? POINT_CONVERSION_COMPRESSED : POINT_CONVERSION_UNCOMPRESSED;
	const size_t buf_size = EC_POINT_point2oct(pCurve.data(), pPoint, form, nullptr, 0, nullptr);

	if (buf_size == 0)
	{
		qCCritical(card) << "Cannot encode elliptic curve point";
		return QByteArray();
	}
	if (buf_size > INT_MAX)
	{
		qCCritical(card) << "Cannot encode elliptic curve point";
		return QByteArray();
	}

	QByteArray oct(static_cast<int>(buf_size), 0);
	if (!EC_POINT_point2oct(pCurve.data(), pPoint, form, reinterpret_cast<uchar*>(oct.data()), buf_size, nullptr))
	{
		qCCritical(card) << "Cannot encode elliptic curve point";
		return QByteArray();
	}

	// Compression as defined in TR 03110-3 A.2.2.3 without a leading
	// byte in front of the x coordinate indicating the compression type,
	// i.e. not the typical SECG SEC1 v1.0 format used by OpenSSL.
	return pCompressed ? oct.mid(1) : oct;
}


QSharedPointer<EC_POINT> EcUtil::oct2point(const QSharedPointer<const EC_GROUP>& pCurve, const QByteArray& pCompressedData)
{
	if (!pCurve)
	{
		qCCritical(card) << "Cannot use undefined curve";
		return nullptr;
	}

	QSharedPointer<EC_POINT> point = EcUtil::create(EC_POINT_new(pCurve.data()));
	if (!EC_POINT_oct2point(pCurve.data(), point.data(), reinterpret_cast<const uchar*>(pCompressedData.constData()), static_cast<size_t>(pCompressedData.size()), nullptr))
	{
		qCCritical(card) << "Cannot decode elliptic curve point";
		return nullptr;
	}
	if (!EC_POINT_is_on_curve(pCurve.data(), point.data(), nullptr))
	{
		qCCritical(card) << "Decoded point is not on curve";
		return nullptr;
	}

	return point;
}


#if OPENSSL_VERSION_NUMBER >= 0x30000000L
QByteArray EcUtil::getEncodedPublicKey(const QSharedPointer<EVP_PKEY>& pKey, bool pCompressed)
{
	if (pKey.isNull())
	{
		qCCritical(card) << "Cannot use undefined key";
		return nullptr;
	}

	uchar* key = nullptr;
	const size_t length = EVP_PKEY_get1_encoded_public_key(pKey.data(), &key);
	const auto guard = qScopeGuard([key] {
				OPENSSL_free(key);
			});

	if (length == 0)
	{
		return QByteArray();
	}

	const QByteArray uncompressed(reinterpret_cast<char*>(key), static_cast<int>(length));
	return pCompressed ? EcUtil::compressPoint(uncompressed) : uncompressed;
}


QSharedPointer<BIGNUM> EcUtil::getPrivateKey(const QSharedPointer<const EVP_PKEY>& pKey)
{
	BIGNUM* privKey = nullptr;
	EVP_PKEY_get_bn_param(pKey.data(), "priv", &privKey);
	return EcUtil::create(privKey);
}


QSharedPointer<OSSL_PARAM> EcUtil::create(const std::function<bool(OSSL_PARAM_BLD* pBuilder)>& pFunc)
{
	OSSL_PARAM_BLD* bld = OSSL_PARAM_BLD_new();
	const auto guard = qScopeGuard([bld] {
				OSSL_PARAM_BLD_free(bld);
			});

	if (bld == nullptr)
	{
		qCCritical(card) << "Cannot create parameter builder";
		return nullptr;
	}

	if (!pFunc(bld))
	{
		qCCritical(card) << "Cannot initialize parameter builder";
		return nullptr;
	}

	if (OSSL_PARAM* params = OSSL_PARAM_BLD_to_param(bld); params != nullptr)
	{
		static auto deleter = [](OSSL_PARAM* pParam)
				{
					OSSL_PARAM_free(pParam);
				};

		return QSharedPointer<OSSL_PARAM>(params, deleter);
	}

	qCCritical(card) << "Cannot create parameter";
	return nullptr;
}


QSharedPointer<EVP_PKEY> EcUtil::generateKey(int pNid)
{
	const char* curve_name = OBJ_nid2sn(pNid);
	// https://github.com/openssl/openssl/issues/31608
	return EcUtil::create(EVP_EC_gen(const_cast<char*>(curve_name)));
}


#else
QByteArray EcUtil::getEncodedPublicKey(const QSharedPointer<EC_KEY>& pKey, bool pCompressed)
{
	if (pKey.isNull())
	{
		qCCritical(card) << "Cannot use undefined key";
		return QByteArray();
	}

	const auto& curve = EcUtil::create(EC_GROUP_dup(EC_KEY_get0_group(pKey.data())));
	return EcUtil::point2oct(curve, EC_KEY_get0_public_key(pKey.data()), pCompressed);
}


QSharedPointer<BIGNUM> EcUtil::getPrivateKey(const QSharedPointer<const EC_KEY>& pKey)
{
	if (pKey.isNull())
	{
		qCCritical(card) << "Cannot use undefined key";
		return nullptr;
	}

	return create(BN_dup(EC_KEY_get0_private_key(pKey.data())));
}


QSharedPointer<EC_KEY> EcUtil::generateKey(int pNid)
{
	auto key = EcUtil::create(EC_KEY_new_by_curve_name(pNid));
	if (!key || !EC_KEY_generate_key(key.data()))
	{
		qCCritical(card) << "Error EC_KEY_generate_key";
		return nullptr;
	}

	return key;
}


#endif
