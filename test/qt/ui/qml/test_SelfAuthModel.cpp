/*!
 * \brief Unit tests for \ref SelfAuthModel
 *
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthModel.h"

#include "context/SelfAuthContext.h"

#include "MockCardConnectionWorker.h"
#include "TestFileHelper.h"

#include <QtTest>


using namespace governikus;

class test_SelfAuthModel
	: public QObject
{
	Q_OBJECT

	private:
		SelfAuthModel* mModel = nullptr;
		QSharedPointer<SelfAuthContext> mContext;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new SelfAuthContext());
			mModel = Env::getSingleton<SelfAuthModel>();
		}


		void cleanup()
		{
			mContext.clear();
			mModel->resetContext();
		}


		void test_StartWorkflow()
		{
			QSignalSpy spy(mModel, &SelfAuthModel::fireStartWorkflow);

			mModel->startWorkflow();
			QCOMPARE(spy.count(), 1);
		}


		void test_CancelWorkflow()
		{
			QSignalSpy spy(mContext.data(), &SelfAuthContext::fireCancelWorkflow);

			mModel->cancelWorkflow();
			QCOMPARE(spy.count(), 0);

			mModel->resetContext(mContext);
			mModel->cancelWorkflow();
			QCOMPARE(spy.count(), 1);
		}


		void test_IsBasicReader()
		{

			QThread workerThread;
			workerThread.start();

			QVERIFY(mModel->isBasicReader());

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			mContext->setCardConnection(connection);
			ReaderInfo info;

			info.setBasicReader(true);
			Q_EMIT worker->fireReaderInfoChanged(info);
			mModel->resetContext(mContext);
			QVERIFY(mModel->isBasicReader());

			info.setBasicReader(false);
			Q_EMIT worker->fireReaderInfoChanged(info);
			QVERIFY(!mModel->isBasicReader());

			workerThread.quit();
			workerThread.wait();
		}


		void test_RoleNames()
		{
			QVERIFY(mModel->roleNames().keys().contains(SelfAuthModel::DataRoles::NAME));
			QVERIFY(mModel->roleNames().keys().contains(SelfAuthModel::DataRoles::VALUE));
			QVERIFY(mModel->roleNames().values().contains("name"));
			QVERIFY(mModel->roleNames().values().contains("value"));
		}


		void test_OnSelfAuthenticationDataChanged()
		{
			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationDataNoAddress.json");
			SelfAuthenticationData selfAuthenticationData(data);
			mContext->setSelfAuthenticationData(selfAuthenticationData);
			mModel->resetContext(mContext);
			for (int i = 0; i < selfAuthenticationData.getOrderedSelfData().length(); i++)
			{
				QModelIndex index = mModel->index(i, 0);
				QCOMPARE(mModel->data(index, SelfAuthModel::DataRoles::NAME), selfAuthenticationData.getOrderedSelfData().at(i).first);
				QCOMPARE(mModel->data(index, SelfAuthModel::DataRoles::VALUE), selfAuthenticationData.getOrderedSelfData().at(i).second);
				QCOMPARE(mModel->data(index, 100), QVariant());
			}
		}


};

QTEST_GUILESS_MAIN(test_SelfAuthModel)
#include "test_SelfAuthModel.moc"
