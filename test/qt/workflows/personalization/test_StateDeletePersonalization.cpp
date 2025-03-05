/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateDeletePersonalization.h"

#include "ReaderManager.h"
#include "context/PersonalizationContext.h"

#include "mock/eid_applet_interface_mock.h"

#include <QSharedPointer>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(EidServiceResult)
Q_DECLARE_METATYPE(std::optional<FailureCode>)


class test_StateDeletePersonalization
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			auto* readerManager = Env::getSingleton<ReaderManager>();
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
			QTest::addColumn<EidServiceResult>("deletePersonalizationResult");
			QTest::addColumn<QStringList>("logEntries");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");

			QTest::addRow("SUCCESS")
				<< EidServiceResult::SUCCESS
				<< QStringList {"Successfully deleted the Smart-eID personalization"_L1}
				<< GlobalStatus::Code::No_Error
				<< std::optional<FailureCode>();
			QTest::addRow("ERROR")
				<< EidServiceResult::ERROR
				<< QStringList {"Deletion of Smart-eID personalization failed"_L1}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Personalization_Failed);
			QTest::addRow("UNSUPPORTED")
				<< EidServiceResult::UNSUPPORTED
				<< QStringList {"Deletion of Smart-eID personalization failed"_L1}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Personalization_Failed);
			QTest::addRow("OVERLOAD_PROTECTION")
				<< EidServiceResult::OVERLOAD_PROTECTION
				<< QStringList {"Deletion of Smart-eID personalization failed"_L1}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Personalization_Failed);
			QTest::addRow("UNDER_MAINTENANCE")
				<< EidServiceResult::UNDER_MAINTENANCE
				<< QStringList {"Deletion of Smart-eID personalization failed"_L1}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Personalization_Failed);
			QTest::addRow("NFC_NOT_ACTIVATED")
				<< EidServiceResult::NFC_NOT_ACTIVATED
				<< QStringList {"Deletion of Smart-eID personalization failed"_L1}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Personalization_Failed);
			QTest::addRow("INTEGRITY_CHECK_FAILED")
				<< EidServiceResult::INTEGRITY_CHECK_FAILED
				<< QStringList {"Deletion of Smart-eID personalization failed"_L1}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Personalization_Failed);
			QTest::addRow("NOT_AUTHENTICATED")
				<< EidServiceResult::NOT_AUTHENTICATED
				<< QStringList {"Deletion of Smart-eID personalization failed"_L1}
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Delete_Personalization_Failed);
		}


		void run()
		{
			QFETCH(EidServiceResult, deletePersonalizationResult);
			QFETCH(QStringList, logEntries);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(std::optional<FailureCode>, failureCode);

			setDeletePersonalizationResult(deletePersonalizationResult);

			auto context = QSharedPointer<PersonalizationContext>::create(QString());

			StateDeletePersonalization state(context);
			QSignalSpy spyAbort(&state, &StateDeletePersonalization::fireAbort);
			QSignalSpy spyContinue(&state, &StateDeletePersonalization::fireContinue);

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

QTEST_GUILESS_MAIN(test_StateDeletePersonalization)
#include "test_StateDeletePersonalization.moc"
