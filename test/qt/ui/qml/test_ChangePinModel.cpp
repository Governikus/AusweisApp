/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
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
		void cleanup()
		{
			Env::getSingleton<ChangePinModel>()->resetChangePinContext();
		}


		void test_ResetContext()
		{
			const auto& model = Env::getSingleton<ChangePinModel>();
			QSharedPointer<ChangePinContext> context(new ChangePinContext());

			QSignalSpy resultChanged(model, &ChangePinModel::fireResultChanged);

			model->resetChangePinContext();
			QCOMPARE(resultChanged.count(), 1);

			Q_EMIT context->fireSuccessMessageChanged();
			QCOMPARE(resultChanged.count(), 1);

			model->resetChangePinContext(context);
			QCOMPARE(resultChanged.count(), 2);

			Q_EMIT context->fireSuccessMessageChanged();
			QCOMPARE(resultChanged.count(), 3);
		}


		void test_GetResultString()
		{
			const auto& model = Env::getSingleton<ChangePinModel>();
			QCOMPARE(model->getResultString(), QString());

			QSharedPointer<ChangePinContext> context(new ChangePinContext());
			context->setStatus(GlobalStatus::Code::No_Error);
			context->setSuccessMessage(QStringLiteral("success"));
			model->resetChangePinContext(context);
			QCOMPARE(model->getResultString(), QStringLiteral("success"));

			context->setStatus(GlobalStatus::Code::Card_Invalid_Pin);
			QCOMPARE(model->getResultString(), QStringLiteral("The given PIN is not correct."));
		}


		void test_supportedPluginTypes()
		{
			const auto& model = Env::getSingleton<ChangePinModel>();
			QSharedPointer<ChangePinContext> context(new ChangePinContext());
			model->resetChangePinContext(context);
#if __has_include("context/PersonalizationContext.h")
			QVERIFY(model->getSupportedReaderPluginTypes().contains(ReaderManagerPluginType::SMART));
#else
			QVERIFY(!model->getSupportedReaderPluginTypes().contains(ReaderManagerPluginType::SMART));
#endif

			QSharedPointer<ChangePinContext> contextTransportPin(new ChangePinContext(true));
			model->resetChangePinContext(contextTransportPin);
			QVERIFY(!model->getSupportedReaderPluginTypes().contains(ReaderManagerPluginType::SMART));
		}


		void test_getStatusCodeAnimation()
		{
			const auto& model = Env::getSingleton<ChangePinModel>();
			QCOMPARE(model->getStatusCodeAnimation(), GAnimation::CHANGEPIN_SUCCESS);

			QSharedPointer<ChangePinContext> context(new ChangePinContext());
			context->setStatus(GlobalStatus::Code::Card_Invalid_Pin);
			model->resetChangePinContext(context);
			QCOMPARE(model->getStatusCodeAnimation(), GAnimation::PIN_ERROR);
		}


		void test_isRequestTransportPin()
		{
			const auto& model = Env::getSingleton<ChangePinModel>();
			QCOMPARE(model->isRequestTransportPin(), false);

			QSharedPointer<ChangePinContext> context(new ChangePinContext(true));
			model->resetChangePinContext(context);
			QCOMPARE(model->isRequestTransportPin(), true);
		}


		void test_isOnlyCheckPin()
		{
			const auto& model = Env::getSingleton<ChangePinModel>();
			QCOMPARE(model->isOnlyCheckPin(), false);

			QSharedPointer<ChangePinContext> context(new ChangePinContext(true, true, true));
			model->resetChangePinContext(context);
			QCOMPARE(model->isOnlyCheckPin(), true);
		}


};

QTEST_MAIN(test_ChangePinModel)
#include "test_ChangePinModel.moc"
