/*!
 * \brief Unit tests for \ref BuildHelper
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BuildHelper.h"

#include <QtTest/QtTest>

using namespace governikus;

class test_BuildHelper
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void dateTime()
		{
			QVERIFY(BuildHelper::getDateTime());

			QString buildDateTime = QLatin1String(BuildHelper::getDateTime());
			QCOMPARE(buildDateTime.size(), 22);
			QVERIFY(buildDateTime.contains(__DATE__));
		}


};

QTEST_GUILESS_MAIN(test_BuildHelper)
#include "test_BuildHelper.moc"
