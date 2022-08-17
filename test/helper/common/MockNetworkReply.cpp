/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "MockNetworkReply.h"

using namespace governikus;


MockNetworkReply::MockNetworkReply(const QByteArray& pData, http_status pStatusCode, QObject* pParent)
	: QNetworkReply(pParent)
	, mSocket()
	, mFinished(false)
{
	mSocket.mReadBuffer = pData;
	setOpenMode(QIODevice::ReadOnly);
	setAttribute(QNetworkRequest::HttpStatusCodeAttribute, QVariant(pStatusCode));
}


MockNetworkReply::~MockNetworkReply()
{
}


qint64 MockNetworkReply::readData(char* pDst, qint64 pMaxSize)
{
	return mSocket.readData(pDst, pMaxSize);
}


void MockNetworkReply::fireFinished()
{
	if (!mFinished)
	{
		mFinished = true;
		Q_EMIT finished();
	}
}
