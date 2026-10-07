/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#include "ReaderModel.h"

#include "Env.h"
#include "MockReaderConfiguration.h"
#include "MockReaderDetector.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class MockReaderManager
	: public ReaderManager
{
	Q_OBJECT

	private:
		const QList<ReaderInfo>& mReaderInfos;
		bool mIsScanRunning;

	public:
		MockReaderManager(const QList<ReaderInfo>& pReaderInfos);
		ReaderManagerPluginInfo getPluginInfo(ReaderManagerPluginType pType) const override;
		QList<ReaderInfo> getReaderInfos(const ReaderFilter& pFilter = ReaderFilter()) const override;

		void setIsScanRunning(bool pRunning);
};


MockReaderManager::MockReaderManager(const QList<ReaderInfo>& pReaderInfos)
	: mReaderInfos(pReaderInfos)
	, mIsScanRunning(false)
{
}


ReaderManagerPluginInfo MockReaderManager::getPluginInfo(ReaderManagerPluginType pType) const
{
	ReaderManagerPluginInfo info(pType);
	info.setScanRunning(mIsScanRunning);
	return info;
}


QList<ReaderInfo> MockReaderManager::getReaderInfos(const ReaderFilter& pFilter) const
{
	Q_UNUSED(pFilter)
	return mReaderInfos;
}


void MockReaderManager::setIsScanRunning(bool pRunning)
{
	mIsScanRunning = pRunning;
}


