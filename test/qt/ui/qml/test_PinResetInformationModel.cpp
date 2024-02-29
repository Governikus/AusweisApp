/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref PinResetInformationModel
 */

#include "PinResetInformationModel.h"

#include "Env.h"
#include "ProviderConfiguration.h"

#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(QMetaMethod)

class test_PinResetInformationModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_PinResetUrl()
		{
			QVERIFY(!Env::getSingleton<PinResetInformationModel>()->getPinResetUrl().isEmpty());
		}


		void test_fireUpdateOnTranslationChanged()
		{
			auto pinResetInformationModel = Env::getSingleton<PinResetInformationModel>();
			QSignalSpy spy(pinResetInformationModel, &PinResetInformationModel::fireUpdated);

			pinResetInformationModel->onTranslationChanged();

			QCOMPARE(spy.count(), 1);
		}


		void test_fireUpdateOnProviderConfigurationUpdated()
		{
			QSignalSpy spy(Env::getSingleton<PinResetInformationModel>(), &PinResetInformationModel::fireUpdated);

			Q_EMIT Env::getSingleton<ProviderConfiguration>()->fireUpdated();

			QCOMPARE(spy.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_PinResetInformationModel)
#include "test_PinResetInformationModel.moc"
