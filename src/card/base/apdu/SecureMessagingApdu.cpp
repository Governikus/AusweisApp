/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "SecureMessagingApdu.h"

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/ASN1Util.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


SecureMessagingApdu::SecureMessagingApdu()
	: mValid(false)
	, mEncryptedData()
{
}


bool SecureMessagingApdu::decodeData(QByteArray& pData)
{
	if (pData.isEmpty())
	{
		qCCritical(card) << "No data to decrypt";
		return false;
	}

	if (auto tmp = decodeObject<SM_ENCRYPTED_DATA>(pData, false))
	{
		mEncryptedData = tmp;
		const auto encryptedDataValue = Asn1OctetStringUtil::getValue(mEncryptedData.data());
		if (encryptedDataValue.isEmpty() || encryptedDataValue.at(0) != 0x01)
		{
			qCCritical(card) << "Error on decoding encrypted data";
			return false;
		}

		pData = pData.mid(encodeObject(mEncryptedData.data()).length());
	}

	return true;
}


void SecureMessagingApdu::setValid()
{
	mValid = true;
}


QByteArray SecureMessagingApdu::getEncryptedData() const
{
	return Asn1OctetStringUtil::getValue(mEncryptedData.data()).mid(1);
}


QByteArray SecureMessagingApdu::getEncryptedDataObjectEncoded() const
{
	return encodeObject(mEncryptedData.data());
}


bool SecureMessagingApdu::isValid() const
{
	return mValid;
}
