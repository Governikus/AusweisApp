/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "LogHandler.h"

#include <QDebug>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_LogModel
	: public QObject
{
	Q_OBJECT

	private:
		LogModel* mModel = nullptr;

		void resetModel(LogModel* pModel = nullptr)
		{
			delete mModel;
			mModel = pModel;
		}

	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();
		}


		void init()
		{
			mModel = new LogModel();
			// The test only covers the LogModel functionality, LogHandler will return an
			// error message to the LogModel if not properly initialised; discard that.
			mModel->mLogEntries.clear();
		}


		void cleanup()
		{
			resetModel();
		}


		void test_AddLogEntry_data()
		{
			QTest::addColumn<QLatin1String>("input");
			QTest::addColumn<QLatin1String>("origin");
			QTest::addColumn<QLatin1String>("level");
			QTest::addColumn<QLatin1String>("category");
			QTest::addColumn<QLatin1String>("message");

			QTest::newRow("validEntry") << QLatin1String("input : test") << QLatin1String("input") << QLatin1String("D") << QLatin1String("input") << QLatin1String("test");
			QTest::newRow("empty") << QLatin1String(" : ") << QLatin1String() << QLatin1String("D") << QLatin1String() << QLatin1String();
			QTest::newRow("leftEmpty") << QLatin1String(" : test") << QLatin1String() << QLatin1String("D") << QLatin1String() << QLatin1String("test");
			QTest::newRow("rightEmpty") << QLatin1String("input : ") << QLatin1String("input") << QLatin1String("D") << QLatin1String("input") << QLatin1String();
			QTest::newRow("NoSpaceRight") << QLatin1String("input :test") << QLatin1String("input :test") << QLatin1String("D") << QLatin1String("input") << QLatin1String();
			QTest::newRow("NoSpaceLeft") << QLatin1String("input: test") << QLatin1String("input: test") << QLatin1String("D") << QLatin1String("input:") << QLatin1String();
			QTest::newRow("invalidEntry") << QLatin1String("inputTest") << QLatin1String("inputTest") << QLatin1String("D") << QLatin1String("inputTest") << QLatin1String();
			QTest::newRow("NoSpaces") << QLatin1String("input:test") << QLatin1String("input:test") << QLatin1String("D") << QLatin1String("input:test") << QLatin1String();
			QTest::newRow("emptyString") << QLatin1String() << QLatin1String() << QLatin1String("D") << QLatin1String() << QLatin1String();
			QTest::newRow("::") << QLatin1String("::") << QLatin1String("::") << QLatin1String("D") << QLatin1String("::") << QLatin1String();
			QTest::newRow("a : b : c") << QLatin1String("a : b : c") << QLatin1String("a") << QLatin1String("D") << QLatin1String("a") << QLatin1String("b : c");
			QTest::newRow("categorySpacesRight") << QLatin1String("cat        abc : test") << QLatin1String("cat        abc") << QLatin1String("D") << QLatin1String("cat") << QLatin1String("test");
			QTest::newRow("categorySpaceLeft") << QLatin1String(" cat       abc : test") << QLatin1String("cat       abc") << QLatin1String("D") << QLatin1String("") << QLatin1String("test");
			QTest::newRow("levelW") << QLatin1String("cat 123 W abc : test") << QLatin1String("cat 123 W abc") << QLatin1String("W") << QLatin1String("cat") << QLatin1String("test");
			QTest::newRow("levelA") << QLatin1String("cat 123 A abc : test") << QLatin1String("cat 123 A abc") << QLatin1String("A") << QLatin1String("cat") << QLatin1String("test");
			QTest::newRow("levelLongPid") << QLatin1String("cat 12345 W abc : test") << QLatin1String("cat 12345 W abc") << QLatin1String("W") << QLatin1String("cat") << QLatin1String("test");
			QTest::newRow("levelWrongFormat") << QLatin1String("cat 123W abc : test") << QLatin1String("cat 123W abc") << QLatin1String("D") << QLatin1String("cat") << QLatin1String("test");
		}


		void test_AddLogEntry()
		{
			QFETCH(QLatin1String, input);
			QFETCH(QLatin1String, origin);
			QFETCH(QLatin1String, level);
			QFETCH(QLatin1String, category);
			QFETCH(QLatin1String, message);

			QSignalSpy spyLevel(mModel, &LogModel::fireLevelsChanged);
			QSignalSpy spyCategory(mModel, &LogModel::fireCategoriesChanged);

			mModel->addLogEntry(input);
			QCOMPARE(mModel->mLogEntries.at(0), input);
			const QModelIndex index = mModel->index(0);
			QCOMPARE(mModel->data(index, LogModel::OriginRole), origin);
			QCOMPARE(mModel->data(index, LogModel::LevelRole), level);
			QCOMPARE(mModel->data(index, LogModel::CategoryRole), category);
			QCOMPARE(mModel->data(index, LogModel::MessageRole), message);
			QCOMPARE(mModel->data(index, Qt::DisplayRole), input);

			QCOMPARE(spyLevel.size(), 1);
			QCOMPARE(mModel->getLevels().size(), 1);
			QVERIFY(mModel->getLevels().contains(level));

			QCOMPARE(spyCategory.size(), 1);
			QCOMPARE(mModel->getCategories().size(), 1);
			QVERIFY(mModel->getCategories().contains(category));
		}


		void test_LevelCategory()
		{
			QSignalSpy spyLevel(mModel, &LogModel::fireLevelsChanged);
			QSignalSpy spyCategory(mModel, &LogModel::fireCategoriesChanged);

			mModel->addLogEntry("cat           0000.00.00 00:00:00.000 000 W test : test"_L1);
			QCOMPARE(spyLevel.size(), 1);
			QCOMPARE(mModel->getLevels(), QSet<QString>({"W"_L1}));
			QCOMPARE(spyCategory.size(), 1);
			QCOMPARE(mModel->getCategories(), QSet<QString>({"cat"_L1}));

			mModel->addLogEntry("cat           0000.00.00 00:00:00.000 000 W test : test"_L1);
			QCOMPARE(spyLevel.size(), 1);
			QCOMPARE(mModel->getLevels(), QSet<QString>({"W"_L1}));
			QCOMPARE(spyCategory.size(), 1);
			QCOMPARE(mModel->getCategories(), QSet<QString>({"cat"_L1}));

			mModel->addLogEntry("cat           0000.00.00 00:00:00.000 000 I test : test"_L1);
			QCOMPARE(spyLevel.size(), 2);
			QCOMPARE(mModel->getLevels(), QSet<QString>({"W"_L1, "I"_L1}));
			QCOMPARE(spyCategory.size(), 1);
			QCOMPARE(mModel->getCategories(), QSet<QString>({"cat"_L1}));

			mModel->addLogEntry("dog           0000.00.00 00:00:00.000 000 I test : test"_L1);
			QCOMPARE(spyLevel.size(), 2);
			QCOMPARE(mModel->getLevels(), QSet<QString>({"W"_L1, "I"_L1}));
			QCOMPARE(spyCategory.size(), 2);
			QCOMPARE(mModel->getCategories(), QSet<QString>({"cat"_L1, "dog"_L1}));

			mModel->addLogEntry("dog           0000.00.00 00:00:00.000 000 I test : test"_L1);
			QCOMPARE(spyLevel.size(), 2);
			QCOMPARE(mModel->getLevels(), QSet<QString>({"W"_L1, "I"_L1}));
			QCOMPARE(spyCategory.size(), 2);
			QCOMPARE(mModel->getCategories(), QSet<QString>({"cat"_L1, "dog"_L1}));
		}


		void test_AddMultilineLogEntry()
		{
			mModel->addLogEntry(QStringLiteral("FooBar"));
			QCOMPARE(mModel->mLogEntries.size(), 1);
			QCOMPARE(mModel->mLogEntries.at(0), QLatin1String("FooBar"));

			mModel->addLogEntry(QStringLiteral("FooBar"));
			QCOMPARE(mModel->mLogEntries.size(), 1);
			QCOMPARE(mModel->mLogEntries.at(0), QLatin1String("FooBar\nFooBar"));

			mModel->addLogEntry(QStringLiteral("cat           0000.00.00 00:00:00.000 000 test"));
			QCOMPARE(mModel->mLogEntries.size(), 2);
			QCOMPARE(mModel->mLogEntries.at(0), QLatin1String("FooBar\nFooBar"));
			QCOMPARE(mModel->mLogEntries.at(1), QLatin1String("cat           0000.00.00 00:00:00.000 000 test"));

			mModel->addLogEntry(QStringLiteral("cat           0001.02.03 04:05:06.007 0000008 test"));
			QCOMPARE(mModel->mLogEntries.size(), 3);
			QCOMPARE(mModel->mLogEntries.at(0), QLatin1String("FooBar\nFooBar"));
			QCOMPARE(mModel->mLogEntries.at(1), QLatin1String("cat           0000.00.00 00:00:00.000 000 test"));
			QCOMPARE(mModel->mLogEntries.at(2), QLatin1String("cat           0001.02.03 04:05:06.007 0000008 test"));

			mModel->addLogEntry(QStringLiteral("BarFoo"));
			QCOMPARE(mModel->mLogEntries.size(), 3);
			QCOMPARE(mModel->mLogEntries.at(0), QLatin1String("FooBar\nFooBar"));
			QCOMPARE(mModel->mLogEntries.at(1), QLatin1String("cat           0000.00.00 00:00:00.000 000 test"));
			QCOMPARE(mModel->mLogEntries.at(2), QLatin1String("cat           0001.02.03 04:05:06.007 0000008 test\nBarFoo"));

			mModel->addLogEntry(QStringLiteral("cat 0000.00.00 00:00:00.000 000 test"));
			QCOMPARE(mModel->mLogEntries.size(), 3);
			QCOMPARE(mModel->mLogEntries.at(0), QLatin1String("FooBar\nFooBar"));
			QCOMPARE(mModel->mLogEntries.at(1), QLatin1String("cat           0000.00.00 00:00:00.000 000 test"));
			QCOMPARE(mModel->mLogEntries.at(2), QLatin1String("cat           0001.02.03 04:05:06.007 0000008 test\nBarFoo\ncat 0000.00.00 00:00:00.000 000 test"));
		}


		void test_CurrentLog()
		{
			qDebug() << "log line 1";
			qDebug() << "log line 2";
			qDebug() << "log line 3";
			mModel->setLogFile(0);
			QCOMPARE(mModel->mLogEntries.size(), 3);
		}


		void test_SetLogEntries_data()
		{
			QTest::addColumn<QLatin1String>("fileName");
			QTest::addColumn<int>("logEntriesSize");
			QTest::addColumn<int>("logLevelSize");
			QTest::addColumn<int>("logCategoriesSize");

			QTest::newRow("empty") << QLatin1String(":/logfiles/empty.txt") << 0 << 0 << 0;
			QTest::newRow("size1") << QLatin1String(":/logfiles/size1.txt") << 1 << 1 << 1;
			QTest::newRow("size42") << QLatin1String(":/logfiles/size42.txt") << 42 << 1 << 1;
			QTest::newRow("auth") << QLatin1String(":/logfiles/auth.txt") << 1223 << 4 << 19;
		}


		void test_SetLogEntries()
		{
			QFETCH(QLatin1String, fileName);
			QFETCH(int, logEntriesSize);
			QFETCH(int, logLevelSize);
			QFETCH(int, logCategoriesSize);

			QSignalSpy spyNewLogMsg(mModel, &LogModel::fireNewLogMsg);
			QSignalSpy spyLevel(mModel, &LogModel::fireLevelsChanged);
			QSignalSpy spyCategory(mModel, &LogModel::fireCategoriesChanged);

			QFile file(fileName);
			QVERIFY(file.open(QIODevice::ReadOnly));
			QTextStream stream(&file);

			mModel->setLogEntries(stream);
			QCOMPARE(spyNewLogMsg.size(), 0);
			QCOMPARE(mModel->mLogEntries.size(), logEntriesSize);
			QCOMPARE(spyLevel.size(), 1);
			QCOMPARE(mModel->getLevels().size(), logLevelSize);
			QCOMPARE(spyCategory.size(), 1);
			QCOMPARE(mModel->getCategories().size(), logCategoriesSize);
		}


		void test_OnNewLogMsg_data()
		{
			QTest::addColumn<QLatin1String>("msg");
			QTest::addColumn<QLatin1String>("fileName");
			QTest::addColumn<int>("selectedFile");
			QTest::addColumn<int>("newLogMsgCounter");
			QTest::addColumn<int>("logEntriesSizeChange");

			QTest::newRow("emptyFile_MsgAdded") << QLatin1String("test : input") << QLatin1String(":/logfiles/empty.txt") << 0 << 1 << 1;
			QTest::newRow("emptyFile_MsgNotAdded") << QLatin1String(" : ") << QLatin1String(":/logfiles/empty.txt") << 1 << 0 << 0;

			QTest::newRow("size1_MsgAdded") << QLatin1String("test : input") << QLatin1String(":/logfiles/size1.txt") << 0 << 1 << 1;
			QTest::newRow("size1_MsgNotAdded") << QLatin1String(" : ") << QLatin1String(":/logfiles/size1.txt") << 1 << 0 << 0;
		}


		void test_OnNewLogMsg()
		{
			QFETCH(QLatin1String, msg);
			QFETCH(QLatin1String, fileName);
			QFETCH(int, selectedFile);
			QFETCH(int, newLogMsgCounter);
			QFETCH(int, logEntriesSizeChange);

			QFile file(fileName);
			QVERIFY(file.open(QIODevice::ReadOnly));
			QTextStream stream(&file);
			mModel->setLogEntries(stream);
			const auto oldSize = mModel->mLogEntries.size();
			mModel->mSelectedLogFile = selectedFile;
			QSignalSpy spyNewLogMsg(mModel, &LogModel::fireNewLogMsg);

			qDebug() << msg;
			QCOMPARE(spyNewLogMsg.count(), newLogMsgCounter);
			QCOMPARE(mModel->mLogEntries.size(), oldSize + logEntriesSizeChange);
		}


		void test_RemoveOldLogFile()
		{
			{
				QTemporaryFile oldLogFile(LogHandler::getLogFileTemplate());
				oldLogFile.setAutoRemove(false);
				QVERIFY(oldLogFile.open());
				QVERIFY(!oldLogFile.fileName().isNull());
			}

			// We need to reset() the model since the list of "old" logfiles
			// is only populated in the ctor of LogModel.
			resetModel(new LogModel());

			QCOMPARE(mModel->getLogFileNames().size(), 2);
			mModel->removeOtherLogFiles();
			QCOMPARE(mModel->getLogFileNames().size(), 1);
		}


};

QTEST_GUILESS_MAIN(test_LogModel)
#include "test_LogModel.moc"
