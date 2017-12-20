/*!
 * \brief Mock a QAbstractSocket for tests.
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
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
		int mReaderBufferPosition;
		int mReaderBufferChunk;
		QByteArray mWriteBuffer;

		MockSocket();
		virtual ~MockSocket() override;

		qint64 bytesAvailable() const override;
		qint64 readData(char* pDestination, qint64 pMaxSize) override;
		qint64 writeData(const char* pData, qint64 pMaxSize) override;
};

} /* namespace governikus */
