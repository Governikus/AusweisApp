/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "ChipAuthenticationInfo.h"
#include "KnownOIDs.h"


using namespace governikus;
using namespace governikus::KnownOIDs;


namespace governikus
{

ASN1_SEQUENCE(chipauthenticationinfo_st) = {
	ASN1_SIMPLE(chipauthenticationinfo_st, mProtocol, ASN1_OBJECT),
	ASN1_SIMPLE(chipauthenticationinfo_st, mVersion, ASN1_INTEGER),
	ASN1_OPT(chipauthenticationinfo_st, mKeyId, ASN1_INTEGER)
}


ASN1_SEQUENCE_END(chipauthenticationinfo_st)

IMPLEMENT_ASN1_FUNCTIONS(chipauthenticationinfo_st)


template<>
chipauthenticationinfo_st* decodeAsn1Object<chipauthenticationinfo_st>(chipauthenticationinfo_st** pObject, const unsigned char** pData, long pDataLen)
{
	return d2i_chipauthenticationinfo_st(pObject, pData, pDataLen);
}


template<>
void freeAsn1Object<chipauthenticationinfo_st>(chipauthenticationinfo_st* pObject)
{
	chipauthenticationinfo_st_free(pObject);
}


}  // namespace governikus


bool ChipAuthenticationInfo::acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier)
{
	using namespace KnownOIDs;

	const auto protocol = Asn1ObjectUtil::convertTo(pObjectIdentifier);
	return protocol == id_ca::DH_3DES_CBC_CBC
		   || protocol == id_ca::DH_AES_CBC_CMAC_128
		   || protocol == id_ca::DH_AES_CBC_CMAC_192
		   || protocol == id_ca::DH_AES_CBC_CMAC_256
		   || protocol == id_ca::ECDH_3DES_CBC_CBC
		   || protocol == id_ca::ECDH_AES_CBC_CMAC_128
		   || protocol == id_ca::ECDH_AES_CBC_CMAC_192
		   || protocol == id_ca::ECDH_AES_CBC_CMAC_256;
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


QByteArray ChipAuthenticationInfo::getVersion() const
{
	return Asn1IntegerUtil::getValue(mDelegate->mVersion);
}


QByteArray ChipAuthenticationInfo::getKeyId() const
{
	if (mDelegate->mKeyId)
	{
		return Asn1IntegerUtil::getValue(mDelegate->mKeyId);
	}
	return QByteArray();
}
