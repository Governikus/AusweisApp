/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "DownloadService.h"

#include "DownloadBackend.h"

using namespace governikus;


DownloadService::DownloadService()
	: UpdateService(QSharedPointer<DownloadBackend>(new DownloadBackend), QStringLiteral("download"))
{
}


DownloadService::~DownloadService()
{
}


const QByteArray& DownloadService::getContent() const
{
	return getUpdateBackend<DownloadBackend>()->getContent();
}


const GlobalStatus& DownloadService::getError() const
{
	return getUpdateBackend<DownloadBackend>()->getError();
}
