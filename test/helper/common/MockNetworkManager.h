/*!
 * \brief Mock \ref NetworkManager for tests
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
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
		QSharedPointer<MockNetworkReply> mNextReply;
		QSharedPointer<MockNetworkReply> mLastReply;
		QNetworkRequest mLastRequest;
		QByteArray mLastData;

		[[nodiscard]] QSharedPointer<MockNetworkReply> getReply(const QNetworkRequest& pRequest);

	public:
		MockNetworkManager();
		~MockNetworkManager() override;
		[[nodiscard]] QSharedPointer<QNetworkReply> paos(QNetworkRequest& pRequest,
				const QByteArray& pNamespace,
				const QByteArray& pData,
				bool pUsePsk = true,
				const QByteArray& pSslSession = QByteArray()) override;
		[[nodiscard]] QSharedPointer<QNetworkReply> get(QNetworkRequest& pRequest) override;
		[[nodiscard]] QSharedPointer<QNetworkReply> post(QNetworkRequest& pRequest, const QByteArray& pData) override;

		void setFilename(const QString& pFilename)
		{
			mFilename = pFilename;
		}


		void setNextReply(MockNetworkReply* pNextReply)
		{
			mNextReply = QSharedPointer<MockNetworkReply>(pNextReply, &QObject::deleteLater);
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
