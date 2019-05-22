/*!
 * \brief Elliptic curve utility.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QLoggingCategory>
#include <QSharedPointer>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

class EcUtil
{
	public:
		static QByteArray point2oct(const QSharedPointer<const EC_GROUP>& pCurve, const EC_POINT* pPoint);

		static QSharedPointer<EC_POINT> oct2point(const QSharedPointer<const EC_GROUP>& pCurve, const QByteArray& pCompressedData);

		static QSharedPointer<EC_GROUP> create(EC_GROUP* pEcGroup);

		static QSharedPointer<EC_KEY> create(EC_KEY* pEcKey);

		static QSharedPointer<EC_POINT> create(EC_POINT* pEcPoint);

		static QSharedPointer<BIGNUM> create(BIGNUM* pBigNum);

		static QSharedPointer<ECDSA_SIG> create(ECDSA_SIG* pEcdsaSignature);

};

inline QByteArray EcUtil::point2oct(const QSharedPointer<const EC_GROUP>& pCurve, const EC_POINT* pPoint)
{
	if (pCurve.isNull() || pPoint == nullptr)
	{
		qCCritical(card) << "Invalid input data, cannot encode elliptic curve point";
		return QByteArray();
	}

	size_t buf_size = EC_POINT_point2oct(pCurve.data(), pPoint, POINT_CONVERSION_UNCOMPRESSED, nullptr, 0, nullptr);

	if (buf_size == 0)
	{
		qCCritical(card) << "Cannot encode elliptic curve point";
		Q_ASSERT(buf_size != 0);
		return QByteArray();
	}
	if (buf_size > INT_MAX)
	{
		qCCritical(card) << "Cannot encode elliptic curve point";
		Q_ASSERT(buf_size <= INT_MAX);
		return QByteArray();
	}

	QVector<uchar> buf(static_cast<int>(buf_size));
	if (!EC_POINT_point2oct(pCurve.data(), pPoint, POINT_CONVERSION_UNCOMPRESSED, buf.data(), buf_size, nullptr))
	{
		qCCritical(card) << "Cannot encode elliptic curve point";
		return QByteArray();
	}

	QByteArray uncompressed(reinterpret_cast<char*>(buf.data()), static_cast<int>(buf_size));
	return uncompressed;
}


inline QSharedPointer<EC_POINT> EcUtil::oct2point(const QSharedPointer<const EC_GROUP>& pCurve, const QByteArray& pCompressedData)
{
	QSharedPointer<EC_POINT> point = EcUtil::create(EC_POINT_new(pCurve.data()));
	if (!EC_POINT_oct2point(pCurve.data(), point.data(), reinterpret_cast<const uchar*>(pCompressedData.constData()), static_cast<size_t>(pCompressedData.size()), nullptr))
	{
		qCCritical(card) << "Cannot decode elliptic curve point";
		return QSharedPointer<EC_POINT>();
	}
	if (!EC_POINT_is_on_curve(pCurve.data(), point.data(), nullptr))
	{
		qCCritical(card) << "Decoded point is not on curve";
		return QSharedPointer<EC_POINT>();
	}
	return point;
}


inline QSharedPointer<EC_GROUP> EcUtil::create(EC_GROUP* pEcGroup)
{
	static auto deleter = [](EC_GROUP* ecCurve)
			{
				EC_GROUP_clear_free(ecCurve);
			};

	return QSharedPointer<EC_GROUP>(pEcGroup, deleter);
}


inline QSharedPointer<EC_KEY> EcUtil::create(EC_KEY* pEcKey)
{
	static auto deleter = [](EC_KEY* ecKey)
			{
				EC_KEY_free(ecKey);
			};

	return QSharedPointer<EC_KEY>(pEcKey, deleter);
}


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


inline QSharedPointer<ECDSA_SIG> EcUtil::create(ECDSA_SIG* pEcdsaSignature)
{
	static auto deleter = [](ECDSA_SIG* ecdsaSignature)
			{
				ECDSA_SIG_free(ecdsaSignature);
			};

	return QSharedPointer<ECDSA_SIG>(pEcdsaSignature, deleter);
}


} // namespace governikus
