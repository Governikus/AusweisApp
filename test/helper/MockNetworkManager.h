/*!
 * \brief Mock \ref NetworkManager for tests
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MockNetworkReply.h"
#include "NetworkManager.h"

namespace governikus
{

class MockNetworkManager
	: public NetworkManager
{
	Q_OBJECT

	private:
		QString mFilename;
		MockNetworkReply* mNextReply;
		MockNetworkReply* mLastReply;
		QNetworkRequest* mLastRequest;
		QByteArray mLastData;

		MockNetworkReply* getReply(const QNetworkRequest& pRequest);

	public:
		MockNetworkManager();
		virtual ~MockNetworkManager() override;
		virtual QNetworkReply* paos(QNetworkRequest& pRequest,
				const QByteArray& pNamespace,
				const QByteArray& pData,
				bool pUsePsk = true,
				const QByteArray& pSslSession = QByteArray(),
				int pTimeoutInMilliSeconds = 30000) override;
		virtual QNetworkReply* get(QNetworkRequest& pRequest,
				const QByteArray& pSslSession = QByteArray(),
				int pTimeoutInMilliSeconds = 30000) override;
		virtual QNetworkReply* post(QNetworkRequest& pRequest,
				const QByteArray& pData,
				int pTimeoutInMilliSeconds = 30000) override;

		virtual bool checkUpdateServerCertificate(const QSharedPointer<const QNetworkReply>& pReply) override;

		void setFilename(const QString& pFilename)
		{
			mFilename = pFilename;
		}


		void setNextReply(MockNetworkReply* pNextReply)
		{
			mNextReply = pNextReply;
		}


		void fireFinished()
		{
			Q_ASSERT(mLastReply != nullptr);
			mLastReply->fireFinished();
			mLastReply = nullptr;
		}


		QNetworkRequest* getLastRequest() const
		{
			return mLastRequest;
		}


		const QByteArray getLastData() const
		{
			return mLastData;
		}

	Q_SIGNALS:
		void fireReply();
};

} // namespace governikus
