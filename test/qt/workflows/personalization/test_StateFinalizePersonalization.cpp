/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateFinalizePersonalization.h"

#include "ReaderManager.h"

#include "TestFileHelper.h"
#include "mock/eid_applet_interface_mock.h"

#include <QSharedPointer>
#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


class test_StateFinalizePersonalization
	: public QObject
{
	Q_OBJECT

	QSharedPointer<PersonalizationContext> mContext;

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
			mContext = QSharedPointer<PersonalizationContext>::create(QString());
		}


		void success()
		{
			StateFinalizePersonalization state(mContext);
			QSignalSpy spy(&state, &StateFinalizePersonalization::fireContinue);

			state.run();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void fail()
		{
			StateFinalizePersonalization state(mContext);
			QSignalSpy spy(&state, &StateFinalizePersonalization::fireAbort);

			mContext->setStatus(GlobalStatus::Code::Workflow_Smart_eID_Personalization_Failed);
			state.run();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


};

QTEST_GUILESS_MAIN(test_StateFinalizePersonalization)
#include "test_StateFinalizePersonalization.moc"
