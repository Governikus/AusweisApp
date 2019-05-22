/*!
 * \brief Unit tests for \ref SelfAuthModel
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthModel.h"

#include "context/SelfAuthContext.h"

#include "MockCardConnectionWorker.h"

#include <QtTest>


using namespace governikus;

class test_SelfAuthModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_ResetContext()
		{
			SelfAuthModel model;
			const QSharedPointer<SelfAuthContext> context(new SelfAuthContext());

			model.resetContext(nullptr);
			QVERIFY(!model.mContext);

			model.resetContext(context);
			QCOMPARE(model.mContext, context);
		}


		void test_StartWorkflow()
		{
			SelfAuthModel model;
			QSignalSpy spy(&model, &SelfAuthModel::fireStartWorkflow);

			model.startWorkflow();
			QCOMPARE(spy.count(), 1);
		}


		void test_CancelWorkflow()
		{
			SelfAuthModel model;
			const QSharedPointer<SelfAuthContext> context(new SelfAuthContext());
			QSignalSpy spy(context.data(), &SelfAuthContext::fireCancelWorkflow);

			model.cancelWorkflow();
			QCOMPARE(spy.count(), 0);

			model.mContext = context;
			model.cancelWorkflow();
			QCOMPARE(spy.count(), 1);
		}


		void test_IsBasicReader()
		{
			QThread workerThread;
			workerThread.start();

			SelfAuthModel model;

			QVERIFY(model.isBasicReader());

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<SelfAuthContext> context(new SelfAuthContext());
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			context->setCardConnection(connection);
			ReaderInfo info;

			info.setBasicReader(true);
			connection->mReaderInfo = info;
			model.mContext = context;
			QVERIFY(model.isBasicReader());

			info.setBasicReader(false);
			connection->mReaderInfo = info;
			QVERIFY(!model.isBasicReader());

			workerThread.quit();
			workerThread.wait();
		}


		void test_RoleNames()
		{
			SelfAuthModel model;

			QVERIFY(model.roleNames().keys().contains(SelfAuthModel::DataRoles::NAME));
			QVERIFY(model.roleNames().keys().contains(SelfAuthModel::DataRoles::VALUE));
			QVERIFY(model.roleNames().values().contains("name"));
			QVERIFY(model.roleNames().values().contains("value"));
		}


};

QTEST_GUILESS_MAIN(test_SelfAuthModel)
#include "test_SelfAuthModel.moc"
