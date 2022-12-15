/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ChipAuthenticationInfo.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "SecurityProtocol.h"


using namespace governikus;


namespace governikus
{

ASN1_SEQUENCE(chipauthenticationinfo_st) = {
	ASN1_SIMPLE(chipauthenticationinfo_st, mProtocol, ASN1_OBJECT),
	ASN1_SIMPLE(chipauthenticationinfo_st, mVersion, ASN1_INTEGER),
	ASN1_OPT(chipauthenticationinfo_st, mKeyId, ASN1_INTEGER)
}


ASN1_SEQUENCE_END(chipauthenticationinfo_st)

IMPLEMENT_ASN1_FUNCTIONS(chipauthenticationinfo_st)
IMPLEMENT_ASN1_OBJECT(chipauthenticationinfo_st)


}  // namespace governikus


bool ChipAuthenticationInfo::acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier)
{
	return SecurityProtocol(Oid(pObjectIdentifier)).getProtocol() == ProtocolType::CA;
}


ChipAuthenticationInfo::ChipAuthenticationInfo(const QSharedPointer<const chipauthenticationinfo_st>& pDelegate)
	: SecurityInfo()
	, mDelegate(pDelegate)
{
}


ASN1_OBJECT* ChipAuthenticationInfo::getProtocolObjectIdentifier() const
{
	return mDelegate->mProtocol;
}


QSharedPointer<const ChipAuthenticationInfo> ChipAuthenticationInfo::decode(const QByteArray& pBytes)
{
	if (const auto& delegate = decodeObject<chipauthenticationinfo_st>(pBytes, false))
	{
		if (ChipAuthenticationInfo::acceptsProtocol(delegate->mProtocol))
		{
			const auto& si = QSharedPointer<const ChipAuthenticationInfo>::create(delegate);
			qCDebug(card) << "Parsed SecurityInfo:" << si;
			return si;
		}
	}
	return QSharedPointer<const ChipAuthenticationInfo>();
}


QByteArray ChipAuthenticationInfo::encode() const
{
	return encodeObject(mDelegate.data());
}


int ChipAuthenticationInfo::getVersion() const
{
	return Asn1IntegerUtil::getValue(mDelegate->mVersion);
}


bool ChipAuthenticationInfo::hasKeyId() const
{
	return mDelegate->mKeyId;
}


int ChipAuthenticationInfo::getKeyId() const
{
	return Asn1IntegerUtil::getValue(mDelegate->mKeyId);
}
