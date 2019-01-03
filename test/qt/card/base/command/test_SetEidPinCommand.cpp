/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "command/SetEidPinCommand.h"

#include "LogHandler.h"
#include "MockCardConnectionWorker.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_SetEidPinCommand
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


		void test_InternalExecuteAndResponseApdu()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex(("9000")));
			worker->addResponse(CardReturnCode::PROTOCOL_ERROR, QByteArray::fromHex("1919"));
			worker->addResponse(CardReturnCode::PIN_BLOCKED, QByteArray::fromHex("63c0"));
			const QString pin = QStringLiteral("111111");
			SetEidPinCommand command(worker, pin, 8);

			ResponseApdu apdu1(QByteArray("apdu"));
			command.mResponseApdu = apdu1;
			QCOMPARE(command.getResponseApdu().getBuffer(), QByteArray("apdu"));

			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
			QCOMPARE(command.getResponseApdu().getBuffer(), QByteArray::fromHex("9000"));

			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(command.getResponseApdu().getBuffer(), QByteArray::fromHex("1919"));

			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PIN_BLOCKED);
			QCOMPARE(command.getResponseApdu().getBuffer(), QByteArray::fromHex("63c0"));
		}


};

QTEST_GUILESS_MAIN(test_SetEidPinCommand)
#include "test_SetEidPinCommand.moc"
