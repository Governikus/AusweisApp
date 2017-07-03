/*
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "IconUpdateBackend.h"

#include <QFileInfo>
#include <QLoggingCategory>
#include <QStandardPaths>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(update)


IconUpdateBackend::IconUpdateBackend(const QString& pIconCacheBasePath, const QString& pIcon)
	: mIcon(pIcon)
	, mIconPathInCache(pIconCacheBasePath + pIcon)
{
}


IconUpdateBackend::~IconUpdateBackend()
{
}


QDateTime IconUpdateBackend::getIssueDate() const
{
	QFileInfo info(mIconPathInCache);

	return info.isFile() ? info.lastModified() : QDateTime();
}


void IconUpdateBackend::processSuccess(const QByteArray& pData)
{
	QFile file(mIconPathInCache);
	if (!file.open(QFile::OpenModeFlag::WriteOnly))
	{
		qCCritical(update) << "Cannot open file" << mIconPathInCache;
	}
	else if (file.write(pData) != pData.size())
	{
		qCCritical(update) << "Cannot write file" << mIconPathInCache;
	}

	file.close();
}


void IconUpdateBackend::processError()
{
	// Nothing to do.
}
