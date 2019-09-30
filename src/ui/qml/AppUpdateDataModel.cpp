/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#include  "AppUpdateDataModel.h"

#include "Env.h"
#include "Service.h"
#include "SingletonHelper.h"

using namespace governikus;

defineSingleton(AppUpdateDataModel)

AppUpdateDataModel::AppUpdateDataModel()
	: QObject()
	, mAppUpdateData(Env::getSingleton<Service>()->getUpdateData())
{
}


AppUpdateDataModel& AppUpdateDataModel::getInstance()
{
	return *Instance;
}


bool AppUpdateDataModel::isValid() const
{
	return mAppUpdateData.isValid();
}


const QDateTime AppUpdateDataModel::getDate() const
{
	return mAppUpdateData.getDate();
}


const QString& AppUpdateDataModel::getVersion() const
{
	return mAppUpdateData.getVersion();
}


const QUrl& AppUpdateDataModel::getUrl() const
{
	return mAppUpdateData.getUrl();
}


int AppUpdateDataModel::getSize() const
{
	return mAppUpdateData.getSize();
}


const QUrl& AppUpdateDataModel::getChecksumUrl() const
{
	return mAppUpdateData.getChecksumUrl();
}


const QUrl& AppUpdateDataModel::getNotesUrl() const
{
	return mAppUpdateData.getNotesUrl();
}


const QString& AppUpdateDataModel::getNotes() const
{
	return mAppUpdateData.getNotes();
}


void AppUpdateDataModel::onAppUpdateFinished(bool, const GlobalStatus&)
{
	mAppUpdateData = Env::getSingleton<Service>()->getUpdateData();
}
