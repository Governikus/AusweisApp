/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "SingletonCreator.h"

#include "UiLoader.h"
#include "UiPluginQml.h"

using namespace governikus;

QObject* governikus::registerSingletonModel(QObject* pModel)
{
	QJSEngine::setObjectOwnership(pModel, QJSEngine::CppOwnership);

	const auto& metaObject = pModel->metaObject();
	const auto& onTranslationChangedSlot = metaObject->method(metaObject->indexOfSlot("onTranslationChanged()"));
	if (onTranslationChangedSlot.isValid())
	{
		const auto& qmlPlugin = Env::getSingleton<UiLoader>()->getLoaded<UiPluginQml>();
		if (qmlPlugin)
		{
			QObject::connect(qmlPlugin, QMetaMethod::fromSignal(&UiPluginQml::fireTranslationChanged), pModel, onTranslationChangedSlot);
		}
	}

	return pModel;
}
