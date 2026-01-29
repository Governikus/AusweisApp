/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateChangeSmartPin.h"

#include "VolatileSettings.h"

#include "MockCardConnection.h"
#include "MockCardConnectionWorker.h"
#include "TestHookThread.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class MockEstablishPaceChannelCommand
	: public BaseCardCommand
{
	Q_OBJECT

	public:
		explicit MockEstablishPaceChannelCommand(const QSharedPointer<MockCardConnectionWorker>& pWorker)
			: BaseCardCommand(pWorker)
		{
		}


		~MockEstablishPaceChannelCommand() override = default;

		void internalExecute() override
		{
		}


		using BaseCardCommand::setReturnCode;
};

class test_StateChangeSmartPin
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);
		}


		void test_RunWithNewPin()
		{
			TestHookThread workerThread;

			{
				const auto& worker = MockCardConnectionWorker::create(&workerThread);
				const QSharedPointer<PersonalizationContext> context(new PersonalizationContext(QString()));
				context->setNewPin("123456"_L1);
				context->setCardConnection(QSharedPointer<CardConnection>::create(worker));
				StateChangeSmartPin state(context);

				QSignalSpy spy(&state, &StateChangeSmartPin::fireAbort);
				QTest::ignoreMessage(QtDebugMsg, "Invoke set Eid PIN command");
				state.run();
				QCOMPARE(state.mConnections.size(), 1);
				QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			}
		}


		void test_RunWithoutNewPin()
		{
			TestHookThread workerThread;

			{
				const auto& worker = MockCardConnectionWorker::create(&workerThread);
				const QSharedPointer<PersonalizationContext> context(new PersonalizationContext(QString()));
				context->setCardConnection(QSharedPointer<CardConnection>::create(worker));
				StateChangeSmartPin state(context);

				QSignalSpy spy(&state, &StateChangeSmartPin::fireAbort);
				state.run();
				QCOMPARE(state.mConnections.size(), 1);
				QCOMPARE(spy.count(), 0); // clazy:exclude=qstring-allocations

				QTest::ignoreMessage(QtDebugMsg, "Invoke set Eid PIN command");
				context->setNewPin("123456"_L1);
				QCOMPARE(state.mConnections.size(), 2);
				QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			}
		}


		void test_OnSetEidPinDone()
		{
			const QSharedPointer<PersonalizationContext> context(new PersonalizationContext(QString()));
			StateChangeSmartPin state(context);
			const auto& worker = MockCardConnectionWorker::create();
			const QSharedPointer<MockEstablishPaceChannelCommand> command(new MockEstablishPaceChannelCommand(worker));
			const ReaderInfo readerInfo("NFC"_L1, ReaderManagerPluginType::SMART);
			const QSharedPointer<CardConnection> connection(new MockCardConnection(readerInfo));
			context->setCardConnection(connection);

			QSignalSpy spyContinue(&state, &StateChangeSmartPin::fireContinue);
			QSignalSpy spyAbort(&state, &StateChangeSmartPin::fireAbort);

			context->setProgress(0, QStringLiteral("Test"));
			QCOMPARE(context->getProgressValue(), 0);
			QCOMPARE(context->getProgressMessage(), QStringLiteral("Test"));

			command->setReturnCode(CardReturnCode::OK);
			state.onSetEidPinDone(command);
			QCOMPARE(context->getProgressValue(), 100);
			QCOMPARE(context->getProgressMessage(), QStringLiteral("Test"));
			QCOMPARE(spyContinue.count(), 1);

			command->setReturnCode(CardReturnCode::CANCELLATION_BY_USER);
			state.onSetEidPinDone(command);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Card_Cancellation_By_User);
			const FailureCode failureCode({FailureCode::Reason::Change_Smart_Pin_Failed,
										   {FailureCode::Info::Card_Return_Code, Enum<CardReturnCode>::getName(CardReturnCode::CANCELLATION_BY_USER)}
					});
			QCOMPARE(context->getFailureCode().value(), failureCode);
			QVERIFY(context->getFailureCode()->getFailureInfoMap() == failureCode.getFailureInfoMap());
			QCOMPARE(spyAbort.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateChangeSmartPin)
#include "test_StateChangeSmartPin.moc"
