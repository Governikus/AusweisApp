/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateUpdateSupportInfo.h"

#include "ReaderManager.h"
#include "context/PersonalizationContext.h"

#include "mock/eid_applet_interface_mock.h"

#include <QSharedPointer>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(EidSupportStatusResult)
Q_DECLARE_METATYPE(std::optional<FailureCode>)


class test_StateUpdateSupportInfo
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
			QTest::addColumn<EidSupportStatusResult>("updateSupportInfo");
			QTest::addColumn<QByteArray>("logEntry");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");
			QTest::addColumn<QString>("signal");

			// EidServiceResult tests
			QTest::addRow("ERROR")
				<< EidSupportStatusResult {EidServiceResult::ERROR, EidSupportStatus::AVAILABLE}
				<< "updateSupportInfo() failed with \"ERROR\" 0x33000000"_ba
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Update_Support_Info_Service_Response_Fail)
				<< SIGNAL(fireAbort(const FailureCode&));
			QTest::addRow("UNSUPPORTED")
				<< EidSupportStatusResult {EidServiceResult::UNSUPPORTED, EidSupportStatus::AVAILABLE}
				<< "updateSupportInfo() failed with \"UNSUPPORTED\" 0x34000000"_ba
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Update_Support_Info_Service_Response_Unsupported)
				<< SIGNAL(fireAbort(const FailureCode&));
			QTest::addRow("OVERLOAD_PROTECTION")
				<< EidSupportStatusResult {EidServiceResult::OVERLOAD_PROTECTION, EidSupportStatus::AVAILABLE}
				<< "updateSupportInfo() failed with \"OVERLOAD_PROTECTION\" 0x35000000"_ba
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Update_Support_Info_Service_Response_Overload)
				<< SIGNAL(fireAbort(const FailureCode&));
			QTest::addRow("UNDER_MAINTENANCE")
				<< EidSupportStatusResult {EidServiceResult::UNDER_MAINTENANCE, EidSupportStatus::AVAILABLE}
				<< "updateSupportInfo() failed with \"UNDER_MAINTENANCE\" 0x36000000"_ba
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Update_Support_Info_Service_Response_Maintenance)
				<< SIGNAL(fireAbort(const FailureCode&));
			QTest::addRow("NFC_NOT_ACTIVATED")
				<< EidSupportStatusResult {EidServiceResult::NFC_NOT_ACTIVATED, EidSupportStatus::AVAILABLE}
				<< "updateSupportInfo() failed with \"NFC_NOT_ACTIVATED\" 0x37000000"_ba
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Update_Support_Info_Service_Response_Nfc_Disabled)
				<< SIGNAL(fireAbort(const FailureCode&));
			QTest::addRow("INTEGRITY_CHECK_FAILED")
				<< EidSupportStatusResult {EidServiceResult::INTEGRITY_CHECK_FAILED, EidSupportStatus::AVAILABLE}
				<< "updateSupportInfo() failed with \"INTEGRITY_CHECK_FAILED\" 0x38000000"_ba
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Update_Support_Info_Service_Response_Integrity_Check_Failed)
				<< SIGNAL(fireAbort(const FailureCode&));
			QTest::addRow("NOT_AUTHENTICATED")
				<< EidSupportStatusResult {EidServiceResult::NOT_AUTHENTICATED, EidSupportStatus::AVAILABLE}
				<< "updateSupportInfo() failed with \"NOT_AUTHENTICATED\" 0x39000000"_ba
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Update_Support_Info_Service_Response_Not_Authenticated)
				<< SIGNAL(fireAbort(const FailureCode&));

			// EidSupportStatus tests
			QTest::addRow("UNAVAILABLE")
				<< EidSupportStatusResult {EidServiceResult::SUCCESS, EidSupportStatus::UNAVAILABLE}
				<< "getSmartEidSupportInfo() finished with result \"SUCCESS\" 0x0 and status \"UNAVAILABLE\" 0x400010"_ba
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Update_Support_Info_Call_Failed)
				<< SIGNAL(fireAbort(const FailureCode&));
			QTest::addRow("AVAILABLE")
				<< EidSupportStatusResult {EidServiceResult::SUCCESS, EidSupportStatus::AVAILABLE}
				<< "getSmartEidSupportInfo() finished with result \"SUCCESS\" 0x0 and status \"AVAILABLE\" 0x400020"_ba
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Update_Support_Info_Call_Failed)
				<< SIGNAL(fireAbort(const FailureCode&));
			QTest::addRow("UP_TO_DATE")
				<< EidSupportStatusResult {EidServiceResult::SUCCESS, EidSupportStatus::UP_TO_DATE}
				<< "getSmartEidSupportInfo() finished with result \"SUCCESS\" 0x0 and status \"UP_TO_DATE\" 0x400040"_ba
				<< GlobalStatus::Code::No_Error
				<< std::optional<FailureCode>()
				<< SIGNAL(fireContinue());
			QTest::addRow("UPDATE_AVAILABLE")
				<< EidSupportStatusResult {EidServiceResult::SUCCESS, EidSupportStatus::UPDATE_AVAILABLE}
				<< "getSmartEidSupportInfo() finished with result \"SUCCESS\" 0x0 and status \"UPDATE_AVAILABLE\" 0x400030"_ba
				<< GlobalStatus::Code::No_Error
				<< std::optional<FailureCode>()
				<< SIGNAL(fireUpdateAvailable());
			QTest::addRow("INTERNAL_ERROR")
				<< EidSupportStatusResult {EidServiceResult::SUCCESS, EidSupportStatus::INTERNAL_ERROR}
				<< "getSmartEidSupportInfo() finished with result \"SUCCESS\" 0x0 and status \"INTERNAL_ERROR\" 0x2100000"_ba
				<< GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed
				<< std::optional<FailureCode>(FailureCode::Reason::Update_Support_Info_Call_Failed)
				<< SIGNAL(fireAbort(const FailureCode&));
		}


		void run()
		{
			QFETCH(EidSupportStatusResult, updateSupportInfo);
			QFETCH(QByteArray, logEntry);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(std::optional<FailureCode>, failureCode);
			QFETCH(QString, signal);

			setSmartEidSupportStatusResult(updateSupportInfo);

			auto context = QSharedPointer<PersonalizationContext>::create(QString());

			StateUpdateSupportInfo state(context);
			QSignalSpy spy(&state, qPrintable(signal));

			QTest::ignoreMessage(QtDebugMsg, logEntry.data());
			state.run();

			QTRY_COMPARE(spy.size(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(context->getStatus(), statusCode);
			QCOMPARE(context->getFailureCode(), failureCode);
		}


};

QTEST_GUILESS_MAIN(test_StateUpdateSupportInfo)
#include "test_StateUpdateSupportInfo.moc"
