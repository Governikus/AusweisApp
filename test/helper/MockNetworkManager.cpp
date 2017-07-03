/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "MockNetworkManager.h"

#include <QTest>

using namespace governikus;


MockNetworkManager::MockNetworkManager()
	: mNextReply(nullptr)
	, mLastReply(nullptr)
{
}


MockNetworkManager::~MockNetworkManager()
{
}


MockNetworkReply* MockNetworkManager::getReply(const QNetworkRequest& pRequest)
{
	if (mNextReply)
	{
		mLastReply = mNextReply;
		mNextReply = nullptr;
	}
	else
	{
		QByteArray content;
		if (!mFilename.isNull())
		{
			QFile msgFile(mFilename);
			msgFile.open(QIODevice::ReadOnly | QIODevice::Text);
			content = msgFile.readAll();
			msgFile.close();
		}

		mLastReply = new MockNetworkReply(content);
	}

	mLastReply->setRequest(pRequest);
	return mLastReply;
}


QNetworkReply* MockNetworkManager::get(QNetworkRequest& pRequest, int pTimeoutInMilliSeconds)
{
	Q_UNUSED(pRequest);
	Q_UNUSED(pTimeoutInMilliSeconds);

	return getReply(pRequest);
}


QNetworkReply* MockNetworkManager::paos(QNetworkRequest& pRequest, const QByteArray& pData, bool pUsePsk, int pTimeoutInMilliSeconds)
{
	Q_UNUSED(pRequest);
	Q_UNUSED(pData);
	Q_UNUSED(pUsePsk);
	Q_UNUSED(pTimeoutInMilliSeconds);

	return getReply(pRequest);
}
