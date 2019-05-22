/*!
 * \brief Unit tests for \ref LogModel
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
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
	QSharedPointer<LogModel> mModel;

	private Q_SLOTS:
		void init()
		{
			mModel.reset(new LogModel());
		}


		void cleanup()
		{
			mModel.clear();
		}


		void test_AddLogEntry_data()
		{
			QTest::addColumn<QString>("input");
			QTest::addColumn<QString>("entry1");
			QTest::addColumn<QString>("entry2");

			QTest::newRow("validEntry") << QString("input : test") << QString("input") << QString("test");
			QTest::newRow("empty") << QString(" : ") << QString() << QString();
			QTest::newRow("leftEmpty") << QString(" : test") << QString() << QString("test");
			QTest::newRow("rightEmpty") << QString("input : ") << QString("input") << QString();
			QTest::newRow("NoSpaceRight") << QString("input :test") << QString("input :test") << QString();
			QTest::newRow("NoSpaceLeft") << QString("input: test") << QString("input: test") << QString();
			QTest::newRow("invalidEntry") << QString("inputTest") << QString("inputTest") << QString();
			QTest::newRow("NoSpaces") << QString("input:test") << QString("input:test") << QString();
			QTest::newRow("emptyString") << QString() << QString() << QString();
			QTest::newRow("::") << QString("::") << QString("::") << QString();
			QTest::newRow("a : b : c") << QString("a : b : c") << QString("a") << QString("b : c");
		}


		void test_AddLogEntry()
		{
			QFETCH(QString, input);
			QFETCH(QString, entry1);
			QFETCH(QString, entry2);

			mModel->addLogEntry(input);
			QCOMPARE(mModel->mLogEntries.at(0), entry1);
			QCOMPARE(mModel->mLogEntries.at(1), entry2);
		}


		void test_MoveViewNoChange_data()
		{
			QTest::addColumn<QString>("fileName");

			QTest::newRow("Empty") << QString(":/logfiles/empty.txt");
			QTest::newRow("Size78") << QString(":/logfiles/size78.txt");
			QTest::newRow("Size80") << QString(":/logfiles/size80.txt");
		}


		void test_MoveViewNoChange()
		{
			QFETCH(QString, fileName);
			QFile file(fileName);
			file.open(QIODevice::ReadOnly);
			QTextStream stream(&file);
			mModel->setLogEntries(stream);

			QSignalSpy spyRemove(mModel.data(), &LogModel::rowsRemoved);
			QSignalSpy spyInsert(mModel.data(), &LogModel::rowsInserted);
			QSignalSpy spyVisibleAreaChanged(mModel.data(), &LogModel::fireVisibleAreaChanged);

			for (int i = -5; i <= 5; i++)
			{
				mModel->moveView(i);
				QCOMPARE(spyRemove.count(), 0);
				QCOMPARE(spyInsert.count(), 0);
				QCOMPARE(spyVisibleAreaChanged.count(), 0);
			}
		}


		void test_MoveView_data()
		{
			QTest::addColumn<QString>("fileName");
			QTest::addColumn<int>("input");
			QTest::addColumn<int>("index");
			QTest::addColumn<int>("rowsRemovedStart");
			QTest::addColumn<int>("rowsRemovedEnd");
			QTest::addColumn<int>("rowsInsertedStart");
			QTest::addColumn<int>("rowsInsertedEnd");
			QTest::addColumn<int>("newIndex");
			QTest::addColumn<QString>("dataNewIndex");

			QTest::newRow("ScrollUpSuccessful_Size82") << QString(":/logfiles/size82.txt") << -2 << 2 << 78 << 79 << 0 << 1 << 0 << QString("1 input");
			QTest::newRow("ScrollDownSuccessful_Size82") << QString(":/logfiles/size82.txt") << 1 << 0 << 0 << 0 << 79 << 79 << 1 << QString("1 test");
			QTest::newRow("ScrollUp_DistanceOutOfRange_Size82") << QString(":/logfiles/size82.txt") << -50 << 20 << 60 << 79 << 0 << 19 << 0 << QString("1 input");
			QTest::newRow("ScrollDown_DistanceOutOfRange_Size82") << QString(":/logfiles/size82.txt") << 50 << 70 << 12 << 79 << 0 << 67 << 2 << QString("2 input");

			QTest::newRow("ScrollUpSuccessful_Size160") << QString(":/logfiles/size160.txt") << -20 << 50 << 60 << 79 << 0 << 19 << 30 << QString("16 input");
			QTest::newRow("ScrollDownSuccessful_Size160") << QString(":/logfiles/size160.txt") << 25 << 0 << 0 << 24 << 55 << 79 << 25 << QString("13 test");
			QTest::newRow("ScrollUp_DistanceOutOfRange_Size160") << QString(":/logfiles/size160.txt") << -41 << 40 << 40 << 79 << 0 << 39 << 0 << QString("1 input");
			QTest::newRow("ScrollDown_DistanceOutOfRange_Size160") << QString(":/logfiles/size160.txt") << 82 << 79 << 0 << 0 << 79 << 79 << 80 << QString("41 input");
		}


		void test_MoveView()
		{
			QFETCH(QString, fileName);
			QFETCH(int, index);
			QFETCH(int, input);
			QFETCH(int, rowsRemovedStart);
			QFETCH(int, rowsRemovedEnd);
			QFETCH(int, rowsInsertedStart);
			QFETCH(int, rowsInsertedEnd);
			QFETCH(int, newIndex);
			QFETCH(QString, dataNewIndex);

			QFile file(fileName);
			file.open(QIODevice::ReadOnly);
			QTextStream stream(&file);
			mModel->setLogEntries(stream);
			mModel->mIndex = index;
			QSignalSpy spyRemove(mModel.data(), &LogModel::rowsRemoved);
			QSignalSpy spyInsert(mModel.data(), &LogModel::rowsInserted);
			QSignalSpy spyVisibleAreaChanged(mModel.data(), &LogModel::fireVisibleAreaChanged);

			mModel->moveView(input);
			QCOMPARE(spyRemove.count(), 1);
			QCOMPARE(spyInsert.count(), 1);
			QCOMPARE(spyVisibleAreaChanged.count(), 1);
			QList<QVariant> argumentsRemove = spyRemove.takeFirst();
			QCOMPARE(argumentsRemove.at(1).toInt(), rowsRemovedStart);
			QCOMPARE(argumentsRemove.at(2).toInt(), rowsRemovedEnd);
			QList<QVariant> argumentsInsert = spyInsert.takeFirst();
			QCOMPARE(argumentsInsert.at(1).toInt(), rowsInsertedStart);
			QCOMPARE(argumentsInsert.at(2).toInt(), rowsInsertedEnd);
			QCOMPARE(mModel->mIndex, newIndex);

			if (!mModel->mLogEntries.isEmpty() && !mModel->mLogEntries.at(newIndex).isNull())
			{
				QModelIndex indexBegin = mModel->createIndex(0, 0);
				QCOMPARE(mModel->data(indexBegin, 0), QVariant(dataNewIndex));
			}
		}


		void test_SetLogEntries_data()
		{
			QTest::addColumn<QString>("fileName");
			QTest::addColumn<int>("logEntriesSize");
			QTest::addColumn<int>("count");

			QTest::newRow("empty") << QString(":/logfiles/empty.txt") << 0 << 0;
			QTest::newRow("size78") << QString(":/logfiles/size78.txt") << 78 << 78;
			QTest::newRow("size80") << QString(":/logfiles/size80.txt") << 80 << 80;
			QTest::newRow("size82") << QString(":/logfiles/size82.txt") << 82 << 80;
			QTest::newRow("size160") << QString(":/logfiles/size160.txt") << 160 << 80;
		}


		void test_SetLogEntries()
		{
			QFETCH(QString, fileName);
			QFETCH(int, logEntriesSize);
			QFETCH(int, count);

			QFile file(fileName);
			file.open(QIODevice::ReadOnly);
			QTextStream stream(&file);
			QSignalSpy spy(mModel.data(), &LogModel::fireVisibleAreaChanged);

			mModel->setLogEntries(stream);
			QCOMPARE(mModel->mLogEntries.size(), logEntriesSize);
			QCOMPARE(mModel->mCount, count);
			QCOMPARE(mModel->mIndex, 0);
			QCOMPARE(spy.count(), 1);
		}


		void test_OnNewLogMsg_data()
		{
			QTest::addColumn<QString>("msg");
			QTest::addColumn<QString>("fileName");
			QTest::addColumn<bool>("autoFlick");
			QTest::addColumn<int>("index");
			QTest::addColumn<int>("selectedFile");
			QTest::addColumn<int>("visibleAreaChangedCounter");
			QTest::addColumn<int>("newLogMsgCounter");

			QTest::newRow("emptyFile_MsgAdded") << QString(" : ") << QString(":/logfiles/empty.txt") << true << 1 << 0 << 1 << 0;
			QTest::newRow("emptyFile_MsgNotAdded") << QString(" : ") << QString(":/logfiles/empty.txt") << false << 0 << 1 << 0 << 0;
			QTest::newRow("emptyFile_MsgAdded_ViewChanged") << QString("test : input") << QString(":/logfiles/empty.txt") << true << 0 << 0 << 2 << 1;

			QTest::newRow("MsgAdded_Size78") << QString() << QString(":/logfiles/size78.txt") << false << 5 << 0 << 1 << 0;
			QTest::newRow("MsgAdded_ViewChanged_Size78") << QString("test : input") << QString(":/logfiles/size78.txt") << true << 0 << 0 << 2 << 1;
			QTest::newRow("MsgNotAdded_Size78") << QString("test : input") << QString(":/logfiles/size78.txt") << true << 5 << 2 << 0 << 0;

			QTest::newRow("MsgAdded_Size80") << QString() << QString(":/logfiles/size80.txt") << false << 5 << 0 << 1 << 0;
			QTest::newRow("MsgNotAdded_Size80") << QString("test : input") << QString(":/logfiles/size80.txt") << false << 0 << 1 << 0 << 0;
			QTest::newRow("MsgAdded_ViewChanged_Size80") << QString("test : input") << QString(":/logfiles/size80.txt") << true << 0 << 0 << 2 << 1;

			QTest::newRow("MsgAdded_Size82") << QString("test : input") << QString(":/logfiles/size82.txt") << true << 0 << 0 << 1 << 0;
			QTest::newRow("MsgNotAdded_Size82") << QString(" : ") << QString(":/logfiles/size82.txt") << true << 0 << 3 << 0 << 0;
			QTest::newRow("MsgAdded_ViewChanged_Size82") << QString("test : input") << QString(":/logfiles/size82.txt") << true << 2 << 0 << 2 << 1;
		}


		void test_OnNewLogMsg()
		{
			QFETCH(QString, msg);
			QFETCH(QString, fileName);
			QFETCH(bool, autoFlick);
			QFETCH(int, index);
			QFETCH(int, selectedFile);
			QFETCH(int, visibleAreaChangedCounter);
			QFETCH(int, newLogMsgCounter);

			QFile file(fileName);
			file.open(QIODevice::ReadOnly);
			QTextStream stream(&file);
			mModel->setLogEntries(stream);
			int oldSize = mModel->mLogEntries.size();
			mModel->mSelectedLogFile = selectedFile;
			mModel->mIndex = index;
			mModel->mAutoFlick = autoFlick;
			QSignalSpy spyVisibleAreaChanged(mModel.data(), &LogModel::fireVisibleAreaChanged);
			QSignalSpy spyNewLogMsg(mModel.data(), &LogModel::fireNewLogMsg);

			mModel->onNewLogMsg(msg);
			QCOMPARE(spyVisibleAreaChanged.count(), visibleAreaChangedCounter);
			QCOMPARE(spyNewLogMsg.count(), newLogMsgCounter);

			if (visibleAreaChangedCounter == 0)
			{
				QCOMPARE(mModel->mLogEntries.size(), oldSize);
			}
			else
			{
				QCOMPARE(mModel->mLogEntries.size(), oldSize + 2);
			}
		}


};

QTEST_GUILESS_MAIN(test_LogModel)
#include "test_LogModel.moc"
