/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AppUpdateDataModel.h"

#include <QtTest>


using namespace governikus;


class test_AppUpdateDataModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
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


};

QTEST_GUILESS_MAIN(test_AppUpdateDataModel)
#include "test_AppUpdateDataModel.moc"
