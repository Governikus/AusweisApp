/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ResourceLoader.h"
#include "UIPlugInQml.h"

#include <QtGui/QtGui>
#include <QtQuickTest>


using namespace governikus;


static void preRoutine()
{
	ResourceLoader::getInstance().init();

	UIPlugInQml::registerQmlTypes();
}


Q_COREAPP_STARTUP_FUNCTION(preRoutine)

QUICK_TEST_MAIN(qml)
