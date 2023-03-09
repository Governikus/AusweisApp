/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SecurityInfo.h"

#include "ASN1Util.h"
#include "ChipAuthenticationInfo.h"
#include "PaceInfo.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

ASN1_SEQUENCE(securityinfo_st) = {
	ASN1_SIMPLE(securityinfo_st, mProtocol, ASN1_OBJECT),
	ASN1_SIMPLE(securityinfo_st, mRequiredData, ASN1_ANY),
	ASN1_OPT(securityinfo_st, mOptionalData, ASN1_ANY)
}


ASN1_SEQUENCE_END(securityinfo_st)

IMPLEMENT_ASN1_FUNCTIONS(securityinfo_st)

IMPLEMENT_ASN1_OBJECT(securityinfo_st)

}  // namespace governikus


SecurityInfo::SecurityInfo()
	: mDelegate(nullptr)
{
}


SecurityInfo::SecurityInfo(const QSharedPointer<const securityinfo_st>& pDelegate)
	: mDelegate(pDelegate)
{
	Q_ASSERT(pDelegate != nullptr);
}


ASN1_OBJECT* SecurityInfo::getProtocolObjectIdentifier() const
{
	return mDelegate->mProtocol;
}


QSharedPointer<const SecurityInfo> SecurityInfo::decode(const QByteArray& pBytes)
{
	if (const auto& delegate = decodeObject<securityinfo_st>(pBytes))
	{
		const auto& si = QSharedPointer<const SecurityInfo>::create(delegate);
		qCDebug(card) << "Parsed SecurityInfo:" << si;
		return si;
	}
	return QSharedPointer<SecurityInfo>();
}


Oid SecurityInfo::getOid() const
{
	return Oid(getProtocolObjectIdentifier());
}


SecurityProtocol SecurityInfo::getProtocol() const
{
	return SecurityProtocol(getOid());
}
