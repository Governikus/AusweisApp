/*!
 * \brief Unit tests for \ref DeviceInfo
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include <QSysInfo>
#include <QtTest>

#include "DeviceInfo.h"


using namespace governikus;


class test_DeviceInfo
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void getter()
		{
#ifdef Q_OS_ANDROID
			QVERIFY(!DeviceInfo::getPrettyInfo().isEmpty());
			QVERIFY(!DeviceInfo::getFingerprint().isEmpty());
			QVERIFY(!DeviceInfo::getName().isEmpty());
#else
			QCOMPARE(DeviceInfo::getPrettyInfo(), QSysInfo::machineHostName() + QStringLiteral(" ()"));
			QVERIFY(DeviceInfo::getFingerprint().isNull());
			QCOMPARE(DeviceInfo::getName(), QSysInfo::machineHostName());
#endif
		}


};

QTEST_GUILESS_MAIN(test_DeviceInfo)
#include "test_DeviceInfo.moc"
