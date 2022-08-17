/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "EFCardSecurity.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "Oid.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)

#ifndef OPENSSL_NO_CMS
namespace governikus
{

IMPLEMENT_ASN1_OBJECT(CMS_ContentInfo)


} // namespace governikus

#endif


QSharedPointer<EFCardSecurity> EFCardSecurity::fromHex(const QByteArray& pHexString)
{
	return decode(QByteArray::fromHex(pHexString));
}


QSharedPointer<EFCardSecurity> EFCardSecurity::decode(const QByteArray& pBytes)
{
#ifdef OPENSSL_NO_CMS
	#error Cryptographic Message Syntax (CMS) is required. Do you use LibreSSL?
	Q_UNUSED(pBytes)
	return QSharedPointer<EFCardSecurity>();

#else
	const auto contentInfo = decodeObject<CMS_ContentInfo>(pBytes);
	if (contentInfo == nullptr)
	{
		qCCritical(card) << "No contentInfo";
		return QSharedPointer<EFCardSecurity>();
	}

	if (const Oid type(CMS_get0_type(contentInfo.data())); type != KnownOid::ID_SIGNED_DATA)
	{
		qCCritical(card) << "Got unexpected ContentInfo type" << type;
		return QSharedPointer<EFCardSecurity>();
	}

	if (const Oid eContentType(CMS_get0_eContentType(contentInfo.data())); eContentType != KnownOid::ID_SECURITY_OBJECT)
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

	return QSharedPointer<EFCardSecurity>::create(securityInfos);

#endif
}


const QSharedPointer<const SecurityInfos>& EFCardSecurity::getSecurityInfos() const
{
	return mSecurityInfos;
}


EFCardSecurity::EFCardSecurity(const QSharedPointer<const SecurityInfos>& pSecurityInfos)
	: mSecurityInfos(pSecurityInfos)
{
}
