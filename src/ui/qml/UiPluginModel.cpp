/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginModel.h"

#include "AppSettings.h"
#include "Env.h"
#include "UiLoader.h"
#include "UiPluginQml.h"

using namespace governikus;

UiPluginModel::UiPluginModel()
	: UiPlugin()
	, mUpdateInformationPending(false)
{
}


UiPluginModel* UiPluginModel::create(const QQmlEngine*, const QJSEngine*)
{
	auto* obj = Env::getSingleton<UiLoader>()->getLoaded<UiPluginQml>();
	QJSEngine::setObjectOwnership(obj, QJSEngine::CppOwnership);
	return obj;
}


bool UiPluginModel::isUpdatePending() const
{
	return mUpdateInformationPending;
}


void UiPluginModel::setUpdatePending(bool pNewIsUpdatePending)
{
	if (mUpdateInformationPending == pNewIsUpdatePending)
	{
		return;
	}

	mUpdateInformationPending = pNewIsUpdatePending;
	Q_EMIT fireIsUpdatePendingChanged();
}


bool UiPluginModel::showUpdateInformationIfPending()
{
	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	if (!(isUpdatePending() && generalSettings.isTrayIconEnabled() && generalSettings.isAutoUpdateCheck()))
	{
		return false;
	}

	setUpdatePending(false);
	onShowUi(UiModule::UPDATEINFORMATION);

	return true;
}
