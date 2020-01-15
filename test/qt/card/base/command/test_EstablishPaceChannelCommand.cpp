/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "command/EstablishPaceChannelCommand.h"

#include "LogHandler.h"
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
			const QString passwort = QStringLiteral("passwort");
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
			QCOMPARE(command.getPaceOutput().getCARcurr(), QByteArray("carCurr"));
			QCOMPARE(command.getPaceOutput().getCARprev(), QByteArray("carPrev"));
			QCOMPARE(command.getPaceOutput().getEfCardAccess(), QByteArray("cardAccess"));
			QCOMPARE(command.getPaceOutput().getIDicc(), QByteArray("IdIcc"));
			QCOMPARE(command.getPaceOutput().getMseStatusSetAt(), QByteArray("status"));
		}


		void test_InternalExecute()
		{
			const QString passwort = QStringLiteral("passwort");
			QByteArray chat("chat");
			QByteArray description("description");
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			worker->addPaceCode(CardReturnCode::OK);
			worker->addPaceCode(CardReturnCode::OK);
			worker->addPaceCode(CardReturnCode::UNKNOWN);
			worker->addPaceCode(CardReturnCode::PROTOCOL_ERROR);
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


};

QTEST_GUILESS_MAIN(test_EstablishPaceChannelCommand)
#include "test_EstablishPaceChannelCommand.moc"
