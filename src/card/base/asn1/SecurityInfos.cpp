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

	QVector<QSharedPointer<const SecurityInfo> > securityInfos;
	QVector<QSharedPointer<const PACEInfo> > paceInfos;
	QVector<QSharedPointer<const ChipAuthenticationInfo> > chipAuthenticationInfos;

	for (int i = 0; i < SKM_sk_num(securityinfo_st, securityInfosStruct.data()); ++i)
	{
		securityinfo_st* secInfoStruct = SKM_sk_value(securityinfo_st, securityInfosStruct.data(), i);
		QByteArray bytes = encodeObject(secInfoStruct);

		if (auto pi = PACEInfo::decode(bytes))
		{
			qCDebug(card) << "Parsed PACEInfo";
			paceInfos << pi;
			securityInfos << pi;
		}
		else if (auto cai = ChipAuthenticationInfo::decode(bytes))
		{
			qCDebug(card) << "Parsed ChipAuthenticationInfo";
			chipAuthenticationInfos << cai;
			securityInfos << cai;
		}
		else if (auto secInfo = SecurityInfo::decode(bytes))
		{
			qCDebug(card) << "Parsed SecurityInfo for protocol" << secInfo->getProtocol();
			securityInfos << secInfo;
		}
		else
		{
			qCCritical(card) << "Cannot parse as SecurityInfo" << pBytes.toHex();
			return QSharedPointer<SecurityInfos>();
		}
	}

	return QSharedPointer<SecurityInfos>(new SecurityInfos(pBytes, securityInfos, paceInfos, chipAuthenticationInfos));
}


SecurityInfos::SecurityInfos(const QByteArray& pBytes,
		const QVector<QSharedPointer<const SecurityInfo> >& pSecurityInfos,
		const QVector<QSharedPointer<const PACEInfo> >& pPACEInfos,
		const QVector<QSharedPointer<const ChipAuthenticationInfo> >& pChipAuthenticationInfos)
	: mContentBytes(pBytes)
	, mSecurityInfos(pSecurityInfos)
	, mPACEInfos(pPACEInfos)
	, mChipAuthenticationInfos(pChipAuthenticationInfos)
{
}


const QByteArray& SecurityInfos::getContentBytes() const
{
	return mContentBytes;
}


const QVector<QSharedPointer<const SecurityInfo> >& SecurityInfos::getSecurityInfos() const
{
	return mSecurityInfos;
}


const QVector<QSharedPointer<const PACEInfo> >& SecurityInfos::getPACEInfos() const
{
	return mPACEInfos;
}


const QVector<QSharedPointer<const ChipAuthenticationInfo> >& SecurityInfos::getChipAuthenticationInfos() const
{
	return mChipAuthenticationInfos;
}
