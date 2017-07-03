/*!
 * \brief Unit tests for \ref AppUpdateService
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "AppUpdateService.h"

#include "LogHandler.h"
#include "MockHttpServer.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;


class test_AppService
	: public QObject
{
	Q_OBJECT

	private:
		MockHttpServer mServer;

	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();
		}


		void init()
		{
			mServer.reset();
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
		}


		void notSecureConnection()
		{
			QSignalSpy spyLog(&LogHandler::getInstance(), &LogHandler::fireLog);

			auto& instance = AppUpdateService::getInstance();
			instance.setUpdateUrl(mServer.getAddress());

			QSignalSpy spy(&instance, &UpdateService::fireUpdateFinished);
			instance.runUpdate();
			spy.wait();
			QCOMPARE(spy.count(), 1);

			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("It wasn't possible to connect to the server: a secure connection could not be established.")));
			QVERIFY(!instance.getUpdateData().isValid());
		}


};

QTEST_GUILESS_MAIN(test_AppService)
#include "test_AppService.moc"
