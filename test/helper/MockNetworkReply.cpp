/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "MockNetworkReply.h"

using namespace governikus;


MockNetworkReply::MockNetworkReply(const QByteArray& pData, QObject* pParent)
	: QNetworkReply(pParent)
	, mSocket()
{
	mSocket.mReadBuffer = pData;
	setOpenMode(QIODevice::ReadOnly);
}


MockNetworkReply::~MockNetworkReply()
{
}


qint64 MockNetworkReply::readData(char* pDst, qint64 pMaxSize)
{
	return mSocket.readData(pDst, pMaxSize);
}
