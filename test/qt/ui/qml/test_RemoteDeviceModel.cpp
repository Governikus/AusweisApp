/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"
#include "IfdDescriptor.h"
#include "RemoteDeviceModel.h"
#include "RemoteIfdClient.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(RemoteDeviceModel::SettingsRemoteRoles)


class MockRemoteIfdClient
	: public RemoteIfdClient
{
	Q_OBJECT

	[[nodiscard]] QList<QSharedPointer<IfdListEntry>> getAnnouncingRemoteDevices() const override;
};


QList<QSharedPointer<IfdListEntry>> MockRemoteIfdClient::getAnnouncingRemoteDevices() const
{
	const auto dName = QStringLiteral("new_name");
	const auto dId = QByteArrayLiteral("myDeviceId");
	Discovery disco(dName, dId, 12345, {IfdVersion::Version::latest}, false);
	disco.setAddresses({QHostAddress(QStringLiteral("127.0.0.1"))});
	const auto ifdDesc = IfdDescriptor(disco);
	const QSharedPointer<IfdListEntry> remoteDeviceListEntry(new IfdListEntry(ifdDesc));
	remoteDeviceListEntry->setIfdDescriptor(ifdDesc);
	return {remoteDeviceListEntry};
}


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
			mName = QStringLiteral("name&lt;");
			mModel.reset(new RemoteDeviceModel());
			mEntry = QSharedPointer<RemoteDeviceModelEntry>::create(mName);
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


		void test_IsPairing()
		{
			QVERIFY(!mEntry->isPairing());
			mEntry->setIsPairing(true);
			QVERIFY(mEntry->isPairing());
		}


		void test_DeviceNameEscaped()
		{
			QCOMPARE(mEntry->getDeviceNameEscaped(), mName);
		}


		void test_Id()
		{
			const QByteArray id = QByteArrayLiteral("id");

			QCOMPARE(mEntry->getId(), QByteArray());

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
			const QByteArray id = QByteArrayLiteral("id");
			const QDateTime time(QDateTime::currentDateTime());
			RemoteDeviceModelEntry entry1(name);
			const IfdDescriptor descriptor = IfdDescriptor();
			QSharedPointer<IfdListEntry> pointer(new IfdListEntry(descriptor));
			RemoteDeviceModelEntry entry2(name, id, true, true, true, true, time, pointer);

			QVERIFY(!entry1.isSupported());
			QVERIFY(entry2.isSupported());
		}


		void test_RemoteDeviceListEntry()
		{
			const QString name = QStringLiteral("name");
			const QByteArray id = QByteArrayLiteral("id");
			RemoteDeviceModelEntry entry1(name);
			QCOMPARE(entry1.getRemoteDeviceListEntry(), nullptr);

			const IfdDescriptor defaultDescriptor = IfdDescriptor();
			QSharedPointer<IfdListEntry> listEntry1(new IfdListEntry(defaultDescriptor));
			RemoteDeviceModelEntry entry2(listEntry1);
			QCOMPARE(entry2.getRemoteDeviceListEntry(), listEntry1);

			Discovery discovery(name, id, 11111, {IfdVersion::supported()}, true);
			discovery.setAddresses({QHostAddress(QHostAddress::LocalHost)});
			const IfdDescriptor descriptor = IfdDescriptor(discovery, true);
			QSharedPointer<IfdListEntry> listEntry2(new IfdListEntry(descriptor));
			RemoteDeviceModelEntry entry3(listEntry2);
			QCOMPARE(entry3.getRemoteDeviceListEntry(), listEntry2);
			QCOMPARE(entry3.getId(), id);
			QCOMPARE(entry3.getDeviceNameEscaped(), name);
			QVERIFY(entry3.isSupported());
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
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::IS_LAST_ADDED_DEVICE));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::LINK_QUALITY));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::IS_PAIRING));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::IS_PAIRED));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::IS_SUPPORTED));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::IS_NETWORK_VISIBLE));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::DEVICE_ID));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::LAST_CONNECTED));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::REMOTE_DEVICE_STATUS));
			QVERIFY(mModel->roleNames().keys().contains(RemoteDeviceModel::SettingsRemoteRoles::REMOTE_DEVICE_NAME));

			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("remoteDeviceName")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("remoteDeviceStatus")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("lastConnected")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("deviceId")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("isNetworkVisible")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("isSupported")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("isPaired")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("isPairing")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("linkQualityInPercent")));
			QVERIFY(mModel->roleNames().values().contains(QByteArrayLiteral("isLastAddedDevice")));
		}


		void test_GetStatus()
		{
			RemoteDeviceModelEntry entry;

			QCOMPARE(mModel->getStatus(entry), "Not connected"_L1);

			mModel->mAllRemoteReaders.insert(0, entry);

			QCOMPARE(mModel->getStatus(entry), "Unsupported"_L1);

			entry.mSupported = true;
			QCOMPARE(mModel->getStatus(entry), "Not paired"_L1);

			entry.setPaired(true);
			QCOMPARE(mModel->getStatus(entry), "Unavailable"_L1);

			entry.setNetworkVisible(true);
			QCOMPARE(mModel->getStatus(entry), "Available"_L1);

			entry.mSupported = false;
			QCOMPARE(mModel->getStatus(entry), "Paired, but unsupported"_L1);

			entry.setNetworkVisible(true);
			entry.mSupported = true;
			entry.mIsPairing = true;
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
			QCOMPARE(mModel->getStatus(entry), "Click to pair"_L1);
#else
			QCOMPARE(mModel->getStatus(entry), "Available"_L1);

			entry.setPaired(false);
			entry.mIsPairing = false;
			QCOMPARE(mModel->getStatus(entry), "Not paired"_L1);
#endif
		}


		void test_GetRemoteDeviceListEntryId()
		{
			RemoteDeviceModelEntry entry1;
			RemoteDeviceModelEntry entry2;
			entry1.setId("id"_ba);

			QCOMPARE(mModel->getRemoteDeviceListEntry("id"_ba), QSharedPointer<IfdListEntry>());

			mModel->mAllRemoteReaders.insert(0, entry1);
			mModel->mAllRemoteReaders.insert(1, entry2);

			QCOMPARE(mModel->getRemoteDeviceListEntry("id"), nullptr);
		}


		void test_GetRemoteDeviceListEntryModelIndex()
		{
			Discovery discovery(QStringLiteral("entry 1"), QByteArrayLiteral("01"), 11111, {IfdVersion::supported()}, true);
			discovery.setAddresses({QHostAddress(QHostAddress::LocalHost)});
			const IfdDescriptor descriptor = IfdDescriptor(discovery, true);
			QSharedPointer<IfdListEntry> listEntry(new IfdListEntry(descriptor));
			RemoteDeviceModelEntry entry1(listEntry);

			RemoteDeviceModelEntry entry2("entry 2"_L1);
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

			QTest::newRow("device name") << RemoteDeviceModel::SettingsRemoteRoles::REMOTE_DEVICE_NAME << 0 << 0 << QVariant("reader 1"_L1);
			QTest::newRow("device status") << RemoteDeviceModel::SettingsRemoteRoles::REMOTE_DEVICE_STATUS << 1 << 0 << QVariant("Unsupported"_L1);
			QTest::newRow("last connected") << RemoteDeviceModel::SettingsRemoteRoles::LAST_CONNECTED << 0 << 0 << QVariant("14.05.2019 12:00 AM"_L1);
			QTest::newRow("device id") << RemoteDeviceModel::SettingsRemoteRoles::DEVICE_ID << 0 << 0 << QVariant("test id"_ba);
			QTest::newRow("network visible") << RemoteDeviceModel::SettingsRemoteRoles::IS_NETWORK_VISIBLE << 1 << 0 << QVariant(bool(false));
			QTest::newRow("supported") << RemoteDeviceModel::SettingsRemoteRoles::IS_SUPPORTED << 0 << 0 << QVariant(bool(true));
			QTest::newRow("paired") << RemoteDeviceModel::SettingsRemoteRoles::IS_PAIRED << 0 << 0 << QVariant(bool(true));
			QTest::newRow("is pairing") << RemoteDeviceModel::SettingsRemoteRoles::IS_PAIRED << 0 << 0 << QVariant(bool(true));
			QTest::newRow("link quality") << RemoteDeviceModel::SettingsRemoteRoles::LINK_QUALITY << 0 << 0 << QVariant(int(0));
			QTest::newRow("is last added device") << RemoteDeviceModel::SettingsRemoteRoles::IS_LAST_ADDED_DEVICE << 0 << 0 << QVariant(bool(false));
		}


		void test_Data()
		{
			QFETCH(RemoteDeviceModel::SettingsRemoteRoles, role);
			QFETCH(int, row);
			QFETCH(int, column);
			QFETCH(QVariant, output);

			QList<RemoteDeviceModelEntry> readers;
			QSharedPointer<IfdListEntry> listEntry(new IfdListEntry(IfdDescriptor()));
			const RemoteDeviceModelEntry entry1("reader 1"_L1, "test id"_ba, true, false, true, false, QDateTime(QDate(2019, 5, 14), QTime(0, 0)), listEntry);
			const RemoteDeviceModelEntry entry2("reader 2"_L1);
			readers << entry1 << entry2;
			mModel->mAllRemoteReaders = readers;
			const auto& index = mModel->createIndex(row, column);
			QCOMPARE(mModel->data(index, role), output);
		}


		void test_SetLastPairedReader()
		{
			const auto cert = QSslCertificate();

			RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.addTrustedCertificate(cert);

			const auto dName = QStringLiteral("myDeviceName");
			const auto dId = QByteArrayLiteral("myDeviceId");
			Discovery disco(dName, dId, 12345, {IfdVersion::Version::latest}, false);
			disco.setAddresses({QHostAddress(QStringLiteral("127.0.0.1"))});
			const auto ifdDesc = IfdDescriptor(disco);
			const QSharedPointer<IfdListEntry> remoteDeviceListEntry(new IfdListEntry(ifdDesc));

			mModel->addOrUpdateReader(RemoteDeviceModelEntry(remoteDeviceListEntry));

			QSignalSpy spy(mModel.get(), &RemoteDeviceModel::dataChanged);
			mModel->setLastPairedReader(cert);
			QTRY_COMPARE(spy.size(), 1);
			QVERIFY(mModel->data(mModel->index(0), RemoteDeviceModel::IS_LAST_ADDED_DEVICE).toBool());
		}


		void test_ChangedName()
		{
			MockRemoteIfdClient remoteIfdClient;
			Env::set(RemoteIfdClient::staticMetaObject, &remoteIfdClient);

			RemoteDeviceModel model;

			RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.addTrustedCertificate(QSslCertificate());

			const auto dName = QStringLiteral("old_name");
			const auto dId = QByteArrayLiteral("myDeviceId");
			Discovery disco(dName, dId, 12345, {IfdVersion::Version::latest}, false);
			disco.setAddresses({QHostAddress(QStringLiteral("127.0.0.1"))});
			const auto ifdDesc = IfdDescriptor(disco);
			const QSharedPointer<IfdListEntry> remoteDeviceListEntry(new IfdListEntry(ifdDesc));

			model.mAllRemoteReaders.clear();
			model.addOrUpdateReader(RemoteDeviceModelEntry(remoteDeviceListEntry));

			const auto newName = QStringLiteral("new_name");
			Discovery newDisco(newName, dId, 12345, {IfdVersion::Version::latest}, false);
			newDisco.setAddresses({QHostAddress(QStringLiteral("127.0.0.1"))});
			const auto newIfdDesc = IfdDescriptor(newDisco);
			remoteDeviceListEntry->setIfdDescriptor(newIfdDesc);

			const auto name = model.data(mModel->index(0), RemoteDeviceModel::SettingsRemoteRoles::REMOTE_DEVICE_NAME).toString();
			QCOMPARE(name, QStringLiteral("new_name (was old_name)"));

			Env::clear();
		}


		void test_startStopDetection()
		{
			QVERIFY(!Env::getSingleton<RemoteIfdClient>()->isDetecting());
			QTest::ignoreMessage(QtDebugMsg, "Starting Remote Device Detection");
			QTest::ignoreMessage(QtDebugMsg, "Bound on port: 24727");
			mModel->startDetection();
			QVERIFY(Env::getSingleton<RemoteIfdClient>()->isDetecting());
			mModel->startDetection();
			QVERIFY(Env::getSingleton<RemoteIfdClient>()->isDetecting());
			mModel->stopDetection(false);
			QVERIFY(Env::getSingleton<RemoteIfdClient>()->isDetecting());
			mModel->stopDetection(false);
			QVERIFY(Env::getSingleton<RemoteIfdClient>()->isDetecting());
			mModel->stopDetection(true);
			QVERIFY(Env::getSingleton<RemoteIfdClient>()->isDetecting());
			QTest::ignoreMessage(QtDebugMsg, "Starting Remote Device Detection");
			QTest::ignoreMessage(QtDebugMsg, "Detection already started");
			mModel->startDetection();
			QVERIFY(Env::getSingleton<RemoteIfdClient>()->isDetecting());
			QTest::ignoreMessage(QtDebugMsg, "Stopping Remote Device Detection");
			QTest::ignoreMessage(QtDebugMsg, "Shutdown socket");
			mModel->stopDetection(true);
			QVERIFY(!Env::getSingleton<RemoteIfdClient>()->isDetecting());
			mModel->stopDetection(true);
			QVERIFY(!Env::getSingleton<RemoteIfdClient>()->isDetecting());
			mModel->stopDetection(false);
			QVERIFY(!Env::getSingleton<RemoteIfdClient>()->isDetecting());
		}


};

QTEST_MAIN(test_RemoteDeviceModel)
#include "test_RemoteDeviceModel.moc"
