/*!
 * \brief Tests the StateSelectReader
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateSelectReader.h"

#include <QtTest>

using namespace governikus;


class test_StateSelectReader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_RequiresCard()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			StateSelectReader state(context);

			QVERIFY(state.requiresCard(ReaderManagerPlugInType::PCSC));
			QVERIFY(state.requiresCard(ReaderManagerPlugInType::REMOTE));
			QVERIFY(!state.requiresCard(ReaderManagerPlugInType::UNKNOWN));
			QVERIFY(!state.requiresCard(ReaderManagerPlugInType::BLUETOOTH));
		}


		void test_OnReaderInfoChangedNoSelectableReaders()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			context->setReaderPlugInTypes({ReaderManagerPlugInType::PCSC, ReaderManagerPlugInType::UNKNOWN, ReaderManagerPlugInType::REMOTE});
			StateSelectReader state(context);

			QTest::ignoreMessage(QtDebugMsg, "No selectable reader detected");
			state.onReaderInfoChanged();
		}


		void test_OnReaderDeviceError()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			StateSelectReader state(context);
			QSignalSpy spyAbort(&state, &StateSelectReader::fireAbort);

			state.onReaderDeviceError(GlobalStatus::Code::No_Error);
			QCOMPARE(spyAbort.count(), 0);

			state.onReaderDeviceError(GlobalStatus::Code::Workflow_Reader_Device_Scan_Error);
			QCOMPARE(spyAbort.count(), 0);

			state.onReaderDeviceError(GlobalStatus::Code::Card_Communication_Error);
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Card_Communication_Error);
		}


		void test_OnEntry()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			StateSelectReader state(context);
			const QString stateName("name");
			state.setStateName(stateName);

			QSignalSpy spyRetry(&state, &StateSelectReader::fireRetry);

			state.onEntry(nullptr);

			Q_EMIT context->fireAbortCardSelection();
			QCOMPARE(spyRetry.count(), 1);

			Q_EMIT context->fireReaderPlugInTypesChanged();
			QCOMPARE(spyRetry.count(), 2);
		}


};

QTEST_GUILESS_MAIN(test_StateSelectReader)
#include "test_StateSelectReader.moc"
