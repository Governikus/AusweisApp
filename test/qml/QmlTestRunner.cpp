/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "LogHandler.h"
#include "ResourceLoader.h"
#include "UIPlugInQml.h"

#include <QLoggingCategory>
#include <QtGui/QtGui>
#include <QtQuickTest>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(init)

static void preRoutine()
{
	auto& logHandler = *Env::getSingleton<LogHandler>();
	qCDebug(init) << "LogHandler initialized" << logHandler;
	ResourceLoader::getInstance().init();

	UIPlugInQml::registerQmlTypes();
}


Q_COREAPP_STARTUP_FUNCTION(preRoutine)

QUICK_TEST_MAIN(qml)
