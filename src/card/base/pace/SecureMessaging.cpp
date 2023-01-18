/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "apdu/SecureMessagingCommand.h"
#include "apdu/SecureMessagingResponse.h"
#include "asn1/ASN1Util.h"
#include "pace/SecureMessaging.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(secure)


const char ISO_LEADING_PAD_BYTE = static_cast<char>(0x80);
const char ISO_PAD_BYTE = 0x00;


SecureMessaging::SecureMessaging(const SecurityProtocol& pSecurityProtocol, const QByteArray& pEncKey, const QByteArray& pMacKey)
	: mCipher(pSecurityProtocol, pEncKey)
	, mCipherMac(pSecurityProtocol, pMacKey)
	, mSendSequenceCounter(0)
{
	qCDebug(secure) << "Encryption key:" << pEncKey.toHex();
	qCDebug(secure) << "MAC key:" << pMacKey.toHex();
}


bool SecureMessaging::isInitialized() const
{
	return mCipher.isInitialized() && mCipherMac.isInitialized();
}


QByteArray SecureMessaging::padToCipherBlockSize(const QByteArray& pData) const
{
	Q_ASSERT(!pData.isEmpty());

	const auto remainder = pData.size() % mCipher.getBlockSize();
	const auto paddingSize = (remainder == 0) ? mCipher.getBlockSize() : mCipher.getBlockSize() - remainder;

	QByteArray paddedData;
	paddedData += pData;
	paddedData += ISO_LEADING_PAD_BYTE;
	paddedData += QByteArray(paddingSize - 1, ISO_PAD_BYTE);
	return paddedData;
}


QByteArray SecureMessaging::unpadFromCipherBlockSize(const QByteArray& pData) const
{
	Q_ASSERT(!pData.isEmpty());

	if (pData.size() % mCipher.getBlockSize() != 0)
	{
		qCCritical(card) << "Size of data and block size is invalid";
		return QByteArray();
	}

	const auto position = pData.lastIndexOf(ISO_LEADING_PAD_BYTE);
	if (position == -1)
	{
		qCCritical(card) << "Cannot find padding delimiter! Message seems to be broken";
		return QByteArray();
	}

	return pData.mid(0, position);
}


CommandApdu SecureMessaging::encrypt(const CommandApdu& pCommandApdu)
{
	if (!isInitialized())
	{
		qCCritical(card) << "SecureMessaging not successfully initialized";
		return CommandApdu();
	}

	++mSendSequenceCounter;

	if (pCommandApdu.isEmpty())
	{
		qCCritical(card) << "CommandApdu is empty";
		return CommandApdu();
	}

	qCDebug(secure) << "Plain CommandApdu:" << pCommandApdu;

	QByteArray formattedEncryptedData;
	if (!pCommandApdu.getData().isEmpty())
	{
		QByteArray paddedCommandData = padToCipherBlockSize(pCommandApdu.getData());
		mCipher.setIv(getEncryptedIv());
		QByteArray encryptedData = mCipher.encrypt(paddedCommandData).prepend(0x01);

		auto encryptedDataObject = newObject<SM_ENCRYPTED_DATA>();
		Asn1OctetStringUtil::setValue(encryptedData, encryptedDataObject.data());
		formattedEncryptedData = encodeObject(encryptedDataObject.data());
	}

	QByteArray securedHeader = createSecuredHeader(pCommandApdu);
	QByteArray securedLe;
	if (pCommandApdu.getLe() > CommandApdu::NO_LE)
	{
		auto protectedLeObject = newObject<SM_PROTECTED_LE>();
		Asn1OctetStringUtil::setValue(createSecuredLe(pCommandApdu.getLe()), protectedLeObject.data());
		securedLe = encodeObject(protectedLeObject.data());
	}
	QByteArray mac = createMac(securedHeader, formattedEncryptedData, securedLe);
	QByteArray securedData = formattedEncryptedData + securedLe + mac;
	int newLe = createNewLe(securedData, pCommandApdu.getLe());

	return CommandApdu(securedHeader, securedData, newLe);
}


CommandApdu SecureMessaging::decrypt(const CommandApdu& pEncryptedCommandApdu)
{
	if (!isInitialized())
	{
		qCCritical(card) << "SecureMessaging not successfully initialized";
		return CommandApdu();
	}

	++mSendSequenceCounter;

	const SecureMessagingCommand secureCommand(pEncryptedCommandApdu);
	if (!secureCommand.isValid())
	{
		qCCritical(card) << "Secured CommandApdu is invalid";
		return CommandApdu();
	}

	if (createMac(pEncryptedCommandApdu.getHeaderBytes(), secureCommand.getEncryptedDataObjectEncoded(), secureCommand.getExpectedLengthObjectEncoded()).mid(2) != secureCommand.getMac())
	{
		qCCritical(card) << "MAC on secured CommandApdu does not match";
		return CommandApdu();
	}

	QByteArray decryptedData;
	if (!secureCommand.getEncryptedData().isEmpty())
	{
		mCipher.setIv(getEncryptedIv());
		QByteArray paddedDecryptedData = mCipher.decrypt(secureCommand.getEncryptedData());
		decryptedData = unpadFromCipherBlockSize(paddedDecryptedData);
	}

	CommandApdu encrypted(pEncryptedCommandApdu.getHeaderBytes(), decryptedData, secureCommand.getExpectedLength());
	const QByteArray macToCheck = decryptedData.right(10);
	if (macToCheck.size() != 10 || !macToCheck.startsWith(QByteArray::fromHex("8E08")))
	{
		encrypted.setSecureMessaging(false);
	}

	return encrypted;
}


