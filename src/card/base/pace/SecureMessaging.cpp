/*!
 * SecureMessaging.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */
#include "asn1/ASN1Util.h"
#include "pace/SecureMessaging.h"
#include "SecureMessagingResponse.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(secure)


const char ISO_LEADING_PAD_BYTE = char(0x80);
const char ISO_PAD_BYTE = 0x00;


namespace governikus
{

ASN1_ITEM_TEMPLATE(SM_PROTECTED_LE) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_CONTEXT, 0x17, SM_PROTECTED_LE, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(SM_PROTECTED_LE)
IMPLEMENT_ASN1_FUNCTIONS(SM_PROTECTED_LE)
IMPLEMENT_ASN1_OBJECT(SM_PROTECTED_LE)


}  // namespace governikus


SecureMessaging::SecureMessaging(const QByteArray& pPaceAlgorithm, const QByteArray& pEncKey, const QByteArray& pMacKey)
	: mCipher(pPaceAlgorithm, pEncKey)
	, mCipherMac(pPaceAlgorithm, pMacKey)
	, mSendSequenceCounter(0)
{
	qCDebug(secure) << "Encryption key: " << pEncKey.toHex();
	qCDebug(secure) << "MAC key:" << pMacKey.toHex();
}


SecureMessaging::~SecureMessaging()
{
}


bool SecureMessaging::isInitialized()
{
	return mCipher.isInitialized() && mCipherMac.isInitialized();
}


QByteArray SecureMessaging::padToCipherBlockSize(const QByteArray& pData) const
{
	Q_ASSERT(!pData.isEmpty());

	int remainder = pData.size() % mCipher.getBlockSize();
	int paddingSize = (remainder == 0) ? mCipher.getBlockSize() : mCipher.getBlockSize() - remainder;

	QByteArray paddedData;
	paddedData.append(pData);
	paddedData.append(ISO_LEADING_PAD_BYTE);
	paddedData.append(QByteArray().fill(ISO_PAD_BYTE, paddingSize - 1));
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

	int position = pData.lastIndexOf(ISO_LEADING_PAD_BYTE);
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
		return pCommandApdu;
	}

	++mSendSequenceCounter;

	qCDebug(secure) << "Plain CommandApdu: " << pCommandApdu.getBuffer().toHex();

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
	if (pCommandApdu.getLe() > Apdu::NO_LE)
	{
		auto protectedLeObject = newObject<SM_PROTECTED_LE>();
		Asn1OctetStringUtil::setValue(createSecuredLe(pCommandApdu.getLe()), protectedLeObject.data());
		securedLe = encodeObject(protectedLeObject.data());
	}
	QByteArray mac = createMac(securedHeader, formattedEncryptedData, securedLe);
	QByteArray securedData = QByteArray().append(formattedEncryptedData).append(securedLe).append(mac);
	int newLe = createNewLe(securedData, pCommandApdu.getLe());

	return CommandApdu(securedHeader, securedData, newLe);
}


QByteArray SecureMessaging::createSecuredHeader(const CommandApdu& pCommandApdu) const
{
	QByteArray securedHeader;
	securedHeader.append((pCommandApdu.getCLA() & 0xF0) | Apdu::CLA_SECURE_MESSAGING);
	securedHeader.append(pCommandApdu.getINS());
	securedHeader.append(pCommandApdu.getP1());
	securedHeader.append(pCommandApdu.getP2());
	return securedHeader;
}


QByteArray SecureMessaging::createSecuredLe(int pLe)
{
	QByteArray buffer;
	if (pLe > Apdu::NO_LE)
	{
		if (pLe > Apdu::SHORT_MAX_LE)
		{
			buffer.append(pLe >> 0x08 & 0xff);
		}
		buffer.append(pLe >> 0x00 & 0xff);
	}
	return buffer;
}


QByteArray SecureMessaging::createMac(const QByteArray& pSecuredHeader,
		const QByteArray& pFormattedEncryptedData,
		const QByteArray& pSecuredLe)
{
	QByteArray dataToMac(pSecuredHeader);
	dataToMac = padToCipherBlockSize(dataToMac);
	dataToMac.append(pFormattedEncryptedData);
	dataToMac.append(pSecuredLe);
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
	QByteArray ssc = toBigEndian(mSendSequenceCounter);
	return QByteArray().fill(0x00, mCipher.getBlockSize() - ssc.size()).append(ssc);
}


QByteArray SecureMessaging::getEncryptedIv()
{
	mCipher.setIv(QByteArray().fill(0x00, mCipher.getBlockSize()));
	return mCipher.encrypt(getSendSequenceCounter());
}


bool SecureMessaging::decrypt(const ResponseApdu& pEncryptedResponseApdu, ResponseApdu& pDecryptedResponseApdu)
{
	if (!isInitialized())
	{
		qCCritical(card) << "SecureMessaging not successfully initialized";
		return false;
	}

	++mSendSequenceCounter;

	SecureMessagingResponse secureResponse(pEncryptedResponseApdu.getBuffer());
	if (secureResponse.isInvalid())
	{
		return false;
	}
	if (secureResponse.getSecuredStatusCode() != secureResponse.getReturnCode())
	{
		qCCritical(card) << "SW1SW2 on secured ResponseApdu does not match";
		return false;
	}

	QByteArray dataToMac;
	if (!secureResponse.getEncryptedData().isEmpty())
	{
		dataToMac.append(secureResponse.getEncryptedDataObjectEncoded());
	}
	dataToMac.append(secureResponse.getSecuredStatusCodeObjectEncoded());
	dataToMac = padToCipherBlockSize(dataToMac);
	dataToMac.prepend(getSendSequenceCounter());
	if (mCipherMac.generate(dataToMac) != secureResponse.getMac())
	{
		qCCritical(card) << "MAC on secured ResponseApdu does not match";
		return false;
	}

	QByteArray decryptedData;
	if (!secureResponse.getEncryptedData().isEmpty())
	{
		mCipher.setIv(getEncryptedIv());
		QByteArray paddedDecryptedData = mCipher.decrypt(secureResponse.getEncryptedData());
		decryptedData = unpadFromCipherBlockSize(paddedDecryptedData);
	}

	pDecryptedResponseApdu.setBuffer(decryptedData.append(secureResponse.getSecuredStatusCodeBytes()));

	qCDebug(secure) << "Plain ResponseApdu: " << pDecryptedResponseApdu.getBuffer().toHex();

	return true;
}
