/*!
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

#include "MockNetworkManager.h"

#include <http_parser.h>
#include <QTest>

using namespace governikus;


MockNetworkManager::MockNetworkManager()
	: mNextReply(nullptr)
	, mLastReply(nullptr)
	, mLastRequest()
{
}


MockNetworkManager::~MockNetworkManager()
{
	delete mLastReply;
	delete mNextReply;
}


MockNetworkReply* MockNetworkManager::getReply(const QNetworkRequest& pRequest)
{
	delete mLastReply;

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

			mLastReply = new MockNetworkReply(content, HTTP_STATUS_OK);
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


QNetworkReply* MockNetworkManager::get(QNetworkRequest& pRequest,
		const QByteArray& pSslSession,
		int pTimeoutInMilliSeconds)
{
	Q_UNUSED(pRequest)
	Q_UNUSED(pSslSession)
	Q_UNUSED(pTimeoutInMilliSeconds)

	mLastRequest = pRequest;

	return getReply(pRequest);
}


QNetworkReply* MockNetworkManager::post(QNetworkRequest& pRequest, const QByteArray& pData, int pTimeoutInMilliSeconds)
{
	Q_UNUSED(pTimeoutInMilliSeconds)

	mLastRequest = pRequest;
	mLastData = QByteArray(pData);

	return getReply(pRequest);
}


QNetworkReply* MockNetworkManager::paos(QNetworkRequest& pRequest,
		const QByteArray& pNamespace,
		const QByteArray& pData,
		bool pUsePsk,
		const QByteArray& pSslSession,
		int pTimeoutInMilliSeconds)
{
	Q_UNUSED(pRequest)
	Q_UNUSED(pNamespace)
	Q_UNUSED(pData)
	Q_UNUSED(pUsePsk)
	Q_UNUSED(pSslSession)
	Q_UNUSED(pTimeoutInMilliSeconds)

	return getReply(pRequest);
}


bool MockNetworkManager::checkUpdateServerCertificate(const QSharedPointer<const QNetworkReply>& pReply)
{
	Q_UNUSED(pReply)

	return true;
}
