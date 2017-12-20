/*!
 * \brief Unit tests for \ref ResourceLoader
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ResourceLoader.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtTest/QtTest>

using namespace governikus;

class test_ResourceLoader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();
		}


		void cleanup()
		{
			if (ResourceLoader::getInstance().isLoaded())
			{
				ResourceLoader::getInstance().shutdown();
			}
			LogHandler::getInstance().resetBacklog();
		}


		void initAndLog()
		{
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);

			QVERIFY(!ResourceLoader::getInstance().isLoaded());
			ResourceLoader::getInstance().init();
			QVERIFY(ResourceLoader::getInstance().isLoaded());

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeAt(0);
			QVERIFY(param.at(0).toString().contains("Register resource: "));
			QVERIFY(param.at(0).toString().contains(" | true"));
		}


		void initAndShutdown()
		{
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);

			QVERIFY(!ResourceLoader::getInstance().isLoaded());
			ResourceLoader::getInstance().init();
			QVERIFY(ResourceLoader::getInstance().isLoaded());
			ResourceLoader::getInstance().shutdown();
			QVERIFY(!ResourceLoader::getInstance().isLoaded());

			QCOMPARE(spy.count(), 2);
			auto param = spy.takeAt(1);
			QVERIFY(param.at(0).toString().contains("Unregister resource: "));
			QVERIFY(param.at(0).toString().contains(" | true"));
		}


		void read()
		{
			QVERIFY(!ResourceLoader::getInstance().isLoaded());
			ResourceLoader::getInstance().init();

			QVERIFY(!QFile(":/non_existing_dummy_test").exists());
			QVERIFY(QFile(":/images/ausweis.png").exists());
		}


};

QTEST_GUILESS_MAIN(test_ResourceLoader)
#include "test_ResourceLoader.moc"
