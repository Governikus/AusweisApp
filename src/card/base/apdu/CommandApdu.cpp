/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "CommandApdu.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)

constexpr std::byte CLA {0x00};
constexpr std::byte CLA_PROPRIETARY {0x80};
constexpr std::byte CLA_COMMAND_CHAINING {0x10};
constexpr std::byte CLA_SECURE_MESSAGING {0x0C};


[[nodiscard]] static inline int processLe(int pRawLe, bool pExtendedLength)
{
	if (pRawLe > 0)
	{
		return pRawLe;
	}

	return pExtendedLength
			? CommandApdu::EXTENDED_MAX_LE
			: CommandApdu::SHORT_MAX_LE;
}


bool CommandApdu::isExtendedLength(const QByteArray& pData, int pLe)
{
	return pData.size() > SHORT_MAX_LC || pLe > SHORT_MAX_LE;
}


CommandApdu::CommandApdu(const QByteArray& pBuffer)
	: CommandApdu(pBuffer.left(4), QByteArray(), NO_LE)
{
	auto buffer = pBuffer.mid(4);
	if (buffer.isEmpty())
	{
		return;
	}

	bool extendedLength = false;
	int length = qFromBigEndian<quint8>(buffer.data());
	buffer.remove(0, 1);

	if (length == 0 && !buffer.isEmpty())
	{
		if (buffer.size() < 2)
		{
			qCCritical(card) << "Unexpected end of data";
			return;
		}
		extendedLength = true;
		length = qFromBigEndian<quint16>(buffer.data());
		buffer.remove(0, 2);
	}

	if (buffer.isEmpty())
	{
		mLe = processLe(length, extendedLength);
		return;
	}

	if (buffer.size() < length)
	{
		qCCritical(card) << "Unexpected end of data";
		return;
	}

	mData = buffer.left(length);
	buffer.remove(0, length);
	if (buffer.isEmpty())
	{
		return;
	}

	if (extendedLength)
	{
		if (buffer.size() < 2)
		{
			buffer.prepend('\0');
			qCCritical(card) << "Extended length expected";
		}
		mLe = processLe(qFromBigEndian<quint16>(buffer.data()), extendedLength);
		buffer.remove(0, 2);
	}
	else
	{
		mLe = processLe(qFromBigEndian<quint8>(buffer.data()), false);
		buffer.remove(0, 1);
	}

	if (!buffer.isEmpty())
	{
		qCCritical(card) << "Unexpected additional data:" << buffer.toHex();
	}
}


CommandApdu::CommandApdu(const QByteArray& pHeader, const QByteArray& pData, int pLe)
	:
	mCla(static_cast<std::byte>(pHeader.size() > 0 ? pHeader.at(0) : 0)),
	mIns(pHeader.size() > 1 ? static_cast<uchar>(pHeader.at(1)) : 0),
	mP1(pHeader.size() > 2 ? static_cast<uchar>(pHeader.at(2)) : 0),
	mP2(pHeader.size() > 3 ? static_cast<uchar>(pHeader.at(3)) : 0),
	mData(pData),
	mLe(pLe)
{
	if (pHeader.size() > 0 && pHeader.size() != 4)
	{
		qCCritical(card) << "Wrong command header size!";
	}

	if (mData.size() > EXTENDED_MAX_LC)
	{
		qCCritical(card).nospace() << "Command data exceeds maximum of 0x" << Qt::hex << EXTENDED_MAX_LC;
	}

	if (mLe > EXTENDED_MAX_LE)
	{
		qCCritical(card).nospace() << "Expected length exceeds maximum value of 0x" << Qt::hex << EXTENDED_MAX_LE;
	}
}


CommandApdu::CommandApdu(Ins pIns, uchar pP1, uchar pP2, const QByteArray& pData, int pLe)
	: mCla(CLA)
	, mIns(Enum<Ins>::getValue(pIns))
	, mP1(pP1)
	, mP2(pP2)
	, mData(pData)
	, mLe(pLe)
{
	if (mData.size() > EXTENDED_MAX_LC)
	{
		qCCritical(card).nospace() << "Command data exceeds maximum of 0x" << Qt::hex << EXTENDED_MAX_LC;
	}

	if (mLe > EXTENDED_MAX_LE)
	{
		qCCritical(card).nospace() << "Expected length exceeds maximum value of 0x" << Qt::hex << EXTENDED_MAX_LE;
	}
}


CommandApdu::~CommandApdu() = default;


bool CommandApdu::isProprietary() const
{
	return (mCla & CLA_PROPRIETARY) == CLA_PROPRIETARY;
}


void CommandApdu::enableCommandChaining()
{
	mCla |= CLA_COMMAND_CHAINING;
}


bool CommandApdu::isCommandChaining() const
{
	return (mCla & CLA_COMMAND_CHAINING) == CLA_COMMAND_CHAINING;
}


void CommandApdu::setSecureMessaging(bool pEnabled)
{
	if (pEnabled)
	{
		mCla |= CLA_SECURE_MESSAGING;
		return;
	}

	mCla &= ~CLA_SECURE_MESSAGING;
}


bool CommandApdu::isSecureMessaging() const
{
	return (mCla & CLA_SECURE_MESSAGING) == CLA_SECURE_MESSAGING;
}


bool CommandApdu::isEmpty() const
{
	return mIns == Ins::UNKNOWN;
}


Ins CommandApdu::getINS() const
{
	if (mIns != Ins::UNKNOWN && Enum<Ins>::isValue(mIns))
	{
		return Ins(mIns);
	}

	qCCritical(card).nospace().noquote() << "Unknown INS value, returning UNKNOWN, value: 0x" << Qt::hex << mIns;
	return Ins::UNKNOWN;
}


uchar CommandApdu::getP1() const
{
	return mP1;
}


uchar CommandApdu::getP2() const
{
	return mP2;
}


QByteArray CommandApdu::getHeaderBytes() const
{
	QByteArray header;
	header += std::to_integer<char>(mCla);
	header += static_cast<char>(mIns);
	header += static_cast<char>(mP1);
	header += static_cast<char>(mP2);
	return header;
}


const QByteArray& CommandApdu::getData() const
{
	return mData;
}


int CommandApdu::getLe() const
{
	return mLe;
}


bool CommandApdu::isExtendedLength() const
{
	return isExtendedLength(mData, mLe);
}


QByteArray CommandApdu::generateLengthField(int pLength) const
{
	QByteArray field;
	if (isExtendedLength())
	{
		field += static_cast<char>(pLength >> 8 & 0xFF);
	}
	field += static_cast<char>(pLength & 0xFF);

	return field;
}


CommandApdu::operator QByteArray() const
{
	if (mData.size() > EXTENDED_MAX_LC || mLe > EXTENDED_MAX_LE)
	{
		return QByteArray();
	}

	QByteArray cmd = getHeaderBytes();

	// According to ISO-7816-4, 5.2 Syntax
	if (isExtendedLength())
	{
		cmd += '\0';
	}

	if (mData.size() > 0)
	{
		cmd += generateLengthField(static_cast<int>(mData.size()));
		cmd += mData;
	}

	if (mLe > 0)
	{
		cmd += generateLengthField(mLe);
	}

	return cmd;
}


#if !defined(QT_NO_DEBUG) && __has_include(<QTest>)
	#include <QTest>
char* governikus::toString(const CommandApdu& pCommandApdu)
{
	return QTest::toString(QByteArray(pCommandApdu).toHex());
}


#endif


#include "moc_CommandApdu.cpp"
