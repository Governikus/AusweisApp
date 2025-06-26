/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdDescriptor.h"

#include "messages/Discovery.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_IfdDescriptor
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void defaultValues()
		{
			const IfdDescriptor descriptor((Discovery(QJsonObject())));
			QCOMPARE(descriptor.getIfdName(), QString());
			QCOMPARE(descriptor.getIfdId(), QByteArray());
			QVERIFY(descriptor.getSupportedApis().isEmpty());
			QCOMPARE(descriptor.isSupported(), false);
			QCOMPARE(descriptor.isPairing(), false);
			QVERIFY(descriptor.getAddresses().isEmpty());
			QCOMPARE(descriptor.isNull(), true);
			QCOMPARE(descriptor.isLocalIfd(), false);
		}


		void values()
		{
			Discovery discovery("Device"_L1, "0123456789ABCDEF"_ba, 1337, {IfdVersion::Version::latest}, true);
			discovery.setAddresses({QHostAddress("192.168.1.42"_L1)});
			const IfdDescriptor descriptor(discovery, true);
			QCOMPARE(descriptor.getIfdName(), "Device"_L1);
			QCOMPARE(descriptor.getIfdId(), "0123456789ABCDEF"_ba);
			QCOMPARE(descriptor.getSupportedApis(), QList<IfdVersion::Version>({IfdVersion::Version::latest}));
			QCOMPARE(descriptor.isSupported(), true);
			QCOMPARE(descriptor.isPairing(), true);
			QCOMPARE(descriptor.getAddresses(), QList<QUrl>({QUrl("wss://192.168.1.42:1337"_L1)}));
			QCOMPARE(descriptor.isNull(), false);
			QCOMPARE(descriptor.isLocalIfd(), true);
		}


};

QTEST_GUILESS_MAIN(test_IfdDescriptor)
#include "test_IfdDescriptor.moc"
