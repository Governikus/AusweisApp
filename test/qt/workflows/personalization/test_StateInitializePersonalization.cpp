/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
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


using namespace governikus;


class test_StateInitializePersonalization
	: public QObject
{
	Q_OBJECT

	QSharedPointer<PersonalizationContext> mContext;

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
			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Initialization of personalization failed"));
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
			QTest::addColumn<QString>("pin");

			QTest::newRow("UNKNOWN") << SmartEidType::UNKNOWN << QString();
			QTest::newRow("APPLET") << SmartEidType::APPLET << QString();
			QTest::newRow("NON_APPLET") << SmartEidType::NON_APPLET << QString("123456");
		}


		void success()
		{
			QFETCH(SmartEidType, type);
			QFETCH(QString, pin);

			setSmartEidStatus(EidStatus::NO_PERSONALIZATION);
			setInitializePersonalizationResult({EidServiceResult::SUCCESS, std::string("data containing the PIN when Smart-eID is of type HWKeyStore")});
			mContext->setServiceInformation(type, QString(), QString());
			mContext->setNewPin(QString("123456"));
			mContext->setChallenge(QString("FooBar"));

			StateInitializePersonalization state(mContext);
			QSignalSpy spyAbort(&state, &StateInitializePersonalization::fireAbort);
			QSignalSpy spyContinue(&state, &StateInitializePersonalization::fireContinue);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			state.run();

			QTRY_COMPARE(spyContinue.size(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyAbort.size(), 0);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::No_Error);
			QCOMPARE(mContext->getPreparePersonalizationData(), QString("data containing the PIN when Smart-eID is of type HWKeyStore"));

			for (const auto& entry : logSpy)
			{
				QVERIFY(!entry.at(0).toString().contains(QLatin1String("data containing the PIN")));
			}

			QCOMPARE(dequeueReceivedParameter(), QString("FooBar"));
			QCOMPARE(dequeueReceivedParameter(), pin);
		}


};

QTEST_GUILESS_MAIN(test_StateInitializePersonalization)
#include "test_StateInitializePersonalization.moc"
