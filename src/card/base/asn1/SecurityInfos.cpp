/*!
 * SecurityInfos.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ASN1TemplateUtil.h"
#include "SecurityInfos.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{
ASN1_ITEM_TEMPLATE(securityinfos_st) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_SET_OF, 0x00, securityinfos_st, securityinfo_st)
ASN1_ITEM_TEMPLATE_END(securityinfos_st)


IMPLEMENT_ASN1_FUNCTIONS(securityinfos_st)

IMPLEMENT_ASN1_OBJECT(securityinfos_st)

}  // namespace governikus


QSharedPointer<SecurityInfos> SecurityInfos::fromHex(const QByteArray& pHexString)
{
	return SecurityInfos::decode(QByteArray::fromHex(pHexString));
}


QSharedPointer<SecurityInfos> SecurityInfos::decode(const QByteArray& pBytes)
{
	auto securityInfosStruct = decodeObject<securityinfos_st>(pBytes);
	if (securityInfosStruct == nullptr)
	{
		return QSharedPointer<SecurityInfos>();
	}

	QVector<QSharedPointer<SecurityInfo> > securityInfos;

	for (int i = 0; i < SKM_sk_num(securityinfo_st, securityInfosStruct.data()); ++i)
	{
		securityinfo_st* secInfoStruct = SKM_sk_value(securityinfo_st, securityInfosStruct.data(), i);
		QByteArray secInfoStructBytes = encodeObject(secInfoStruct);
		auto securityInfo = SecurityInfoFactory::createSecurityInfo(secInfoStructBytes);
		if (securityInfo == nullptr)
		{
			return QSharedPointer<SecurityInfos>();
		}
		securityInfos += securityInfo;
	}

	return QSharedPointer<SecurityInfos>(new SecurityInfos(pBytes, securityInfos));
}


SecurityInfos::SecurityInfos(const QByteArray& pBytes, const QVector<QSharedPointer<SecurityInfo> >& pSecurityInfos)
	: mContentBytes(pBytes)
	, mSecurityInfos(pSecurityInfos)
{
}


const QByteArray& SecurityInfos::getContentBytes() const
{
	return mContentBytes;
}
