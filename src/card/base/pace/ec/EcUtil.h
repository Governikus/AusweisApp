/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QSharedPointer>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>

#include <functional>


namespace governikus
{

class EcUtil
{
	public:
		static QByteArray compressPoint(const QByteArray& pPoint);
		static QByteArray point2oct(const QSharedPointer<const EC_GROUP>& pCurve, const EC_POINT* pPoint, bool pCompressed = false);
		static QSharedPointer<EC_POINT> oct2point(const QSharedPointer<const EC_GROUP>& pCurve, const QByteArray& pCompressedData);

		static QSharedPointer<EC_GROUP> create(EC_GROUP* pEcGroup);
#if OPENSSL_VERSION_NUMBER < 0x30000000L
		static QSharedPointer<EC_KEY> create(EC_KEY* pEcKey);
#endif
		static QSharedPointer<EC_POINT> create(EC_POINT* pEcPoint);
		static QSharedPointer<BIGNUM> create(BIGNUM* pBigNum);
		static QSharedPointer<EVP_PKEY> create(EVP_PKEY* pEcGroup);
		static QSharedPointer<EVP_PKEY_CTX> create(EVP_PKEY_CTX* pEcGroup);

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
		static QByteArray getEncodedPublicKey(const QSharedPointer<EVP_PKEY>& pKey, bool pCompressed = false);
		static QSharedPointer<BIGNUM> getPrivateKey(const QSharedPointer<const EVP_PKEY>& pKey);
		static QSharedPointer<OSSL_PARAM> create(const std::function<bool(OSSL_PARAM_BLD* pBuilder)>& pFunc);
		static QSharedPointer<EVP_PKEY> generateKey(const QSharedPointer<const EC_GROUP>& pCurve);
#else
		static QByteArray getEncodedPublicKey(const QSharedPointer<EC_KEY>& pKey, bool pCompressed = false);
		static QSharedPointer<BIGNUM> getPrivateKey(const QSharedPointer<const EC_KEY>& pKey);
		static QSharedPointer<EC_KEY> generateKey(const QSharedPointer<const EC_GROUP>& pCurve);
#endif

		static QSharedPointer<EC_GROUP> createCurve(int pNid);
};


inline QSharedPointer<EC_GROUP> EcUtil::create(EC_GROUP* pEcGroup)
{
	static auto deleter = [](EC_GROUP* ecCurve)
			{
				EC_GROUP_free(ecCurve);
			};

	return QSharedPointer<EC_GROUP>(pEcGroup, deleter);
}


#if OPENSSL_VERSION_NUMBER < 0x30000000L
inline QSharedPointer<EC_KEY> EcUtil::create(EC_KEY* pEcKey)
{
	static auto deleter = [](EC_KEY* ecKey)
			{
				EC_KEY_free(ecKey);
			};

	return QSharedPointer<EC_KEY>(pEcKey, deleter);
}


#endif

inline QSharedPointer<EC_POINT> EcUtil::create(EC_POINT* pEcPoint)
{
	static auto deleter = [](EC_POINT* ecPoint)
			{
				EC_POINT_clear_free(ecPoint);
			};

	return QSharedPointer<EC_POINT>(pEcPoint, deleter);
}


inline QSharedPointer<BIGNUM> EcUtil::create(BIGNUM* pBigNum)
{
	static auto deleter = [](BIGNUM* bigNum)
			{
				BN_clear_free(bigNum);
			};

	return QSharedPointer<BIGNUM>(pBigNum, deleter);
}


inline QSharedPointer<EVP_PKEY> EcUtil::create(EVP_PKEY* pKey)
{
	static auto deleter = [](EVP_PKEY* key)
			{
				EVP_PKEY_free(key);
			};

	return QSharedPointer<EVP_PKEY>(pKey, deleter);
}


inline QSharedPointer<EVP_PKEY_CTX> EcUtil::create(EVP_PKEY_CTX* pCtx)
{
	static auto deleter = [](EVP_PKEY_CTX* ctx)
			{
				EVP_PKEY_CTX_free(ctx);
			};

	return QSharedPointer<EVP_PKEY_CTX>(pCtx, deleter);
}


} // namespace governikus
