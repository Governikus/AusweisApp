/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "MockNetworkReply.h"

using namespace governikus;


MockNetworkReply::MockNetworkReply(const QByteArray& pData, HttpStatusCode pStatusCode, QObject* pParent)
	: QNetworkReply(pParent)
	, mSocket()
{
	mSocket.mReadBuffer = pData;
	setOpenMode(QIODevice::ReadOnly);
	setAttribute(QNetworkRequest::HttpStatusCodeAttribute, QVariant(Enum<HttpStatusCode>::getValue(pStatusCode)));
}


MockNetworkReply::~MockNetworkReply()
{
}


qint64 MockNetworkReply::readData(char* pDst, qint64 pMaxSize)
{
	return mSocket.readData(pDst, pMaxSize);
}
