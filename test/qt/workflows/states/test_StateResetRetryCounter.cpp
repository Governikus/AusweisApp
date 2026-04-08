/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateResetRetryCounter.h"

#include "FailureCode.h"
#include "MockCardConnectionWorker.h"
#include "TestHookThread.h"
#include "TestWorkflowContext.h"

#include <QSharedPointer>
#include <QtTest>

#include <optional>


using namespace governikus;


class test_StateResetRetryCounter
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_RunNoCardConnection()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			StateResetRetryCounter state(context);

			QSignalSpy spyContinue(&state, &StateResetRetryCounter::fireContinue);
			QSignalSpy spyAbort(&state, &StateResetRetryCounter::fireAbort);
			QSignalSpy spyNoCardConnection(&state, &StateResetRetryCounter::fireNoCardConnection);

			QTest::ignoreMessage(QtDebugMsg, "No card connection available");
			state.run();
			QCOMPARE(spyContinue.count(), 0);
			QCOMPARE(spyAbort.count(), 0);
			QCOMPARE(spyNoCardConnection.count(), 1);
			QVERIFY(!context->getFailureCode().has_value());
		}


		void test_ResetRetryCounterDone_data()
		{
			QTest::addColumn<CardReturnCode>("returnCode");
			QTest::addColumn<StatusCode>("statusCode");
			QTest::addColumn<QByteArray>("logMessage");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");

			QTest::addRow("OK") << CardReturnCode::OK << StatusCode::SUCCESS << QByteArray()
								<< std::optional<FailureCode>();
			QTest::addRow("PUK inoperative") << CardReturnCode::OK << StatusCode::ACCESS_DENIED << QByteArray()
											 << std::optional<FailureCode>(FailureCode::Reason::Establish_Pace_Channel_Puk_Inoperative);
			QTest::addRow("Unexpected statusCode") << CardReturnCode::OK << StatusCode::COMMAND_NOT_ALLOWED
												   << QByteArray("Received an unexpected StatusCode (COMMAND_NOT_ALLOWED), cannot reset retry counter")
												   << std::optional<FailureCode>();
			QTest::addRow("Response empty") << CardReturnCode::RESPONSE_EMPTY << StatusCode::UNKNOWN
											<< QByteArray("An error (RESPONSE_EMPTY) occurred while communicating with the card reader")
											<< std::optional<FailureCode>();
			QTest::addRow("Command failed") << CardReturnCode::COMMAND_FAILED << StatusCode::UNKNOWN
											<< QByteArray("An error (COMMAND_FAILED) occurred while communicating with the card reader")
											<< std::optional<FailureCode>();
		}


		void test_ResetRetryCounterDone()
		{
			TestHookThread workerThread;

			{
				QFETCH(CardReturnCode, returnCode);
				QFETCH(StatusCode, statusCode);
				QFETCH(QByteArray, logMessage);
				QFETCH(std::optional<FailureCode>, failureCode);

				const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
				StateResetRetryCounter state(context);
				auto worker = MockCardConnectionWorker::create(&workerThread);
				context->setCardConnection(QSharedPointer<CardConnection>::create(worker));
				worker->addResponse(returnCode, statusCode == StatusCode::UNKNOWN ? ResponseApdu() : ResponseApdu(statusCode));

				QSignalSpy spyContinue(&state, &StateResetRetryCounter::fireContinue);
				QSignalSpy spyAbort(&state, &StateResetRetryCounter::fireAbort);
				QSignalSpy spyNoCardConnection(&state, &StateResetRetryCounter::fireNoCardConnection);
				if (!logMessage.isNull())
				{
					QTest::ignoreMessage(QtCriticalMsg, logMessage.data());
				}
				state.run();
				if (returnCode == CardReturnCode::OK && (statusCode == StatusCode::SUCCESS || statusCode == StatusCode::ACCESS_DENIED))
				{
					if (failureCode.has_value())
					{
						QTRY_COMPARE(spyAbort.count(), 1);
						QCOMPARE(spyContinue.count(), 0);
					}
					else
					{
						QTRY_COMPARE(spyContinue.count(), 1);
						QCOMPARE(spyAbort.count(), 0);
					}
					QCOMPARE(spyNoCardConnection.count(), 0);
				}
				else
				{
					QTRY_COMPARE(spyNoCardConnection.count(), 1);
					QCOMPARE(spyContinue.count(), 0);
					QCOMPARE(spyAbort.count(), 0);
				}
				QCOMPARE(context->getFailureCode(), failureCode);
			}
		}


};

QTEST_GUILESS_MAIN(test_StateResetRetryCounter)
#include "test_StateResetRetryCounter.moc"
