/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateInitializePersonalization.h"

#include "LogHandler.h"
#include "ReaderManager.h"
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
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished
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
			StateInitializePersonalization state(mContext);
			QSignalSpy spyAbort(&state, &StateInitializePersonalization::fireAbort);
			QSignalSpy spyContinue(&state, &StateInitializePersonalization::fireContinue);

			setInitializePersonalizationResult({EidServiceResult::ERROR});
			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Initialization of personalization failed"));
			state.run();

			QTRY_COMPARE(spyAbort.size(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyContinue.size(), 0);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::Workflow_Smart_eID_PrePersonalization_Failed);
		}


		void success()
		{
			setSmartEidStatus(EidStatus::NO_PERSONALIZATION);
			setInitializePersonalizationResult({EidServiceResult::SUCCESS, std::string("data containing the PIN when Smart-eID is of type HWKeyStore")});

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
		}


};

QTEST_GUILESS_MAIN(test_StateInitializePersonalization)
#include "test_StateInitializePersonalization.moc"
