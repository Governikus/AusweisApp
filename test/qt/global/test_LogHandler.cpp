/*!
 * \brief Unit tests for \ref LogHandler
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void benchmark()
		{
			QBENCHMARK {
				qDebug() << "Add some dummy" << "messages" << "in different" << "strings";
			}
		}


		void checkBacklog()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
			QByteArray blog = Env::getSingleton<LogHandler>()->getBacklog();
			QCOMPARE(blog.size(), 0);

			QByteArray msg("dummy message with some useless information ... 123456");
			qDebug() << msg;

			blog = Env::getSingleton<LogHandler>()->getBacklog();
			QVERIFY(blog.size() > 0);
			QVERIFY(blog.contains(msg));

			Env::getSingleton<LogHandler>()->resetBacklog();
			blog = Env::getSingleton<LogHandler>()->getBacklog();
			QCOMPARE(blog.size(), 0);
		}


		void fireLog()
		{
			QSignalSpy spy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			qDebug() << "hallo";
			qDebug() << "test nachricht";

			QCOMPARE(spy.count(), 2);
			auto param1 = spy.takeFirst();
			auto param2 = spy.takeLast();

#ifdef Q_OS_WIN
			const QLatin1String lineBreak("\r\n");
#else
			const QLatin1Char lineBreak('\n');
#endif

			QVERIFY(param1.at(0).toString().endsWith(QStringLiteral("hallo") + lineBreak));
			QVERIFY(param2.at(0).toString().endsWith(QStringLiteral("test nachricht") + lineBreak));
		}


		void otherLogFilesWithoutCurrent()
		{
			auto list = Env::getSingleton<LogHandler>()->getOtherLogfiles();
			QVERIFY(!list.contains(Env::getSingleton<LogHandler>()->mLogFile));
		}


		void debugStream()
		{
			QSignalSpy spy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			qDebug() << *Env::getSingleton<LogHandler>();

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains(Env::getSingleton<LogHandler>()->mLogFile.fileName()));
		}


		void copyFile()
		{
			qDebug() << "dummy";
			QVERIFY(!Env::getSingleton<LogHandler>()->copy(QString()));
			QVERIFY(!Env::getSingleton<LogHandler>()->copy(QStringLiteral("                     ")));
		}


		void initReset()
		{
			QVERIFY(Env::getSingleton<LogHandler>()->isInitialized());

			QSignalSpy spy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			qDebug() << "dummy";
			QCOMPARE(spy.count(), 1);
			const auto& backlog = Env::getSingleton<LogHandler>()->getBacklog();
			QVERIFY(!backlog.isEmpty());
			spy.clear();

			Env::getSingleton<LogHandler>()->reset();
			QVERIFY(!Env::getSingleton<LogHandler>()->isInitialized());
			qDebug() << "dummy";
			QCOMPARE(spy.count(), 0);
			QCOMPARE(Env::getSingleton<LogHandler>()->getBacklog(), backlog);

			Env::getSingleton<LogHandler>()->init();
			QVERIFY(Env::getSingleton<LogHandler>()->isInitialized());
			qDebug() << "dummy";
			QCOMPARE(spy.count(), 1);
			QVERIFY(Env::getSingleton<LogHandler>()->getBacklog().size() > backlog.size());
		}


		void useLogfile()
		{
			const auto& logger = Env::getSingleton<LogHandler>();
			logger->resetBacklog();
			QVERIFY(logger->useLogfile());

			// will backlog
			qDebug() << "dummy";
			QVERIFY(logger->getBacklog().contains(QByteArrayLiteral("dummy")));

			// enable already enabled one
			logger->setLogfile(true);
			QVERIFY(logger->useLogfile());
			QVERIFY(logger->getBacklog().contains(QByteArrayLiteral("dummy")));

			// disable it
			logger->setLogfile(false);
			qDebug() << "another dummy";
			QVERIFY(!logger->useLogfile());
			QVERIFY(logger->getBacklog().isNull());
			QVERIFY(!logger->getCurrentLogfileDate().isValid());
			logger->resetBacklog();
			QVERIFY(logger->getBacklog().isNull());

			// disable already disabled one
			logger->setLogfile(false);
			QVERIFY(!logger->useLogfile());
			QVERIFY(logger->getBacklog().isNull());
			QVERIFY(!logger->getCurrentLogfileDate().isValid());

			// enable again
			logger->setLogfile(true);
			QVERIFY(logger->getBacklog().isEmpty());
			qDebug() << "another yummy";
			QVERIFY(logger->useLogfile());
			QVERIFY(logger->getBacklog().contains(QByteArrayLiteral("another yummy")));
			QVERIFY(logger->getCurrentLogfileDate().isValid());
		}


};

QTEST_GUILESS_MAIN(test_LogHandler)
#include "test_LogHandler.moc"
