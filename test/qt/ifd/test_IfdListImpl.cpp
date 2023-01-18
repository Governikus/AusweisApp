/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "IfdList.h"

#include "messages/Discovery.h"

#include <QtTest>

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

			const Discovery offerMsg1 = Discovery("Dev1", QStringLiteral("0123456789ABCDEF"), 1234, {IfdVersion::Version::latest});
			const Discovery offerMsg2 = Discovery("Dev1", QStringLiteral("0123456789ABCDFF"), 1234, {IfdVersion::Version::latest});
			const QHostAddress addr1 = QHostAddress(QString("5.6.7.8"));
			const QHostAddress addr2 = QHostAddress(QString("5.6.7.9"));

			{
				const IfdDescriptor descr(offerMsg1, addr1);

				QSignalSpy spy(&deviceList, &IfdListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 1);
			}

			{
				const IfdDescriptor descr(offerMsg1, addr1);

				QSignalSpy spy(&deviceList, &IfdListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 0);
			}

			{
				const IfdDescriptor descr(offerMsg1, addr2);

				QSignalSpy spy(&deviceList, &IfdListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 0);
			}

			{
				const IfdDescriptor descr(offerMsg2, addr2);

				QSignalSpy spy(&deviceList, &IfdListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 1);
			}

			{
				const IfdDescriptor descr(offerMsg2, addr1);

				QSignalSpy spy(&deviceList, &IfdListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 0);
			}

			QTRY_COMPARE(spyVanished.count(), 2); // clazy:exclude=qstring-allocations
		}


};

QTEST_GUILESS_MAIN(test_IfdListImpl)
#include "test_IfdListImpl.moc"
