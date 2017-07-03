/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "MockNetworkReply.h"

using namespace governikus;


MockNetworkReply::MockNetworkReply(const QByteArray& pData, QObject* pParent)
	: QNetworkReply(pParent)
	, mData(pData)
	, mDataPosition(0)
{
	setOpenMode(QIODevice::ReadOnly);
}


MockNetworkReply::~MockNetworkReply()
{
}


qint64 MockNetworkReply::readData(char* pDst, qint64 pMaxSize)
{
	QByteArray data = mData.mid(mDataPosition, pMaxSize);
	int length = data.length();
	if (length)
	{
		qstrncpy(pDst, data.constData(), length);
		mDataPosition += length;
	}
	return length;
}
