/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StatePrepareApplet.h"

#include "ReaderManager.h"
#include "SmartManager.h"
#include "context/PersonalizationContext.h"
#include "paos/retrieve/TransmitParser.h"

#include "TestFileHelper.h"
#include "mock/eid_applet_interface_mock.h"

#include <QSharedPointer>

#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


class test_StatePrepareApplet
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void run_data()
		{
			QTest::addColumn<EidStatus>("status");
			QTest::addColumn<EidUpdateInfo>("updateInfo");
			QTest::addColumn<EidServiceResult>("result");
			QTest::addColumn<QStringList>("logEntries");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");

			QTest::newRow("unavailable")
				<< EidStatus::UNAVAILABLE << EidUpdateInfo::UP_TO_DATE << EidServiceResult::SUCCESS
				<< QStringList("Smart-eID is not available on this device") << std::optional<FailureCode>(FailureCode::Reason::Prepare_Applet_Unavailable);

			QTest::newRow("no_provisioning - success")
				<< EidStatus::NO_PROVISIONING << EidUpdateInfo::UP_TO_DATE << EidServiceResult::SUCCESS
				<< QStringList("Successfully installed Smart-eID") << std::optional<FailureCode>();
			QTest::newRow("no_provisioning - fail")
				<< EidStatus::NO_PROVISIONING << EidUpdateInfo::UP_TO_DATE << EidServiceResult::ERROR
				<< QStringList("Installation of Smart-eID failed") << std::optional<FailureCode>(FailureCode::Reason::Prepare_Applet_Installation_Failed);

			QTest::newRow("no_personalization - internal_error")
				<< EidStatus::NO_PERSONALIZATION << EidUpdateInfo::INTERNAL_ERROR << EidServiceResult::SUCCESS
				<< QStringList("updateInfo() failed") << std::optional<FailureCode>(FailureCode::Reason::Prepare_Applet_UpdateInfo_Call_Failed);
			QTest::newRow("no_personalization - up_to_data")
				<< EidStatus::NO_PERSONALIZATION << EidUpdateInfo::UP_TO_DATE << EidServiceResult::SUCCESS
				<< QStringList("No update available") << std::optional<FailureCode>();
			QTest::newRow("no_personalization - unavailable")
				<< EidStatus::NO_PERSONALIZATION << EidUpdateInfo::UNAVAILABLE << EidServiceResult::SUCCESS
				<< QStringList("No update available") << std::optional<FailureCode>();
			QTest::newRow("no_personalization - no_provisioning")
				<< EidStatus::NO_PERSONALIZATION << EidUpdateInfo::NO_PROVISIONING << EidServiceResult::SUCCESS
				<< QStringList("No update available") << std::optional<FailureCode>();
			QTest::newRow("no_personalization - update_available - success")
				<< EidStatus::NO_PERSONALIZATION << EidUpdateInfo::UPDATE_AVAILABLE << EidServiceResult::SUCCESS
				<< QStringList({"Update available, delete the Smart-eID first", "Successfully deleted Smart-eID"}) << std::optional<FailureCode>();
			QTest::newRow("no_personalization - update_available - fail")
				<< EidStatus::NO_PERSONALIZATION << EidUpdateInfo::UPDATE_AVAILABLE << EidServiceResult::ERROR
				<< QStringList({"Update available, delete the Smart-eID first", "Deletion of Smart-eID failed"}) << std::optional<FailureCode>(FailureCode::Reason::Prepare_Applet_Delete_Smart_Failed);

			QTest::newRow("applet_unusable - success")
				<< EidStatus::APPLET_UNUSABLE << EidUpdateInfo::UP_TO_DATE << EidServiceResult::SUCCESS
				<< QStringList("Successfully deleted Smart-eID") << std::optional<FailureCode>();
			QTest::newRow("applet_unusable - fail")
				<< EidStatus::APPLET_UNUSABLE << EidUpdateInfo::UP_TO_DATE << EidServiceResult::ERROR
				<< QStringList("Deletion of Smart-eID failed") << std::optional<FailureCode>(FailureCode::Reason::Prepare_Applet_Delete_Smart_Failed);

			QTest::newRow("personalized - fail")
				<< EidStatus::PERSONALIZED << EidUpdateInfo::UP_TO_DATE << EidServiceResult::ERROR
				<< QStringList("Deletion of Smart-eID personalization failed") << std::optional<FailureCode>(FailureCode::Reason::Prepare_Applet_Delete_Personalization_Failed);
			QTest::newRow("personalized - internal_error")
				<< EidStatus::PERSONALIZED << EidUpdateInfo::INTERNAL_ERROR << EidServiceResult::SUCCESS
				<< QStringList({"Successfully deleted the Smart-eID personalization", "updateInfo() failed"}) << std::optional<FailureCode>(FailureCode::Reason::Prepare_Applet_UpdateInfo_Call_Failed);
			QTest::newRow("personalized - up_to_data")
				<< EidStatus::PERSONALIZED << EidUpdateInfo::UP_TO_DATE << EidServiceResult::SUCCESS
				<< QStringList({"Successfully deleted the Smart-eID personalization", "No update available"}) << std::optional<FailureCode>();
			QTest::newRow("personalized - unavailable")
				<< EidStatus::PERSONALIZED << EidUpdateInfo::UNAVAILABLE << EidServiceResult::SUCCESS
				<< QStringList({"Successfully deleted the Smart-eID personalization", "No update available"}) << std::optional<FailureCode>();
			QTest::newRow("personalized - no_provisioning")
				<< EidStatus::PERSONALIZED << EidUpdateInfo::NO_PROVISIONING << EidServiceResult::SUCCESS
				<< QStringList({"Successfully deleted the Smart-eID personalization", "No update available"}) << std::optional<FailureCode>();
			QTest::newRow("personalized - update_available")
				<< EidStatus::PERSONALIZED << EidUpdateInfo::UPDATE_AVAILABLE << EidServiceResult::SUCCESS
				<< QStringList({"Successfully deleted the Smart-eID personalization", "Update available, delete the Smart-eID first", "Successfully deleted Smart-eID"}) << std::optional<FailureCode>();

			QTest::newRow("internal_error")
				<< EidStatus::INTERNAL_ERROR << EidUpdateInfo::UP_TO_DATE << EidServiceResult::SUCCESS
				<< QStringList("getSmartEidStatus() failed") << std::optional<FailureCode>(FailureCode::Reason::Prepare_Applet_Status_Call_Failed);
		}


		void run()
		{
			QFETCH(EidStatus, status);
			QFETCH(EidUpdateInfo, updateInfo);
			QFETCH(EidServiceResult, result);
			QFETCH(QStringList, logEntries);
			QFETCH(std::optional<FailureCode>, failureCode);

			setSmartEidStatus(status);
			setUpdateInfo(updateInfo);
			setInstallSmartEidResult(result);
			setDeletePersonalizationResult(result);
			setDeleteSmartEidResult(result);

			auto context = QSharedPointer<PersonalizationContext>::create(QString());

			StatePrepareApplet state(context);
			QSignalSpy spyAbort(&state, &StatePrepareApplet::fireAbort);
			QSignalSpy spyContinue(&state, &StatePrepareApplet::fireContinue);

			for (const auto& log : std::as_const(logEntries))
			{
				QTest::ignoreMessage(QtDebugMsg, log.toUtf8().data());
			}
			state.run();

			if (failureCode.has_value())
			{
				QTRY_COMPARE(spyAbort.size(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(spyContinue.size(), 0);
				QCOMPARE(context->getStatus(), GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
				QCOMPARE(context->getFailureCode(), failureCode);
			}
			else
			{
				QTRY_COMPARE(spyContinue.size(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(spyAbort.size(), 0);
				QCOMPARE(context->getStatus(), GlobalStatus::Code::No_Error);
			}
		}


		void progress_data()
		{
			QTest::addColumn<int>("initial");
			QTest::addColumn<int>("max");
			QTest::addColumn<int>("progress");
			QTest::addColumn<int>("expected");

			QTest::newRow("full_10") << 0 << 100 << 10 << 10;
			QTest::newRow("full_50") << 0 << 100 << 50 << 50;
			QTest::newRow("full_99") << 0 << 100 << 99 << 99;
			QTest::newRow("full_100") << 0 << 100 << 100 << 100;

			QTest::newRow("first_half_10") << 0 << 50 << 10 << 5;
			QTest::newRow("first_half_50") << 0 << 50 << 50 << 25;
			QTest::newRow("first_half_90") << 0 << 50 << 90 << 45;
			QTest::newRow("first_half_100") << 0 << 50 << 100 << 50;

			QTest::newRow("second_half_10") << 50 << 100 << 10 << 55;
			QTest::newRow("second_half_50") << 50 << 100 << 50 << 75;
			QTest::newRow("second_half_90") << 50 << 100 << 90 << 95;
			QTest::newRow("second_half_100") << 50 << 100 << 100 << 100;
		}


		void progress()
		{
			QFETCH(int, initial);
			QFETCH(int, max);
			QFETCH(int, progress);
			QFETCH(int, expected);

			auto context = QSharedPointer<PersonalizationContext>::create(QString());
			QSignalSpy spy(context.get(), &WorkflowContext::fireProgressChanged);

			StatePrepareApplet state(context);
			state.setProgress(progress, QStringLiteral("dummy"), initial, max);
			QCOMPARE(spy.count(), 1);
			QCOMPARE(context->getProgressValue(), expected);
		}


};

QTEST_GUILESS_MAIN(test_StatePrepareApplet)
#include "test_StatePrepareApplet.moc"
