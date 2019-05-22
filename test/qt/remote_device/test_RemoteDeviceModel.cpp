/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceModel.h"

#include <QtTest>


using namespace governikus;


class test_RemoteDeviceModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Paired()
		{
			const QString name = QStringLiteral("name");
			RemoteDeviceModelEntry entry(name);
			QVERIFY(!entry.isPaired());
			entry.setPaired(true);
			QVERIFY(entry.isPaired());
		}


		void test_DeviceName()
		{
			const QString name = QStringLiteral("name");
			RemoteDeviceModelEntry entry(name);
			QCOMPARE(entry.getDeviceName(), name);
		}


		void test_Id()
		{
			const QString name = QStringLiteral("name");
			const QString id = QStringLiteral("id");
			RemoteDeviceModelEntry entry(name);

			QCOMPARE(entry.getId(), QString());

			entry.setId(id);
			QCOMPARE(entry.getId(), id);
		}


		void test_NetworkVisible()
		{
			const QString name = QStringLiteral("name");
			RemoteDeviceModelEntry entry(name);

			QVERIFY(!entry.isNetworkVisible());

			entry.setNetworkVisible(true);
			QVERIFY(entry.isNetworkVisible());
		}


		void test_Supported()
		{
			const QString name = QStringLiteral("name");
			const QString id = QStringLiteral("id");
			const QDateTime time(QDateTime::currentDateTime());
			RemoteDeviceModelEntry entry1(name);
			RemoteDeviceModelEntry entry2(name, id, true, true, true, time);

			QVERIFY(!entry1.isSupported());
			QVERIFY(entry2.isSupported());
		}


		void test_RemoteDeviceListEntry()
		{
			const QString name = QStringLiteral("name");

			RemoteDeviceModelEntry entry1(name);
			QCOMPARE(entry1.getRemoteDeviceListEntry(), nullptr);

			const RemoteDeviceDescriptor descriptor = RemoteDeviceDescriptor();
			QSharedPointer<RemoteDeviceListEntry> pointer(new RemoteDeviceListEntry(descriptor));
			const QString id = QStringLiteral("id");
			RemoteDeviceModelEntry entry2(name, id, pointer);
			QCOMPARE(entry2.getRemoteDeviceListEntry(), pointer);
		}


		void test_LastConnected()
		{
			const QString name = QStringLiteral("name");
			RemoteDeviceModelEntry entry(name);
			QDateTime time(QDateTime::currentDateTime());

			QCOMPARE(entry.getLastConnected(), QDateTime());

			entry.setLastConnected(time);
			QCOMPARE(entry.getLastConnected(), time);
		}


		void test_RoleNames()
		{
			RemoteDeviceModel model;
			QVERIFY(model.roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::REMOTE_DEVICE_NAME));
			QVERIFY(model.roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::LAST_CONNECTED));
			QVERIFY(model.roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::DEVICE_ID));
			QVERIFY(model.roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::IS_NETWORK_VISIBLE));
			QVERIFY(model.roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::IS_SUPPORTED));

			QVERIFY(model.roleNames().values().contains(QByteArrayLiteral("remoteDeviceName")));
			QVERIFY(model.roleNames().values().contains(QByteArrayLiteral("lastConnected")));
			QVERIFY(model.roleNames().values().contains(QByteArrayLiteral("deviceId")));
			QVERIFY(model.roleNames().values().contains(QByteArrayLiteral("isNetworkVisible")));
			QVERIFY(model.roleNames().values().contains(QByteArrayLiteral("isSupported")));
		}


		void test_GetStatus()
		{
			RemoteDeviceModelEntry entry;

			RemoteDeviceModel model;

			QCOMPARE(model.getStatus(entry), QString("Not connected"));

			model.mAllRemoteReaders.insert(0, entry);

			QCOMPARE(model.getStatus(entry), QString("Unsupported version"));

			entry.mSupported = true;
			QCOMPARE(model.getStatus(entry), QString("Not paired"));

			entry.setPaired(true);
			QCOMPARE(model.getStatus(entry), QString("Paired, but unavailable"));

			entry.setNetworkVisible(true);
			QCOMPARE(model.getStatus(entry), QString("Paired and available"));

			entry.mSupported = false;
			QCOMPARE(model.getStatus(entry), QString("Paired, but unsupported"));
		}


		void test_HeaderData()
		{
			RemoteDeviceModel model;

			QCOMPARE(model.headerData(3, Qt::Vertical, 3), QVariant());
			QCOMPARE(model.headerData(0, Qt::Horizontal, 3), QVariant());
			QCOMPARE(model.headerData(0, Qt::Horizontal, 0), QVariant("Device"));
			QCOMPARE(model.headerData(1, Qt::Horizontal, 0), QVariant("Status"));
		}


		void test_ColumnCount()
		{
			RemoteDeviceModel model;
			QCOMPARE(model.columnCount(), 2);
		}


		void test_GetRemoteListEntry()
		{
			RemoteDeviceModel model;
			RemoteDeviceModelEntry entry1;
			RemoteDeviceModelEntry entry2;
			entry1.setId(QString("id"));

			QCOMPARE(model.getRemoteDeviceListEntry(QString("id")), QSharedPointer<RemoteDeviceListEntry>());

			model.mAllRemoteReaders.insert(0, entry1);
			model.mAllRemoteReaders.insert(1, entry2);

			QCOMPARE(model.getRemoteDeviceListEntry(QString("id")), nullptr);
		}


};

QTEST_GUILESS_MAIN(test_RemoteDeviceModel)
#include "test_RemoteDeviceModel.moc"
