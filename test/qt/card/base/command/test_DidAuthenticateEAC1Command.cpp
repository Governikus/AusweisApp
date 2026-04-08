/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "command/DidAuthenticateEAC1Command.h"

#include "LogHandler.h"
#include "MockCardConnectionWorker.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
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
			qApp->processEvents();
		}


		void test_InternalExecuteOKEmptyChallenge()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			const auto& worker = MockCardConnectionWorker::create();

			ResponseApdu response(QByteArray::fromHex("9000"));
			worker->addResponse(CardReturnCode::OK, response);
			DidAuthenticateEAC1Command command(worker);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
			QCOMPARE(response.getStatusCode(), StatusCode::SUCCESS);
			QCOMPARE(command.getChallenge(), QByteArray());
			QTRY_COMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Challenge has wrong size. Expect 8 bytes, got"_L1));
		}


		void test_InternalExecuteOK()
		{
			const auto& worker = MockCardConnectionWorker::create();

			ResponseApdu response(QByteArray::fromHex("00000000000000009000"));
			worker->addResponse(CardReturnCode::OK, response);
			DidAuthenticateEAC1Command command(worker);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
			QCOMPARE(response.getStatusCode(), StatusCode::SUCCESS);
			QCOMPARE(command.getChallenge(), QByteArray::fromHex("0000000000000000"));
		}


		void test_InternalExecuteFailedWrongResponse()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			const auto& worker = MockCardConnectionWorker::create();

			ResponseApdu response(QByteArray::fromHex("6982"));
			worker->addResponse(CardReturnCode::PROTOCOL_ERROR, response);
			DidAuthenticateEAC1Command command(worker);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(response.getStatusCode(), StatusCode::ACCESS_DENIED);
			QCOMPARE(command.getChallenge(), QByteArray());
			QTRY_COMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("GetChallenge failed"_L1));
		}


		void test_InternalExecuteFailedProtocolError()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			const auto& worker = MockCardConnectionWorker::create();

			ResponseApdu response(QByteArray::fromHex("00000000000000000000"));
			worker->addResponse(CardReturnCode::OK, response);
			DidAuthenticateEAC1Command command(worker);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(response.getStatusCode(), StatusCode::UNKNOWN);
			QCOMPARE(command.getChallenge(), QByteArray());
			QTRY_COMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, "Unexpected StatusCode in GetChallenge"_L1));
		}


		void test_GetChallenge()
		{
			const auto& worker = MockCardConnectionWorker::create();
			DidAuthenticateEAC1Command command(worker);
			command.mChallenge = QByteArray("abc");
			QCOMPARE(command.getChallenge(), QByteArray("abc"));
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateEAC1Command)
#include "test_DidAuthenticateEAC1Command.moc"
