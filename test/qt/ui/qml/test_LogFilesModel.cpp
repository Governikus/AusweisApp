/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "LogFilesModel.h"

#include "LogHandler.h"

#include <QDebug>
#include <QSignalSpy>
#include <QTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

// convenient helper class to create and open a self-deleting temporary file
struct TemporaryLogFile // clazy:exclude=rule-of-three
{
	QString mUniqueName;

	TemporaryLogFile(const QString& pTemplateName)
	{
		// Use QTemporaryFile to obtain a unique name, as in the LogHandler class.
		QTemporaryFile tempFile(pTemplateName);
		// On Windows, calling removeOtherLogFiles() immediately after opening the file fails,
		// therefore, the file is deleted manually in the destructor.
		tempFile.setAutoRemove(false);
		QVERIFY(tempFile.open());
		// On Linux, QTemporaryFile attempts to create unnamed temporary files,
		// fileName() call is needed to find that files.
		mUniqueName = tempFile.fileName();
	}


	~TemporaryLogFile()
	{
		QFile file(mUniqueName);
		if (file.exists())
		{
			QVERIFY(file.remove());
		}
	}


};


class test_LogFilesModel
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
			LogHandler::getInstance().removeOtherLogFiles();
		}


		void test_GetSelectedLogFileName()
		{
			TemporaryLogFile oldLogFile(LogHandler::getLogFileTemplate());
			LogFilesModel model;

			QCOMPARE(model.getLogFileName(0), QLatin1String("Current log"));
			QVERIFY(!model.getLogFileName(1).isEmpty());
		}


		void test_RemoveOtherLogFiles()
		{
			TemporaryLogFile oldLogFile(LogHandler::getLogFileTemplate());
			LogFilesModel model;
			QVERIFY(model.getCount() == 2);

			model.removeOtherLogFiles();

			QCOMPARE(model.getCount(), 1);
		}


		void test_GetSelectedLogFile_first_isAlwaysEmptyString()
		{
			LogFilesModel model;
			QCOMPARE(model.getLogFilePath(0), QString());
		}


		void test_GetSelectedLogFile_notFirst_isExistingFile()
		{
			TemporaryLogFile oldLogFile(LogHandler::getLogFileTemplate());
			LogFilesModel model;
			QVERIFY(model.getCount() == 2);
			QCOMPARE(model.getLogFilePath(1), oldLogFile.mUniqueName);
		}


		void test_saveDummyLogFile()
		{
			LogFilesModel model;
			const auto fileCount = model.getCount();

			model.saveDummyLogFile();

			QCOMPARE(model.getCount(), fileCount + 1);
		}


};

QTEST_GUILESS_MAIN(test_LogFilesModel)
#include "test_LogFilesModel.moc"
