/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MockSocket.h"

using namespace governikus;

MockSocket::MockSocket()
	: QTcpSocket()
	, mReadBuffer()
	, mReaderBufferPosition(0)
	, mReaderBufferChunk(-1)
	, mWriteBuffer()
{
	open(QIODevice::ReadWrite);
}


MockSocket::~MockSocket()
{
}


qint64 MockSocket::bytesAvailable() const
{
	return mReadBuffer.size() - mReaderBufferPosition;
}


qint64 MockSocket::readData(char* pDestination, qint64 pMaxSize)
{
	Q_ASSERT(pMaxSize <= INT_MAX);

	int chunk = static_cast<int>(pMaxSize);
	if (mReaderBufferChunk > -1 && mReaderBufferChunk < chunk)
	{
		chunk = mReaderBufferChunk;
	}

	QByteArray data = mReadBuffer.mid(mReaderBufferPosition, chunk);
	int length = data.length();
	if (length >= 0)
	{
		qstrncpy(pDestination, data.constData(), static_cast<uint>(length));
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
