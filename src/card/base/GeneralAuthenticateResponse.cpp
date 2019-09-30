/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ASN1Util.h"
#include "GeneralAuthenticateResponse.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;


GAResponseApdu::GAResponseApdu(const ResponseApdu& pResponseApdu)
	: mResponseApdu(pResponseApdu)
{
	if (!isValid())
	{
		qCCritical(card) << "General authentication data seems invalid because of StatusCode:" << pResponseApdu.getReturnCode();
	}
}


bool GAResponseApdu::isValid() const
{
	return StatusCode::SUCCESS == getReturnCode();
}


StatusCode GAResponseApdu::getReturnCode() const
{
	return mResponseApdu.getReturnCode();
}


namespace governikus
{

ASN1_SEQUENCE(ga_encryptednoncedata_st) = {
	ASN1_IMP(ga_encryptednoncedata_st, mEncryptedNonce, ASN1_OCTET_STRING, 0x00)
}


ASN1_SEQUENCE_END(ga_encryptednoncedata_st)


ASN1_ITEM_TEMPLATE(GA_ENCRYPTEDNONCEDATA) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x1C, GA_ENCRYPTEDNONCEDATA, ga_encryptednoncedata_st)
ASN1_ITEM_TEMPLATE_END(GA_ENCRYPTEDNONCEDATA)
IMPLEMENT_ASN1_FUNCTIONS(GA_ENCRYPTEDNONCEDATA)
IMPLEMENT_ASN1_OBJECT(GA_ENCRYPTEDNONCEDATA)

}  // namespace governikus


void GAEncryptedNonceResponse::parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData)
{
	auto data = decodeObject<GA_ENCRYPTEDNONCEDATA>(pDynamicAuthenticationData);
	if (data == nullptr)
	{
		qCCritical(card) << "Cannot parse encrypted nonce, return empty byte array";
		return;
	}
	mEncryptedNonce = Asn1OctetStringUtil::getValue(data->mEncryptedNonce);
}


GAEncryptedNonceResponse::GAEncryptedNonceResponse(const ResponseApdu& pResponseApdu)
	: GAResponseApdu(pResponseApdu)
	, mEncryptedNonce()
{
	if (isValid())
	{
		parseDynamicAuthenticationData(mResponseApdu.getData());
	}
}


const QByteArray& GAEncryptedNonceResponse::getEncryptedNonce() const
{
	return mEncryptedNonce;
}


namespace governikus
{

ASN1_SEQUENCE(ga_mapnoncedata_st) = {
	ASN1_IMP(ga_mapnoncedata_st, mMappingData, ASN1_OCTET_STRING, 0x02)
}


ASN1_SEQUENCE_END(ga_mapnoncedata_st)


ASN1_ITEM_TEMPLATE(GA_MAPNONCEDATA) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x1C, GA_MAPNONCEDATA, ga_mapnoncedata_st)
ASN1_ITEM_TEMPLATE_END(GA_MAPNONCEDATA)
IMPLEMENT_ASN1_FUNCTIONS(GA_MAPNONCEDATA)
IMPLEMENT_ASN1_OBJECT(GA_MAPNONCEDATA)

}  // namespace governikus


void GAMapNonceResponse::parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData)
{
	auto data = decodeObject<GA_MAPNONCEDATA>(pDynamicAuthenticationData);
	if (data == nullptr)
	{
		qCCritical(card) << "Cannot parse mapping data, return empty byte array";
		return;
	}
	mMappingData = Asn1OctetStringUtil::getValue(data->mMappingData);
}


GAMapNonceResponse::GAMapNonceResponse(const ResponseApdu& pResponseApdu)
	: GAResponseApdu(pResponseApdu)
	, mMappingData()
{
	if (isValid())
	{
		parseDynamicAuthenticationData(mResponseApdu.getData());
	}
}


const QByteArray& GAMapNonceResponse::getMappingData() const
{
	return mMappingData;
}


namespace governikus
{

ASN1_SEQUENCE(ga_performkeyagreementdata_st) = {
	ASN1_IMP(ga_performkeyagreementdata_st, mEphemeralPublicKey, ASN1_OCTET_STRING, 0x04)
}


ASN1_SEQUENCE_END(ga_performkeyagreementdata_st)


ASN1_ITEM_TEMPLATE(GA_PERFORMKEYAGREEMENTDATA) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x1C, GA_PERFORMKEYAGREEMENTDATA, ga_performkeyagreementdata_st)
ASN1_ITEM_TEMPLATE_END(GA_PERFORMKEYAGREEMENTDATA)
IMPLEMENT_ASN1_FUNCTIONS(GA_PERFORMKEYAGREEMENTDATA)
IMPLEMENT_ASN1_OBJECT(GA_PERFORMKEYAGREEMENTDATA)

}  // namespace governikus


void GAPerformKeyAgreementResponse::parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData)
{
	auto data = decodeObject<GA_PERFORMKEYAGREEMENTDATA>(pDynamicAuthenticationData);
	if (data == nullptr)
	{
		qCCritical(card) << "Cannot parse ephemeral public key, return empty byte array";
		return;
	}
	mEphemeralPublicKey = Asn1OctetStringUtil::getValue(data->mEphemeralPublicKey);
}


