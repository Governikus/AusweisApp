/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceModel.h"

#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(RemoteDeviceModel::SettingsRemoteRoles)

class test_RemoteDeviceModel
	: public QObject
{
	Q_OBJECT
	QSharedPointer<RemoteDeviceModel> mModel;
	QSharedPointer<RemoteDeviceModelEntry> mEntry;
	QString mName;

	private Q_SLOTS:
		void init()
		{
			mName = QStringLiteral("name");
			mModel.reset(new RemoteDeviceModel());
			mEntry.reset(new RemoteDeviceModelEntry(mName));
		}


		void cleanup()
		{
			mModel.clear();
			mEntry.clear();
		}


		void test_Paired()
		{
			QVERIFY(!mEntry->isPaired());
			mEntry->setPaired(true);
			QVERIFY(mEntry->isPaired());
		}


		void test_DeviceName()
		{
			QCOMPARE(mEntry->getDeviceName(), mName);
		}


		void test_Id()
		{
			const QString id = QStringLiteral("id");

			QCOMPARE(mEntry->getId(), QString());

			mEntry->setId(id);
			QCOMPARE(mEntry->getId(), id);
		}


		void test_NetworkVisible()
		{
			QVERIFY(!mEntry->isNetworkVisible());

			mEntry->setNetworkVisible(true);
			QVERIFY(mEntry->isNetworkVisible());
		}


		void test_Supported()
		{
			const QString name = QStringLiteral("name");
			const QString id = QStringLiteral("id");
			const QDateTime time(QDateTime::currentDateTime());
			RemoteDeviceModelEntry entry1(name);
			const RemoteDeviceDescriptor descriptor = RemoteDeviceDescriptor();
			QSharedPointer<RemoteDeviceListEntry> pointer(new RemoteDeviceListEntry(descriptor));
			RemoteDeviceModelEntry entry2(name, id, true, true, true, time, pointer);

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
			QDateTime time(QDateTime::currentDateTime());

			QCOMPARE(mEntry->getLastConnected(), QDateTime());

			mEntry->setLastConnected(time);
			QCOMPARE(mEntry->getLastConnected(), time);
		}


		void test_RoleNames()
		{
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::REMOTE_DEVICE_NAME));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::LAST_CONNECTED));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::DEVICE_ID));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::IS_NETWORK_VISIBLE));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::IS_SUPPORTED));

			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("remoteDeviceName")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("lastConnected")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("deviceId")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("isNetworkVisible")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("isSupported")));
		}


		void test_GetStatus()
		{
			RemoteDeviceModelEntry entry;

			QCOMPARE(mModel->getStatus(entry), QString("Not connected"));

			mModel->mAllRemoteReaders.insert(0, entry);

			QCOMPARE(mModel->getStatus(entry), QString("Unsupported"));

			entry.mSupported = true;
			QCOMPARE(mModel->getStatus(entry), QString("Not paired"));

			entry.setPaired(true);
			QCOMPARE(mModel->getStatus(entry), QString("Paired, but unavailable"));

			entry.setNetworkVisible(true);
			QCOMPARE(mModel->getStatus(entry), QString("Available"));

			entry.mSupported = false;
			QCOMPARE(mModel->getStatus(entry), QString("Paired, but unsupported"));
		}


		void test_HeaderData()
		{
			QCOMPARE(mModel->headerData(3, Qt::Vertical, 3), QVariant());
			QCOMPARE(mModel->headerData(0, Qt::Horizontal, 3), QVariant());
			QCOMPARE(mModel->headerData(0, Qt::Horizontal, 0), QVariant("Device"));
			QCOMPARE(mModel->headerData(1, Qt::Horizontal, 0), QVariant("Status"));
		}


		void test_ColumnCount()
		{
			QCOMPARE(mModel->columnCount(), 2);
		}


		void test_GetRemoteDeviceListEntryId()
		{
			RemoteDeviceModelEntry entry1;
			RemoteDeviceModelEntry entry2;
			entry1.setId(QString("id"));

			QCOMPARE(mModel->getRemoteDeviceListEntry(QString("id")), QSharedPointer<RemoteDeviceListEntry>());

			mModel->mAllRemoteReaders.insert(0, entry1);
			mModel->mAllRemoteReaders.insert(1, entry2);

			QCOMPARE(mModel->getRemoteDeviceListEntry(QString("id")), nullptr);
		}


		void test_GetRemoteDeviceListEntryModelIndex()
		{
			QSharedPointer<RemoteDeviceListEntry> listEntry(new RemoteDeviceListEntry(RemoteDeviceDescriptor()));
			RemoteDeviceModelEntry entry1(QString("entry 1"), QString("01"), listEntry);
			RemoteDeviceModelEntry entry2(QString("entry 2"));
			mModel->mAllRemoteReaders << entry1 << entry2;

			const auto& index1 = mModel->createIndex(0, 0);
			QCOMPARE(mModel->getRemoteDeviceListEntry(index1), listEntry);

			const auto& index2 = mModel->createIndex(1, 0);
			QCOMPARE(mModel->getRemoteDeviceListEntry(index2), nullptr);
		}


		void test_Data_data()
		{
			QTest::addColumn<RemoteDeviceModel::SettingsRemoteRoles>("role");
			QTest::addColumn<int>("row");
			QTest::addColumn<int>("column");
			QTest::addColumn<QVariant>("output");

			QTest::newRow("device name") << RemoteDeviceModel::SettingsRemoteRoles::REMOTE_DEVICE_NAME << 0 << 0 << QVariant(QString("reader 1"));
			QTest::newRow("device status") << RemoteDeviceModel::SettingsRemoteRoles::REMOTE_DEVICE_STATUS << 1 << 0 << QVariant(QString("Unsupported"));
			QTest::newRow("last connected") << RemoteDeviceModel::SettingsRemoteRoles::LAST_CONNECTED << 0 << 0 << QVariant(QString("14.05.2019 12:00 AM"));
			QTest::newRow("device id") << RemoteDeviceModel::SettingsRemoteRoles::DEVICE_ID << 0 << 0 << QVariant(QString("test id"));
			QTest::newRow("network visible") << RemoteDeviceModel::SettingsRemoteRoles::IS_NETWORK_VISIBLE << 1 << 0 << QVariant(bool(false));
			QTest::newRow("supported") << RemoteDeviceModel::SettingsRemoteRoles::IS_SUPPORTED << 0 << 0 << QVariant(bool(true));
			QTest::newRow("paired") << RemoteDeviceModel::SettingsRemoteRoles::IS_PAIRED << 0 << 0 << QVariant(bool(true));
			QTest::newRow("link quality") << RemoteDeviceModel::SettingsRemoteRoles::LINK_QUALITY << 0 << 0 << QVariant(int(0));
		}


		void test_Data()
		{
			QFETCH(RemoteDeviceModel::SettingsRemoteRoles, role);
			QFETCH(int, row);
			QFETCH(int, column);
			QFETCH(QVariant, output);

			QVector<RemoteDeviceModelEntry> readers;
			QSharedPointer<RemoteDeviceListEntry> listEntry(new RemoteDeviceListEntry(RemoteDeviceDescriptor()));
			const RemoteDeviceModelEntry entry1(QString("reader 1"), QString("test id"), true, false, true, QDateTime(QDate(2019, 5, 14), QTime(0, 0)), listEntry);
			const RemoteDeviceModelEntry entry2(QString("reader 2"));
			readers << entry1 << entry2;
			mModel->mAllRemoteReaders = readers;
			const auto& index = mModel->createIndex(row, column);
			QCOMPARE(mModel->data(index, role), output);
		}


};

QTEST_GUILESS_MAIN(test_RemoteDeviceModel)
#include "test_RemoteDeviceModel.moc"
