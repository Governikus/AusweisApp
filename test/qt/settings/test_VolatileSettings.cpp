/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref VolatileSettings
 */

#include "VolatileSettings.h"

#include "Env.h"

#include <QSignalSpy>
#include <QtTest>

using namespace governikus;

class test_VolatileSettings
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isUsedAsSdkChanged()
		{
			auto* settings = Env::getSingleton<VolatileSettings>();
			QSignalSpy spy(settings, &VolatileSettings::fireUsedAsSdkChanged);

			QVERIFY(settings->isUsedAsSDK());
			settings->setUsedAsSDK(true);
			QCOMPARE(spy.count(), 0); // default value
			QVERIFY(settings->isUsedAsSDK());

			settings->setUsedAsSDK(false);
			QVERIFY(!settings->isUsedAsSDK());
			QCOMPARE(spy.count(), 1);
			settings->setUsedAsSDK(false);
			QVERIFY(!settings->isUsedAsSDK());
			QCOMPARE(spy.count(), 1);

			settings->setUsedAsSDK(true);
			QVERIFY(settings->isUsedAsSDK());
			QCOMPARE(spy.count(), 2);
		}


		void deadlockIfSlotCallsGetter()
		{
			auto* settings = Env::getSingleton<VolatileSettings>();
			connect(settings, &VolatileSettings::fireUsedAsSdkChanged, this, []
				{
					Q_UNUSED(Env::getSingleton<VolatileSettings>()->isUsedAsSDK());
				});
			QSignalSpy spy(settings, &VolatileSettings::fireUsedAsSdkChanged);

			QVERIFY(settings->isUsedAsSDK());
			settings->setUsedAsSDK(false);
			QCOMPARE(spy.count(), 1);
		}


		void delay()
		{
			auto* settings = Env::getSingleton<VolatileSettings>();
			QCOMPARE(settings->getDelay(), 0);
			settings->setDelay(500);
			QCOMPARE(settings->getDelay(), 500);
			settings->setDelay();
			QCOMPARE(settings->getDelay(), 0);
		}


};

QTEST_GUILESS_MAIN(test_VolatileSettings)
#include "test_VolatileSettings.moc"
