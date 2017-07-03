/*!
 * \brief Mock a QNetworkReply for tests.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <QNetworkReply>

namespace governikus
{

class MockNetworkReply
	: public QNetworkReply
{
	Q_OBJECT

	private:
		const QByteArray mData;
		int mDataPosition;

	public:
		MockNetworkReply(const QByteArray& pData = QByteArray(), QObject* pParent = nullptr);
		virtual ~MockNetworkReply();
		virtual void abort() override
		{

		}


		virtual qint64 readData(char* pDst, qint64 pMaxSize) override;

		void fireFinished()
		{
			Q_EMIT finished();
		}


		void setNetworkError(NetworkError pErrorCode, const QString& pErrorString)
		{
			setError(pErrorCode, pErrorString);
		}


};

} /* namespace governikus */
