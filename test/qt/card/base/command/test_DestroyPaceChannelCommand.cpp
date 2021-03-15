/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "command/DestroyPaceChannelCommand.h"

#include "LogHandler.h"
#include "MockCardConnectionWorker.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_DestroyPaceChannelCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_InternalExecute()
		{
			QSharedPointer<MockCardConnectionWorker> worker1(new MockCardConnectionWorker());
			worker1->addPaceCode(CardReturnCode::OK);
			DestroyPaceChannelCommand command1(worker1);
			command1.internalExecute();
			QCOMPARE(command1.getReturnCode(), CardReturnCode::OK);

			QSharedPointer<MockCardConnectionWorker> worker2(new MockCardConnectionWorker());
			worker2->addPaceCode(CardReturnCode::UNKNOWN);
			DestroyPaceChannelCommand command2(worker2);
			command2.internalExecute();
			QCOMPARE(command2.getReturnCode(), CardReturnCode::UNKNOWN);
		}


};

QTEST_GUILESS_MAIN(test_DestroyPaceChannelCommand)
#include "test_DestroyPaceChannelCommand.moc"
