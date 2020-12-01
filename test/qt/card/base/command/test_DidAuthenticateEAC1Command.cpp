/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "command/DidAuthenticateEAC1Command.h"

#include "LogHandler.h"
#include "MockCardConnectionWorker.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_DidAuthenticateEAC1Command
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void test_InternalExecuteOKEmptyChallenge()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			ResponseApdu response(QByteArray::fromHex("9000"));
			worker->addResponse(CardReturnCode::OK, response.getBuffer());
			DidAuthenticateEAC1Command command(worker);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
			QCOMPARE(response.getReturnCode(), StatusCode::SUCCESS);
			QCOMPARE(command.getChallenge(), QByteArray());
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Challenge has wrong size. Expect 8 bytes, got"));
		}


		void test_InternalExecuteOK()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			ResponseApdu response(QByteArray::fromHex("00000000000000009000"));
			worker->addResponse(CardReturnCode::OK, response.getBuffer());
			DidAuthenticateEAC1Command command(worker);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
			QCOMPARE(response.getReturnCode(), StatusCode::SUCCESS);
			QCOMPARE(command.getChallenge(), QByteArray::fromHex("0000000000000000"));
		}


		void test_InternalExecuteFailed()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			ResponseApdu response1(QByteArray::fromHex("63c0"));
			worker->addResponse(CardReturnCode::PIN_BLOCKED, response1.getBuffer());
			DidAuthenticateEAC1Command command1(worker);
			command1.internalExecute();
			QCOMPARE(command1.getReturnCode(), CardReturnCode::PIN_BLOCKED);
			QCOMPARE(response1.getReturnCode(), StatusCode::PIN_BLOCKED);
			QCOMPARE(command1.getChallenge(), QByteArray());
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("GetChallenge failed"));

			ResponseApdu response2(QByteArray::fromHex("19191919191919"));
			worker->addResponse(CardReturnCode::PROTOCOL_ERROR, response2.getBuffer());
			DidAuthenticateEAC1Command command2(worker);
			command2.internalExecute();
			QCOMPARE(command2.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(response2.getReturnCode(), StatusCode::INVALID);
			QCOMPARE(command2.getChallenge(), QByteArray());
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("GetChallenge failed"));
		}


		void test_GetChallenge()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			DidAuthenticateEAC1Command command(worker);
			command.mChallenge = QByteArray("abc");
			QCOMPARE(command.getChallenge(), QByteArray("abc"));
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateEAC1Command)
#include "test_DidAuthenticateEAC1Command.moc"
