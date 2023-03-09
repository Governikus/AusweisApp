/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "EcUtil.h"

#include <QLoggingCategory>
#include <QScopeGuard>

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	#include <openssl/param_build.h>
#endif

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
QByteArray EcUtil::getEncodedPublicKey(const QSharedPointer<EVP_PKEY>& pKey)
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

	return length > 0 ? QByteArray(reinterpret_cast<char*>(key), static_cast<int>(length)) : QByteArray();
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

	if (OSSL_PARAM* params = nullptr;
			pFunc(bld) && (params = OSSL_PARAM_BLD_to_param(bld)) != nullptr)
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


QSharedPointer<EVP_PKEY> EcUtil::generateKey(const QSharedPointer<const EC_GROUP>& pCurve)
{
	if (pCurve.isNull())
	{
		qCCritical(card) << "Curve is undefined";
		return nullptr;
	}

	auto generator = EcUtil::point2oct(pCurve, EC_GROUP_get0_generator(pCurve.data()));

	auto order = EcUtil::create(BN_new());
	if (!EC_GROUP_get_order(pCurve.data(), order.data(), nullptr))
	{
		qCCritical(card) << "Cannot fetch order";
		return nullptr;
	}

	auto cofactor = EcUtil::create(BN_new());
	if (!EC_GROUP_get_cofactor(pCurve.data(), cofactor.data(), nullptr))
	{
		qCCritical(card) << "Cannot fetch cofactor";
		return nullptr;
	}

	auto p = EcUtil::create(BN_new());
	auto a = EcUtil::create(BN_new());
	auto b = EcUtil::create(BN_new());
	if (!EC_GROUP_get_curve(pCurve.data(), p.data(), a.data(), b.data(), nullptr))
	{
		qCCritical(card) << "Cannot fetch a, b or p";
		return nullptr;
	}

	const auto& params = EcUtil::create([&p, &a, &b, &order, &cofactor, &generator](OSSL_PARAM_BLD* pBuilder){
			return OSSL_PARAM_BLD_push_BN(pBuilder, "p", p.data())
				   && OSSL_PARAM_BLD_push_BN(pBuilder, "a", a.data())
				   && OSSL_PARAM_BLD_push_BN(pBuilder, "b", b.data())
				   && OSSL_PARAM_BLD_push_BN(pBuilder, "order", order.data())
				   && OSSL_PARAM_BLD_push_BN(pBuilder, "cofactor", cofactor.data())
				   && OSSL_PARAM_BLD_push_octet_string(pBuilder, "generator", generator.data(), static_cast<size_t>(generator.size()))
				   && OSSL_PARAM_BLD_push_utf8_string(pBuilder, "field-type", "prime-field", 12);
		});

	if (params == nullptr)
	{
		qCCritical(card) << "Cannot set parameter";
		return nullptr;
	}

	auto ctx = EcUtil::create(EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr));
	if (!ctx)
	{
		qCCritical(card) << "Cannot create EVP_PKEY_CTX";
		return nullptr;
	}
	EVP_PKEY_keygen_init(ctx.data());

	if (!EVP_PKEY_CTX_set_params(ctx.data(), params.data()))
	{
		qCCritical(card) << "Cannot set params to EVP_PKEY_CTX";
		return nullptr;
	}

	EVP_PKEY* key = nullptr;
	if (!EVP_PKEY_generate(ctx.data(), &key))
	{
		qCCritical(card) << "Cannot create EVP_PKEY";
		return nullptr;
	}

	return EcUtil::create(key);
}


#else
QSharedPointer<EC_KEY> EcUtil::generateKey(const QSharedPointer<const EC_GROUP>& pCurve)
{
	if (pCurve.isNull())
	{
		qCCritical(card) << "Curve is undefined";
		return nullptr;
	}

	auto key = EcUtil::create(EC_KEY_new());
	if (!EC_KEY_set_group(key.data(), pCurve.data()))
	{
		qCCritical(card) << "Error EC_KEY_set_group";
		return nullptr;
	}

	if (!EC_KEY_generate_key(key.data()))
	{
		qCCritical(card) << "Error EC_KEY_generate_key";
		return nullptr;
	}

	return key;
}


#endif
