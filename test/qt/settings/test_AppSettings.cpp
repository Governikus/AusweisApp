/*!
 * \brief Unit tests for \ref AppSettings
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include <QCoreApplication>
#include <QFile>
#include <QtTest>

#include "AppSettings.h"


using namespace governikus;

class test_AppSettings
	: public QObject
{
	Q_OBJECT

	private:
		AppSettings* settings = nullptr;

	private Q_SLOTS:
		void init()
		{
			AbstractSettings::mTestDir.clear();
			settings = new AppSettings;
		}


		void cleanup()
		{
			delete settings;
		}


};

QTEST_GUILESS_MAIN(test_AppSettings)
#include "test_AppSettings.moc"
