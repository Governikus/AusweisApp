/*!
 * \brief Unit tests for \ref LogHandler
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "LogHandler.h"

#include <QtTest>

#ifndef Q_OS_WIN
#include <sys/time.h>
#include <sys/types.h>
#include <utime.h>
#endif

Q_DECLARE_LOGGING_CATEGORY(fileprovider)
Q_DECLARE_LOGGING_CATEGORY(securestorage)
Q_DECLARE_LOGGING_CATEGORY(configuration)

using namespace governikus;

class test_LogHandler
	: public QObject
{
	Q_OBJECT

	void fakeLastModifiedAndLastAccessTime(const QString& pPath)
	{
	#ifdef Q_OS_WIN
		Q_UNUSED(pPath);
	#else
		struct timeval tv[2];

		struct timeval& accessTime = tv[0];
		gettimeofday(&accessTime, nullptr);

		struct timeval& modifyTime = tv[1];
		gettimeofday(&modifyTime, nullptr);

		time_t fiveteenDays = 60 * 60 * 24 * 15;
		modifyTime.tv_sec -= fiveteenDays;
		utimes(pPath.toLatin1().constData(), tv);
	#endif
	}


	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
			Env::getSingleton<LogHandler>()->setUseHandler(true);
			Env::getSingleton<LogHandler>()->setLogfile(true);
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
			logger->setUseHandler(false);
			QVERIFY(!logger->useHandler());
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


		void removeUpOldLogfiles()
		{
			#ifdef Q_OS_WIN
			QSKIP("File time stamp mocking unimplemented on windows");
			#endif

			const auto& logger = Env::getSingleton<LogHandler>();

			const auto& initialFiles = logger->getOtherLogfiles();
			QTemporaryFile tmp(LogHandler::getLogFileTemplate());
			QVERIFY(tmp.open());
			tmp.fileName(); // touch it
			const auto& filesWithMock = logger->getOtherLogfiles();
			QVERIFY(filesWithMock.size() > initialFiles.size());

			logger->removeOldLogfiles();
			QVERIFY(tmp.exists());
			QCOMPARE(filesWithMock.size(), logger->getOtherLogfiles().size());

			fakeLastModifiedAndLastAccessTime(tmp.fileName());
			logger->removeOldLogfiles();
			QCOMPARE(logger->getOtherLogfiles().size(), initialFiles.size());
			QVERIFY(!tmp.exists());
		}


		void removeUpMultipleOldLogfilesWithInit()
		{
			#ifdef Q_OS_WIN
			QSKIP("File time stamp mocking unimplemented on windows");
			#endif

			const auto& logger = Env::getSingleton<LogHandler>();
			logger->reset();

			const auto& initialFiles = logger->getOtherLogfiles();
			QTemporaryFile tmp1(LogHandler::getLogFileTemplate());
			QVERIFY(tmp1.open());
			tmp1.fileName(); // touch it

			QTemporaryFile tmp2(LogHandler::getLogFileTemplate());
			QVERIFY(tmp2.open());
			tmp2.fileName(); // touch it

			const auto& filesWithMock = logger->getOtherLogfiles();
			QVERIFY(filesWithMock.size() > initialFiles.size());

			fakeLastModifiedAndLastAccessTime(tmp1.fileName());
			fakeLastModifiedAndLastAccessTime(tmp2.fileName());
			logger->init();
			QTRY_COMPARE(logger->getOtherLogfiles().size(), initialFiles.size());
			QVERIFY(!tmp1.exists());
			QVERIFY(!tmp2.exists());
		}


		void getCriticalLogWindow()
		{
			const auto& logger = Env::getSingleton<LogHandler>();
			logger->setCriticalLogCapacity(3);

			QVERIFY(!logger->hasCriticalLog());
			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());
			for (int i = 0; i < 10; ++i)
			{
				qDebug() << "debug dummy";
			}
			QVERIFY(!logger->hasCriticalLog());
			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());

			qWarning() << "warning dummy";
			qInfo() << "info dummy";
			QVERIFY(!logger->hasCriticalLog());
			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());

			qCritical() << "critical dummy";
			QVERIFY(logger->hasCriticalLog());

			auto window = logger->getCriticalLogWindow();
			QVERIFY(window.size() > 0);
			QVERIFY(!window.contains("debug dummy"));
			QVERIFY(window.contains("warning dummy"));
			QVERIFY(window.contains("info dummy"));
			QVERIFY(window.contains("critical dummy"));

			qDebug() << "debug dummy";
			qCritical() << "critical 2 dummy";
			QVERIFY(!window.contains("debug dummy"));
			QVERIFY(window.contains("warning dummy"));
			QVERIFY(window.contains("info dummy"));
			QVERIFY(window.contains("critical dummy"));
			QVERIFY(!window.contains("critical 2 dummy"));

			logger->resetBacklog();
			QVERIFY(!logger->hasCriticalLog());
			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());
		}


		void getCriticalLogWindowIgnore()
		{
			const auto& logger = Env::getSingleton<LogHandler>();
			logger->setCriticalLogCapacity(4);

			QVERIFY(!logger->hasCriticalLog());
			qCCritical(securestorage) << "do not log 1";
			qCCritical(securestorage) << "do not log 2";
			qCCritical(fileprovider) << "do not log 3";
			qCCritical(configuration) << "do not log 4";
			QVERIFY(!logger->hasCriticalLog());
			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());

			qCritical() << "critical dummy";
			auto window = logger->getCriticalLogWindow();
			QVERIFY(logger->hasCriticalLog());
			QVERIFY(window.size() > 0);
			QVERIFY(!window.contains("do not log 1"));
			QVERIFY(window.contains("do not log 2"));
			QVERIFY(window.contains("do not log 3"));
			QVERIFY(window.contains("do not log 4"));
			QVERIFY(window.contains("critical dummy"));
		}


		void getCriticalLogWindowWithoutLogfile()
		{
			const auto& logger = Env::getSingleton<LogHandler>();
			logger->setCriticalLogCapacity(10);
			QCOMPARE(logger->getCriticalLogCapacity(), 10);
			logger->setLogfile(false);

			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());
			QVERIFY(!logger->hasCriticalLog());

			qCritical() << "critical dummy";
			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());
			QVERIFY(!logger->hasCriticalLog());

			logger->setLogfile(true);
			qCritical() << "critical dummy";
			QVERIFY(logger->getCriticalLogWindow().contains("critical dummy"));
			QVERIFY(logger->hasCriticalLog());

			logger->setLogfile(false);
			logger->setLogfile(true);
			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());
			QVERIFY(!logger->hasCriticalLog());
		}


};

QTEST_GUILESS_MAIN(test_LogHandler)
#include "test_LogHandler.moc"
