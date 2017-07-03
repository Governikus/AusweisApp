/*!
 * SecurityInfo.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "ChipAuthenticationInfo.h"
#include "KnownOIDs.h"
#include "PACEInfo.h"
#include "SecurityInfo.h"

#include <QLoggingCategory>


using namespace governikus;
using namespace KnownOIDs;


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


QByteArray SecurityInfo::getProtocolValueBytes() const
{
	return QByteArray(reinterpret_cast<const char*>(getProtocolObjectIdentifier()->data), getProtocolObjectIdentifier()->length);
}


QByteArray SecurityInfo::getProtocol() const
{
	return Asn1ObjectUtil::convertTo(getProtocolObjectIdentifier());
}


SecurityInfo::~SecurityInfo()
{
}
