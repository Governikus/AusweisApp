/*
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ASN1Util.h"

#include "SecureMessagingResponse.h"

#include <openssl/x509v3.h>
#include <QDate>
#include <QDebug>

using namespace governikus;


ASN1_OBJECT* Asn1ObjectUtil::parseFrom(const QByteArray& pOidAsText)
{
	return OBJ_txt2obj(pOidAsText.constData(), 1);
}


QByteArray Asn1ObjectUtil::convertTo(const ASN1_OBJECT* pAsn1Object)
{
	if (pAsn1Object == nullptr)
	{
		return QByteArray();
	}

	/*
	 * According to OpenSSL's documentation on OBJ_nid2obj:
	 * " A buffer length of 80 should be more than enough to handle any OID encountered in practice."
	 */
	char buf[80] = {};
	if (OBJ_obj2txt(buf, sizeof(buf), pAsn1Object, 1) == sizeof(buf))
	{
		qCritical() << "The OID may not fit into the given array, just return an empty string";
		return QByteArray();
	}
	return QByteArray(buf);
}


QByteArray Asn1ObjectUtil::getValue(const ASN1_OBJECT* pAsn1Object)
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	return QByteArray(reinterpret_cast<const char*>(pAsn1Object->data), pAsn1Object->length);

#else
	const size_t len = OBJ_length(pAsn1Object);
	const unsigned char* data = OBJ_get0_data(pAsn1Object);
	return QByteArray(reinterpret_cast<const char*>(data), static_cast<int>(len));

#endif
}


void Asn1OctetStringUtil::setValue(const QByteArray& pValue, ASN1_OCTET_STRING* pAsn1OctetString)
{
	ASN1_OCTET_STRING_set(pAsn1OctetString, reinterpret_cast<unsigned const char*>(pValue.data()), pValue.length());
}


QByteArray Asn1OctetStringUtil::getValue(ASN1_OCTET_STRING* pAsn1OctetString)
{
	if (pAsn1OctetString == nullptr)
	{
		return QByteArray();
	}

	return QByteArray(reinterpret_cast<char*>(pAsn1OctetString->data), pAsn1OctetString->length);
}


void Asn1StringUtil::setValue(const QString& pString, ASN1_STRING* pOut)
{
	QByteArray bytes = pString.toUtf8();
	ASN1_STRING_set(pOut, bytes.data(), bytes.length());
}


QString Asn1StringUtil::getValue(ASN1_STRING* pString)
{
	if (pString == nullptr)
	{
		return QString();
	}

	unsigned char* buf;
	int buf_len = ASN1_STRING_to_UTF8(&buf, pString);

	QString result;
	if (buf_len > 0)
	{
		result = QString::fromUtf8(reinterpret_cast<char*>(buf), buf_len);
		OPENSSL_free(buf);
	}

	return result;
}


QByteArray Asn1TypeUtil::encode(ASN1_TYPE* pAny)
{
	if (pAny == nullptr)
	{
		return QByteArray();
	}

	unsigned char* buf = nullptr;
	int buf_len = i2d_ASN1_TYPE(pAny, &buf);

	QByteArray result;
	if (buf_len > 0)
	{
		result = QByteArray(reinterpret_cast<char*>(buf), buf_len);
		OPENSSL_free(buf);
	}

	return result;
}


QByteArray Asn1IntegerUtil::getValue(const ASN1_INTEGER* pInteger)
{
	if (pInteger == nullptr)
	{
		return QByteArray();
	}
	return QByteArray(reinterpret_cast<const char*>(pInteger->data), pInteger->length);
}


QByteArray Asn1BCDDateUtil::convertFromQDateToUnpackedBCD(QDate pDate)
{
	QByteArray aBCD = pDate.toString(QStringLiteral("yyMMdd")).toLocal8Bit();

	if (aBCD.length() != 6)
	{
		qCritical() << "Invalid date length.";
		return QByteArray();
	}

	// convert to unpacked BCD digits
	for (int i = 0; i <= 5; i++)
	{
		aBCD[i] = static_cast<char>(aBCD[i] - 0x30);
	}

	return aBCD;
}


QDate Asn1BCDDateUtil::convertFromUnpackedBCDToQDate(ASN1_OCTET_STRING* pDateBCD)
{
	if (pDateBCD == nullptr)
	{
		qCritical() << "Date pointer null.";
		return QDate();
	}

	if (pDateBCD->length != 6)
	{
		qCritical() << "Invalid date length.";
		return QDate();
	}

	int year = 2000 + pDateBCD->data[0] * 10 + pDateBCD->data[1];
	int month = pDateBCD->data[2] * 10 + pDateBCD->data[3];
	int day = pDateBCD->data[4] * 10 + pDateBCD->data[5];

	return QDate(year, month, day);

}


QByteArray Asn1Util::encode(char pTagByte, const QByteArray& pData)
{
	// 1. encode as ASN1_OCTET_STRING using our template utils
	// (in fact SM_CHECKSUM ::= [8E] IMPLICIT OCTET STRING)
	auto octetString = newObject<SM_CHECKSUM>();
	Asn1OctetStringUtil::setValue(pData, octetString.data());
	auto encodedOctetString = encodeObject(octetString.data());

	// 2. replace the tag byte
	return encodedOctetString.replace(0, 1, QByteArray(1, pTagByte));
}
