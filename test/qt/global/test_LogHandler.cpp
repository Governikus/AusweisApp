/*!
 * test_LogHandler.h
 *
 * \brief Unit tests for \ref LogHandler
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "LogHandler.h"

#include <QtTest>

using namespace governikus;

class test_LogHandler
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
			LogHandler::getInstance().resetBacklog();
		}


		void benchmark()
		{
			QBENCHMARK {
				qDebug() << "Add some dummy" << "messages" << "in different" << "strings";
			}
		}


		void backlog()
		{
			LogHandler::getInstance().resetBacklog();
			QByteArray blog = LogHandler::getInstance().getBacklog();
			QCOMPARE(blog.size(), 0);

			QByteArray msg("dummy message with some useless information ... 123456");
			qDebug() << msg;

			blog = LogHandler::getInstance().getBacklog();
			QVERIFY(blog.size() > 0);
			QVERIFY(blog.contains(msg));

			LogHandler::getInstance().resetBacklog();
			blog = LogHandler::getInstance().getBacklog();
			QCOMPARE(blog.size(), 0);
		}


		void fireLog()
		{
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);

			qDebug() << "hallo";
			qDebug() << "test nachricht";


			QCOMPARE(spy.count(), 2);
			auto param1 = spy.takeFirst();
			auto param2 = spy.takeLast();
			QVERIFY(param1.at(0).toString().endsWith("hallo\n"));
			QVERIFY(param2.at(0).toString().endsWith("test nachricht\n"));
		}


		void otherLogFilesWithoutCurrent()
		{
			auto list = LogHandler::getInstance().getOtherLogfiles();
			QVERIFY(!list.contains(LogHandler::getInstance().mLogFile));
		}


		void debugStream()
		{
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);
			qDebug() << LogHandler::getInstance();

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains(LogHandler::getInstance().mLogFile.fileName()));
		}


		void copyFile()
		{
			qDebug() << "dummy";
			QVERIFY(!LogHandler::getInstance().copy(QString()));
			QVERIFY(!LogHandler::getInstance().copy(QStringLiteral("                     ")));
		}


};

QTEST_GUILESS_MAIN(test_LogHandler)
#include "test_LogHandler.moc"
