/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
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

	const QSharedPointer<const STACK_OF(X509)> certs(CMS_get1_certs(contentInfo.data()), [](STACK_OF(X509)* pInfo){sk_X509_pop_free(pInfo, X509_free);});
	for (int i = 0; certs && i < sk_X509_num(certs.data()); ++i)
	{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
		const
#endif
		auto* const name = X509_get_subject_name(sk_X509_value(certs.data(), i));
		const int index = X509_NAME_get_index_by_NID(name, NID_serialNumber, -1);
		const auto* const serial = X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, index));
		qCDebug(card) << "Parsed EFCardSecurity signed with DocSigner:" << Asn1StringUtil::getValue(serial);
	}

	return QSharedPointer<EFCardSecurity>::create(securityInfos);
}


const QSharedPointer<const SecurityInfos>& EFCardSecurity::getSecurityInfos() const
{
	return mSecurityInfos;
}


EFCardSecurity::EFCardSecurity(const QSharedPointer<const SecurityInfos>& pSecurityInfos)
	: mSecurityInfos(pSecurityInfos)
{
}
