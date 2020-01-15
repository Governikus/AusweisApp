/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "SecureMessagingResponse.h"

#include "asn1/ASN1Util.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

ASN1_ITEM_TEMPLATE(SM_ENCRYPTED_DATA) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_CONTEXT, 0x07, SM_ENCRYPTED_DATA, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(SM_ENCRYPTED_DATA)
IMPLEMENT_ASN1_FUNCTIONS(SM_ENCRYPTED_DATA)
IMPLEMENT_ASN1_OBJECT(SM_ENCRYPTED_DATA)


ASN1_ITEM_TEMPLATE(SM_PROCESSING_STATUS) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_CONTEXT, 0x19, SM_PROCESSING_STATUS, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(SM_PROCESSING_STATUS)
IMPLEMENT_ASN1_FUNCTIONS(SM_PROCESSING_STATUS)
IMPLEMENT_ASN1_OBJECT(SM_PROCESSING_STATUS)


ASN1_ITEM_TEMPLATE(SM_CHECKSUM) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_CONTEXT, 0x0E, SM_CHECKSUM, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(SM_CHECKSUM)
IMPLEMENT_ASN1_FUNCTIONS(SM_CHECKSUM)
IMPLEMENT_ASN1_OBJECT(SM_CHECKSUM)


}  // namespace governikus


SecureMessagingResponse::SecureMessagingResponse(const QByteArray& pBuffer)
	: ResponseApdu()
	, mInvalid(true)
	, mEncryptedData()
	, mProcessingStatus()
	, mChecksum()
{
	ResponseApdu::setBuffer(pBuffer);
	QByteArray data = getData();

	if (auto tmp = decodeObject<SM_ENCRYPTED_DATA>(data, false))
	{
		mEncryptedData = tmp;
		QByteArray encryptedDataValue = Asn1OctetStringUtil::getValue(mEncryptedData.data());
		if (encryptedDataValue.isEmpty() || encryptedDataValue.at(0) != 0x01)
		{
			qCCritical(card) << "Error on encoding encrypted data";
			return;
		}
		data = data.mid(encodeObject(mEncryptedData.data()).length());
	}


	mProcessingStatus = decodeObject<SM_PROCESSING_STATUS>(data);
	if (mProcessingStatus == nullptr || mProcessingStatus->length != 2)
	{
		qCCritical(card) << "Error on encoding status";
		return;
	}
	data = data.mid(encodeObject(mProcessingStatus.data()).length());


	mChecksum = decodeObject<SM_CHECKSUM>(data);
	if (mChecksum == nullptr || mChecksum->length != 8)
	{
		qCCritical(card) << "Error on encoding mac";
		return;
	}
	mInvalid = false;
}


SecureMessagingResponse::~SecureMessagingResponse()
{
}


QByteArray SecureMessagingResponse::getEncryptedData() const
{
	return Asn1OctetStringUtil::getValue(mEncryptedData.data()).mid(1);
}


QByteArray SecureMessagingResponse::getEncryptedDataObjectEncoded() const
{
	return encodeObject(mEncryptedData.data());
}


QByteArray SecureMessagingResponse::getMac() const
{
	return Asn1OctetStringUtil::getValue(mChecksum.data());
}


StatusCode SecureMessagingResponse::getSecuredStatusCode() const
{
	if (mInvalid)
	{
		return StatusCode::INVALID_SM_OBJECTS;
	}
	bool ok;
	int statusCode = getSecuredStatusCodeBytes().toHex().toInt(&ok, 16);
	if (!ok)
	{
		qCCritical(card) << "Error on converting encoding status";
		return StatusCode::INVALID_SM_OBJECTS;
	}
	return StatusCode(statusCode);
}


QByteArray SecureMessagingResponse::getSecuredStatusCodeBytes() const
{
	return Asn1OctetStringUtil::getValue(mProcessingStatus.data());
}


QByteArray SecureMessagingResponse::getSecuredStatusCodeObjectEncoded() const
{
	return encodeObject(mProcessingStatus.data());
}


bool SecureMessagingResponse::isInvalid() const
{
	return mInvalid;
}
