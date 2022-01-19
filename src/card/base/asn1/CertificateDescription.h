/*!
 * \brief Implementation of ASN.1 type CertificateDescription with OpenSSL
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"

#include <openssl/asn1t.h>
#include <openssl/err.h>

#include <QSet>
#include <QSharedPointer>
#include <QString>
#include <QVector>


namespace governikus
{

/**
 * As specified in TR-03110-4 chapter 2.2.6:
 *
 *
 * CertificateDescription ::= SEQUENCE {
 *      descriptionType        OBJECT IDENTIFIER,
 *      issuerName             [0x01] UTF8String,
 *      issuerURL              [0x02] PrintableString OPTIONAL,
 *      subjectName            [0x03] UTF8String,
 *      subjectURL             [0x04] PrintableString OPTIONAL,
 *      termsOfUsage           [0x05] ANY DEFINED BY descriptionType,
 *      redirectURL            [0x06] PrintableString OPTIONAL,
 *      commCertificates       [0x07] SET OF OCTET STRING OPTIONAL
 * }
 */
struct CertificateDescription
{
	enum class TermsOfUsageType
	{
		PLAIN_TEXT, HTML, PDF
	};

	ASN1_OBJECT* mDescriptionType;
	ASN1_UTF8STRING* mIssuerName;
	ASN1_PRINTABLESTRING* mIssuerURL;
	ASN1_UTF8STRING* mSubjectName;
	ASN1_PRINTABLESTRING* mSubjectURL;
	ASN1_TYPE* mTermsOfUsage;
	ASN1_PRINTABLESTRING* mRedirectURL;
	STACK_OF(ASN1_OCTET_STRING) * mCommCertificates;


	static QSharedPointer<const CertificateDescription> fromHex(const QByteArray& pHexValue);
	static QSharedPointer<const CertificateDescription> decode(const QByteArray& pBytes);
	QByteArray encode();

	void setDescriptionType(const QByteArray& pOidAsText);
	[[nodiscard]] QByteArray getDescriptionType() const;

	void setIssuerName(const QString& pIssuerName);
	[[nodiscard]] QString getIssuerName() const;

	void setIssuerUrl(const QString& pIssuerUrl);
	[[nodiscard]] QString getIssuerUrl() const;

	void setSubjectName(const QString& pSubjectName);
	[[nodiscard]] QString getSubjectName() const;

	void setSubjectUrl(const QString& pSubjectUrl);
	[[nodiscard]] QString getSubjectUrl() const;

	[[nodiscard]] TermsOfUsageType getTermsOfUsageType() const;
	[[nodiscard]] QString getTermsOfUsage() const;

	void setRedirectUrl(const QString& pRedirectUrl);
	[[nodiscard]] QString getRedirectUrl() const;

	[[nodiscard]] QSet<QString> getCommCertificates() const;

	/*!
	 * \brief Returns the address of provider.
	 *
	 * \return The address.
	 */
	[[nodiscard]] QString getServiceProviderAddress() const;

	/*!
	 * \brief Returns the purpose of the certificate description.
	 *
	 * \return The purpose.
	 */
	[[nodiscard]] QString getPurpose() const;

	/*!
	 * \brief Returns the data security officer of the certificate description.
	 *
	 * \return The data security officer.
	 */
	[[nodiscard]] QString getDataSecurityOfficer() const;
};

DECLARE_ASN1_FUNCTIONS(CertificateDescription)
DECLARE_ASN1_OBJECT(CertificateDescription)

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
	#define sk_ASN1_OCTET_STRING_num(data) data->stack.num
	#define sk_ASN1_OCTET_STRING_value(data, i) SKM_sk_value(ASN1_OCTET_STRING, data, i)
#endif

} // namespace governikus
