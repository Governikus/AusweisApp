/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

#include "MockNetworkManager.h"

#include <QTest>
#include <llhttp.h>

using namespace governikus;


MockNetworkManager::MockNetworkManager()
	: mNextReply(nullptr)
	, mLastReply(nullptr)
	, mLastRequest()
{
}


MockNetworkManager::~MockNetworkManager()
{
	mLastReply.clear();
	mNextReply.clear();
}


QSharedPointer<MockNetworkReply> MockNetworkManager::getReply(const QNetworkRequest& pRequest)
{
	mLastReply.clear();

	if (mNextReply)
	{
		mLastReply = mNextReply;
		mNextReply.clear();
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

			mLastReply = QSharedPointer<MockNetworkReply>(new MockNetworkReply(content, HTTP_STATUS_OK), &QObject::deleteLater);
		}
		else
		{
			mLastReply = QSharedPointer<MockNetworkReply>(new MockNetworkReply(), &QObject::deleteLater);
		}
	}

	mLastReply->setRequest(pRequest);
	Q_EMIT fireReply();
	return mLastReply;
}


QSharedPointer<QNetworkReply> MockNetworkManager::get(QNetworkRequest& pRequest)
{
	return processRequest(pRequest, [this] (const QNetworkRequest& request){
				mLastRequest = request;
				return getReply(request);
			});
}


QSharedPointer<QNetworkReply> MockNetworkManager::post(QNetworkRequest& pRequest, const QByteArray& pData)
{
	return processRequest(pRequest, [this, pData] (const QNetworkRequest& request){
				mLastRequest = request;
				mLastData = QByteArray(pData);
				return getReply(request);
			});
}


QSharedPointer<QNetworkReply> MockNetworkManager::paos(QNetworkRequest& pRequest,
		const QByteArray& pNamespace,
		const QByteArray& pData,
		bool pUsePsk,
		const QByteArray& pSslSession)
{
	Q_UNUSED(pNamespace)
	Q_UNUSED(pUsePsk)
	Q_UNUSED(pSslSession)

	mLastRequest = pRequest;
	mLastData = QByteArray(pData);

	return getReply(pRequest);
}
