/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdListImpl.h"

#include "messages/Discovery.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_IfdListImpl
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testAddAndRemove()
		{
			IfdListImpl deviceList(10, 1);
			QSignalSpy spyVanished(&deviceList, &IfdListImpl::fireDeviceVanished);

			const Discovery offerMsg1 = Discovery("Dev1"_L1, QByteArrayLiteral("0123456789ABCDEF"), 1234, {IfdVersion::Version::latest});
			const Discovery offerMsg2 = Discovery("Dev1"_L1, QByteArrayLiteral("0123456789ABCDFF"), 1234, {IfdVersion::Version::latest});
			const QHostAddress addr1 = QHostAddress("5.6.7.8"_L1);
			const QHostAddress addr2 = QHostAddress("5.6.7.9"_L1);

			{
				auto discovery = offerMsg1;
				discovery.setAddresses({addr1});
				const IfdDescriptor descr(discovery);

				QSignalSpy spy(&deviceList, &IfdListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 1);
			}

			{
				auto discovery = offerMsg1;
				discovery.setAddresses({addr1});
				const IfdDescriptor descr(discovery);

				QSignalSpy spy(&deviceList, &IfdListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 0);
			}

			{
				auto discovery = offerMsg1;
				discovery.setAddresses({addr2});
				const IfdDescriptor descr(discovery);

				QSignalSpy spy(&deviceList, &IfdListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 0);
			}

			{
				auto discovery = offerMsg2;
				discovery.setAddresses({addr2});
				const IfdDescriptor descr(discovery);

				QSignalSpy spy(&deviceList, &IfdListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 1);
			}

			{
				auto discovery = offerMsg2;
				discovery.setAddresses({addr1});
				const IfdDescriptor descr(discovery);

				QSignalSpy spy(&deviceList, &IfdListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 0);
			}

			QTRY_COMPARE(spyVanished.count(), 2); // clazy:exclude=qstring-allocations
		}


};

QTEST_GUILESS_MAIN(test_IfdListImpl)
#include "test_IfdListImpl.moc"
