/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include  "AppUpdateDataModel.h"

#include "AppUpdater.h"
#include "Env.h"
#include "SingletonHelper.h"

using namespace governikus;

defineSingleton(AppUpdateDataModel)

AppUpdateDataModel::AppUpdateDataModel()
	: QObject()
	, mUpdateAvailable(false)
{
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppUpdateCheckFinished, this, &AppUpdateDataModel::onAppUpdateCheckFinished);
}


AppUpdateDataModel& AppUpdateDataModel::getInstance()
{
	return *Instance;
}


void AppUpdateDataModel::onAppUpdateCheckFinished(bool pUpdateAvailable, const GlobalStatus& pStatus)
{
	Q_UNUSED(pStatus)
	mUpdateAvailable = pUpdateAvailable;
	Q_EMIT fireAppUpdateDataChanged();
}


bool AppUpdateDataModel::isUpdateAvailable() const
{
	return mUpdateAvailable;
}


bool AppUpdateDataModel::isValid() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().isValid();
}


const QDateTime& AppUpdateDataModel::getDate() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getDate();
}


const QString& AppUpdateDataModel::getVersion() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getVersion();
}


const QUrl& AppUpdateDataModel::getUrl() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getUrl();
}


int AppUpdateDataModel::getSize() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getSize();
}


const QUrl& AppUpdateDataModel::getChecksumUrl() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getChecksumUrl();
}


const QUrl& AppUpdateDataModel::getNotesUrl() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getNotesUrl();
}


const QString& AppUpdateDataModel::getNotes() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData().getNotes();
}


void AppUpdateDataModel::skipUpdate() const
{
	if (isValid())
	{
		Env::getSingleton<AppUpdater>()->skipVersion(getVersion());
	}
}
