/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateTransmitPersonalization.h"

#include "ReaderManager.h"
#include "context/PersonalizationContext.h"
#include "paos/retrieve/TransmitParser.h"

#include "TestFileHelper.h"
#include "mock/eid_applet_interface_mock.h"

#include <QSharedPointer>

#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


class test_StateTransmitPersonalization
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
			mContext = QSharedPointer<PersonalizationContext>::create(QString());
			QByteArray content = TestFileHelper::readFile(":/paos/Transmit.xml");
			QSharedPointer<Transmit> transmit(static_cast<Transmit*>(TransmitParser().parse(content)));
			QVERIFY(!transmit.isNull());
			mContext->setTransmit(transmit);

			QSharedPointer<TransmitResponse> response(new TransmitResponse());
			mContext->setTransmitResponse(response);
		}


		void noPersonalization()
		{
			StateTransmitPersonalization state(mContext);
			QSignalSpy spyAbort(&state, &StateTransmitPersonalization::fireAbort);
			QSignalSpy spyContinue(&state, &StateTransmitPersonalization::fireContinue);

			QTest::ignoreMessage(QtWarningMsg, "performPersonalization failed");
			state.run();

			QTRY_COMPARE(spyAbort.size(), 1);  // clazy:exclude=qstring-allocations
			QCOMPARE(spyContinue.size(), 0);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::Workflow_Smart_eID_Personalization_Failed);
			QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Transmit_Personalization_Size_Mismatch);
		}


		void performPersonalizationFails()
		{
			setSmartEidStatus(EidStatus::NO_PERSONALIZATION);

			StateTransmitPersonalization state(mContext);
			QSignalSpy spyAbort(&state, &StateTransmitPersonalization::fireAbort);
			QSignalSpy spyContinue(&state, &StateTransmitPersonalization::fireContinue);

			QTest::ignoreMessage(QtWarningMsg, "performPersonalization failed");
			state.run();

			QTRY_COMPARE(spyAbort.size(), 1);  // clazy:exclude=qstring-allocations
			QCOMPARE(spyContinue.size(), 0);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::Workflow_Smart_eID_Personalization_Failed);
			QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Transmit_Personalization_Size_Mismatch);
		}


		void performPersonalizationStatusCode()
		{
			setSmartEidStatus(EidStatus::NO_PERSONALIZATION);
			setEphemeralResult(false);
			setPersonalizationResult({EidServiceResult::SUCCESS, QByteArray("9000").toStdString()});

			StateTransmitPersonalization state(mContext);
			QSignalSpy spyAbort(&state, &StateTransmitPersonalization::fireAbort);
			QSignalSpy spyContinue(&state, &StateTransmitPersonalization::fireContinue);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtWarningMsg, "Transmit unsuccessful. StatusCode does not start with acceptable status code QList(\"9001\")");
#else
			QTest::ignoreMessage(QtWarningMsg, "Transmit unsuccessful. StatusCode does not start with acceptable status code (\"9001\")");
#endif
			state.run();

			QTRY_COMPARE(spyAbort.size(), 1);  // clazy:exclude=qstring-allocations
			QCOMPARE(spyContinue.size(), 0);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::Workflow_Smart_eID_Personalization_Failed);
			QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Transmit_Personalization_Size_Mismatch);
		}


		void performPersonalizationSuccess()
		{
			QByteArray content = TestFileHelper::readFile(":/paos/Transmit2.xml");
			QSharedPointer<Transmit> transmit(static_cast<Transmit*>(TransmitParser().parse(content)));
			QVERIFY(!transmit.isNull());
			mContext->setTransmit(transmit);

			setSmartEidStatus(EidStatus::NO_PERSONALIZATION);
			setEphemeralResult(false);
			setPersonalizationResult({EidServiceResult::SUCCESS, QByteArray::fromHex("9000").toBase64().toStdString()});

			StateTransmitPersonalization state(mContext);
			QSignalSpy spyAbort(&state, &StateTransmitPersonalization::fireAbort);
			QSignalSpy spyContinue(&state, &StateTransmitPersonalization::fireContinue);

			state.run();

			QTRY_COMPARE(spyContinue.size(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyAbort.size(), 0);
			QCOMPARE(mContext->getStatus(), GlobalStatus::Code::No_Error);
		}


};

QTEST_GUILESS_MAIN(test_StateTransmitPersonalization)
#include "test_StateTransmitPersonalization.moc"
