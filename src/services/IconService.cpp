/*
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "IconService.h"

#include "IconUpdateBackend.h"


using namespace governikus;


IconService::IconService(const QString& pIconCacheBasePath, const QString& pIcon)
	: UpdateService(QSharedPointer<UpdateBackend>(new IconUpdateBackend(pIconCacheBasePath, pIcon)),
			QStringLiteral("icon"))
	, mIcon(pIcon)
{
	connect(this, &UpdateService::fireUpdateFinished, this, &IconService::onUpdateFinished);
}


IconService::~IconService()
{
}


void IconService::onUpdateFinished()
{
	Q_EMIT fireProcessingFinished(mIcon);
}
