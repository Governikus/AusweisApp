/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "command/EstablishPaceChannelCommand.h"

#include "MockCardConnectionWorker.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_EstablishPaceChannelCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_GetPaceOutput()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QByteArray passwort = QByteArrayLiteral("passwort");
			QByteArray chat("chat");
			QByteArray description("description");
			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PIN,
					passwort, chat, description);

			EstablishPaceChannelOutput output;
			output.setPaceReturnCode(CardReturnCode::OK);
			output.setCarCurr(QByteArray("carCurr"));
			output.setCarPrev(QByteArray("carPrev"));
			output.setEfCardAccess(QByteArray("cardAccess"));
			output.setIdIcc(QByteArray("IdIcc"));
			output.setStatusMseSetAt(QByteArray("status"));
			command.mPaceOutput = output;

			QCOMPARE(command.getPaceOutput().getPaceReturnCode(), CardReturnCode::OK);
			QCOMPARE(command.getPaceOutput().getCarCurr(), QByteArray("carCurr"));
			QCOMPARE(command.getPaceOutput().getCarPrev(), QByteArray("carPrev"));
			QCOMPARE(command.getPaceOutput().getEfCardAccess(), QByteArray("cardAccess"));
			QCOMPARE(command.getPaceOutput().getIdIcc(), QByteArray("IdIcc"));
			QCOMPARE(command.getPaceOutput().getStatusMseSetAt(), QByteArray("status"));
		}


		void test_InternalExecute()
		{
			MockReader reader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 0, true, true);
			reader.setInfoCardInfo(info);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));

			worker->addPaceCode(CardReturnCode::OK);
			worker->addPaceCode(CardReturnCode::OK);
			worker->addPaceCode(CardReturnCode::UNKNOWN);
			worker->addPaceCode(CardReturnCode::PROTOCOL_ERROR);

			const QByteArray passwort = QByteArrayLiteral("passwort");
			QByteArray chat("chat");
			QByteArray description("description");
			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PIN,
					passwort, chat, description);

			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);

			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);

			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::UNKNOWN);

			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void test_InternalExecuteNoReader()
		{
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QByteArray puk("00000000000");

			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PUK, puk, nullptr, nullptr);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::CARD_NOT_FOUND);
		}


		void test_InternalExecuteNoCard()
		{
			MockReader reader(QStringLiteral("reader"));
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			const QByteArray puk("00000000000");

			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PUK, puk, nullptr, nullptr);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::CARD_NOT_FOUND);
		}


		void test_InternalExecutePinNotBlocked()
		{
			MockReader reader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 0, true, true);
			reader.setInfoCardInfo(info);

			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			const QByteArray puk("00000000000");
			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PUK, puk, nullptr, nullptr);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PIN_NOT_BLOCKED);
		}


		void test_InternalExecuteProtocolError()
		{
			MockReader reader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 0, false, true);
			reader.setInfoCardInfo(info);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));

			worker->addPaceCode(CardReturnCode::PROTOCOL_ERROR);
			const QByteArray puk("12131415");
			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PUK, puk, nullptr, nullptr);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void test_InternalExecuteOK()
		{
			MockReader reader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 0, false, true);
			reader.setInfoCardInfo(info);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			worker->addPaceCode(CardReturnCode::OK);
			const QByteArray puk("00000000000");
			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PUK, puk, nullptr, nullptr);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
		}


};

QTEST_GUILESS_MAIN(test_EstablishPaceChannelCommand)
#include "test_EstablishPaceChannelCommand.moc"
