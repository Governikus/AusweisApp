/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "SecureMessagingResponse.h"

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/ASN1Util.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


SecureMessagingResponse::SecureMessagingResponse(const ResponseApdu& pApdu)
	: SecureMessagingApdu()
	, mProcessingStatus()
	, mChecksum()
{
	QByteArray data = pApdu.getData();
	if (!decodeData(data))
	{
		return;
	}


	mProcessingStatus = decodeObject<SM_PROCESSING_STATUS>(data);
	if (mProcessingStatus == nullptr || mProcessingStatus->length != 2)
	{
		qCCritical(card) << "Error on decoding status";
		return;
	}
	data = data.mid(encodeObject(mProcessingStatus.data()).length());


	mChecksum = decodeObject<SM_CHECKSUM>(data);
	if (mChecksum == nullptr || mChecksum->length != 8)
	{
		qCCritical(card) << "Error on decoding mac";
		return;
	}

	setValid();
}


QByteArray SecureMessagingResponse::getMac() const
{
	return Asn1OctetStringUtil::getValue(mChecksum.data());
}


StatusCode SecureMessagingResponse::getSecuredStatusCode() const
{
	if (!isValid())
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
