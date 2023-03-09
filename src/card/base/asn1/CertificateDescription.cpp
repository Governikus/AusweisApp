/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "CertificateDescription.h"

#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>


using namespace governikus;


namespace
{
QStringList takeWhileNonEmpty(const QStringList& lines)
{
	QStringList result;
	for (const auto& line : lines)
	{
		const QString trimmedLine = line.trimmed();
		if (trimmedLine.isEmpty())
		{
			return result;
		}
		else
		{
			result += trimmedLine;
		}
	}

	return result;
}


QString getField(const QString& pData, const QStringList& pSearchItems)
{
	const QLatin1Char NEW_LINE('\n');
	const QRegularExpression REGEX_EMPTY_SECTION(QStringLiteral("^\\R{2,}"));

	for (const auto& item : pSearchItems)
	{
		const auto pos = pData.indexOf(item);
		if (pos != -1)
		{
			const QString rest = pData.mid(pos + item.length());
			if (REGEX_EMPTY_SECTION.match(rest).hasMatch())
			{
				continue;
			}

			const QStringList lines = takeWhileNonEmpty(rest.trimmed().split(NEW_LINE));
			if (lines.isEmpty())
			{
				continue;
			}

			return lines.join(NEW_LINE);
		}
	}

	return QString();
}


} // namespace


namespace governikus
{

ASN1_SEQUENCE(CertificateDescription) = {
	ASN1_SIMPLE(CertificateDescription, mDescriptionType, ASN1_OBJECT),
	ASN1_EXP(CertificateDescription, mIssuerName, ASN1_UTF8STRING, 1),
	ASN1_EXP_OPT(CertificateDescription, mIssuerURL, ASN1_PRINTABLESTRING, 2),
	ASN1_EXP(CertificateDescription, mSubjectName, ASN1_UTF8STRING, 3),
	ASN1_EXP_OPT(CertificateDescription, mSubjectURL, ASN1_PRINTABLESTRING, 4),
	ASN1_EXP(CertificateDescription, mTermsOfUsage, ASN1_ANY, 5),
	ASN1_EXP_OPT(CertificateDescription, mRedirectURL, ASN1_PRINTABLESTRING, 6),
	ASN1_EXP_SET_OF_OPT(CertificateDescription, mCommCertificates, ASN1_OCTET_STRING, 7),
}


ASN1_SEQUENCE_END(CertificateDescription)

IMPLEMENT_ASN1_FUNCTIONS(CertificateDescription)
IMPLEMENT_ASN1_OBJECT(CertificateDescription)

} // namespace governikus


QSharedPointer<const CertificateDescription> CertificateDescription::fromHex(const QByteArray& pHexValue)
{
	return decode(QByteArray::fromHex(pHexValue));
}


QSharedPointer<const CertificateDescription> CertificateDescription::decode(const QByteArray& pBytes)
{
	return decodeObject<CertificateDescription>(pBytes);
}


QByteArray CertificateDescription::encode()
{
	return encodeObject(this);
}


Oid CertificateDescription::getDescriptionType() const
{
	return Oid(mDescriptionType);
}


QString CertificateDescription::getIssuerName() const
{
	return Asn1StringUtil::getValue(mIssuerName);
}


QString CertificateDescription::getIssuerUrl() const
{
	return Asn1StringUtil::getValue(mIssuerURL);
}


QString CertificateDescription::getSubjectName() const
{
	return Asn1StringUtil::getValue(mSubjectName);
}


QString CertificateDescription::getSubjectUrl() const
{
	return Asn1StringUtil::getValue(mSubjectURL);
}


CertificateDescription::TermsOfUsageType CertificateDescription::getTermsOfUsageType() const
{
	if (getDescriptionType() == KnownOid::ID_PLAIN_FORMAT)
	{
		return TermsOfUsageType::PLAIN_TEXT;
	}
	if (getDescriptionType() == KnownOid::ID_HTML_FORMAT)
	{
		return TermsOfUsageType::HTML;
	}
	return TermsOfUsageType::PDF;
}


QString CertificateDescription::getTermsOfUsage() const
{
	QString string;
	if (mTermsOfUsage->type == V_ASN1_UTF8STRING)
	{
		string = Asn1StringUtil::getValue(mTermsOfUsage->value.utf8string);
	}
	else if (mTermsOfUsage->type == V_ASN1_IA5STRING)
	{
		string = Asn1StringUtil::getValue(mTermsOfUsage->value.ia5string);
	}
	else if (mTermsOfUsage->type == V_ASN1_OCTET_STRING && mTermsOfUsage->value.octet_string != nullptr)
	{
		string = QString::fromLatin1(Asn1TypeUtil::encode(mTermsOfUsage).toHex().toUpper());
	}

	return string;
}


QString CertificateDescription::getRedirectUrl() const
{
	return Asn1StringUtil::getValue(mRedirectURL);
}


QSet<QString> CertificateDescription::getCommCertificates() const
{
	QSet<QString> commCerts;
	if (mCommCertificates != nullptr)
	{
		const auto size = sk_ASN1_OCTET_STRING_num(mCommCertificates);
		commCerts.reserve(size);
		for (int i = 0; i < size; i++)
		{
			const ASN1_OCTET_STRING* octetString = sk_ASN1_OCTET_STRING_value(mCommCertificates, i);
			QByteArray byteBuf(reinterpret_cast<char*>(octetString->data), octetString->length);
			commCerts += QString::fromLatin1(byteBuf.toHex().toUpper());
		}
	}
	return commCerts;
}


QString CertificateDescription::getServiceProviderAddress() const
{
	static const QStringList SEARCH_ITEMS({
				QStringLiteral("Name, Anschrift und E-Mail-Adresse des Diensteanbieters:")
			});

	return getField(getTermsOfUsage(), SEARCH_ITEMS);
}


QString CertificateDescription::getPurpose() const
{
	static const QStringList SEARCH_ITEMS({
				QStringLiteral("Gesch\u00E4ftszweck:"),
				QStringLiteral("Zweck des Auslesevorgangs:"),
				QStringLiteral("Verwendung der Daten:"),
				QStringLiteral("Zweck der Daten\u00FCbermittlung:")
			});

	return getField(getTermsOfUsage(), SEARCH_ITEMS);
}


QString CertificateDescription::getDataSecurityOfficer() const
{
	static const QStringList SEARCH_ITEMS({QStringLiteral(
			"Hinweis auf die f\u00FCr den Diensteanbieter zust\u00E4ndigen Stellen, "
			"die die Einhaltung der Vorschriften zum Datenschutz kontrollieren:")});

	return getField(getTermsOfUsage(), SEARCH_ITEMS);
}
