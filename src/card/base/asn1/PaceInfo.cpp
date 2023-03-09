/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "PaceInfo.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "SecurityProtocol.h"

#include <QLoggingCategory>


using namespace governikus;


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
IMPLEMENT_ASN1_OBJECT(paceinfo_st)


}  // namespace governikus


bool PaceInfo::acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier)
{
	return SecurityProtocol(Oid(pObjectIdentifier)).getProtocol() == ProtocolType::PACE;
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


QSharedPointer<const PaceInfo> PaceInfo::decode(const QByteArray& pBytes)
{
	if (const auto& delegate = decodeObject<paceinfo_st>(pBytes, false))
	{
		if (PaceInfo::acceptsProtocol(delegate->mProtocol))
		{
			const auto& si = QSharedPointer<const PaceInfo>::create(delegate);
			qCDebug(card) << "Parsed SecurityInfo:" << si;
			return si;
		}
	}
	return QSharedPointer<const PaceInfo>();
}


int PaceInfo::getVersion() const
{
	return Asn1IntegerUtil::getValue(mDelegate->mVersion);
}


bool PaceInfo::hasParameterId() const
{
	return mDelegate->mParameterId;
}


int PaceInfo::getParameterId() const
{
	return Asn1IntegerUtil::getValue(mDelegate->mParameterId);
}


int PaceInfo::getParameterIdAsNid() const
{
	return getMappedNid(getParameterId());
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
	const auto protocol = getProtocol();
	const auto keyAgreement = protocol.getKeyAgreement();
	const auto mapping = protocol.getMapping();
	const auto paramId = getParameterId();

	if (keyAgreement == KeyAgreementType::DH)
	{
		return 0 <= paramId && paramId <= 3;
	}
	if (keyAgreement == KeyAgreementType::ECDH && mapping == MappingType::GM)
	{
		return 8 <= paramId && paramId <= 18;
	}
	if (keyAgreement == KeyAgreementType::ECDH && mapping == MappingType::IM)
	{
		return 8 <= paramId && paramId <= 18 && paramId != 10;
	}

	return false;
}


int PaceInfo::getMappedNid(int pCurveIndex)
{
	switch (pCurveIndex)
	{
		case 8:
			// According to RFC 5480 2.1.1.1 secp192r1 equals NID_X9_62_prime192v1
			return NID_X9_62_prime192v1;

		case 9:
			return NID_brainpoolP192r1;

		case 10:
			// cannot be used with IM
			return NID_secp224r1;

		case 11:
			return NID_brainpoolP224r1;

		case 12:
			// According to RFC 5480 2.1.1.1 secp256r1 equals NID_X9_62_prime256v1
			return NID_X9_62_prime256v1;

		case 13:
			return NID_brainpoolP256r1;

		case 14:
			return NID_brainpoolP320r1;

		case 15:
			return NID_secp384r1;

		case 16:
			return NID_brainpoolP384r1;

		case 17:
			return NID_brainpoolP512r1;

		case 18:
			return NID_secp521r1;

		default:
			qCWarning(card) << "Standardized elliptic curve" << pCurveIndex << "not supported";
			return NID_undef;
	}
}
