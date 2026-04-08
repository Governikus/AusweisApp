/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

#include "command/ResetRetryCounterCommand.h"

#include "MockCardConnectionWorker.h"

#include <QByteArray>
#include <QSharedPointer>
#include <QtTest>


using namespace governikus;


class test_ResetRetryCounterCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_InternalExecute_data()
		{
			QTest::addColumn<CardReturnCode>("returnCode");
			QTest::addColumn<StatusCode>("statusCode");
			QTest::addColumn<bool>("inoperative");

			QTest::addRow("Reset success") << CardReturnCode::OK << StatusCode::SUCCESS << false;
			QTest::addRow("PUK inoperative") << CardReturnCode::OK << StatusCode::ACCESS_DENIED << true;
			QTest::addRow("Unexpected statusCode") << CardReturnCode::OK << StatusCode::COMMAND_NOT_ALLOWED << false;
			QTest::addRow("Response empty") << CardReturnCode::RESPONSE_EMPTY << StatusCode::UNKNOWN << false;
			QTest::addRow("Command failed") << CardReturnCode::COMMAND_FAILED << StatusCode::UNKNOWN << false;
		}


		void test_InternalExecute()
		{
			QFETCH(CardReturnCode, returnCode);
			QFETCH(StatusCode, statusCode);
			QFETCH(bool, inoperative);

			QPointer<MockReader> reader = new MockReader(QStringLiteral("reader"));
			const auto& worker = MockCardConnectionWorker::create(reader);
			worker->addResponse(returnCode, statusCode == StatusCode::UNKNOWN ? ResponseApdu() : ResponseApdu(statusCode));

			ResetRetryCounterCommand command(worker);
			command.internalExecute();

			QCOMPARE(command.getReturnCode(), returnCode);
			QCOMPARE(command.getStatusCode(), statusCode);
			QCOMPARE(reader->getReaderInfo().isPukInoperative(), inoperative);
		}


};

QTEST_GUILESS_MAIN(test_ResetRetryCounterCommand)
#include "test_ResetRetryCounterCommand.moc"
