/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Tests the StateSelectReader
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
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
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


		void test_fireReaderPlugInTypesChanged()
		{
			QSignalSpy spyRetry(mState.data(), &StateSelectReader::fireRetry);
			Q_EMIT mContext->fireReaderPlugInTypesChanged();
			QCOMPARE(spyRetry.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateSelectReader)
#include "test_StateSelectReader.moc"
