/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "asn1/ASN1Util.h"

#include <QDate>
#include <QLoggingCategory>
#include <QScopeGuard>
#include <openssl/asn1.h>
#include <openssl/x509v3.h>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;

static_assert(std::is_standard_layout_v<Asn1OctetStringUtil>, "virtual method is not allowed because of OpenSSL parsing and reinterpret_cast!");

bool Asn1OctetStringUtil::setValue(const QByteArray& pValue, ASN1_OCTET_STRING* pAsn1OctetString)
{
	return ASN1_OCTET_STRING_set(pAsn1OctetString, reinterpret_cast<unsigned const char*>(pValue.data()), static_cast<int>(pValue.length())) == 1;
}


QByteArray Asn1OctetStringUtil::getValue(const ASN1_OCTET_STRING* pAsn1OctetString)
{
	if (pAsn1OctetString == nullptr)
	{
		return QByteArray();
	}

	return QByteArray(reinterpret_cast<const char*>(ASN1_STRING_get0_data(pAsn1OctetString)), ASN1_STRING_length(pAsn1OctetString));
}


QByteArray Asn1OctetStringUtil::getValue(const Asn1OctetStringUtil* pString)
{
	return getValue(reinterpret_cast<const ASN1_OCTET_STRING*>(pString));
}


bool Asn1OctetStringUtil::setValue(const QByteArray& pValue)
{
	return setValue(pValue, reinterpret_cast<ASN1_OCTET_STRING*>(this));
}


int Asn1OctetStringUtil::getLength() const
{
	return ASN1_STRING_length(reinterpret_cast<const ASN1_OCTET_STRING*>(this));
}


void Asn1StringUtil::setValue(const QString& pString, ASN1_STRING* pOut)
{
	QByteArray bytes = pString.toUtf8();
	ASN1_STRING_set(pOut, bytes.data(), static_cast<int>(bytes.length()));
}


QString Asn1StringUtil::getValue(const ASN1_STRING* pString)
{
	if (pString == nullptr)
	{
		return QString();
	}

	uchar* buffer = nullptr;
	const int length = ASN1_STRING_to_UTF8(&buffer, pString);
	const auto guard = qScopeGuard([buffer] {
				OPENSSL_free(buffer);
			});

	if (length < 0)
	{
		return QString();
	}

	return QString::fromUtf8(reinterpret_cast<char*>(buffer), length);
}


#if OPENSSL_VERSION_NUMBER < 0x30000000L
QByteArray Asn1TypeUtil::encode(ASN1_TYPE* pAny)
#else
QByteArray Asn1TypeUtil::encode(const ASN1_TYPE* pAny)
#endif
{
	if (pAny == nullptr)
	{
		return QByteArray();
	}

	uchar* buffer = nullptr;
	const int length = i2d_ASN1_TYPE(pAny, &buffer);
	const auto guard = qScopeGuard([buffer] {
				OPENSSL_free(buffer);
			});

	if (length < 0)
	{
		return QByteArray();
	}

	return QByteArray(reinterpret_cast<char*>(buffer), length);
}


int Asn1IntegerUtil::getValue(const ASN1_INTEGER* pInteger)
{
	if (pInteger)
	{
		if (int64_t version = 0;
				ASN1_INTEGER_get_int64(&version, pInteger))
		{
			return static_cast<int>(version);
		}

		qCCritical(card) << "Conversion error on ASN1_INTEGER";
	}

	return -1;
}


QByteArray Asn1BCDDateUtil::convertFromQDateToUnpackedBCD(QDate pDate)
{
	QByteArray aBCD = pDate.toString(QStringLiteral("yyMMdd")).toLocal8Bit();

	if (aBCD.length() != 6)
	{
		qCCritical(card) << "Invalid date length.";
		return QByteArray();
	}

	// convert to unpacked BCD digits
	for (int i = 0; i <= 5; i++)
	{
		aBCD[i] = static_cast<char>(aBCD[i] - 0x30);
	}

	return aBCD;
}


QDate Asn1BCDDateUtil::convertFromUnpackedBCDToQDate(const ASN1_OCTET_STRING* pDateBCD)
{
	if (pDateBCD == nullptr)
	{
		qCCritical(card) << "Date pointer null.";
		return QDate();
	}

	if (ASN1_STRING_length(pDateBCD) != 6)
	{
		qCCritical(card) << "Invalid date length.";
		return QDate();
	}

	const uchar* data = ASN1_STRING_get0_data(pDateBCD);
	int year = 2000 + data[0] * 10 + data[1];
	int month = data[2] * 10 + data[3];
	int day = data[4] * 10 + data[5];

	return QDate(year, month, day);
}


QByteArray Asn1Util::encode(int pClass, int pTag, const QByteArray& pData, bool pConstructed)
{
	const int constructed = pConstructed ? V_ASN1_CONSTRUCTED : 0;
	const int size = ASN1_object_size(constructed, static_cast<int>(pData.length()), pTag);
	QByteArray result(size, 0);

	auto* p = reinterpret_cast<uchar*>(result.data());
	ASN1_put_object(&p, constructed, static_cast<int>(pData.length()), pTag, pClass);
	Q_ASSERT(reinterpret_cast<uchar*>(result.data()) + result.length() == p + pData.length());
	memcpy(p, pData.data(), static_cast<size_t>(pData.length()));

	return result;
}
