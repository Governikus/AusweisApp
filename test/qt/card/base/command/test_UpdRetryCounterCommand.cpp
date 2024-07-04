/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "command/UpdateRetryCounterCommand.h"

#include "MockCardConnectionWorker.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_UpdateRetryCounterCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_InternalExecuteNoCard()
		{
			MockReader reader("name"_L1);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			UpdateRetryCounterCommand command(worker);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::CARD_NOT_FOUND);
		}


		void test_InternalExecuteWithCard()
		{
			MockReader reader("reader"_L1);
			CardInfo info(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 1, true, true);
			reader.setInfoCardInfo(info);

			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			UpdateRetryCounterCommand command(worker);
			command.internalExecute();
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
		}


};

QTEST_GUILESS_MAIN(test_UpdateRetryCounterCommand)
#include "test_UpdRetryCounterCommand.moc"
