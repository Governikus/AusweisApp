/*!
 * \brief Unit tests for \ref DeviceInfo
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

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
			QVERIFY(!DeviceInfo::getModel().isEmpty());
#else
			QVERIFY(DeviceInfo::getPrettyInfo().isNull());
			QVERIFY(DeviceInfo::getFingerprint().isNull());
			QVERIFY(DeviceInfo::getModel().isNull());
#endif
		}


};

QTEST_GUILESS_MAIN(test_DeviceInfo)
#include "test_DeviceInfo.moc"
