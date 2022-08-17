/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationTemplates.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "FileRef.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{
ASN1_ITEM_TEMPLATE(ApplicationTemplatesInternal) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_SET_OF, 0x00, ApplicationTemplatesInternal, ApplicationTemplate)
ASN1_ITEM_TEMPLATE_END(ApplicationTemplatesInternal)


IMPLEMENT_ASN1_FUNCTIONS(ApplicationTemplatesInternal)

IMPLEMENT_ASN1_OBJECT(ApplicationTemplatesInternal)

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
	#define sk_ApplicationTemplate_num(data) SKM_sk_num(ApplicationTemplate, data)
	#define sk_ApplicationTemplate_value(data, i) SKM_sk_value(ApplicationTemplate, data, i)
#endif

} // namespace governikus


QSharedPointer<ApplicationTemplates> ApplicationTemplates::decode(const QByteArray& pBytes)
{
	// Add "SET OF" class/tag and add length, as they are not part of the card data structure.
	const QByteArray setBytes = Asn1Util::encode(V_ASN1_UNIVERSAL, 17, pBytes, true);

	auto applicationTemplatesStruct = decodeObject<ApplicationTemplatesInternal>(setBytes);
	if (applicationTemplatesStruct == nullptr)
	{
		return QSharedPointer<ApplicationTemplates>();
	}

	QVector<QSharedPointer<const ApplicationTemplate>> applicationTemplates;
	for (int i = 0; i < sk_ApplicationTemplate_num(applicationTemplatesStruct.data()); ++i)
	{
		ApplicationTemplate* applicationTemplateStruct = sk_ApplicationTemplate_value(applicationTemplatesStruct.data(), i);
		if (!applicationTemplatesStruct)
		{
			return QSharedPointer<ApplicationTemplates>();
		}

		const auto applicationTemplate = ApplicationTemplate::decode(encodeObject(applicationTemplateStruct));

		if (applicationTemplate.isNull())
		{
			return QSharedPointer<ApplicationTemplates>();
		}
		applicationTemplates << applicationTemplate;
		qCDebug(card) << "Parsed ApplicationTemplate:" << applicationTemplate;
	}

	return QSharedPointer<ApplicationTemplates>::create(pBytes, applicationTemplates);
}


ApplicationTemplates::ApplicationTemplates(const QByteArray& pBytes,
		const QVector<QSharedPointer<const ApplicationTemplate>>& pApplicationTemplates)
	: mContentBytes(pBytes)
	, mApplicationTemplates(pApplicationTemplates)
{
}


const QByteArray& ApplicationTemplates::getContentBytes() const
{
	return mContentBytes;
}


const QVector<QSharedPointer<const ApplicationTemplate>>& ApplicationTemplates::getApplicationTemplates() const
{
	return mApplicationTemplates;
}


bool ApplicationTemplates::contains(const QByteArray& pIdentifier)
{
	return std::any_of(mApplicationTemplates.constBegin(), mApplicationTemplates.constEnd(), [pIdentifier](const auto& entry)
		{
			return entry->getAid() == pIdentifier;
		});
}


int ApplicationTemplates::count() const
{
	return mApplicationTemplates.size();
}
