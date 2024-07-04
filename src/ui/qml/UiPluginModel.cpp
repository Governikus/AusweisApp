/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginModel.h"

#include "Env.h"
#include "UiLoader.h"
#include "UiPluginQml.h"

using namespace governikus;

UiPluginModel::UiPluginModel()
	: UiPlugin()
{
}


UiPluginModel* UiPluginModel::create(const QQmlEngine*, const QJSEngine*)
{
	auto* obj = Env::getSingleton<UiLoader>()->getLoaded<UiPluginQml>();
	QJSEngine::setObjectOwnership(obj, QJSEngine::CppOwnership);
	return obj;
}
