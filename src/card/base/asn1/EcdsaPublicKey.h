/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ASN1TemplateUtil.h"
#include "SecurityProtocol.h"

#include <QByteArray>
#include <QSharedPointer>

#include <openssl/asn1t.h>
#include <openssl/evp.h>

#if OPENSSL_VERSION_NUMBER < 0x30000000L
	#include <openssl/ec.h>
#endif


namespace governikus
{

/*!
 * According to TR-03110-3 chapter D
 * elliptic curve public keys are defined as follows:
 *
 * EcdsaPublicKey ::= [APPLICATION 0x49] IMPLICIT SEQUENCE {
 *         ObjectIdentifier     OBJECT IDENTIFIER,
 *         PrimeModulus         [0x01] IMPLICIT UnsignedInteger OPTIONAL
 *         FirstCoefficient     [0x02] IMPLICIT UnsignedInteger OPTIONAL
 *         SecondCoefficient    [0x03] IMPLICIT UnsignedInteger OPTIONAL
 *         BasePoint            [0x04] IMPLICIT UncompressedEllipticCurvePoint OPTIONAL
 *         OrderOfTheBasePoint  [0x05] IMPLICIT UnsignedInteger OPTIONAL
 *         PublicPoint          [0x06] IMPLICIT UncompressedEllipticCurvePoint
 *         Cofactor             [0x07] IMPLICIT UnsignedInteger OPTIONAL
 * }
 *
 * UncompressedEllipticCurvePoint ::= OCTET STRING -- see TR-03111
 *
 * UnsignedInteger -- see TR-03110 D.2.1.1
 *
 */
using EcdsaPublicKey = struct ecdsapublickey_st
{
	ASN1_OBJECT* mObjectIdentifier;
	ASN1_OCTET_STRING* mPrimeModulus;
	ASN1_OCTET_STRING* mFirstCoefficient;
	ASN1_OCTET_STRING* mSecondCoefficient;
	ASN1_OCTET_STRING* mBasePoint;
	ASN1_OCTET_STRING* mOrderOfTheBasePoint;
	ASN1_OCTET_STRING* mPublicPoint;
	ASN1_OCTET_STRING* mCofactor;

	static QSharedPointer<ecdsapublickey_st> fromHex(const QByteArray& pHexValue);
	static QSharedPointer<ecdsapublickey_st> decode(const QByteArray& pBytes);
	QByteArray encode();

	[[nodiscard]] bool isComplete() const;
	[[nodiscard]] SecurityProtocol getSecurityProtocol() const;

	/**
	 * Returns the raw bytes of the OID value, i.e. not the tag and not the length structure, but the value bytes.
	 */
	[[nodiscard]] Oid getOid() const;
	[[nodiscard]] QByteArray getUncompressedPublicPoint() const;

	/**
	 * Create signing key using the given public point and the current curve parameters.
	 *
	 * \param pPublicPoint Another public key
	 * \returns A new EVP_PKEY with given public key.
	 */
	[[nodiscard]] QSharedPointer<EVP_PKEY> createKey(const QByteArray& pPublicPoint) const;

#ifndef QT_NO_DEBUG

	/**
	 * Create signing key using current public point and the current curve parameters.
	 *
	 * \returns A new EVP_PKEY with current public key.
	 */
	[[nodiscard]] QSharedPointer<EVP_PKEY> createKey() const;
#endif

	private:
		struct CurveData
		{
			QSharedPointer<BIGNUM> p;
			QSharedPointer<BIGNUM> a;
			QSharedPointer<BIGNUM> b;
			QSharedPointer<BIGNUM> order;
			QSharedPointer<BIGNUM> cofactor;

			[[nodiscard]] bool isValid() const
			{
				return !p.isNull() && !a.isNull() && !b.isNull() && !order.isNull();
			}


		};

		[[nodiscard]] static bool isAllValid(const ecdsapublickey_st* pKey);
		[[nodiscard]] static bool isAllInvalid(const ecdsapublickey_st* pKey);

		[[nodiscard]] CurveData createCurveData() const;
		[[nodiscard]] QSharedPointer<EVP_PKEY> createKey(const uchar* pPublicPoint, int pPublicPointLength) const;
#if OPENSSL_VERSION_NUMBER < 0x30000000L
		[[nodiscard]] QSharedPointer<EC_GROUP> createGroup(const CurveData& pData) const;
#endif

	public:
		static int decodeCallback(int pOperation, ASN1_VALUE** pVal, const ASN1_ITEM* pIt, void* pExarg);
};


DECLARE_ASN1_FUNCTIONS(EcdsaPublicKey)
DECLARE_ASN1_OBJECT(EcdsaPublicKey)

} // namespace governikus
