/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
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
			QTest::addColumn<SW1>("sw1");
			QTest::addColumn<CardReturnCode>("returnCode");

			QTest::addRow("PUK inoperative") << SW1::ERROR_COMMAND_NOT_ALLOWED << CardReturnCode::PUK_INOPERATIVE;
			QTest::addRow("Reset success") << SW1::SUCCESS << CardReturnCode::OK;
		}


		void test_InternalExecute()
		{
			QFETCH(SW1, sw1);
			QFETCH(CardReturnCode, returnCode);

			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			QByteArray buffer;
			buffer += static_cast<char>(sw1);
			buffer += static_cast<char>(0x00); // SW2 - Does not matter for this test
			worker->addResponse(CardReturnCode::OK, buffer);

			ResetRetryCounterCommand command(worker);
			command.internalExecute();

			QCOMPARE(command.getReturnCode(), returnCode);
		}


};

QTEST_GUILESS_MAIN(test_ResetRetryCounterCommand)
#include "test_ResetRetryCounterCommand.moc"
