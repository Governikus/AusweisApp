/*!
 * \brief Implementation of ISO 7816 application template
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ASN1TemplateUtil.h"
#include "FileRef.h"
#include "SecurityProtocol.h"

#include <QDebug>
#include <QSharedPointer>
#include <openssl/asn1t.h>


namespace governikus
{

/**
 * ApplicationTemplate ::= SET {
 *      applicationIdentifier OBJECT IDENTIFIER,
 *      applicationLabel PrintableString OPTIONAL,
 *      fileReference OCTET STRING OPTIONAL,
 *      commandApdu OCTET STRING OPTIONAL,
 *      discretionaryData OCTET STRING OPTIONAL,
 *      discretionaryTemplate OCTET STRING OPTIONAL,
 *      uniformResourceLocator OCTET STRING OPTIONAL,
 *      applicationRelatedDataObjects ANY DEFINED BY applicationIdentifier OPTIONAL
 * }
 *
 * derived from the description in 7816-4:2005 8.2.1
 */

using ApplicationTemplate = struct application_template_st
{
	ASN1_OCTET_STRING* mApplicationIdentifier;
	ASN1_PRINTABLESTRING* mApplicationLabel;
	ASN1_OCTET_STRING* mFileReference;
	ASN1_OCTET_STRING* mCommandApdu;
	ASN1_OCTET_STRING* mDiscretionaryData;
	ASN1_OCTET_STRING* mUniformResourceLocator;

	static QSharedPointer<application_template_st> decode(const QByteArray& pBytes);

	application_template_st(const application_template_st& pCopy);
	application_template_st& operator=(const application_template_st&) = delete;
	~application_template_st();

	/**
	 * Returns the protocol AID.
	 */
	[[nodiscard]] QByteArray getAid() const;
};


inline QDebug operator<<(QDebug pDbg, const QSharedPointer<ApplicationTemplate>& pApplicationTemplate)
{
	QDebugStateSaver saver(pDbg);
	pDbg << FileRef(FileRef::TYPE::APPLICATION, pApplicationTemplate->getAid());
	return pDbg;
}


#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
DECLARE_STACK_OF(ApplicationTemplate)
#else
DEFINE_STACK_OF(ApplicationTemplate)
#endif

DECLARE_ASN1_FUNCTIONS(ApplicationTemplate)
DECLARE_ASN1_OBJECT(ApplicationTemplate)


} // namespace governikus
