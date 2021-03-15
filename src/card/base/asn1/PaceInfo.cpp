/*!
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */


#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "KnownOIDs.h"
#include "PaceInfo.h"

#include <QLoggingCategory>


using namespace governikus;
using namespace governikus::KnownOIDs;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

ASN1_SEQUENCE(paceinfo_st) = {
	ASN1_SIMPLE(paceinfo_st, mProtocol, ASN1_OBJECT),
	ASN1_SIMPLE(paceinfo_st, mVersion, ASN1_INTEGER),
	ASN1_OPT(paceinfo_st, mParameterId, ASN1_INTEGER)
}


ASN1_SEQUENCE_END(paceinfo_st)

IMPLEMENT_ASN1_FUNCTIONS(paceinfo_st)

template<>
paceinfo_st* decodeAsn1Object<paceinfo_st>(paceinfo_st** pObject, const unsigned char** pData, long pDataLen)
{
	return d2i_paceinfo_st(pObject, pData, pDataLen);
}


template<>
void freeAsn1Object<paceinfo_st>(paceinfo_st* pObject)
{
	paceinfo_st_free(pObject);
}


}  // namespace governikus


bool PaceInfo::acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier)
{
	const auto protocol = Asn1ObjectUtil::convertTo(pObjectIdentifier);
	return protocol == id_PACE::DH::GM_3DES_CBC_CBC
		   || protocol == id_PACE::DH::GM_AES_CBC_CMAC_128
		   || protocol == id_PACE::DH::GM_AES_CBC_CMAC_192
		   || protocol == id_PACE::DH::GM_AES_CBC_CMAC_256
		   || protocol == id_PACE::ECDH::GM_3DES_CBC_CBC
		   || protocol == id_PACE::ECDH::GM_AES_CBC_CMAC_128
		   || protocol == id_PACE::ECDH::GM_AES_CBC_CMAC_192
		   || protocol == id_PACE::ECDH::GM_AES_CBC_CMAC_256
		   || protocol == id_PACE::DH::IM_3DES_CBC_CBC
		   || protocol == id_PACE::DH::IM_AES_CBC_CMAC_128
		   || protocol == id_PACE::DH::IM_AES_CBC_CMAC_192
		   || protocol == id_PACE::DH::IM_AES_CBC_CMAC_256
		   || protocol == id_PACE::ECDH::IM_3DES_CBC_CBC
		   || protocol == id_PACE::ECDH::IM_AES_CBC_CMAC_128
		   || protocol == id_PACE::ECDH::IM_AES_CBC_CMAC_192
		   || protocol == id_PACE::ECDH::IM_AES_CBC_CMAC_256;
}


PaceInfo::PaceInfo(const QSharedPointer<const paceinfo_st>& pDelegate)
	: SecurityInfo()
	, mDelegate(pDelegate)
{
	if (getVersion() != 2)
	{
		qCWarning(card) << "Expect version=2, got: " << getVersion();
	}
}


ASN1_OBJECT* PaceInfo::getProtocolObjectIdentifier() const
{
	return mDelegate->mProtocol;
}


QByteArray PaceInfo::getParameterId() const
{
	if (mDelegate->mParameterId)
	{
		return Asn1IntegerUtil::getValue(mDelegate->mParameterId);
	}
	return QByteArray();
}


int PaceInfo::getVersion() const
{
	const auto& version = Asn1IntegerUtil::getValue(mDelegate->mVersion);
	return version.isEmpty() || version.size() > 1 ? -1 : version[0];
}


KeyAgreementType PaceInfo::getKeyAgreementType() const
{
	const auto& protocol = getProtocol();
	if (protocol.startsWith(toByteArray(KnownOIDs::id_PACE::DH::GM))
			|| protocol.startsWith(toByteArray(KnownOIDs::id_PACE::DH::IM)))
	{
		return KeyAgreementType::DH;
	}
	else
	{
		return KeyAgreementType::ECDH;
	}
}


MappingType PaceInfo::getMappingType() const
{
	const auto& protocol = getProtocol();
	if (protocol.startsWith(toByteArray(KnownOIDs::id_PACE::DH::GM))
			|| protocol.startsWith(toByteArray(KnownOIDs::id_PACE::ECDH::GM)))
	{
		return MappingType::GM;
	}
	else
	{
		return MappingType::IM;
	}
}


int PaceInfo::getParameterIdAsInt() const
{
	bool conversionOkay = false;

	/*
	 * According to the Qt documentation of QByteArray:
	 * "base must be between 2 and 36, or 0."
	 * So we convert first to hex and parse  it then with base 16
	 */
	int parameterIdAsInt = getParameterId().toHex().toInt(&conversionOkay, 16);
	if (!conversionOkay)
	{
		qCCritical(card) << "Conversion error on parameterId";
		return -1;
	}
	return parameterIdAsInt;
}


bool PaceInfo::isStandardizedDomainParameters() const
{
	/*
	 * According to TR-03110-3 the standardized domain parameters are:
	 *
	 *  0 1024-bit MODP Group with 160-bit Prime Order Subgroup 1024/160 GFP
	 *  1 2048-bit MODP Group with 224-bit Prime Order Subgroup 2048/224 GFP
	 *  2 2048-bit MODP Group with 256-bit Prime Order Subgroup 2048/256 GFP
	 *  3 - 7 RFU
	 *  8 NIST P-192 (secp192r1) 192 ECP
	 *  9 BrainpoolP192r1 192 ECP
	 * 10 NIST P-224 (secp224r1) 224 ECP - This curve cannot be used with the integrated mapping.
	 * 11 BrainpoolP224r1 224 ECP
	 * 12 NIST P-256 (secp256r1) 256 ECP
	 * 13 BrainpoolP256r1 256 ECP
	 * 14 BrainpoolP320r1 320 ECP
	 * 15 NIST P-384 (secp384r1) 384 ECP
	 * 16 BrainpoolP384r1 384 ECP
	 * 17 BrainpoolP512r1 512 ECP
	 * 18 NIST P-521 (secp521r1) 521 ECP
	 * 19-31 RFU
	 */
	int paramId = getParameterIdAsInt();
	if (getKeyAgreementType() == KeyAgreementType::DH)
	{
		return 0 <= paramId && paramId <= 3;
	}
	if (getKeyAgreementType() == KeyAgreementType::ECDH && getMappingType() == MappingType::GM)
	{
		return 8 <= paramId && paramId <= 18;
	}
	if (getKeyAgreementType() == KeyAgreementType::ECDH && getMappingType() == MappingType::IM)
	{
		return 8 <= paramId && paramId <= 18 && paramId != 10;
	}

	return false;
}


#include "moc_PaceInfo.cpp"
