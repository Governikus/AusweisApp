/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */


#include "SettingsModel.h"

#include <QSignalSpy>
#include <QtTest>


using namespace governikus;


class test_SettingsModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testAnimations()
		{
			const auto model = Env::getSingleton<SettingsModel>();
			QSignalSpy animationsSpy(model, &SettingsModel::fireUseAnimationsChanged);

			QCOMPARE(animationsSpy.count(), 0);
			QCOMPARE(model->isUseAnimations(), true);

			model->setUseAnimations(false);
			QCOMPARE(animationsSpy.count(), 1);
			QCOMPARE(model->isUseAnimations(), false);

			model->setUseAnimations(false);
			QCOMPARE(animationsSpy.count(), 1);
			QCOMPARE(model->isUseAnimations(), false);

			model->setUseAnimations(true);
			QCOMPARE(animationsSpy.count(), 2);
			QCOMPARE(model->isUseAnimations(), true);

			model->setUseAnimations(true);
			QCOMPARE(animationsSpy.count(), 2);
			QCOMPARE(model->isUseAnimations(), true);
		}


};

QTEST_GUILESS_MAIN(test_SettingsModel)
#include "test_SettingsModel.moc"
