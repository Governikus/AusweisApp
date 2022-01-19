/*!
 * \brief Mock \ref NetworkManager for tests
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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
		QNetworkRequest mLastRequest;
		QByteArray mLastData;

		MockNetworkReply* getReply(const QNetworkRequest& pRequest);

	public:
		MockNetworkManager();
		~MockNetworkManager() override;
		QNetworkReply* paos(QNetworkRequest& pRequest,
				const QByteArray& pNamespace,
				const QByteArray& pData,
				bool pUsePsk = true,
				const QByteArray& pSslSession = QByteArray(),
				int pTimeoutInMilliSeconds = 30000) override;
		QNetworkReply* get(QNetworkRequest& pRequest,
				const QList<QSslCertificate>& pCaCerts = QList<QSslCertificate>(),
				const QByteArray& pSslSession = QByteArray(),
				int pTimeoutInMilliSeconds = 30000) override;
		QNetworkReply* post(QNetworkRequest& pRequest,
				const QByteArray& pData,
				const QList<QSslCertificate>& pCaCerts = QList<QSslCertificate>(),
				int pTimeoutInMilliSeconds = 30000) override;

		bool checkUpdateServerCertificate(const QSharedPointer<const QNetworkReply>& pReply) override;

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


		[[nodiscard]] QNetworkRequest getLastRequest() const
		{
			return mLastRequest;
		}


		[[nodiscard]] const QByteArray getLastData() const
		{
			return mLastData;
		}

	Q_SIGNALS:
		void fireReply();
};

} // namespace governikus
