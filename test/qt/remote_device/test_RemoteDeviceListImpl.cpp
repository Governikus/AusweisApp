/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceList.h"

#include "messages/Discovery.h"

#include <QtTest/QtTest>


using namespace governikus;


class test_RemoteDeviceListImpl
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testAddAndRemove()
		{
			RemoteDeviceListImpl deviceList(10, 1);
			QSignalSpy spyVanished(&deviceList, &RemoteDeviceListImpl::fireDeviceVanished);

			QSharedPointer<const Discovery> offerMsg1;
			QHostAddress addr1;
			{
				offerMsg1 = QSharedPointer<const Discovery>(new Discovery("Dev1", QStringLiteral("0123456789ABCDEF"), 1234, {IfdVersion::Version::v0}));
				addr1 = QHostAddress(QString("5.6.7.8"));
				const RemoteDeviceDescriptor descr(offerMsg1, addr1);

				QSignalSpy spy(&deviceList, &RemoteDeviceListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 1);
			}

			QSharedPointer<const Discovery> offerMsg2;
			QHostAddress addr2;
			{
				offerMsg2 = QSharedPointer<const Discovery>(new Discovery("Dev1", QStringLiteral("0123456789ABCDEF"), 1234, {IfdVersion::Version::v0}));
				addr2 = QHostAddress(QString("5.6.7.8"));
				const RemoteDeviceDescriptor descr(offerMsg1, addr2);

				QSignalSpy spy(&deviceList, &RemoteDeviceListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 0);
			}

			{
				offerMsg2 = QSharedPointer<const Discovery>(new Discovery("Dev1", QStringLiteral("0123456789ABCDEF"), 1234, {IfdVersion::Version::v0}));
				addr2 = QHostAddress(QString("5.6.7.9"));
				const RemoteDeviceDescriptor descr(offerMsg1, addr2);

				QSignalSpy spy(&deviceList, &RemoteDeviceListImpl::fireDeviceAppeared);
				deviceList.update(descr);
				QCOMPARE(spy.count(), 1);
			}

			spyVanished.wait();
			QCOMPARE(spyVanished.count(), 2);
		}


};

QTEST_GUILESS_MAIN(test_RemoteDeviceListImpl)
#include "test_RemoteDeviceListImpl.moc"