class test_ReaderModel
	: public QObject
{
	Q_OBJECT

	public:
		test_ReaderModel()
			: mMockReaderConfiguration()
			, mUsbIds()
			, mMockReaderDetector(mUsbIds)
			, mReaderInfos()
			, mMockReaderManager(mReaderInfos)
		{
			Env::set(ReaderDetector::staticMetaObject, &mMockReaderDetector);
			Env::set(ReaderManager::staticMetaObject, &mMockReaderManager);
		}

	private:
		QScopedPointer<MockReaderConfiguration> mMockReaderConfiguration;

		QList<UsbId> mUsbIds;
		MockReaderDetector mMockReaderDetector;

		QList<ReaderInfo> mReaderInfos;
		MockReaderManager mMockReaderManager;

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			mMockReaderConfiguration.reset(new MockReaderConfiguration());
			Env::set(ReaderConfiguration::staticMetaObject, mMockReaderConfiguration.data());

			mUsbIds.clear();
			mReaderInfos.clear();
		}


		void cleanup()
		{
			mMockReaderConfiguration.reset();
		}


		void test_empty()
		{
			mMockReaderConfiguration->clearReaderConfiguration();

			ReaderModel readerModel;
			QVERIFY(!readerModel.hasConnectedReader());
			QCOMPARE(readerModel.rowCount(), 1);
		}


		void test_settings()
		{
			ReaderModel readerModel;
			QVERIFY(!readerModel.hasConnectedReader());
			QCOMPARE(readerModel.rowCount(), 1);
		}


		void test_usbId()
		{
#if defined(Q_OS_FREEBSD)
			QSKIP("No driver available for UsbId(0x0C4B, 0x0501) on FreeBSD");
#endif

			mUsbIds += UsbId(0x0C4B, 0x0501);    // REINER SCT cyberJack RFID komfort

			ReaderModel readerModel;
			QCOMPARE(readerModel.rowCount(), 1);
			const auto& index = readerModel.index(0, 0, QModelIndex());
			const auto& htmlDescription = readerModel.data(index, ReaderModel::UserRoles::READER_HTML_DESCRIPTION).toString();
			QVERIFY(htmlDescription.startsWith(tr("The smartcard service of your system is not reachable.")));
		}


		void test_usbId_unknown()
		{
			mUsbIds += UsbId(0x1, 0x2);    // Unknown

			ReaderModel readerModel;
			QVERIFY(!readerModel.hasConnectedReader());
			QCOMPARE(readerModel.rowCount(), 1);
		}


		void test_usbId_readerManager_different_scan_not_running()
		{
#if defined(Q_OS_FREEBSD)
			QSKIP("No driver available for UsbId(0x04E6, 0x5790) on FreeBSD");
#endif
			mUsbIds += UsbId(0x0C4B, 0x0501);    // REINER SCT cyberJack RFID komfort
			mUsbIds += UsbId(0x04E6, 0x5790);    // Identiv Cloud 3700 F
			mReaderInfos += ReaderInfo("Identiv CLOUD 3700 F Contactless Reader 0"_L1, ReaderManagerPluginType::PCSC);

			QModelIndex index;
			ReaderModel readerModel;
			QCOMPARE(readerModel.rowCount(), 2);
			index = readerModel.index(0, 0, QModelIndex());
			QCOMPARE(readerModel.data(index, ReaderModel::UserRoles::READER_HTML_DESCRIPTION).toString(), tr("The smartcard service of your system is not reachable."));
			index = readerModel.index(1, 0, QModelIndex());
			const auto& htmlDescription = readerModel.data(index, ReaderModel::UserRoles::READER_HTML_DESCRIPTION).toString();
			QVERIFY(htmlDescription.startsWith(tr("The smartcard service of your system is not reachable.")));
		}


		void test_usbId_readerManager_different_scan_running()
		{
#if defined(Q_OS_FREEBSD)
			QSKIP("No driver available for UsbId(0x04E6, 0x5790) on FreeBSD");
#endif
			mUsbIds += UsbId(0x0C4B, 0x0501);    // REINER SCT cyberJack RFID komfort
			mUsbIds += UsbId(0x04E6, 0x5790);    // Identiv Cloud 3700 F
			mReaderInfos += ReaderInfo("Identiv CLOUD 3700 F Contactless Reader 0"_L1, ReaderManagerPluginType::PCSC);

			mMockReaderManager.setIsScanRunning(true);

			QModelIndex index;
			ReaderModel readerModel;
			QCOMPARE(readerModel.rowCount(), 2);
			index = readerModel.index(0, 0, QModelIndex());
			QCOMPARE(readerModel.data(index, ReaderModel::UserRoles::READER_HTML_DESCRIPTION).toString(), tr("Driver installed"));
			index = readerModel.index(1, 0, QModelIndex());
			const auto& htmlDescription = readerModel.data(index, ReaderModel::UserRoles::READER_HTML_DESCRIPTION).toString();
			QVERIFY(htmlDescription.startsWith(tr("No driver installed")));

		}


		void test_settings_readerManager_unknown()
		{
			mReaderInfos += ReaderInfo("Governikus Special Reader"_L1, ReaderManagerPluginType::PCSC);

			ReaderModel readerModel;
			QCOMPARE(readerModel.rowCount(), 1);
		}


		void test_RoleData()
		{
			const auto ri = ReaderConfigurationInfo(42, {}, "DummyReader"_L1, "DummyUrl"_L1, {}, {});
			mMockReaderConfiguration->readerConfigurationInfos().append(ri);

			mMockReaderManager.setIsScanRunning(true);

			QModelIndex index;
			ReaderModel readerModel;
			readerModel.mConnectedReaders = {ri};
			QCOMPARE(readerModel.rowCount(), 1);
			index = readerModel.index(0, 0, QModelIndex());
			const auto& htmlDescription = readerModel.data(index, ReaderModel::UserRoles::READER_HTML_DESCRIPTION).toString();
			QVERIFY(htmlDescription.startsWith(tr("No driver installed")));
			QCOMPARE(readerModel.data(index, ReaderModel::UserRoles::READER_NAME).toString(), "DummyReader"_L1);
			QCOMPARE(readerModel.data(index, ReaderModel::UserRoles::READER_DRIVER_URL).toString(), "DummyUrl"_L1);
			QCOMPARE(readerModel.data(index, ReaderModel::UserRoles::READER_IMAGE_PATH).toString(), "qrc:///images/desktop/default_reader.png"_L1);
			QVERIFY(readerModel.data(index, ReaderModel::UserRoles::READER_SUPPORTED).toBool());
			QVERIFY(!readerModel.data(index, ReaderModel::UserRoles::READER_INSTALLED).toBool());
			QVERIFY(readerModel.data(index, ReaderModel::UserRoles::SHOW_STATUS_ICON).toBool());
		}


		void test_translation()
		{
			ReaderModel readerModel;
			QSignalSpy spy(&readerModel, &ReaderModel::fireModelChanged);
			readerModel.onTranslationChanged();
			QCOMPARE(spy.count(), 1);
		}


		void test_readerCountChanged_data()
		{
			QTest::addColumn<QList<ReaderInfo>>("initialReader");
			QTest::addColumn<QList<ReaderInfo>>("finalReader");

			const auto dummy1 = ReaderInfo("DummyReader1"_L1, ReaderManagerPluginType::PCSC);
			const auto dummy2 = ReaderInfo("DummyReader2"_L1, ReaderManagerPluginType::PCSC);

			QTest::addRow("One reader added") << QList<ReaderInfo>() << QList<ReaderInfo>({dummy1});
			QTest::addRow("Second reader added") << QList<ReaderInfo>({dummy1}) << QList<ReaderInfo>({dummy1, dummy2});
			QTest::addRow("Second reader added - different order") << QList<ReaderInfo>({dummy1}) << QList<ReaderInfo>({dummy2, dummy1});
			QTest::addRow("One reader removed") << QList<ReaderInfo>({dummy1}) << QList<ReaderInfo>();
			QTest::addRow("Second reader removed") << QList<ReaderInfo>({dummy1, dummy2}) << QList<ReaderInfo>({dummy1});
			QTest::addRow("Second reader removed - different order") << QList<ReaderInfo>({dummy2, dummy1}) << QList<ReaderInfo>({dummy1});
		}


		void test_readerCountChanged()
		{
			QFETCH(QList<ReaderInfo>, initialReader);
			QFETCH(QList<ReaderInfo>, finalReader);

			mReaderInfos = initialReader;
			mMockReaderManager.setIsScanRunning(true);

			ReaderModel readerModel;
			const int initialCount = std::max(1, static_cast<int>(initialReader.count()));
			QCOMPARE(readerModel.rowCount(), initialCount);

			mReaderInfos = finalReader;
			const int finalCount = std::max(1, static_cast<int>(finalReader.count()));
			QSignalSpy spyNotify(&readerModel, finalCount > initialCount ? &QAbstractListModel::rowsAboutToBeInserted : &QAbstractListModel::rowsAboutToBeRemoved);
			QSignalSpy spyResult(&readerModel, finalCount > initialCount ? &QAbstractListModel::rowsInserted : &QAbstractListModel::rowsRemoved);
			QSignalSpy spyDataChanged(&readerModel, &QAbstractListModel::dataChanged);

			readerModel.onUpdateContent();
			QCOMPARE(readerModel.rowCount(), finalCount);
			QCOMPARE(spyNotify.count(), finalCount != initialCount ? 1 : 0);
			QCOMPARE(spyResult.count(), finalCount != initialCount ? 1 : 0);
			QCOMPARE(spyDataChanged.count(), 1);
		}


		void test_hasConnectedReader()
		{
			ReaderModel readerModel;
			QCOMPARE(readerModel.rowCount(), 1);
			QVERIFY(!readerModel.hasConnectedReader());

			mReaderInfos += ReaderInfo("DummyReader1"_L1, ReaderManagerPluginType::PCSC);
			readerModel.onUpdateContent();
			QCOMPARE(readerModel.rowCount(), 1);
			QVERIFY(readerModel.hasConnectedReader());
		}


		void test_handleDummyReaderInfo()
		{
			ReaderModel readerModel;
			QCOMPARE(readerModel.rowCount(), 1);
			const QModelIndex index = readerModel.index(0);
			QCOMPARE(readerModel.data(index, ReaderModel::UserRoles::READER_NAME).toString(), "No card reader connected"_L1);
			QCOMPARE(readerModel.data(index, ReaderModel::UserRoles::READER_IMAGE_PATH).toString(), "qrc:///images/desktop/default_reader.png"_L1);
			QCOMPARE(readerModel.data(index, ReaderModel::UserRoles::READER_DRIVER_URL).toString(), ""_L1);
			QCOMPARE(readerModel.data(index, ReaderModel::UserRoles::READER_HTML_DESCRIPTION).toString(), ""_L1);
			QVERIFY(!readerModel.data(index, ReaderModel::UserRoles::READER_SUPPORTED).toBool());
			QVERIFY(!readerModel.data(index, ReaderModel::UserRoles::READER_INSTALLED).toBool());
			QVERIFY(!readerModel.data(index, ReaderModel::UserRoles::SHOW_STATUS_ICON).toBool());
		}


};


QTEST_GUILESS_MAIN(test_ReaderModel)
#include "test_ReaderModel.moc"
