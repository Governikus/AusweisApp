/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "command/TransmitCommand.h"

#include "LanguageLoader.h"
#include "LogHandler.h"
#include "MockCardConnectionWorker.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_TransmitCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			QTemporaryDir translationDir;
			Env::getSingleton<LogHandler>()->init();
			TestFileHelper::createTranslations(translationDir.path());
			LanguageLoader::getInstance().setPath(translationDir.path());
		}


		void isAcceptable_data()
		{
			QTest::addColumn<QByteArrayList>("acceptable");
			QTest::addColumn<QByteArray>("response");
			QTest::addColumn<bool>("accepted");

			QTest::newRow("all-empty") << QByteArrayList({""}) << QByteArray("") << true;
			QTest::newRow("all-incomplete") << QByteArrayList({""}) << QByteArray("90") << true;
			QTest::newRow("all-code-only") << QByteArrayList({""}) << QByteArray("9000") << true;
			QTest::newRow("all-data+code") << QByteArrayList({""}) << QByteArray("abcd9000") << true;
			QTest::newRow("starting-with-90-empty") << QByteArrayList({"90"}) << QByteArray("") << false;
			QTest::newRow("starting-with-90-incomplete") << QByteArrayList({"90"}) << QByteArray("90") << true;
			QTest::newRow("starting-with-90-code-only") << QByteArrayList({"90"}) << QByteArray("9000") << true;
			QTest::newRow("starting-with-90-data+code") << QByteArrayList({"90"}) << QByteArray("abcd9000") << true;
			QTest::newRow("equal-to-9000-empty") << QByteArrayList({"9000"}) << QByteArray("") << false;
			QTest::newRow("equal-to-9000-incomplete") << QByteArrayList({"9000"}) << QByteArray("90") << false;
			QTest::newRow("equal-to-9000-code-only") << QByteArrayList({"9000"}) << QByteArray("9000") << true;
			QTest::newRow("equal-to-9000-data+code") << QByteArrayList({"9000"}) << QByteArray("abcd9000") << true;
			QTest::newRow("multi-empty") << QByteArrayList({"8000", "9000"}) << QByteArray("") << false;
			QTest::newRow("multi-code-only-wrong") << QByteArrayList({"8000", "9000"}) << QByteArray("7000") << false;
			QTest::newRow("multi-code-only-true-1") << QByteArrayList({"8000", "9000"}) << QByteArray("8000") << true;
			QTest::newRow("multi-code-only-true-2") << QByteArrayList({"8000", "9000"}) << QByteArray("9000") << true;
			QTest::newRow("multi-data+code-wrong") << QByteArrayList({"8000", "9000"}) << QByteArray("abcd7000") << false;
			QTest::newRow("multi-data+code-true-1") << QByteArrayList({"8000", "9000"}) << QByteArray("abcd8000") << true;
			QTest::newRow("multi-data+code-true-2") << QByteArrayList({"8000", "9000"}) << QByteArray("abcd9000") << true;
		}


		void isAcceptable()
		{
			QFETCH(QByteArrayList, acceptable);
			QFETCH(QByteArray, response);
			QFETCH(bool, accepted);

			InputAPDUInfo info;
			for (const auto& code : acceptable)
			{
				info.addAcceptableStatusCode(code);
			}
			ResponseApdu apdu(QByteArray::fromHex(response));
			QCOMPARE(TransmitCommand::isAcceptable(info, apdu), accepted);
		}


		void test_InternalExecuteOkSingleCommandWithoutAcceptableStatusCode()
		{
			QVector<InputAPDUInfo> inputApduInfos(1);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			TransmitCommand command(worker, inputApduInfos, QStringLiteral("slotname"));
			command.internalExecute();
			QCOMPARE(command.getOutputApduAsHex().size(), 1);
			QCOMPARE(command.getOutputApduAsHex()[0], QByteArray("9000"));
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
		}


		void test_InternalExecuteOkSingleCommandWithAcceptableStatusCode()
		{
			QVector<InputAPDUInfo> inputApduInfos(1);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			inputApduInfos[0].addAcceptableStatusCode(QByteArray("90"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			TransmitCommand command(worker, inputApduInfos, QStringLiteral("slotname"));
			command.internalExecute();
			QCOMPARE(command.getOutputApduAsHex().size(), 1);
			QCOMPARE(command.getOutputApduAsHex()[0], QByteArray("9000"));
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
		}


		void test_InternalExecuteUnsuccesfulSingleCommand()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QVector<InputAPDUInfo> inputApduInfos(1);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			worker->addResponse(CardReturnCode::PROTOCOL_ERROR, QByteArray::fromHex("1919"));
			TransmitCommand command1(worker, inputApduInfos, QStringLiteral("slotname"));
			command1.internalExecute();
			QVERIFY(command1.getOutputApduAsHex().isEmpty());
			QCOMPARE(command1.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Transmit unsuccessful. Return code:"));

			worker->addResponse(CardReturnCode::PIN_BLOCKED, QByteArray::fromHex("63c0"));
			TransmitCommand command2(worker, inputApduInfos, QStringLiteral("slotname"));
			command2.internalExecute();
			QVERIFY(command2.getOutputApduAsHex().isEmpty());
			QCOMPARE(command2.getReturnCode(), CardReturnCode::PIN_BLOCKED);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Transmit unsuccessful. Return code:"));
		}


		void test_InternalExecuteUnexpectedStatusSingleCommand()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QVector<InputAPDUInfo> inputApduInfos(1);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			inputApduInfos[0].addAcceptableStatusCode(QByteArray("1010"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			TransmitCommand command(worker, inputApduInfos, QStringLiteral("slotname"));
			command.internalExecute();
			QCOMPARE(command.getOutputApduAsHex().size(), 1);
			QCOMPARE(command.getOutputApduAsHex()[0], QByteArray("9000"));
			QCOMPARE(command.getReturnCode(), CardReturnCode::UNEXPECTED_TRANSMIT_STATUS);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Transmit unsuccessful. StatusCode does not start with acceptable status code"));
		}


		void test_InternalExecuteOkMultipleCommand()
		{
			QVector<InputAPDUInfo> inputApduInfos(2);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			TransmitCommand command(worker, inputApduInfos, QStringLiteral("slotname"));
			command.internalExecute();
			QCOMPARE(command.getOutputApduAsHex().size(), 2);
			QCOMPARE(command.getOutputApduAsHex()[0], QByteArray("9000"));
			QCOMPARE(command.getOutputApduAsHex()[1], QByteArray("9000"));
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
		}


		void test_InternalExecuteUnsuccessfulMultipleCommand()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QVector<InputAPDUInfo> inputApduInfos(2);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			worker->addResponse(CardReturnCode::PROTOCOL_ERROR, QByteArray::fromHex("1919"));
			TransmitCommand command1(worker, inputApduInfos, QStringLiteral("slotname"));
			command1.internalExecute();
			QCOMPARE(command1.getOutputApduAsHex().size(), 1);
			QCOMPARE(command1.getOutputApduAsHex()[0], QByteArray("9000"));
			QCOMPARE(command1.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Transmit unsuccessful. Return code"));

			worker->addResponse(CardReturnCode::PROTOCOL_ERROR, QByteArray::fromHex("1919"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			TransmitCommand command2(worker, inputApduInfos, QStringLiteral("slotname"));
			command2.internalExecute();
			QCOMPARE(command2.getOutputApduAsHex().size(), 0);
			QCOMPARE(command2.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Transmit unsuccessful. Return code"));
		}


		void test_InternalExecuteUnexpectedStatusMultipleCommand()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QVector<InputAPDUInfo> inputApduInfos1(2);
			QVector<InputAPDUInfo> inputApduInfos2(2);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			inputApduInfos1[0].addAcceptableStatusCode(QByteArray("90"));
			inputApduInfos1[1].addAcceptableStatusCode(QByteArray("1010"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			TransmitCommand command1(worker, inputApduInfos1, QStringLiteral("slotname"));
			command1.internalExecute();
			QCOMPARE(command1.getOutputApduAsHex().size(), 2);
			QCOMPARE(command1.getOutputApduAsHex()[0], QByteArray("9000"));
			QCOMPARE(command1.getOutputApduAsHex()[1], QByteArray("9000"));
			QCOMPARE(command1.getReturnCode(), CardReturnCode::UNEXPECTED_TRANSMIT_STATUS);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Transmit unsuccessful. StatusCode does not start with acceptable status code"));

			inputApduInfos2[0].addAcceptableStatusCode(QByteArray("1010"));
			inputApduInfos2[1].addAcceptableStatusCode(QByteArray("9000"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			TransmitCommand command2(worker, inputApduInfos2, QStringLiteral("slotname"));
			command2.internalExecute();
			QCOMPARE(command2.getOutputApduAsHex().size(), 1);
			QCOMPARE(command2.getOutputApduAsHex()[0], QByteArray("9000"));
			QCOMPARE(command2.getReturnCode(), CardReturnCode::UNEXPECTED_TRANSMIT_STATUS);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Transmit unsuccessful. StatusCode does not start with acceptable status code"));
		}


		void test_SlotHandle()
		{
			QVector<InputAPDUInfo> inputApduInfos(1);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QString name = QStringLiteral("slotname");
			TransmitCommand command(worker, inputApduInfos, name);
			QCOMPARE(command.getSlotHandle(), name);
		}


};

QTEST_GUILESS_MAIN(test_TransmitCommand)
#include "test_TransmitCommand.moc"
