/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "DownloadBackend.h"

using namespace governikus;


QDateTime DownloadBackend::getIssueDate() const
{
	return QDateTime();
}


void DownloadBackend::processSuccess(const QByteArray& pData)
{
	mContent = pData;
	mError = GlobalStatus::Code::No_Error;
}


void DownloadBackend::processError(const GlobalStatus& pError)
{
	mError = pError;
	mContent.clear();
}


const QByteArray& DownloadBackend::getContent() const
{
	return mContent;
}


const GlobalStatus& DownloadBackend::getError() const
{
	return mError;
}
