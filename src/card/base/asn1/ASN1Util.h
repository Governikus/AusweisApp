/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Utility functions, templates and other ASN.1 related helper stuff
 */

#pragma once

#include <openssl/asn1t.h>
#include <openssl/err.h>

#include <QSharedPointer>
#include <QString>

DEFINE_STACK_OF(ASN1_OCTET_STRING)

namespace governikus
{

class Asn1Util
{
	private:
		Asn1Util() = delete;
		~Asn1Util() = delete;

	public:
		/*!
		 * Encodes the data as ASN.1 object with specified tag byte.
		 */
		static QByteArray encode(int pClass, int pTag, const QByteArray& pData, bool pConstructed = false);
};


/*!
 * Utility for OpenSSL type ASN1_OCTET_STRING, i.e. ASN.1 type OCTET STRING
 */
class Asn1OctetStringUtil
{
	private:
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
	private:
		Asn1StringUtil() = delete;
		~Asn1StringUtil() = delete;

	public:
		static void setValue(const QString& pString, ASN1_STRING* pOut);
		static QString getValue(const ASN1_STRING* pString);
};


/*!
 * Utility for OpenSSL type ASN1_TYPE, i.e. ASN.1 type ANY
 */
class Asn1TypeUtil
{
	private:
		Asn1TypeUtil() = delete;
		~Asn1TypeUtil() = delete;

	public:
#if OPENSSL_VERSION_NUMBER < 0x30000000L
		static QByteArray encode(ASN1_TYPE* pAny);
#else
		static QByteArray encode(const ASN1_TYPE* pAny);
#endif
};


/*!
 * Utility for OpenSSL type ASN1_INTEGER, i.e. ASN.1 type INTEGER
 */
class Asn1IntegerUtil
{
	private:
		Asn1IntegerUtil() = delete;
		~Asn1IntegerUtil() = delete;

	public:
		[[nodiscard]] static int getValue(const ASN1_INTEGER* pInteger);
		[[nodiscard]] static QByteArray encode(int pValue);
};


/*!
 * Utility for BCD coded dates
 */
class Asn1BCDDateUtil
{
	private:
		Asn1BCDDateUtil() = delete;
		~Asn1BCDDateUtil() = delete;

	public:
		static QByteArray convertFromQDateToUnpackedBCD(QDate date);
		static QDate convertFromUnpackedBCDToQDate(const ASN1_OCTET_STRING* pDateBCD);
};


} // namespace governikus
