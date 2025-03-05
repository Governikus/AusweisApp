/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MobileEIDTypeInfo.h"

#include "ASN1TemplateUtil.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

ASN1_SEQUENCE(mobileeidtypeinfo_st) = {
	ASN1_SIMPLE(mobileeidtypeinfo_st, mProtocol, ASN1_OBJECT),
	ASN1_SIMPLE(mobileeidtypeinfo_st, mRequiredData, ASN1_ANY)
}


ASN1_SEQUENCE_END(mobileeidtypeinfo_st)

IMPLEMENT_ASN1_FUNCTIONS(mobileeidtypeinfo_st)
IMPLEMENT_ASN1_OBJECT(mobileeidtypeinfo_st)


}  // namespace governikus


MobileEIDTypeInfo::MobileEIDTypeInfo(const QSharedPointer<const mobileeidtypeinfo_st>& pDelegate)
	: SecurityInfo()
	, mDelegate(pDelegate)
{
}


ASN1_OBJECT* MobileEIDTypeInfo::getProtocolObjectIdentifier() const
{
	return mDelegate->mProtocol;
}


bool MobileEIDTypeInfo::acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier)
{
	auto oid = Oid(pObjectIdentifier);
	return oid == Oid(KnownOid::ID_MOBILE_EID_TYPE_SE_CERTIFIED)
		   || oid == Oid(KnownOid::ID_MOBILE_EID_TYPE_SE_ENDORSED)
		   || oid == Oid(KnownOid::ID_MOBILE_EID_TYPE_HW_KEYSTORE);
}


QSharedPointer<const MobileEIDTypeInfo> MobileEIDTypeInfo::decode(const QByteArray& pBytes)
{
	if (const auto& delegate = decodeObject<mobileeidtypeinfo_st>(pBytes, false))
	{
		if (MobileEIDTypeInfo::acceptsProtocol(delegate->mProtocol))
		{
			const auto& si = QSharedPointer<const MobileEIDTypeInfo>::create(delegate);
			qCDebug(card) << "Parsed SecurityInfo:" << si;
			return si;
		}
	}
	return QSharedPointer<const MobileEIDTypeInfo>();
}


[[nodiscard]] QByteArray MobileEIDTypeInfo::encode() const
{
	return encodeObject(mDelegate.data());
}
