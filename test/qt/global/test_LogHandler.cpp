/*!
 * \brief Unit tests for \ref LogHandler
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "LogHandler.h"

#include <QDir>
#include <QFile>
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
		Q_UNUSED(pPath)
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


	QString randomLogFileName(bool pAutoRemove = true)
	{
		QTemporaryFile tmp(LogHandler::getLogFileTemplate());
		tmp.open();
		tmp.setAutoRemove(pAutoRemove);
		return tmp.fileName();
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
			Env::getSingleton<LogHandler>()->setLogFile(true);
		}


		void benchmark()
		{
			QBENCHMARK{
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

			blog = Env::getSingleton<LogHandler>()->getBacklog(true);
			QVERIFY(blog.size() > 0);
			QVERIFY(blog.contains(msg));
		}


		void fireLog()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			qDebug() << "hallo";
			qDebug() << "test nachricht";

			QCOMPARE(logSpy.count(), 2);
			auto param1 = logSpy.takeFirst();
			auto param2 = logSpy.takeLast();

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
			Env::getSingleton<LogHandler>()->setLogFile(true);

			auto list = Env::getSingleton<LogHandler>()->getOtherLogFiles();
			QVERIFY(!list.contains(QFileInfo(*Env::getSingleton<LogHandler>()->mLogFile)));

			Env::getSingleton<LogHandler>()->setLogFile(false);
			auto nextList = Env::getSingleton<LogHandler>()->getOtherLogFiles();
			QCOMPARE(list, nextList);
		}


		void debugStream()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			qDebug() << *Env::getSingleton<LogHandler>();

			QCOMPARE(logSpy.count(), 1);
			auto param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains(Env::getSingleton<LogHandler>()->mLogFile->fileName()));
		}


		void copyFileFail()
		{
			qDebug() << "dummy";
			QVERIFY(!Env::getSingleton<LogHandler>()->copy(QString()));
			QVERIFY(!Env::getSingleton<LogHandler>()->copy(QStringLiteral("                     ")));
		}


		void copyFileOverwrite()
		{
			qDebug() << "dummy";
			const QString fileName = randomLogFileName();
			QVERIFY(Env::getSingleton<LogHandler>()->copy(fileName));
			QVERIFY(QFile::exists(fileName));
			QVERIFY(Env::getSingleton<LogHandler>()->copy(fileName));
			QVERIFY(QFile::exists(fileName));

			Env::getSingleton<LogHandler>()->setLogFile(false);
			QVERIFY(!Env::getSingleton<LogHandler>()->copy(fileName));
			QVERIFY(QFile::exists(fileName));
		}


		void copyOther_data()
		{
			QTest::addColumn<bool>("withLogFile");

			QTest::newRow("true") << true;
			QTest::newRow("false") << false;
		}


		void copyOther()
		{
			QFETCH(bool, withLogFile);

			Env::getSingleton<LogHandler>()->setLogFile(withLogFile);

			const QString logFileName = randomLogFileName(false);
			QVERIFY(QFile::exists(logFileName));
			const QString targetFileName = randomLogFileName();
			QVERIFY(Env::getSingleton<LogHandler>()->copyOther(logFileName, targetFileName));
			QVERIFY(QFile::exists(targetFileName));
			QVERIFY(QFile::remove(targetFileName));
			QVERIFY(QFile::remove(logFileName));
			QVERIFY(!Env::getSingleton<LogHandler>()->copyOther(QStringLiteral("foo.log"), targetFileName));
			QVERIFY(!QFile::exists(targetFileName));
		}


		void initReset()
		{
			QVERIFY(Env::getSingleton<LogHandler>()->isInstalled());

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			qDebug() << "dummy";
			QCOMPARE(logSpy.count(), 1);
			const auto& backlog = Env::getSingleton<LogHandler>()->getBacklog();
			QVERIFY(!backlog.isEmpty());
			logSpy.clear();

			Env::getSingleton<LogHandler>()->reset();
			QVERIFY(!Env::getSingleton<LogHandler>()->isInstalled());
			qDebug() << "dummy";
			QCOMPARE(logSpy.count(), 0);
			QCOMPARE(Env::getSingleton<LogHandler>()->getBacklog(), backlog);

			Env::getSingleton<LogHandler>()->init();
			QVERIFY(Env::getSingleton<LogHandler>()->isInstalled());
			qDebug() << "dummy";
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(Env::getSingleton<LogHandler>()->getBacklog().size() > backlog.size());
		}


		void useLogFile()
		{
			const auto& logger = Env::getSingleton<LogHandler>();
			logger->resetBacklog();
			logger->setUseHandler(false);
			QVERIFY(!logger->useHandler());
			QVERIFY(logger->useLogFile());

			// will backlog
			qDebug() << "dummy";
			QVERIFY(logger->getBacklog().contains(QByteArrayLiteral("dummy")));

			// enable already enabled one
			logger->setLogFile(true);
			QVERIFY(logger->useLogFile());
			QVERIFY(logger->getBacklog().contains(QByteArrayLiteral("dummy")));

			// disable it
			logger->setLogFile(false);
			qDebug() << "another dummy";
			QVERIFY(!logger->useLogFile());
			QVERIFY(logger->getBacklog().isNull());
			QVERIFY(!logger->getCurrentLogFileDate().isValid());
			logger->resetBacklog();
			QVERIFY(logger->getBacklog().isNull());

			// disable already disabled one
			logger->setLogFile(false);
			QVERIFY(!logger->useLogFile());
			QVERIFY(logger->getBacklog().isNull());
			QVERIFY(!logger->getCurrentLogFileDate().isValid());

			// enable again
			logger->setLogFile(true);
			QVERIFY(logger->getBacklog().isEmpty());
			qDebug() << "another yummy";
			QVERIFY(logger->useLogFile());
			QVERIFY(logger->getBacklog().contains(QByteArrayLiteral("another yummy")));
			QVERIFY(logger->getCurrentLogFileDate().isValid());
		}


		void removeUpOldLogFiles()
		{
#ifdef Q_OS_WIN
			QSKIP("File time stamp mocking unimplemented on windows");
#endif

			const auto& logger = Env::getSingleton<LogHandler>();

			const auto& initialFiles = logger->getOtherLogFiles();
			QTemporaryFile tmp(LogHandler::getLogFileTemplate());
			QVERIFY(tmp.open());
			tmp.fileName(); // touch it
			const auto& filesWithMock = logger->getOtherLogFiles();
			QVERIFY(filesWithMock.size() > initialFiles.size());

			logger->removeOldLogFiles();
			QVERIFY(tmp.exists());
			QCOMPARE(filesWithMock.size(), logger->getOtherLogFiles().size());

			fakeLastModifiedAndLastAccessTime(tmp.fileName());
			logger->removeOldLogFiles();
			QCOMPARE(logger->getOtherLogFiles().size(), initialFiles.size());
			QVERIFY(!tmp.exists());
		}


		void handleMessage()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			const auto& logger = Env::getSingleton<LogHandler>();

			QMessageLogContext logContext1("/src/ui/qml/ApplicationModel.cpp", 411, "bool ApplicationModel::isScreenReaderRunning()", "ui");
			logger->handleMessage(QtMsgType::QtDebugMsg, logContext1, "testMessage");
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("ApplicationModel::isScreenReaderRunning(ui/qml/ApplicationModel.cpp:411)   : testMessage"));

			QMessageLogContext logContext2("/src/ui/qml/ApplicationModel.cpp", 411, "bool ApplicationModel::isScreenReaderRunning() const", "ui");
			logger->handleMessage(QtMsgType::QtDebugMsg, logContext2, "testMessage2");
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("ApplicationModel::isScreenReaderRunning(ui/qml/ApplicationModel.cpp:411)   : testMessage2"));
		}


		void removeUpMultipleOldLogFilesWithInit()
		{
#ifdef Q_OS_WIN
			QSKIP("File time stamp mocking unimplemented on windows");
#endif

			const auto& logger = Env::getSingleton<LogHandler>();
			logger->reset();

			const auto& initialFiles = logger->getOtherLogFiles();
			QTemporaryFile tmp1(LogHandler::getLogFileTemplate());
			QVERIFY(tmp1.open());
			tmp1.fileName(); // touch it

			QTemporaryFile tmp2(LogHandler::getLogFileTemplate());
			QVERIFY(tmp2.open());
			tmp2.fileName(); // touch it

			const auto& filesWithMock = logger->getOtherLogFiles();
			QVERIFY(filesWithMock.size() > initialFiles.size());

			fakeLastModifiedAndLastAccessTime(tmp1.fileName());
			fakeLastModifiedAndLastAccessTime(tmp2.fileName());
			logger->init();
			QTRY_COMPARE(logger->getOtherLogFiles().size(), initialFiles.size()); // clazy:exclude=qstring-allocations
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


		void getCriticalLogWindowWithoutLogFile()
		{
			const auto& logger = Env::getSingleton<LogHandler>();
			logger->setCriticalLogCapacity(10);
			QCOMPARE(logger->getCriticalLogCapacity(), 10);
			logger->setLogFile(false);

			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());
			QVERIFY(!logger->hasCriticalLog());

			qCritical() << "critical dummy";
			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());
			QVERIFY(!logger->hasCriticalLog());

			logger->setLogFile(true);
			qCritical() << "critical dummy";
			QVERIFY(logger->getCriticalLogWindow().contains("critical dummy"));
			QVERIFY(logger->hasCriticalLog());

			logger->setLogFile(false);
			logger->setLogFile(true);
			QCOMPARE(logger->getCriticalLogWindow(), QByteArray());
			QVERIFY(!logger->hasCriticalLog());
		}


		void formatFunction_data()
		{
			QTest::addColumn<QByteArray>("function");
			QTest::addColumn<QByteArray>("result");

			QTest::newRow("LanguageLoader::createTranslator") << QByteArray("QSharedPointer<QTranslator> governikus::LanguageLoader::createTranslator(const QLocale&, const QString&)")
															  << QByteArray("LanguageLoader::createTranslator");

			QTest::newRow("LanguageLoader::load") << QByteArray("void governikus::LanguageLoader::load(const QLocale&)")
												  << QByteArray("LanguageLoader::load");

			QTest::newRow("createSingleton") << QByteArray("T* governikus::Env::createSingleton() [with T = governikus::AppUpdater]")
											 << QByteArray("Env::createSingleton");

			QTest::newRow("printInfo") << QByteArray("void printInfo()")
									   << QByteArray("printInfo");

			QTest::newRow("FileDestination::getPath") << QByteArray("static QString governikus::FileDestination::getPath(const QString&, QStandardPaths::LocateOption, QStandardPaths::StandardLocation)")
													  << QByteArray("FileDestination::getPath");

			QTest::newRow("Env::setCreator") << QByteArray("static void governikus::Env::setCreator(std::function<_Res(_ArgTypes ...)>) [with Q = {anonymous}::TestEmptyTmp; Args = {}]")
											 << QByteArray("Env::setCreator");

			QTest::newRow("lambdaParam") << QByteArray("void lambdaParam(std::function<bool()>)")
										 << QByteArray("lambdaParam");

			QTest::newRow("lambda") << QByteArray("governikus::initApp(int&, char**)::<lambda()>")
									<< QByteArray("initApp");

			// the const is fake for better unit testing
			QTest::newRow("UILoader::load") << QByteArray("std::enable_if_t<std::is_base_of<governikus::UIPlugIn, T>::value, bool> governikus::UILoader::load() const [with T = governikus::UIPlugInJson; std::enable_if_t<std::is_base_of<governikus::UIPlugIn, T>::value, bool> = bool]")
											<< QByteArray("UILoader::load");

		}


		void formatFunction()
		{
			QFETCH(QByteArray, function);
			QFETCH(QByteArray, result);

			const auto* handler = Env::getSingleton<LogHandler>();
			const auto& formattedFunction = handler->formatFunction(function.constData(), QByteArray(), 194);

			QCOMPARE(formattedFunction, result);
		}


};

QTEST_GUILESS_MAIN(test_LogHandler)
#include "test_LogHandler.moc"
