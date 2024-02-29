/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "SecurityInfos.h"

#include "ASN1TemplateUtil.h"

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
} // namespace governikus


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

	QList<QSharedPointer<const SecurityInfo>> securityInfos;
	QList<QSharedPointer<const PaceInfo>> paceInfos;
	QList<QSharedPointer<const ChipAuthenticationInfo>> chipAuthenticationInfos;
	QSharedPointer<const MobileEIDTypeInfo> mobileEIDTypeInfo;

	for (int i = 0; i < sk_securityinfo_st_num(securityInfosStruct.data()); ++i)
	{
		securityinfo_st* secInfoStruct = sk_securityinfo_st_value(securityInfosStruct.data(), i);
		QByteArray bytes = encodeObject(secInfoStruct);

		if (auto meid = MobileEIDTypeInfo::decode(bytes))
		{
			if (mobileEIDTypeInfo)
			{
				qCCritical(card) << "More than one MobileEIDTypeInfo found:" << meid;
				return QSharedPointer<SecurityInfos>();
			}
			mobileEIDTypeInfo = meid;
			securityInfos << meid;
		}
		else if (auto pi = PaceInfo::decode(bytes))
		{
			paceInfos << pi;
			securityInfos << pi;
		}
		else if (auto cai = ChipAuthenticationInfo::decode(bytes))
		{
			chipAuthenticationInfos << cai;
			securityInfos << cai;
		}
		else if (auto secInfo = SecurityInfo::decode(bytes))
		{
			securityInfos << secInfo;
		}

		if (securityInfos.size() == i)
		{
			qCCritical(card) << "Cannot parse as SecurityInfo" << pBytes.toHex();
			return QSharedPointer<SecurityInfos>();
		}
	}

	return QSharedPointer<SecurityInfos>::create(pBytes, securityInfos, paceInfos, chipAuthenticationInfos, mobileEIDTypeInfo);
}


SecurityInfos::SecurityInfos(const QByteArray& pBytes,
		const QList<QSharedPointer<const SecurityInfo>>& pSecurityInfos,
		const QList<QSharedPointer<const PaceInfo>>& pPaceInfos,
		const QList<QSharedPointer<const ChipAuthenticationInfo>>& pChipAuthenticationInfos,
		const QSharedPointer<const MobileEIDTypeInfo>& pMobileEIDTypeInfo)
	: mContentBytes(pBytes)
	, mSecurityInfos(pSecurityInfos)
	, mPaceInfos(pPaceInfos)
	, mChipAuthenticationInfos(pChipAuthenticationInfos)
	, mMobileEIDTypeInfo(pMobileEIDTypeInfo)
{
}


const QByteArray& SecurityInfos::getContentBytes() const
{
	return mContentBytes;
}


const QList<QSharedPointer<const SecurityInfo>>& SecurityInfos::getSecurityInfos() const
{
	return mSecurityInfos;
}


const QList<QSharedPointer<const PaceInfo>>& SecurityInfos::getPaceInfos() const
{
	return mPaceInfos;
}


const QList<QSharedPointer<const ChipAuthenticationInfo>>& SecurityInfos::getChipAuthenticationInfos() const
{
	return mChipAuthenticationInfos;
}


const QSharedPointer<const MobileEIDTypeInfo>& SecurityInfos::getMobileEIDTypeInfo() const
{
	return mMobileEIDTypeInfo;
}
