/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"
#include "AppUpdateDataModel.h"
#include "AppUpdater.h"
#include "VersionNumber.h"

#include <QtTest>


using namespace governikus;

class test_AppUpdateDataModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			QApplication::setApplicationVersion(QStringLiteral("2.2.2"));
		}


		void cleanup()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			settings.setLastAppcastDate(QDateTime());
			settings.setLastAppcastVersion(QString());
		}


		void test_isUpdateAvailable_data()
		{
			QTest::addColumn<QDateTime>("lastAppcastDate");
			QTest::addColumn<QString>("lastAppcastVersion");
			QTest::addColumn<bool>("result");

			QTest::addRow("No update ever") << QDateTime() << QString() << false;
			QTest::addRow("App is newer than update") << QDateTime(QDate(2026, 1, 2), QTime(12, 34)) << "1.1.1" << false;
			QTest::addRow("No update available") << QDateTime(QDate(2026, 1, 2), QTime(12, 34)) << "2.2.2" << false;
			QTest::addRow("Update available") << QDateTime(QDate(2026, 1, 2), QTime(12, 34)) << "3.3.3" << true;
		}


		void test_isUpdateAvailable()
		{

			QFETCH(QDateTime, lastAppcastDate);
			QFETCH(QString, lastAppcastVersion);
			QFETCH(bool, result);

			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			settings.setLastAppcastDate(lastAppcastDate);
			settings.setLastAppcastVersion(lastAppcastVersion);

			auto* model = Env::getSingleton<AppUpdateDataModel>();
			QCOMPARE(model->isUpdateAvailable(), result);
			QVERIFY(!model->isValid());
		}


		void test_onAppDownloadFinished_data()
		{
			QTest::addColumn<GlobalStatus::Code>("globalStatus");
			QTest::addColumn<bool>("failedSignal");
			QTest::addColumn<bool>("abortedSignal");
			QTest::addColumn<bool>("errorMessage");
			QTest::addColumn<bool>("supportInfo");

			QTest::addRow("Data corrupted") << GlobalStatus::Code::Downloader_Data_Corrupted << true << false << true << false;
			QTest::addRow("Aborted") << GlobalStatus::Code::Downloader_Aborted << false << true << false << false;
			QTest::addRow("Execution failed") << GlobalStatus::Code::Update_Execution_Failed << true << false << true << true;
			QTest::addRow("Unknown") << GlobalStatus::Code::Unknown_Error << true << false << true << false;
#if !defined(Q_OS_WIN) // Avoid to launch msiexec.exe in Unit Tests
			QTest::addRow("No error -> Execution failed") << GlobalStatus::Code::No_Error << true << false << true << true;
#endif
		}


		void test_onAppDownloadFinished()
		{
			auto* model = Env::getSingleton<AppUpdateDataModel>();

			QFETCH(GlobalStatus::Code, globalStatus);
			QFETCH(bool, abortedSignal);
			QFETCH(bool, failedSignal);
			QFETCH(bool, errorMessage);
			QFETCH(bool, supportInfo);

			QSignalSpy abortedSpy(model, &AppUpdateDataModel::fireAppUpdateAborted);
			QSignalSpy failedSpy(model, &AppUpdateDataModel::fireAppUpdateFailed);

			model->onAppDownloadFinished(GlobalStatus(globalStatus));

			QCOMPARE(abortedSpy.count(), abortedSignal ? 1 : 0);
			QCOMPARE(failedSpy.count(), failedSignal ? 1 : 0);

			if (failedSignal)
			{
				const auto arguments = failedSpy.takeFirst();
				QVERIFY(arguments.at(0).toString().isEmpty() != errorMessage);
				QVERIFY(arguments.at(1).toString().isEmpty() != supportInfo);
			}
		}


		void test_onAppcastFinished_data()
		{
			QTest::addColumn<QString>("version");
			QTest::addColumn<bool>("valid");
			QTest::addColumn<GlobalStatusCode>("parsingResult");

			QTest::addColumn<bool>("appcastFailed");
			QTest::addColumn<bool>("appcastUpdateTextEmpty");
			QTest::addColumn<bool>("appcastNoUpdateTextEmpty");
			QTest::addColumn<bool>("appcastErrorTextEmpty");

			const auto& versionCurrent = QStringLiteral("2.2.2");
			const auto& versionUpdate = QStringLiteral("3.3.3");

			QTest::addRow("Abort") << QString() << false << GlobalStatus::Code::Downloader_Aborted << false << true << true << true;
			QTest::addRow("Missing Platform") << QString() << false << GlobalStatus::Code::Downloader_Missing_Platform << true << true << false << false;
			QTest::addRow("No Update") << versionCurrent << true << GlobalStatus::Code::No_Error << false << false << false << true;
			QTest::addRow("Update") << versionUpdate << true << GlobalStatus::Code::No_Error << false << false << false << true;
		}


		void test_onAppcastFinished()
		{
			auto* model = Env::getSingleton<AppUpdateDataModel>();
			auto* updater = Env::getSingleton<AppUpdater>();
			model->mAppcastFinished = false;

			QFETCH(QString, version);
			QFETCH(bool, valid);
			QFETCH(GlobalStatusCode, parsingResult);

			QFETCH(bool, appcastFailed);
			QFETCH(bool, appcastUpdateTextEmpty);
			QFETCH(bool, appcastNoUpdateTextEmpty);
			QFETCH(bool, appcastErrorTextEmpty);

			AppUpdateData data(parsingResult);
			data.mVersion = version;
			if (valid)
			{
				const auto& validUrl = QUrl(QStringLiteral("https://example.com/appcast.json"));
				data.mUrl = validUrl;
				data.mChecksumUrl = validUrl;
			}

			updater->handleVersionInfoDownloadFinished(data);
			QCOMPARE(model->getAppcastFailed(), appcastFailed);
			QCOMPARE(model->getAppcastUpdateText().isEmpty(), appcastUpdateTextEmpty);
			QCOMPARE(model->getAppcastNoUpdateText().isEmpty(), appcastNoUpdateTextEmpty);
			QCOMPARE(model->getAppcastErrorText().isEmpty(), appcastErrorTextEmpty);
		}


};

QTEST_GUILESS_MAIN(test_AppUpdateDataModel)
#include "test_AppUpdateDataModel.moc"
