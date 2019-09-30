/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "MockDownloader.h"

using namespace governikus;


MockDownloader::MockDownloader(GlobalStatus::Code pErrorCode)
	: mErrorCode(pErrorCode)
	, mDate(QDate(2017, 7, 15))
	, mTime(QTime(11, 57, 21))
	, mTestData()
{
}


QDateTime MockDownloader::getTimeStamp()
{
	return QDateTime(mDate, mTime);
}


QString MockDownloader::getTimeStampString()
{
	const QString dateFormat = QStringLiteral("yyyyMMddhhmmss");
	return getTimeStamp().toString(dateFormat);
}


QByteArray MockDownloader::getTestData(const QUrl& pUrl)
{
	return mTestData.value(pUrl);
}


void MockDownloader::download(const QUrl& pUpdateUrl)
{
	if (mErrorCode != GlobalStatus::Code::No_Error)
	{
		Q_EMIT fireDownloadFailed(pUpdateUrl, mErrorCode);
	}
	else if (!mTestData.contains(pUpdateUrl))
	{
		Q_EMIT fireDownloadFailed(pUpdateUrl, GlobalStatus::Code::Downloader_File_Not_Found);
	}
	else
	{
		Q_EMIT fireDownloadSuccess(pUpdateUrl, getTimeStamp(), getTestData(pUpdateUrl));
	}
}


void MockDownloader::downloadIfNew(const QUrl& pUpdateUrl,
		const QDateTime& pCurrentTimestamp)
{
	Q_UNUSED(pCurrentTimestamp)
	download(pUpdateUrl);
}


void MockDownloader::setTestData(QUrl& pUrl, const QByteArray& pData)
{
	mTestData[pUrl] = pData;
}


void MockDownloader::setError(GlobalStatus::Code pErrorCode)
{
	mErrorCode = pErrorCode;
}
