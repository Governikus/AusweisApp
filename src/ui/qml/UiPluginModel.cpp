/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginModel.h"

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
