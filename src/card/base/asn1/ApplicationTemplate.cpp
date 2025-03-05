/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationTemplate.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"

#include <QLoggingCategory>
#include <openssl/asn1.h>
#include <openssl/objects.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

/*
 * Defined in ISO 7816-4:2005 8.2.1.
 */
ASN1_SEQUENCE(application_template_st) = {
	ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x0F, application_template_st, mApplicationIdentifier, ASN1_OCTET_STRING),

	ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION | ASN1_TFLG_OPTIONAL, 0x10, application_template_st, mApplicationLabel, ASN1_PRINTABLESTRING),
	ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION | ASN1_TFLG_OPTIONAL, 0x11, application_template_st, mFileReference, ASN1_OCTET_STRING),
	ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION | ASN1_TFLG_OPTIONAL, 0x12, application_template_st, mCommandApdu, ASN1_OCTET_STRING),
	ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION | ASN1_TFLG_OPTIONAL, 0x13, application_template_st, mDiscretionaryData, ASN1_OCTET_STRING),
	ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION | ASN1_TFLG_OPTIONAL, 0x50, application_template_st, mUniformResourceLocator, ASN1_OCTET_STRING),
}


ASN1_SEQUENCE_END(application_template_st)

ASN1_ITEM_TEMPLATE(ApplicationTemplate) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG, 0x01, ApplicationTemplate, application_template_st)
ASN1_ITEM_TEMPLATE_END(ApplicationTemplate)

IMPLEMENT_ASN1_FUNCTIONS(ApplicationTemplate)
IMPLEMENT_ASN1_OBJECT(ApplicationTemplate)

}  // namespace governikus


ApplicationTemplate::~application_template_st()
{
	/*
	 * When creating an instance of ApplicationTemplate explicitly via the copy
	 * constructor, e.g.
	 * new ApplicationTemplate(otherApplicationTemplateInstance)
	 * we must take care of freeing all properties explicitly also.
	 *
	 * When using the OpenSSL macro system for object creation, the system handles
	 * the memory by invoking malloc and free, so no constructor and no destructor
	 * is called.
	 */
	ASN1_OCTET_STRING_free(mApplicationIdentifier);
	ASN1_PRINTABLESTRING_free(mApplicationLabel);
	ASN1_OCTET_STRING_free(mFileReference);
	ASN1_OCTET_STRING_free(mCommandApdu);
	ASN1_OCTET_STRING_free(mDiscretionaryData);
	ASN1_OCTET_STRING_free(mUniformResourceLocator);
}


QSharedPointer<ApplicationTemplate> ApplicationTemplate::decode(const QByteArray& pBytes)
{
	return decodeObject<ApplicationTemplate>(pBytes);
}


QByteArray ApplicationTemplate::getAid() const
{
	return Asn1OctetStringUtil::getValue(mApplicationIdentifier);
}
