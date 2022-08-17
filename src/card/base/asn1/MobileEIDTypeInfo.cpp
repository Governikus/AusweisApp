/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "MobileEIDTypeInfo.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "SecurityProtocol.h"


using namespace governikus;


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


bool MobileEIDTypeInfo::acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier)
{
	auto oid = Oid(pObjectIdentifier);
	return oid == Oid(KnownOid::ID_MOBILE_EID_TYPE_SE_CERTIFIED)
		   || oid == Oid(KnownOid::ID_MOBILE_EID_TYPE_SE_ENDORSED)
		   || oid == Oid(KnownOid::ID_MOBILE_EID_TYPE_HW_KEYSTORE);
}


MobileEIDTypeInfo::MobileEIDTypeInfo(const QSharedPointer<const mobileeidtypeinfo_st>& pDelegate)
	: SecurityInfo()
	, mDelegate(pDelegate)
{
}


ASN1_OBJECT* MobileEIDTypeInfo::getProtocolObjectIdentifier() const
{
	return mDelegate->mProtocol;
}
