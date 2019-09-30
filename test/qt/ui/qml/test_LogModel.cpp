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

			QFile file(fileName);
			file.open(QIODevice::ReadOnly);
			QTextStream stream(&file);

			mModel->setLogEntries(stream);
			QCOMPARE(mModel->mLogEntries.size(), logEntriesSize);
		}


		void test_OnNewLogMsg_data()
		{
			QTest::addColumn<QString>("msg");
			QTest::addColumn<QString>("fileName");
			QTest::addColumn<int>("selectedFile");
			QTest::addColumn<int>("newLogMsgCounter");
			QTest::addColumn<int>("logEntriesSizeChange");

			QTest::newRow("emptyFile_MsgAdded") << QString(" : ") << QString(":/logfiles/empty.txt") << 0 << 1 << 2;
			QTest::newRow("emptyFile_MsgNotAdded") << QString(" : ") << QString(":/logfiles/empty.txt") << 1 << 0 << 0;
			QTest::newRow("emptyFile_MsgAdded_ViewChanged") << QString("test : input") << QString(":/logfiles/empty.txt") << 0 << 1 << 2;

			QTest::newRow("MsgAdded_Size78") << QString() << QString(":/logfiles/size78.txt") << 0 << 1 << 2;
			QTest::newRow("MsgAdded_ViewChanged_Size78") << QString("test : input") << QString(":/logfiles/size78.txt") << 0 << 1 << 2;
			QTest::newRow("MsgNotAdded_Size78") << QString("test : input") << QString(":/logfiles/size78.txt") << 2 << 0 << 0;

			QTest::newRow("MsgAdded_Size80") << QString() << QString(":/logfiles/size80.txt") << 0 << 1 << 2;
			QTest::newRow("MsgNotAdded_Size80") << QString("test : input") << QString(":/logfiles/size80.txt") << 1 << 0 << 0;
			QTest::newRow("MsgAdded_ViewChanged_Size80") << QString("test : input") << QString(":/logfiles/size80.txt") << 0 << 1 << 2;

			QTest::newRow("MsgAdded_Size82") << QString("test : input") << QString(":/logfiles/size82.txt") << 0 << 1 << 2;
			QTest::newRow("MsgNotAdded_Size82") << QString(" : ") << QString(":/logfiles/size82.txt") << 3 << 0 << 0;
			QTest::newRow("MsgAdded_ViewChanged_Size82") << QString("test : input") << QString(":/logfiles/size82.txt") << 0 << 1 << 2;
		}


		void test_OnNewLogMsg()
		{
			QFETCH(QString, msg);
			QFETCH(QString, fileName);
			QFETCH(int, selectedFile);
			QFETCH(int, newLogMsgCounter);
			QFETCH(int, logEntriesSizeChange);

			QFile file(fileName);
			file.open(QIODevice::ReadOnly);
			QTextStream stream(&file);
			mModel->setLogEntries(stream);
			int oldSize = mModel->mLogEntries.size();
			mModel->mSelectedLogFile = selectedFile;
			QSignalSpy spyNewLogMsg(mModel.data(), &LogModel::fireNewLogMsg);

			mModel->onNewLogMsg(msg);
			QCOMPARE(spyNewLogMsg.count(), newLogMsgCounter);
			QCOMPARE(mModel->mLogEntries.size(), oldSize + logEntriesSizeChange);
		}


};

QTEST_GUILESS_MAIN(test_LogModel)
#include "test_LogModel.moc"
