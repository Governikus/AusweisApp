/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "command/UpdateRetryCounterCommand.h"

#include "LogHandler.h"
#include "MockCardConnectionWorker.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_UpdateRetryCounterCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_InternalExecuteNoCard()
		{
			MockReader reader(QString("name"));
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			UpdateRetryCounterCommand command(worker);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::CARD_NOT_FOUND);
		}


		void test_InternalExecuteWithCard()
		{
			MockReader reader(QStringLiteral("reader"));
			CardInfo info(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 1, true, true);
			reader.getReaderInfo().setCardInfo(info);

			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			UpdateRetryCounterCommand command(worker);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
		}


};

QTEST_GUILESS_MAIN(test_UpdateRetryCounterCommand)
#include "test_UpdRetryCounterCommand.moc"
