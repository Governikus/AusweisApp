/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ASN1TemplateUtil.h"

#include <QByteArray>
#include <QSharedPointer>

#include <openssl/asn1t.h>
#include <openssl/ec.h>


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
	EC_KEY* mEcKey;

	static QSharedPointer<ecdsapublickey_st> fromHex(const QByteArray& pHexValue);
	static QSharedPointer<ecdsapublickey_st> decode(const QByteArray& pBytes);
	QByteArray encode();

	QByteArray getPublicKeyOid() const;

	/**
	 * Returns the raw bytes of the OID value, i.e. not the tag and not the length structure, but the value bytes.
	 */
	QByteArray getPublicKeyOidValueBytes() const;
	QByteArray getUncompressedPublicPoint() const;
	const EC_KEY* getEcKey() const;

	private:
		void initEcKey();

	public:
		static int decodeCallback(int pOperation, ASN1_VALUE** pVal, const ASN1_ITEM* pIt, void* pExarg);
};


DECLARE_ASN1_FUNCTIONS(EcdsaPublicKey)
DECLARE_ASN1_OBJECT(EcdsaPublicKey)

} // namespace governikus
