/*!
 * EFCardSecurity.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "EFCardSecurity.h"
#include "KnownOIDs.h"

#include <QLoggingCategory>


using namespace governikus;
using namespace governikus::KnownOIDs;


Q_DECLARE_LOGGING_CATEGORY(card)

namespace governikus
{

template<>
CMS_ContentInfo* decodeAsn1Object<CMS_ContentInfo>(CMS_ContentInfo** pObject, const unsigned char** pData, long pDataLen)
{
	return d2i_CMS_ContentInfo(pObject, pData, pDataLen);
}


template<>
void freeAsn1Object<CMS_ContentInfo>(CMS_ContentInfo* pObject)
{
	CMS_ContentInfo_free(pObject);
}


}


QSharedPointer<EFCardSecurity> EFCardSecurity::fromHex(const QByteArray& pHexString)
{
	return decode(QByteArray::fromHex(pHexString));
}


QSharedPointer<EFCardSecurity> EFCardSecurity::decode(const QByteArray& pBytes)
{
	const auto contentInfo = decodeObject<CMS_ContentInfo>(pBytes);
	if (contentInfo == nullptr)
	{
		qCCritical(card) << "No contentInfo";
		return QSharedPointer<EFCardSecurity>();
	}

	const auto type = Asn1ObjectUtil::convertTo(CMS_get0_type(contentInfo.data()));
	if (type != KnownOIDs::signedData)
	{
		qCCritical(card) << "Got unexpected ContentInfo type" << type;
		return QSharedPointer<EFCardSecurity>();
	}

	const auto eContentType = Asn1ObjectUtil::convertTo(CMS_get0_eContentType(contentInfo.data()));
	if (eContentType != KnownOIDs::id_SecurityObject)
	{
		qCCritical(card) << "Got unexpected eContentType" << eContentType;
		return QSharedPointer<EFCardSecurity>();
	}

	ASN1_OCTET_STRING** content = CMS_get0_content(contentInfo.data());
	if (content == nullptr || *content == nullptr)
	{
		qCCritical(card) << "Cannot get CMS content";
		return QSharedPointer<EFCardSecurity>();
	}
	QByteArray securityInfosBytes = Asn1OctetStringUtil::getValue(*content);

	const auto securityInfos = SecurityInfos::decode(securityInfosBytes);
	if (securityInfos == nullptr)
	{
		qCCritical(card) << "Cannot parse SecurityInfos";
		return QSharedPointer<EFCardSecurity>();
	}

	return QSharedPointer<EFCardSecurity>(new EFCardSecurity(securityInfos));
}


const QSharedPointer<const SecurityInfos>& EFCardSecurity::getSecurityInfos() const
{
	return mSecurityInfos;
}


EFCardSecurity::EFCardSecurity(const QSharedPointer<const SecurityInfos>& pSecurityInfos)
	: mSecurityInfos(pSecurityInfos)
{
}
