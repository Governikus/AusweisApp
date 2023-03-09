/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MockSocket.h"

#include <cstring>

using namespace governikus;

MockSocket::MockSocket()
	: QTcpSocket()
	, mReadBuffer()
	, mReaderBufferPosition(0)
	, mReaderBufferChunk(-1)
	, mWriteBuffer()
{
	QTcpSocket::open(QIODevice::ReadWrite);
}


MockSocket::~MockSocket()
{
}


qint64 MockSocket::bytesAvailable() const
{
	return mReadBuffer.size() - mReaderBufferPosition + QTcpSocket::bytesAvailable();
}


qint64 MockSocket::readData(char* pDestination, qint64 pMaxSize)
{
	Q_ASSERT(pMaxSize <= INT_MAX);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	auto chunk = pMaxSize;
#else
	auto chunk = static_cast<int>(pMaxSize);
#endif
	if (mReaderBufferChunk > -1 && mReaderBufferChunk < chunk)
	{
		chunk = mReaderBufferChunk;
	}

	const QByteArray data = mReadBuffer.mid(mReaderBufferPosition, chunk);
	const auto length = data.length();
	if (length >= 0)
	{
		memcpy(pDestination, data.constData(), static_cast<size_t>(length));
		mReaderBufferPosition += length;
	}
	return length;
}


qint64 MockSocket::writeData(const char* pData, qint64 pMaxSize)
{
	const auto& data = QByteArray(pData, static_cast<int>(pMaxSize));
	mWriteBuffer += data;
	return data.size();
}


qint64 MockSocket::write(const QByteArray& pByteArray)
{
	mWriteBuffer += pByteArray;
	return QIODevice::write(pByteArray);
}
