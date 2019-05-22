/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "command/UnblockPinCommand.h"

#include "LogHandler.h"
#include "MockCardConnectionWorker.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_UnblockPinCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_InternalExecuteNoCard()
		{
			MockReader reader(QStringLiteral("reader"));
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			const QString puk = QStringLiteral("00000000000");

			UnblockPinCommand command(worker, puk);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::CARD_NOT_FOUND);
		}


		void test_InternalExecuteNoReader()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QString puk = QStringLiteral("00000000000");

			UnblockPinCommand command(worker, puk);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::CARD_NOT_FOUND);
		}


		void test_InternalExecutePinNotBlocked()
		{
			MockReader reader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 0, true, true);
			reader.getReaderInfo().setCardInfo(info);

			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			const QString puk = QStringLiteral("00000000000");
			UnblockPinCommand command(worker, puk);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PIN_NOT_BLOCKED);
		}


		void test_InternalExecuteProtocolError()
		{
			MockReader reader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 0, false, true);
			reader.getReaderInfo().setCardInfo(info);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));

			worker->addPaceCode(CardReturnCode::PROTOCOL_ERROR);
			const QString puk = QStringLiteral("12131415");
			UnblockPinCommand command(worker, puk);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void test_InternalExecuteOK()
		{
			MockReader reader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 0, false, true);
			reader.getReaderInfo().setCardInfo(info);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			worker->addPaceCode(CardReturnCode::OK);
			const QString puk = QStringLiteral("00000000000");
			UnblockPinCommand command(worker, puk);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
		}


		void test_InternalExecutePukInoperative()
		{
			MockReader reader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 0, false, true);
			reader.getReaderInfo().setCardInfo(info);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("6900"));
			worker->addPaceCode(CardReturnCode::OK);
			const QString puk = QStringLiteral("00000000000");
			UnblockPinCommand command(worker, puk);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PUK_INOPERATIVE);
			QVERIFY(worker->getReaderInfo().getCardInfo().isPukInoperative());
		}


};

QTEST_GUILESS_MAIN(test_UnblockPinCommand)
#include "test_UnblockPinCommand.moc"
