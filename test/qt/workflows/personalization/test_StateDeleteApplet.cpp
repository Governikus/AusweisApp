/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateDeleteApplet.h"

#include "ReaderManager.h"
#include "context/PersonalizationContext.h"

#include "mock/eid_applet_interface_mock.h"

#include <QSharedPointer>
#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


Q_DECLARE_METATYPE(EidServiceResult)
Q_DECLARE_METATYPE(std::optional<FailureCode>)


class test_StateDeleteApplet
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void init()
		{
			initMock();
		}


		void run_data()
		{
			QTest::addColumn<EidServiceResult>("deleteSmartResult");
			QTest::addColumn<QStringList>("logEntries");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");

			QTest::addRow("SUCCESS")
				<< EidServiceResult::SUCCESS
				<< QStringList {"Successfully deleted Smart-eID"}
				<< GlobalStatus::Code::No_Error
				<< std::optional<FailureCode>();
			QTest::addRow("ERROR")
				<< EidServiceResult::ERROR
				<< QStringList {"Deletion of Smart-eID failed"}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Smart_Service_Response_Fail);
			QTest::addRow("UNSUPPORTED")
				<< EidServiceResult::UNSUPPORTED
				<< QStringList {"Deletion of Smart-eID failed"}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Smart_Service_Response_Unsupported);
			QTest::addRow("NFC_NOT_ACTIVATED")
				<< EidServiceResult::NFC_NOT_ACTIVATED
				<< QStringList {"Deletion of Smart-eID failed"}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Smart_Service_Response_Nfc_Disabled);
			QTest::addRow("OVERLOAD_PROTECTION")
				<< EidServiceResult::OVERLOAD_PROTECTION
				<< QStringList {"Deletion of Smart-eID failed"}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Smart_Service_Response_Overload);
			QTest::addRow("UNDER_MAINTENANCE")
				<< EidServiceResult::UNDER_MAINTENANCE
				<< QStringList {"Deletion of Smart-eID failed"}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Smart_Service_Response_Maintenance);
			QTest::addRow("INTEGRITY_CHECK_FAILED")
				<< EidServiceResult::INTEGRITY_CHECK_FAILED
				<< QStringList {"Deletion of Smart-eID failed"}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Smart_Service_Response_Integrity_Check_Failed);
			QTest::addRow("NOT_AUTHENTICATED")
				<< EidServiceResult::NOT_AUTHENTICATED
				<< QStringList {"Deletion of Smart-eID failed"}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Smart_Service_Response_Not_Authenticated);
		}


		void run()
		{
			QFETCH(EidServiceResult, deleteSmartResult);
			QFETCH(QStringList, logEntries);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(std::optional<FailureCode>, failureCode);

			setDeleteSmartEidResult(deleteSmartResult);

			auto context = QSharedPointer<PersonalizationContext>::create(QString());

			StateDeleteApplet state(context);
			QSignalSpy spyAbort(&state, &StateDeleteApplet::fireAbort);
			QSignalSpy spyContinue(&state, &StateDeleteApplet::fireContinue);

			for (const auto& log : std::as_const(logEntries))
			{
				QTest::ignoreMessage(QtDebugMsg, log.toUtf8().data());
			}
			state.run();

			if (statusCode == GlobalStatus::Code::No_Error)
			{
				QTRY_COMPARE(spyContinue.size(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(spyAbort.size(), 0);
			}
			else
			{
				QTRY_COMPARE(spyAbort.size(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(spyContinue.size(), 0);
			}
			QCOMPARE(context->getStatus(), statusCode);
			QCOMPARE(context->getFailureCode(), failureCode);
		}


};

QTEST_GUILESS_MAIN(test_StateDeleteApplet)
#include "test_StateDeleteApplet.moc"
