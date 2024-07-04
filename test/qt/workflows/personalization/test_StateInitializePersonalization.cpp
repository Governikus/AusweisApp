/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateInitializePersonalization.h"

#include "LogHandler.h"
#include "ReaderManager.h"
#include "SmartManager.h"
#include "context/PersonalizationContext.h"

#include "mock/eid_applet_interface_mock.h"

#include <QSharedPointer>

#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StateInitializePersonalization
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


		void cleanup()
		{
			SmartManager::releaseConnection();
		}


		void fail()
		{
			StateInitializePersonalization state(mContext);
			QSignalSpy spyAbort(&state, &StateInitializePersonalization::fireAbort);
			QSignalSpy spyContinue(&state, &StateInitializePersonalization::fireContinue);

			setInitializePersonalizationResult({EidServiceResult::ERROR});
			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Initialization of personalization failed"_L1));
			state.run();

			QTRY_COMPARE(spyAbort.size(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyContinue.size(), 0);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::Workflow_Smart_eID_PrePersonalization_Failed);
			QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Initialize_Personalization_Failed);

			QCOMPARE(dequeueReceivedParameter(), QString());
			QCOMPARE(dequeueReceivedParameter(), QString());
		}


		void success_data()
		{
			QTest::addColumn<SmartEidType>("type");
			QTest::addColumn<QLatin1String>("pin");

			QTest::newRow("UNKNOWN") << SmartEidType::UNKNOWN << QLatin1String();
			QTest::newRow("APPLET") << SmartEidType::APPLET << QLatin1String();
			QTest::newRow("NON_APPLET") << SmartEidType::NON_APPLET << "123456"_L1;
		}


		void success()
		{
			QFETCH(SmartEidType, type);
			QFETCH(QLatin1String, pin);

			setSmartEidStatus(EidStatus::NO_PERSONALIZATION);
			setInitializePersonalizationResult({EidServiceResult::SUCCESS, std::string("data containing the PIN when Smart-eID is of type HWKeyStore")});
			mContext->setServiceInformation(type, QString(), QString());
			mContext->setNewPin("123456"_L1);
			mContext->setChallenge("FooBar"_L1);

			StateInitializePersonalization state(mContext);
			QSignalSpy spyAbort(&state, &StateInitializePersonalization::fireAbort);
			QSignalSpy spyContinue(&state, &StateInitializePersonalization::fireContinue);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			state.run();

			QTRY_COMPARE(spyContinue.size(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyAbort.size(), 0);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::No_Error);
			QCOMPARE(mContext->getPreparePersonalizationData(), "data containing the PIN when Smart-eID is of type HWKeyStore"_L1);

			for (const auto& entry : logSpy)
			{
				QVERIFY(!entry.at(0).toString().contains("data containing the PIN"_L1));
			}

			QCOMPARE(dequeueReceivedParameter(), "FooBar"_L1);
			QCOMPARE(dequeueReceivedParameter(), pin);
		}


};

QTEST_GUILESS_MAIN(test_StateInitializePersonalization)
#include "test_StateInitializePersonalization.moc"
