/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Mock a QAbstractSocket for tests.
 */

#pragma once

#include <QByteArray>
#include <QTcpSocket>


namespace governikus
{

class MockSocket
	: public QTcpSocket
{
	Q_OBJECT

	public:
		QByteArray mReadBuffer;
		qint64 mReaderBufferPosition;
		qint64 mReaderBufferChunk;
		QByteArray mWriteBuffer;

		MockSocket();
		~MockSocket() override;

		void setSocketState(QAbstractSocket::SocketState pState)
		{
			QTcpSocket::setSocketState(pState);
		}


		[[nodiscard]] qint64 bytesAvailable() const override;
		qint64 readData(char* pDestination, qint64 pMaxSize) override;
		qint64 writeData(const char* pData, qint64 pMaxSize) override;
		qint64 write(const QByteArray& pByteArray);
};

} // namespace governikus
