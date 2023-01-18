/*!
 * \brief Unit tests for \ref PersonalizationModel
 *
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "PersonalizationModel.h"

#include "Env.h"
#include "context/PersonalizationContext.h"

#include <QtTest>


using namespace governikus;

class test_PersonalizationModel
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<PersonalizationContext> mContext;
		PersonalizationModel* mModel = nullptr;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new PersonalizationContext(QString()));
			mModel = Env::getSingleton<PersonalizationModel>();
		}


		void cleanup()
		{
			mContext.clear();
			mModel->resetPersonalizationContext();
		}


		void test_StartWorkflow()
		{
			QSignalSpy spy(mModel, &PersonalizationModel::fireStartWorkflow);

			mModel->startWorkflow();
			QCOMPARE(spy.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_PersonalizationModel)
#include "test_PersonalizationModel.moc"
