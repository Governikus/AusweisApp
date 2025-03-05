/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StatePrepareChangePin.h"

#include <QtTest>


using namespace governikus;


class test_StatePrepareChangePin
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_State_data()
		{
			QTest::addColumn<bool>("onlyCheckPin");
			QTest::addColumn<QString>("newPin");
			QTest::addColumn<bool>("shouldContinue");
			QTest::addColumn<bool>("shouldSkipChangePin");
			QTest::addColumn<bool>("shouldEnterNewPacePin");

			QTest::addRow("No PIN") << false << QString() << false << false << true;
			QTest::addRow("Has PIN") << false << QStringLiteral("123456") << true << false << false;
			QTest::addRow("No PIN and Skip PIN change") << true << QString() << false << true << false;
			QTest::addRow("Has Pin and skip PIN change") << true << QStringLiteral("123456") << false << true << false;
		}


		void test_State()
		{

			QFETCH(bool, onlyCheckPin);
			QFETCH(QString, newPin);
			QFETCH(bool, shouldContinue);
			QFETCH(bool, shouldSkipChangePin);
			QFETCH(bool, shouldEnterNewPacePin);

			QSharedPointer<ChangePinContext> context;
			context.reset(new ChangePinContext(false, true, onlyCheckPin));
			context->setNewPin(newPin);

			QSharedPointer<StatePrepareChangePin> state;
			state.reset(StateBuilder::createState<StatePrepareChangePin>(context));

			QSignalSpy spyContinue(state.data(), &StatePrepareChangePin::fireContinue);
			QSignalSpy spySkipChangePin(state.data(), &StatePrepareChangePin::fireSkipPinChange);
			QSignalSpy spyEnterNewPacePin(state.data(), &StatePrepareChangePin::fireEnterNewPacePin);

			state->run();
			QCOMPARE(spyContinue.count(), shouldContinue ? 1 : 0);
			QCOMPARE(spySkipChangePin.count(), shouldSkipChangePin ? 1 : 0);
			QCOMPARE(spyEnterNewPacePin.count(), shouldEnterNewPacePin ? 1 : 0);
		}


};

QTEST_GUILESS_MAIN(test_StatePrepareChangePin)
#include "test_StatePrepareChangePin.moc"
