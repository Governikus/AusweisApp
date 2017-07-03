/*!
 * Apdu.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "Apdu.h"
#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


Apdu::Apdu(const QByteArray& pBuffer)
	: mBuffer(pBuffer)
{
}


Apdu::~Apdu()
{
}


size_t Apdu::length() const
{
	return mBuffer.size();
}


const QByteArray& Apdu::getBuffer() const
{
	return mBuffer;
}


CommandApdu::CommandApdu(const QByteArray& pBuffer)
	: Apdu(pBuffer)
{
}


bool CommandApdu::isExtendedLength() const
{
	// no data, no le: size == 4
	// no data, short le: size == 5
	// no data, extended le: size == 7, high order le byte == 0
	// data, with/without le: high order size byte == 0 <=> extended data length
	return mBuffer.size() > 5 && mBuffer.at(4) == static_cast<char>(0x00);
}


bool CommandApdu::isExtendedLength(const QByteArray& pData, int pLe)
{
	return pData.size() > SHORT_MAX_LC || pLe > SHORT_MAX_LE;
}


bool CommandApdu::isSecureMessaging(const QByteArray& pCommandBuffer)
{
	return !pCommandBuffer.isEmpty() && ((pCommandBuffer.at(0) & 0x0F) == CLA_SECURE_MESSAGING);
}


CommandApdu::CommandApdu(const QByteArray& pHeader, const QByteArray& pData, int pLe)
	: CommandApdu(pHeader.at(0), pHeader.at(1), pHeader.at(2), pHeader.at(3), pData, pLe)
{
}


CommandApdu::CommandApdu(char pCla, char pIns, char pP1, char pP2, const QByteArray& pData, int pLe)
	: Apdu(QByteArray())
{
	if (pData.size() > EXTENDED_MAX_LC)
	{
		qCCritical(card) << "Command data exceeds maximum of 0xFFFF bytes";
		return;
	}
	if (pLe > EXTENDED_MAX_LE)
	{
		qCCritical(card) << "Expected length exceeds maximum value of 0x010000";
		return;
	}

	mBuffer.append(pCla);
	mBuffer.append(pIns);
	mBuffer.append(pP1);
	mBuffer.append(pP2);

	//
	// according to ISO 7816 Part 4, chapter 5.1
	//
	if (pData.size() > 0)  // withPayload
	{
		if (CommandApdu::isExtendedLength(pData, pLe))
		{
			mBuffer.append(char(0x00));
			mBuffer.append(pData.size() >> 8 & 0xff);
			mBuffer.append(pData.size() & 0xff);
		}
		else
		{
			mBuffer.append(pData.size() & 0xff);
		}

		mBuffer.append(pData);
	}

	if (pLe > 0)
	{
		if (CommandApdu::isExtendedLength(pData, pLe))
		{
			if (pData.isEmpty())
			{
				mBuffer.append(char(0x00));
			}
			mBuffer.append(pLe >> 8 & 0xff);
		}
		mBuffer.append(pLe & 0xff);
	}
}


CommandApdu::~CommandApdu()
{
}


char CommandApdu::getCLA() const
{
	return mBuffer.size() > 0 ? mBuffer.at(0) : 0;
}


char CommandApdu::getINS() const
{
	return mBuffer.size() > 1 ? mBuffer.at(1) : 0;
}


char CommandApdu::getP1() const
{
	return mBuffer.size() > 2 ? mBuffer.at(2) : 0;
}


char CommandApdu::getP2() const
{
	return mBuffer.size() > 3 ? mBuffer.at(3) : 0;
}


static inline int readLength(const QByteArray& pByteArray, int pOffset)
{
	return static_cast<int>(static_cast<uchar>(pByteArray.at(pOffset)) << 8) | static_cast<int>(static_cast<uchar>(pByteArray.at(pOffset + 1)));
}


int CommandApdu::getLc() const
{
	if (mBuffer.size() <= 5)
	{
		return 0;
	}
	if (!isExtendedLength())
	{
		// short command apdu
		return static_cast<uchar>(mBuffer.at(4));
	}
	// extended length command apdu
	if (mBuffer.size() <= 6)
	{
		qCCritical(card) << "Cannot determine Lc, returning 0";
		return 0;
	}
	return readLength(mBuffer, 5);
}


int CommandApdu::getLe() const
{
	int lc = getLc();
	if (isExtendedLength())
	{
		// no data (so lc==0): we have 4 bytes header and the le field is prefixed with 0 byte
		// with data: we have 4 bytes header, lc field encoded in 3 bytes and the data field
		int offset = lc == 0 ? 5 : 7 + lc;
		if (mBuffer.size() < offset + 2)
		{
			return 0;
		}
		int le = readLength(mBuffer, offset);
		return le == 0 ? EXTENDED_MAX_LE : le;
	}
	// no data (so lc==0): we have 4 bytes header
	// with data: we have 4 bytes header, lc field encoded in 1 byte and the data field
	int offset = lc == 0 ? 4 : 5 + lc;
	if (mBuffer.size() < offset + 1)
	{
		return 0;
	}
	int le = static_cast<uchar>(mBuffer.at(offset));
	return le == 0 ? SHORT_MAX_LE : le;
}


QByteArray CommandApdu::getData() const
{
	int lc = getLc();
	if (lc == 0)
	{
		return QByteArray();
	}
	if (isExtendedLength())
	{
		// 4 bytes header + 3 bytes length
		return mBuffer.mid(7, lc);
	}
	// 4 bytes header + 1 bytes length
	return mBuffer.mid(5, lc);
}


ResponseApdu::ResponseApdu(const QByteArray& pBuffer)
	: Apdu(pBuffer)
{
}


ResponseApdu::~ResponseApdu()
{
}


void ResponseApdu::setBuffer(const QByteArray& pBuffer)
{
	mBuffer = pBuffer;
}


QByteArray ResponseApdu::getData() const
{
	if (length() < 2)
	{
		return QByteArray();
	}

	return mBuffer.left(getDataLength());
}


int ResponseApdu::getDataLength() const
{
	return mBuffer.size() - 2;
}


StatusCode ResponseApdu::getReturnCode() const
{
	unsigned int sw1 = getSW1();
	unsigned int sw2 = getSW2();
	return StatusCode(((sw1 << 8) & 0xff00) + (sw2 & 0xff));
}


int ResponseApdu::getSW1() const
{
	if (mBuffer.size() < 2)
	{
		qCCritical(card) << "Buffer too short, returning 0";
		return 0;
	}
	return mBuffer.at(mBuffer.size() - 2);
}


int ResponseApdu::getSW2() const
{
	if (mBuffer.size() < 1)
	{
		qCCritical(card) << "Buffer too short, returning 0";
		return 0;
	}
	return mBuffer.at(mBuffer.size() - 1);
}


#include "moc_Apdu.cpp"
