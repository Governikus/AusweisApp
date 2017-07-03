/*!
 * \brief Mock \ref NetworkManager for tests
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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

		MockNetworkReply* getReply(const QNetworkRequest& pRequest);

	public:
		MockNetworkManager();
		virtual ~MockNetworkManager();
		virtual QNetworkReply* get(QNetworkRequest& pRequest, int pTimeoutInMilliSeconds = 30000) override;
		virtual QNetworkReply* paos(QNetworkRequest& pRequest, const QByteArray& pData, bool pUsePsk = true, int pTimeoutInMilliSeconds = 30000) override;

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


};

} /* namespace governikus */