GAPerformKeyAgreementResponse::GAPerformKeyAgreementResponse(const ResponseApdu& pResponseApdu)
	: GAResponseApdu(pResponseApdu)
	, mEphemeralPublicKey()
{
	if (isValid())
	{
		parseDynamicAuthenticationData(mResponseApdu.getData());
	}
}


const QByteArray& GAPerformKeyAgreementResponse::getEphemeralPublicKey() const
{
	return mEphemeralPublicKey;
}


namespace governikus
{

ASN1_SEQUENCE(ga_mutualauthenticationdata_st) = {
	ASN1_IMP(ga_mutualauthenticationdata_st, mAuthenticationToken, ASN1_OCTET_STRING, 0x06),
	ASN1_IMP_OPT(ga_mutualauthenticationdata_st, mCarCurr, ASN1_OCTET_STRING, 0x07),
	ASN1_IMP_OPT(ga_mutualauthenticationdata_st, mCarPrev, ASN1_OCTET_STRING, 0x08),
}


ASN1_SEQUENCE_END(ga_mutualauthenticationdata_st)


ASN1_ITEM_TEMPLATE(GA_MUTUALAUTHENTICATIONDATA) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x1C, GA_MUTUALAUTHENTICATIONDATA, ga_mutualauthenticationdata_st)
ASN1_ITEM_TEMPLATE_END(GA_MUTUALAUTHENTICATIONDATA)
IMPLEMENT_ASN1_FUNCTIONS(GA_MUTUALAUTHENTICATIONDATA)
IMPLEMENT_ASN1_OBJECT(GA_MUTUALAUTHENTICATIONDATA)

}  // namespace governikus


void GAMutualAuthenticationResponse::parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData)
{
	auto data = decodeObject<GA_MUTUALAUTHENTICATIONDATA>(pDynamicAuthenticationData);
	if (data == nullptr)
	{
		qCCritical(card) << "Cannot parse authentication token, return empty byte array";
		return;
	}
	mAuthenticationToken = Asn1OctetStringUtil::getValue(data->mAuthenticationToken);
	mCarCurr = Asn1OctetStringUtil::getValue(data->mCarCurr);
	mCarPrev = Asn1OctetStringUtil::getValue(data->mCarPrev);

	qCDebug(card) << "mCarCurr" << mCarCurr;
	qCDebug(card) << "mCarPrev" << mCarPrev;
}


GAMutualAuthenticationResponse::GAMutualAuthenticationResponse(const ResponseApdu& pResponseApdu)
	: GAResponseApdu(pResponseApdu)
	, mAuthenticationToken()
	, mCarCurr()
	, mCarPrev()
{
	if (isValid())
	{
		parseDynamicAuthenticationData(mResponseApdu.getData());
	}
}


const QByteArray& GAMutualAuthenticationResponse::getAuthenticationToken() const
{
	return mAuthenticationToken;
}


const QByteArray& GAMutualAuthenticationResponse::getCarCurr() const
{
	return mCarCurr;
}


const QByteArray& GAMutualAuthenticationResponse::getCarPrev() const
{
	return mCarPrev;
}


namespace governikus
{

ASN1_SEQUENCE(ga_chipauthenticationdata_st) = {
	ASN1_IMP(ga_chipauthenticationdata_st, mNonce, ASN1_OCTET_STRING, 0x01),
	ASN1_IMP(ga_chipauthenticationdata_st, mAuthenticationToken, ASN1_OCTET_STRING, 0x02),
}


ASN1_SEQUENCE_END(ga_chipauthenticationdata_st)


ASN1_ITEM_TEMPLATE(GA_CHIPAUTHENTICATIONDATA) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x1C, GA_CHIPAUTHENTICATIONDATA, ga_chipauthenticationdata_st)
ASN1_ITEM_TEMPLATE_END(GA_CHIPAUTHENTICATIONDATA)
IMPLEMENT_ASN1_FUNCTIONS(GA_CHIPAUTHENTICATIONDATA)
IMPLEMENT_ASN1_OBJECT(GA_CHIPAUTHENTICATIONDATA)

}  // namespace governikus


void GAChipAuthenticationResponse::parseDynamicAuthenticationData(const QByteArray& pDynamicAuthenticationData)
{
	auto data = decodeObject<GA_CHIPAUTHENTICATIONDATA>(pDynamicAuthenticationData);
	if (data == nullptr)
	{
		qCCritical(card) << "Cannot parse chip authentication token, return empty byte array";
		return;
	}
	mNonce = Asn1OctetStringUtil::getValue(data->mNonce);
	mAuthenticationToken = Asn1OctetStringUtil::getValue(data->mAuthenticationToken);
}


GAChipAuthenticationResponse::GAChipAuthenticationResponse(const ResponseApdu& pResponseApdu)
	: GAResponseApdu(pResponseApdu)
	, mNonce()
	, mAuthenticationToken()
{
	if (isValid())
	{
		parseDynamicAuthenticationData(mResponseApdu.getData());
	}
}


const QByteArray& GAChipAuthenticationResponse::getNonce() const
{
	return mNonce;
}


const QByteArray& GAChipAuthenticationResponse::getAuthenticationToken() const
{
	return mAuthenticationToken;
}
