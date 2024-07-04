/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref StateChangePin
 */

#include "states/StateChangePin.h"

#include "MockCardConnection.h"
#include "MockCardConnectionWorker.h"

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


		void setMockReturnCode(CardReturnCode pReturnCode)
		{
			setReturnCode(pReturnCode);
		}


};


class MockSetEidPinCommand
	: public SetEidPinCommand
{
	Q_OBJECT

	public:
		explicit MockSetEidPinCommand(const QSharedPointer<MockCardConnectionWorker>& pWorker)
			: SetEidPinCommand(pWorker, QByteArray("123456"), 1)
		{
		}


		~MockSetEidPinCommand() override = default;

		void internalExecute() override
		{
		}


		void setData(const QByteArray& pData)
		{
			mResponseApdu = ResponseApdu(pData);
		}


		void setMockReturnCode(CardReturnCode pReturnCode)
		{
			setReturnCode(pReturnCode);
		}


};

Q_DECLARE_METATYPE(std::optional<FailureCode>)

class test_StateChangePin
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Run()
		{
			QThread workerThread;
			workerThread.start();

			{
				const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
				worker->moveToThread(&workerThread);
				const QSharedPointer<CardConnection> connection(new CardConnection(worker));
				const QSharedPointer<ChangePinContext> context(new ChangePinContext());
				context->setCardConnection(connection);
				StateChangePin state(context);

				QTest::ignoreMessage(QtDebugMsg, "Invoke set Eid PIN command");
				state.run();
				QCOMPARE(state.mConnections.size(), 1);
			}

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnSetEidPinDone_data()
		{
			QTest::addColumn<CardReturnCode>("returnCode");
			QTest::addColumn<QByteArray>("response");
			QTest::addColumn<GlobalStatus::Code>("globalStatus");
			QTest::addColumn<int>("fireContinue");
			QTest::addColumn<int>("fireAbort");
			QTest::addColumn<int>("fireRetry");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");

			QTest::newRow("OK") << CardReturnCode::OK << QByteArray() << GlobalStatus::Code::Card_Unexpected_Transmit_Status << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_Unexpected_Transmit_Status);
			QTest::newRow("OK_9000") << CardReturnCode::OK << QByteArray("9000") << GlobalStatus::Code::No_Error << 1 << 0 << 0 << std::optional<FailureCode>();
			QTest::newRow("OK_6400") << CardReturnCode::OK << QByteArray("6400") << GlobalStatus::Code::Card_Input_TimeOut << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_Input_Timeout);
			QTest::newRow("OK_6401") << CardReturnCode::OK << QByteArray("6401") << GlobalStatus::Code::Card_Cancellation_By_User << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_User_Cancelled);
			QTest::newRow("OK_6402") << CardReturnCode::OK << QByteArray("6402") << GlobalStatus::Code::Card_NewPin_Mismatch << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_New_Pin_Mismatch);
			QTest::newRow("OK_6403") << CardReturnCode::OK << QByteArray("6403") << GlobalStatus::Code::Card_NewPin_Invalid_Length << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_New_Pin_Invalid_Length);
			//Should retry
			QTest::newRow("CARD_NOT_FOUND") << CardReturnCode::CARD_NOT_FOUND << QByteArray() << GlobalStatus::Code::No_Error << 0 << 0 << 1 << std::optional<FailureCode>();
			QTest::newRow("RESPONSE_EMPTY") << CardReturnCode::RESPONSE_EMPTY << QByteArray() << GlobalStatus::Code::No_Error << 0 << 0 << 1 << std::optional<FailureCode>();
			//Should abort
			QTest::newRow("INPUT_TIME_OUT") << CardReturnCode::INPUT_TIME_OUT << QByteArray() << GlobalStatus::Code::Card_Input_TimeOut << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_Unrecoverable);
			QTest::newRow("CANCELLATION_BY_USER") << CardReturnCode::CANCELLATION_BY_USER << QByteArray() << GlobalStatus::Code::Card_Cancellation_By_User << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_Card_User_Cancelled);
			QTest::newRow("NEW_PIN_MISMATCH") << CardReturnCode::NEW_PIN_MISMATCH << QByteArray() << GlobalStatus::Code::Card_NewPin_Mismatch << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_Card_New_Pin_Mismatch);
			QTest::newRow("PROTOCOL_ERROR") << CardReturnCode::PROTOCOL_ERROR << QByteArray() << GlobalStatus::Code::Card_Protocol_Error << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_Unrecoverable);
			QTest::newRow("NEW_PIN_INVALID_LENGTH") << CardReturnCode::NEW_PIN_INVALID_LENGTH << QByteArray() << GlobalStatus::Code::Card_NewPin_Invalid_Length << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_Unrecoverable);
			QTest::newRow("COMMAND_FAILED") << CardReturnCode::COMMAND_FAILED << QByteArray() << GlobalStatus::Code::Card_Communication_Error << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_Unrecoverable);
			QTest::newRow("PIN_BLOCKED") << CardReturnCode::PIN_BLOCKED << QByteArray() << GlobalStatus::Code::Card_Pin_Blocked << 0 << 1 << 0 << std::optional<FailureCode>(FailureCode::Reason::Change_Pin_Unrecoverable);
		}


		void test_OnSetEidPinDone()
		{
			QFETCH(CardReturnCode, returnCode);
			QFETCH(QByteArray, response);
			QFETCH(GlobalStatus::Code, globalStatus);
			QFETCH(int, fireContinue);
			QFETCH(int, fireAbort);
			QFETCH(int, fireRetry);
			QFETCH(std::optional<FailureCode>, failureCode);

			const QSharedPointer<ChangePinContext> context(new ChangePinContext());
			StateChangePin state(context);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QSharedPointer<MockSetEidPinCommand> command(new MockSetEidPinCommand(worker));
			const ReaderInfo readerInfo("NFC"_L1, ReaderManagerPluginType::NFC);
			const QSharedPointer<CardConnection> connection(new MockCardConnection(readerInfo));
			context->setCardConnection(connection);

			QSignalSpy spyContinue(&state, &StateChangePin::fireContinue);
			QSignalSpy spyAbort(&state, &StateChangePin::fireAbort);
			QSignalSpy spyRetry(&state, &StateChangePin::fireNoCardConnection);

			command->setMockReturnCode(returnCode);
			command->setData(QByteArray::fromHex(response));
			state.onSetEidPinDone(command);
			QCOMPARE(context->getStatus().getStatusCode(), globalStatus);

			if (fireContinue > 0)
			{
				QCOMPARE(context->getSuccessMessage(), tr("You have successfully changed your ID card PIN."));
			}
			QCOMPARE(spyContinue.count(), fireContinue);
			QCOMPARE(spyAbort.count(), fireAbort);
			QCOMPARE(spyRetry.count(), fireRetry);
			QCOMPARE(context->getFailureCode(), failureCode);
		}


		void test_OnSetSmartEidPinDone_data()
		{
			QTest::addColumn<ReaderManagerPluginType>("type");

			const auto& readerTypes = Enum<ReaderManagerPluginType>::getList();
			for (const auto& type : readerTypes)
			{
				QTest::newRow(getEnumName(type).data()) << type;
			}
		}


		void test_OnSetSmartEidPinDone()
		{
			QFETCH(ReaderManagerPluginType, type);

			const QSharedPointer<ChangePinContext> context(new ChangePinContext());
			StateChangePin state(context);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QSharedPointer<MockSetEidPinCommand> command(new MockSetEidPinCommand(worker));
			const ReaderInfo readerInfo("SMART"_L1, type, CardInfo(CardType::SMART_EID));
			const QSharedPointer<CardConnection> connection(new MockCardConnection(readerInfo));
			context->setCardConnection(connection);

			QSignalSpy spyContinue(&state, &StateChangePin::fireContinue);

			command->setMockReturnCode(CardReturnCode::OK);
			command->setData(QByteArray::fromHex("9000"));
			state.onSetEidPinDone(command);
			QCOMPARE(context->getSuccessMessage(), tr("You have successfully changed your Smart-eID PIN."));
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateChangePin)
#include "test_StateChangePin.moc"
