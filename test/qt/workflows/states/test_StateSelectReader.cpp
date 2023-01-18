/*!
 * \brief Tests the StateSelectReader
 *
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateSelectReader.h"

#include "Env.h"
#include "ReaderManager.h"
#include "states/StateBuilder.h"

#include "TestWorkflowContext.h"

#include <QtTest>


using namespace governikus;


class test_StateSelectReader
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StateSelectReader> mState;
	QSharedPointer<WorkflowContext> mContext;

	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void init()
		{
			mContext.reset(new TestWorkflowContext());
			mState.reset(StateBuilder::createState<StateSelectReader>(mContext));
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mState.reset();
			mContext.clear();
		}


		void test_OnReaderInfoChangedNoSelectableReaders()
		{
			mContext->setReaderPlugInTypes({ReaderManagerPlugInType::PCSC, ReaderManagerPlugInType::UNKNOWN, ReaderManagerPlugInType::REMOTE_IFD});
			mContext->setStateApproved();
			const auto readerManager = Env::getSingleton<ReaderManager>();

			QTest::ignoreMessage(QtDebugMsg, "No selectable reader detected");
			Q_EMIT readerManager->fireReaderAdded(ReaderInfo());
			QCoreApplication::processEvents();
		}


		void test_OnReaderStatusChanged()
		{
			const auto pType = ReaderManagerPlugInType::PCSC;
			mContext->setReaderPlugInTypes({pType});
			mContext->setStateApproved();

			QVERIFY(!Env::getSingleton<ReaderManager>()->isScanRunning(pType));

			QSignalSpy spyRetry(mState.data(), &StateSelectReader::fireRetry);

			mState->onReaderStatusChanged(ReaderManagerPlugInInfo(pType));

			QTRY_COMPARE(spyRetry.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_fireReaderPlugInTypesChanged()
		{
			QSignalSpy spyRetry(mState.data(), &StateSelectReader::fireRetry);
			Q_EMIT mContext->fireReaderPlugInTypesChanged();
			QCOMPARE(spyRetry.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateSelectReader)
#include "test_StateSelectReader.moc"
