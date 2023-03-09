/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref LogModel
 */

#include "LogModel.h"

#include "LogHandler.h"

#include <QDebug>
#include <QtTest>


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
			QTest::addColumn<QString>("input");
			QTest::addColumn<QString>("origin");
			QTest::addColumn<QString>("level");
			QTest::addColumn<QString>("category");
			QTest::addColumn<QString>("message");

			QTest::newRow("validEntry") << QString("input : test") << QString("input") << QString("D") << QString("input") << QString("test");
			QTest::newRow("empty") << QString(" : ") << QString() << QString("D") << QString() << QString();
			QTest::newRow("leftEmpty") << QString(" : test") << QString() << QString("D") << QString() << QString("test");
			QTest::newRow("rightEmpty") << QString("input : ") << QString("input") << QString("D") << QString("input") << QString();
			QTest::newRow("NoSpaceRight") << QString("input :test") << QString("input :test") << QString("D") << QString("input") << QString();
			QTest::newRow("NoSpaceLeft") << QString("input: test") << QString("input: test") << QString("D") << QString("input:") << QString();
			QTest::newRow("invalidEntry") << QString("inputTest") << QString("inputTest") << QString("D") << QString("inputTest") << QString();
			QTest::newRow("NoSpaces") << QString("input:test") << QString("input:test") << QString("D") << QString("input:test") << QString();
			QTest::newRow("emptyString") << QString() << QString() << QString("D") << QString() << QString();
			QTest::newRow("::") << QString("::") << QString("::") << QString("D") << QString("::") << QString();
			QTest::newRow("a : b : c") << QString("a : b : c") << QString("a") << QString("D") << QString("a") << QString("b : c");
			QTest::newRow("categorySpacesRight") << QString("cat        abc : test") << QString("cat        abc") << QString("D") << QString("cat") << QString("test");
			QTest::newRow("categorySpaceLeft") << QString(" cat       abc : test") << QString("cat       abc") << QString("D") << QString("") << QString("test");
			QTest::newRow("levelW") << QString("cat 123 W abc : test") << QString("cat 123 W abc") << QString("W") << QString("cat") << QString("test");
			QTest::newRow("levelA") << QString("cat 123 A abc : test") << QString("cat 123 A abc") << QString("A") << QString("cat") << QString("test");
			QTest::newRow("levelLongPid") << QString("cat 12345 W abc : test") << QString("cat 12345 W abc") << QString("W") << QString("cat") << QString("test");
			QTest::newRow("levelWrongFormat") << QString("cat 123W abc : test") << QString("cat 123W abc") << QString("D") << QString("cat") << QString("test");
		}


		void test_AddLogEntry()
		{
			QFETCH(QString, input);
			QFETCH(QString, origin);
			QFETCH(QString, level);
			QFETCH(QString, category);
			QFETCH(QString, message);

			QSignalSpy spyLevel(mModel, &LogModel::fireLevelsChanged);
			QSignalSpy spyCategorie(mModel, &LogModel::fireCategoriesChanged);

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

			QCOMPARE(spyCategorie.size(), 1);
			QCOMPARE(mModel->getCategories().size(), 1);
			QVERIFY(mModel->getCategories().contains(category));
		}


		void test_LevelCategory()
		{
			QSignalSpy spyLevel(mModel, &LogModel::fireLevelsChanged);
			QSignalSpy spyCategorie(mModel, &LogModel::fireCategoriesChanged);

			mModel->addLogEntry(QString("cat           0000.00.00 00:00:00.000 000 W test : test"));
			QCOMPARE(spyLevel.size(), 1);
			QCOMPARE(mModel->getLevels(), QSet({QString("W")}));
			QCOMPARE(spyCategorie.size(), 1);
			QCOMPARE(mModel->getCategories(), QSet({QString("cat")}));

			mModel->addLogEntry(QString("cat           0000.00.00 00:00:00.000 000 W test : test"));
			QCOMPARE(spyLevel.size(), 1);
			QCOMPARE(mModel->getLevels(), QSet({QString("W")}));
			QCOMPARE(spyCategorie.size(), 1);
			QCOMPARE(mModel->getCategories(), QSet({QString("cat")}));

			mModel->addLogEntry(QString("cat           0000.00.00 00:00:00.000 000 I test : test"));
			QCOMPARE(spyLevel.size(), 2);
			QCOMPARE(mModel->getLevels(), QSet({QString("W"), QString("I")}));
			QCOMPARE(spyCategorie.size(), 1);
			QCOMPARE(mModel->getCategories(), QSet({QString("cat")}));

			mModel->addLogEntry(QString("dog           0000.00.00 00:00:00.000 000 I test : test"));
			QCOMPARE(spyLevel.size(), 2);
			QCOMPARE(mModel->getLevels(), QSet({QString("W"), QString("I")}));
			QCOMPARE(spyCategorie.size(), 2);
			QCOMPARE(mModel->getCategories(), QSet({QString("cat"), QString("dog")}));

			mModel->addLogEntry(QString("dog           0000.00.00 00:00:00.000 000 I test : test"));
			QCOMPARE(spyLevel.size(), 2);
			QCOMPARE(mModel->getLevels(), QSet({QString("W"), QString("I")}));
			QCOMPARE(spyCategorie.size(), 2);
			QCOMPARE(mModel->getCategories(), QSet({QString("cat"), QString("dog")}));
		}


		void test_AddMultilineLogEntry()
		{
			mModel->addLogEntry(QString("FooBar"));
			QCOMPARE(mModel->mLogEntries.size(), 1);
			QCOMPARE(mModel->mLogEntries.at(0), QString("FooBar"));

			mModel->addLogEntry(QString("FooBar"));
			QCOMPARE(mModel->mLogEntries.size(), 1);
			QCOMPARE(mModel->mLogEntries.at(0), QString("FooBar\nFooBar"));

			mModel->addLogEntry(QString("cat           0000.00.00 00:00:00.000 000 test"));
			QCOMPARE(mModel->mLogEntries.size(), 2);
			QCOMPARE(mModel->mLogEntries.at(0), QString("FooBar\nFooBar"));
			QCOMPARE(mModel->mLogEntries.at(1), QString("cat           0000.00.00 00:00:00.000 000 test"));

			mModel->addLogEntry(QString("cat           0001.02.03 04:05:06.007 0000008 test"));
			QCOMPARE(mModel->mLogEntries.size(), 3);
			QCOMPARE(mModel->mLogEntries.at(0), QString("FooBar\nFooBar"));
			QCOMPARE(mModel->mLogEntries.at(1), QString("cat           0000.00.00 00:00:00.000 000 test"));
			QCOMPARE(mModel->mLogEntries.at(2), QString("cat           0001.02.03 04:05:06.007 0000008 test"));

			mModel->addLogEntry(QString("BarFoo"));
			QCOMPARE(mModel->mLogEntries.size(), 3);
			QCOMPARE(mModel->mLogEntries.at(0), QString("FooBar\nFooBar"));
			QCOMPARE(mModel->mLogEntries.at(1), QString("cat           0000.00.00 00:00:00.000 000 test"));
			QCOMPARE(mModel->mLogEntries.at(2), QString("cat           0001.02.03 04:05:06.007 0000008 test\nBarFoo"));

			mModel->addLogEntry(QString("cat 0000.00.00 00:00:00.000 000 test"));
			QCOMPARE(mModel->mLogEntries.size(), 3);
			QCOMPARE(mModel->mLogEntries.at(0), QString("FooBar\nFooBar"));
			QCOMPARE(mModel->mLogEntries.at(1), QString("cat           0000.00.00 00:00:00.000 000 test"));
			QCOMPARE(mModel->mLogEntries.at(2), QString("cat           0001.02.03 04:05:06.007 0000008 test\nBarFoo\ncat 0000.00.00 00:00:00.000 000 test"));
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
			QTest::addColumn<QString>("fileName");
			QTest::addColumn<int>("logEntriesSize");
			QTest::addColumn<int>("logLevelSize");
			QTest::addColumn<int>("logCategoriesSize");

			QTest::newRow("empty") << QString(":/logfiles/empty.txt") << 0 << 0 << 0;
			QTest::newRow("size1") << QString(":/logfiles/size1.txt") << 1 << 1 << 1;
			QTest::newRow("size42") << QString(":/logfiles/size42.txt") << 42 << 1 << 1;
			QTest::newRow("auth") << QString(":/logfiles/auth.txt") << 1223 << 4 << 19;
		}


		void test_SetLogEntries()
		{
			QFETCH(QString, fileName);
			QFETCH(int, logEntriesSize);
			QFETCH(int, logLevelSize);
			QFETCH(int, logCategoriesSize);

			QSignalSpy spyNewLogMsg(mModel, &LogModel::fireNewLogMsg);
			QSignalSpy spyLevel(mModel, &LogModel::fireLevelsChanged);
			QSignalSpy spyCategorie(mModel, &LogModel::fireCategoriesChanged);

			QFile file(fileName);
			QVERIFY(file.open(QIODevice::ReadOnly));
			QTextStream stream(&file);

			mModel->setLogEntries(stream);
			QCOMPARE(spyNewLogMsg.size(), 0);
			QCOMPARE(mModel->mLogEntries.size(), logEntriesSize);
			QCOMPARE(spyLevel.size(), 1);
			QCOMPARE(mModel->getLevels().size(), logLevelSize);
			QCOMPARE(spyCategorie.size(), 1);
			QCOMPARE(mModel->getCategories().size(), logCategoriesSize);
		}


		void test_OnNewLogMsg_data()
		{
			QTest::addColumn<QString>("msg");
			QTest::addColumn<QString>("fileName");
			QTest::addColumn<int>("selectedFile");
			QTest::addColumn<int>("newLogMsgCounter");
			QTest::addColumn<int>("logEntriesSizeChange");

			QTest::newRow("emptyFile_MsgAdded") << QString("test : input") << QString(":/logfiles/empty.txt") << 0 << 1 << 1;
			QTest::newRow("emptyFile_MsgNotAdded") << QString(" : ") << QString(":/logfiles/empty.txt") << 1 << 0 << 0;

			QTest::newRow("size1_MsgAdded") << QString("test : input") << QString(":/logfiles/size1.txt") << 0 << 1 << 1;
			QTest::newRow("size1_MsgNotAdded") << QString(" : ") << QString(":/logfiles/size1.txt") << 1 << 0 << 0;
		}


		void test_OnNewLogMsg()
		{
			QFETCH(QString, msg);
			QFETCH(QString, fileName);
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
			mModel->removeCurrentLogFile();
			QCOMPARE(mModel->getLogFileNames().size(), 2);
			mModel->setLogFile(1);
			mModel->removeCurrentLogFile();
			QCOMPARE(mModel->getLogFileNames().size(), 1);
		}


};

QTEST_GUILESS_MAIN(test_LogModel)
#include "test_LogModel.moc"
