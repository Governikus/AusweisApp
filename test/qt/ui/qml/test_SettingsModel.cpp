/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */


#include "SettingsModel.h"

#include "AppSettings.h"

#include <QSignalSpy>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_SettingsModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testAnimations()
		{
			const auto* model = Env::getSingleton<SettingsModel>();
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


		void testModeOption()
		{
			const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			auto* model = Env::getSingleton<SettingsModel>();
			QCOMPARE(model->getDarkMode(), SettingsModel::ModeOption::OFF);

			model->setDarkMode(SettingsModel::ModeOption::AUTO);
			QCOMPARE(settings.getDarkMode(), "AUTO"_L1);
			QCOMPARE(model->getDarkMode(), SettingsModel::ModeOption::AUTO);

			model->setDarkMode(SettingsModel::ModeOption::ON);
			QCOMPARE(settings.getDarkMode(), "ON"_L1);
			QCOMPARE(model->getDarkMode(), SettingsModel::ModeOption::ON);
		}


};

QTEST_GUILESS_MAIN(test_SettingsModel)
#include "test_SettingsModel.moc"
