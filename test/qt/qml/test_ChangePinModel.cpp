/*!
 * \brief Unit tests for \ref ChangePinModel
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinModel.h"

#include <QDebug>
#include <QtTest>

using namespace governikus;


class test_ChangePinModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_ResetContext()
		{
			ChangePinModel model;
			QSharedPointer<ChangePinContext> context(new ChangePinContext());

			QSignalSpy resultChanged(&model, &ChangePinModel::fireResultChanged);
			QSignalSpy newContextSet(&model, &ChangePinModel::fireNewContextSet);

			model.resetContext();
			QCOMPARE(resultChanged.count(), 1);
			QCOMPARE(newContextSet.count(), 0);

			Q_EMIT context->fireSuccessMessageChanged();
			QCOMPARE(resultChanged.count(), 1);

			model.resetContext(context);
			QCOMPARE(resultChanged.count(), 2);
			QCOMPARE(newContextSet.count(), 1);

			Q_EMIT context->fireSuccessMessageChanged();
			QCOMPARE(resultChanged.count(), 3);
		}


		void test_GetResultString()
		{
			ChangePinModel model;
			QSharedPointer<ChangePinContext> context(new ChangePinContext());

			QCOMPARE(model.getResultString(), QString());

			context->setStatus(GlobalStatus::Code::No_Error);
			context->setSuccessMessage(QStringLiteral("success"));
			model.mContext = context;
			QCOMPARE(model.getResultString(), QStringLiteral("success"));
		}


};

QTEST_GUILESS_MAIN(test_ChangePinModel)
#include "test_ChangePinModel.moc"
