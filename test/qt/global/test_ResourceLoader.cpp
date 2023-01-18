/*!
 * \brief Unit tests for \ref ResourceLoader
 *
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ResourceLoader.h"

#include "LogHandler.h"

#include <QtTest>

using namespace governikus;

class test_ResourceLoader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			if (ResourceLoader::getInstance().isLoaded())
			{
				ResourceLoader::getInstance().shutdown();
			}
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void initAndLog()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QVERIFY(!ResourceLoader::getInstance().isLoaded());
			ResourceLoader::getInstance().init();
			QVERIFY(ResourceLoader::getInstance().isLoaded());

			QVERIFY(logSpy.count() > 0);
			auto param = logSpy.takeLast();
			QVERIFY(param.at(0).toString().contains("Register resource: "));
			QVERIFY(param.at(0).toString().contains(" | true"));
		}


		void initAndShutdown()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QVERIFY(!ResourceLoader::getInstance().isLoaded());
			ResourceLoader::getInstance().init();
			QVERIFY(ResourceLoader::getInstance().isLoaded());
			ResourceLoader::getInstance().shutdown();
			QVERIFY(!ResourceLoader::getInstance().isLoaded());

			QVERIFY(logSpy.count() > 1);
			auto param = logSpy.takeLast();
			QVERIFY(param.at(0).toString().contains("Unregister resource: "));
			QVERIFY(param.at(0).toString().contains(" | true"));
		}


		void read()
		{
			QVERIFY(!ResourceLoader::getInstance().isLoaded());
			ResourceLoader::getInstance().init();

			QVERIFY(!QFile(":/non_existing_dummy_test").exists());
			QVERIFY(QFile(":/images/ausweis.svg").exists());
		}


};

QTEST_GUILESS_MAIN(test_ResourceLoader)
#include "test_ResourceLoader.moc"
