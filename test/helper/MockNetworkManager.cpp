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


QNetworkReply* MockNetworkManager::paos(QNetworkRequest& pRequest, const QByteArray& pData, bool pUsePsk, int pTimeoutInMilliSeconds)
{
	Q_UNUSED(pRequest);
	Q_UNUSED(pData);
	Q_UNUSED(pUsePsk);
	Q_UNUSED(pTimeoutInMilliSeconds);

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

	return mLastReply;
}
