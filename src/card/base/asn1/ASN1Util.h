/*!
 * \brief Utility functions, templates and other ASN.1 related helper stuff
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <openssl/asn1t.h>
#include <openssl/err.h>

#include <QSharedPointer>
#include <QString>


/*!
 * OpenSSL type declarations
 */
#if OPENSSL_VERSION_NUMBER < 0x10100000L
DECLARE_STACK_OF(ASN1_OCTET_STRING)
#else
DEFINE_STACK_OF(ASN1_OCTET_STRING)
#endif

namespace governikus
{


class Asn1Util
{
	Asn1Util() = delete;
	~Asn1Util() = delete;

	public:
		/*!
		 * Encodes the data as ASN.1 object with specified tag byte.
		 */
		static QByteArray encode(char pTagByte, const QByteArray& pData);
};


/*!
 * Utility for OpenSSL type ASN1_OBJECT, i.e. ASN.1 type OBJECT_IDENTIFIER
 */
class Asn1ObjectUtil
{
	Asn1ObjectUtil() = delete;
	~Asn1ObjectUtil() = delete;

	public:
		static ASN1_OBJECT* parseFrom(const QByteArray& pOidAsText);
		static QByteArray convertTo(const ASN1_OBJECT* pAsn1Object);
		static QByteArray getValue(const ASN1_OBJECT* pAsn1Object);
};


/*!
 * Utility for OpenSSL type ASN1_OCTET_STRING, i.e. ASN.1 type OCTET STRING
 */
class Asn1OctetStringUtil
{
	Asn1OctetStringUtil() = delete;
	~Asn1OctetStringUtil() = delete;

	public:
		static void setValue(const QByteArray& pValue, ASN1_OCTET_STRING* pAsn1OctetString);
		static QByteArray getValue(ASN1_OCTET_STRING* pAsn1OctetString);
};


/*!
 * Utility for OpenSSL type ASN1_STRING, i.e. ASN.1 string types
 */
class Asn1StringUtil
{
	Asn1StringUtil() = delete;
	~Asn1StringUtil() = delete;

	public:
		static void setValue(const QString& pString, ASN1_STRING* pOut);
		static QString getValue(ASN1_STRING* pString);
};


/*!
 * Utility for OpenSSL type ASN1_TYPE, i.e. ASN.1 type ANY
 */
class Asn1TypeUtil
{
	Asn1TypeUtil() = delete;
	~Asn1TypeUtil() = delete;

	public:
		static QByteArray encode(ASN1_TYPE* pAny);
};


/*!
 * Utility for OpenSSL type ASN1_INTEGER, i.e. ASN.1 type INTEGER
 */
class Asn1IntegerUtil
{
	Asn1IntegerUtil() = delete;
	~Asn1IntegerUtil() = delete;

	public:
		static QByteArray getValue(const ASN1_INTEGER* pInteger);
};


/*!
 * Utility for BCD coded dates
 */
class Asn1BCDDateUtil
{
	Asn1BCDDateUtil() = delete;
	~Asn1BCDDateUtil() = delete;

	public:
		static QByteArray convertFromQDateToUnpackedBCD(QDate date);
		static QDate convertFromUnpackedBCDToQDate(ASN1_OCTET_STRING* pDateBCD);
};


}
