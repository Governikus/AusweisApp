/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetServiceInformation.h"

#include "LogHandler.h"
#include "ReaderManager.h"
#include "context/PersonalizationContext.h"

#include "mock/eid_applet_interface_mock.h"

#include <QSharedPointer>

#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StateGetServiceInformation
	: public QObject
{
	Q_OBJECT

	QSharedPointer<PersonalizationContext> mContext;

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
			Env::getSingleton<LogHandler>()->init();
			mContext = QSharedPointer<PersonalizationContext>::create(QString());
		}


		void fail()
		{
			StateGetServiceInformation state(mContext);
			QSignalSpy spyAbort(&state, &StateGetServiceInformation::fireAbort);
			QSignalSpy spyContinue(&state, &StateGetServiceInformation::fireContinue);

			setServiceInformation({EidServiceResult::ERROR});
			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("ServiceInformation query failed"_L1));
			state.run();

			QTRY_COMPARE(spyAbort.size(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyContinue.size(), 0);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::Workflow_Smart_eID_ServiceInformation_Query_Failed);
			QCOMPARE(mContext->getSmartEidType(), SmartEidType::UNKNOWN);
			QCOMPARE(mContext->getChallengeType(), QString());
		}


		void success()
		{
			setSmartEidStatus(EidStatus::NO_PERSONALIZATION);
			setServiceInformation({EidServiceResult::SUCCESS, SmartEidType::APPLET, std::string("UUID")});

			StateGetServiceInformation state(mContext);
			QSignalSpy spyAbort(&state, &StateGetServiceInformation::fireAbort);
			QSignalSpy spyContinue(&state, &StateGetServiceInformation::fireContinue);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			state.run();

			QTRY_COMPARE(spyContinue.size(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyAbort.size(), 0);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::No_Error);
			QCOMPARE(mContext->getSmartEidType(), SmartEidType::APPLET);
			QCOMPARE(mContext->getChallengeType(), "UUID"_L1);
		}


};

QTEST_GUILESS_MAIN(test_StateGetServiceInformation)
#include "test_StateGetServiceInformation.moc"
