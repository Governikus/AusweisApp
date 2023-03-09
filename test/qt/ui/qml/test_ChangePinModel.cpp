/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ChangePinModel
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
			const auto& model = Env::getSingleton<ChangePinModel>();
			QSharedPointer<ChangePinContext> context(new ChangePinContext());

			QSignalSpy resultChanged(model, &ChangePinModel::fireResultChanged);
			QSignalSpy newContextSet(model, &ChangePinModel::fireNewContextSet);

			model->resetChangePinContext();
			QCOMPARE(resultChanged.count(), 1);
			QCOMPARE(newContextSet.count(), 0);

			Q_EMIT context->fireSuccessMessageChanged();
			QCOMPARE(resultChanged.count(), 1);

			model->resetChangePinContext(context);
			QCOMPARE(resultChanged.count(), 2);
			QCOMPARE(newContextSet.count(), 1);

			Q_EMIT context->fireSuccessMessageChanged();
			QCOMPARE(resultChanged.count(), 3);
		}


		void test_GetResultString()
		{
			const auto& model = Env::getSingleton<ChangePinModel>();
			QSharedPointer<ChangePinContext> context(new ChangePinContext());

			QCOMPARE(model->getResultString(), QString());

			context->setStatus(GlobalStatus::Code::No_Error);
			context->setSuccessMessage(QStringLiteral("success"));
			model->resetChangePinContext(context);
			QCOMPARE(model->getResultString(), QStringLiteral("success"));
		}


		void test_supportedPlugInTypes()
		{
			const auto& model = Env::getSingleton<ChangePinModel>();
			QSharedPointer<ChangePinContext> context(new ChangePinContext());
			model->resetChangePinContext(context);
#if __has_include("context/PersonalizationContext.h")
			QVERIFY(model->getSupportedReaderPlugInTypes().contains(ReaderManagerPlugInType::SMART));
#else
			QVERIFY(!model->getSupportedReaderPlugInTypes().contains(ReaderManagerPlugInType::SMART));
#endif

			QSharedPointer<ChangePinContext> contextTransportPin(new ChangePinContext(true));
			model->resetChangePinContext(contextTransportPin);
			QVERIFY(!model->getSupportedReaderPlugInTypes().contains(ReaderManagerPlugInType::SMART));
		}


};

QTEST_MAIN(test_ChangePinModel)
#include "test_ChangePinModel.moc"
