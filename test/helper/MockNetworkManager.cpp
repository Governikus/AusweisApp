/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "MockNetworkManager.h"

#include "HttpStatusCode.h"

#include <QTest>

using namespace governikus;


MockNetworkManager::MockNetworkManager()
	: mNextReply(nullptr)
	, mLastReply(nullptr)
	, mLastRequest(nullptr)
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
			if (msgFile.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				content = msgFile.readAll();
				msgFile.close();
			}

			mLastReply = new MockNetworkReply(content, HttpStatusCode::OK);
		}
		else
		{
			mLastReply = new MockNetworkReply();
		}
	}

	mLastReply->setRequest(pRequest);
	Q_EMIT fireReply();
	return mLastReply;
}


QNetworkReply* MockNetworkManager::get(QNetworkRequest& pRequest, int pTimeoutInMilliSeconds)
{
	Q_UNUSED(pRequest);
	Q_UNUSED(pTimeoutInMilliSeconds);

	mLastRequest = &pRequest;

	return getReply(pRequest);
}


QNetworkReply* MockNetworkManager::paos(QNetworkRequest& pRequest, const QByteArray& pNamespace, const QByteArray& pData, bool pUsePsk, int pTimeoutInMilliSeconds)
{
	Q_UNUSED(pRequest);
	Q_UNUSED(pNamespace);
	Q_UNUSED(pData);
	Q_UNUSED(pUsePsk);
	Q_UNUSED(pTimeoutInMilliSeconds);

	return getReply(pRequest);
}


bool MockNetworkManager::checkUpdateServerCertificate(const QNetworkReply& pReply)
{
	Q_UNUSED(pReply);

	return true;
}