QByteArray SecureMessaging::createSecuredHeader(const CommandApdu& pCommandApdu) const
{
	CommandApdu apdu(pCommandApdu.getHeaderBytes());
	apdu.setSecureMessaging(true);
	return apdu.getHeaderBytes();
}


QByteArray SecureMessaging::createSecuredLe(int pLe) const
{
	QByteArray buffer;
	if (pLe > CommandApdu::NO_LE)
	{
		if (pLe > CommandApdu::SHORT_MAX_LE)
		{
			buffer += static_cast<char>(pLe >> 0x08 & 0xFF);
		}
		buffer += static_cast<char>(pLe >> 0x00 & 0xFF);
	}
	return buffer;
}


QByteArray SecureMessaging::createMac(const QByteArray& pSecuredHeader,
		const QByteArray& pFormattedEncryptedData,
		const QByteArray& pSecuredLe)
{
	QByteArray dataToMac(pSecuredHeader);
	dataToMac = padToCipherBlockSize(dataToMac);
	dataToMac += pFormattedEncryptedData;
	dataToMac += pSecuredLe;
	if (!pFormattedEncryptedData.isNull() || !pSecuredLe.isNull())
	{
		dataToMac = padToCipherBlockSize(dataToMac);
	}
	dataToMac.prepend(getSendSequenceCounter());

	QByteArray mac = mCipherMac.generate(dataToMac);
	auto macObject = newObject<SM_CHECKSUM>();
	Asn1OctetStringUtil::setValue(mac, macObject.data());
	return encodeObject(macObject.data());
}


int SecureMessaging::createNewLe(const QByteArray& pSecuredData, int pOldLe) const
{
	if (CommandApdu::isExtendedLength(pSecuredData, pOldLe))
	{
		return CommandApdu::EXTENDED_MAX_LE;
	}
	return CommandApdu::SHORT_MAX_LE;
}


QByteArray SecureMessaging::getSendSequenceCounter() const
{
	QByteArray converted(sizeof(mSendSequenceCounter), '\0');
	qToBigEndian(mSendSequenceCounter, converted.data());
	return QByteArray(mCipher.getBlockSize() - converted.size(), 0x00) + converted;
}


QByteArray SecureMessaging::getEncryptedIv()
{
	mCipher.setIv(QByteArray().fill(0x00, mCipher.getBlockSize()));
	return mCipher.encrypt(getSendSequenceCounter());
}


ResponseApdu SecureMessaging::encrypt(const ResponseApdu& pResponseApdu)
{
	if (!isInitialized())
	{
		qCCritical(card) << "SecureMessaging not successfully initialized";
		return ResponseApdu();
	}

	++mSendSequenceCounter;

	QByteArray formattedEncryptedData;
	if (!pResponseApdu.getData().isEmpty())
	{
		QByteArray paddedResponseData = padToCipherBlockSize(pResponseApdu.getData());
		mCipher.setIv(getEncryptedIv());
		QByteArray encryptedData = mCipher.encrypt(paddedResponseData).prepend(0x01);

		auto encryptedDataObject = newObject<SM_ENCRYPTED_DATA>();
		Asn1OctetStringUtil::setValue(encryptedData, encryptedDataObject.data());
		formattedEncryptedData = encodeObject(encryptedDataObject.data());
	}

	const QByteArray status = pResponseApdu.getStatusBytes();
	auto formattedStatus = newObject<SM_PROCESSING_STATUS>();
	Asn1OctetStringUtil::setValue(status, formattedStatus.data());
	formattedEncryptedData += encodeObject(formattedStatus.data());

	QByteArray mac = mCipherMac.generate(getSendSequenceCounter() + padToCipherBlockSize(formattedEncryptedData));
	auto macObject = newObject<SM_CHECKSUM>();
	Asn1OctetStringUtil::setValue(mac, macObject.data());

	return ResponseApdu(formattedEncryptedData + encodeObject(macObject.data()) + status);
}


ResponseApdu SecureMessaging::decrypt(const ResponseApdu& pEncryptedResponseApdu)
{
	if (!isInitialized())
	{
		qCCritical(card) << "SecureMessaging not successfully initialized";
		return ResponseApdu();
	}

	++mSendSequenceCounter;

	SecureMessagingResponse secureResponse(pEncryptedResponseApdu);
	if (!secureResponse.isValid())
	{
		return ResponseApdu();
	}

	if (secureResponse.getSecuredStatusCode() != pEncryptedResponseApdu.getStatusCode())
	{
		qCCritical(card) << "SW1SW2 on secured ResponseApdu does not match";
		return ResponseApdu();
	}

	QByteArray dataToMac;
	if (!secureResponse.getEncryptedData().isEmpty())
	{
		dataToMac += secureResponse.getEncryptedDataObjectEncoded();
	}
	dataToMac += secureResponse.getSecuredStatusCodeObjectEncoded();
	dataToMac = padToCipherBlockSize(dataToMac);
	dataToMac.prepend(getSendSequenceCounter());
	if (mCipherMac.generate(dataToMac) != secureResponse.getMac())
	{
		qCCritical(card) << "MAC on secured ResponseApdu does not match";
		return ResponseApdu();
	}

	QByteArray decryptedData;
	if (!secureResponse.getEncryptedData().isEmpty())
	{
		mCipher.setIv(getEncryptedIv());
		QByteArray paddedDecryptedData = mCipher.decrypt(secureResponse.getEncryptedData());
		decryptedData = unpadFromCipherBlockSize(paddedDecryptedData);
	}

	const ResponseApdu response(decryptedData + secureResponse.getSecuredStatusCodeBytes());
	qCDebug(secure) << "Plain ResponseApdu:" << response;
	return response;
}
