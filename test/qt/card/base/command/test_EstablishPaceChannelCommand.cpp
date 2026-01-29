/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "command/EstablishPaceChannelCommand.h"

#include "MockCardConnectionWorker.h"

#include <QPointer>
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
			const auto& worker = MockCardConnectionWorker::create();
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
			QPointer<MockReader> reader = new MockReader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 0, true, true);
			reader->setInfoCardInfo(info);
			const auto& worker = MockCardConnectionWorker::create(reader);

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
			const auto& worker = MockCardConnectionWorker::create();
			const QByteArray puk("00000000000");

			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PUK, puk, nullptr, nullptr);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::CARD_NOT_FOUND);
		}


		void test_InternalExecuteNoCard()
		{
			QPointer<MockReader> reader = new MockReader(QStringLiteral("reader"));
			const auto& worker = MockCardConnectionWorker::create(reader);
			const QByteArray puk("00000000000");

			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PUK, puk, nullptr, nullptr);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::CARD_NOT_FOUND);
		}


		void test_InternalExecutePinNotBlocked()
		{
			QPointer<MockReader> reader = new MockReader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 0, true, true);
			reader->setInfoCardInfo(info);

			const auto& worker = MockCardConnectionWorker::create(reader);
			const QByteArray puk("00000000000");
			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PUK, puk, nullptr, nullptr);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PIN_NOT_BLOCKED);
		}


		void test_InternalExecuteProtocolError()
		{
			QPointer<MockReader> reader = new MockReader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 0, false, true);
			reader->setInfoCardInfo(info);
			const auto& worker = MockCardConnectionWorker::create(reader);

			worker->addPaceCode(CardReturnCode::PROTOCOL_ERROR);
			const QByteArray puk("12131415");
			EstablishPaceChannelCommand command(worker, PacePasswordId::PACE_PUK, puk, nullptr, nullptr);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void test_InternalExecuteOK()
		{
			QPointer<MockReader> reader = new MockReader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 0, false, true);
			reader->setInfoCardInfo(info);
			const auto& worker = MockCardConnectionWorker::create(reader);

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
