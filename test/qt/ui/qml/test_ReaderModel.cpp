/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
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
		ReaderManagerPlugInInfo getPlugInInfo(ReaderManagerPlugInType pType) const override;
		QList<ReaderInfo> getReaderInfos(const ReaderFilter& pFilter = ReaderFilter()) const override;

		void setIsScanRunning(bool pRunning);
};


MockReaderManager::MockReaderManager(const QList<ReaderInfo>& pReaderInfos)
	: mReaderInfos(pReaderInfos)
	, mIsScanRunning(false)
{
}


ReaderManagerPlugInInfo MockReaderManager::getPlugInInfo(ReaderManagerPlugInType pType) const
{
	ReaderManagerPlugInInfo info(pType);
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
			QCOMPARE(readerModel.rowCount(), 0);
		}


		void test_settings()
		{
			ReaderModel readerModel;
			QCOMPARE(readerModel.rowCount(), 0);
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
			QCOMPARE(readerModel.rowCount(), 0);
		}


		void test_usbId_readerManager_different_scan_not_running()
		{
#if defined(Q_OS_FREEBSD)
			QSKIP("No driver available for UsbId(0x04E6, 0x5790) on FreeBSD");
#endif
			mUsbIds += UsbId(0x0C4B, 0x0501);    // REINER SCT cyberJack RFID komfort
			mUsbIds += UsbId(0x04E6, 0x5790);    // Identiv Cloud 3700 F
			mReaderInfos += ReaderInfo("Identiv CLOUD 3700 F Contactless Reader 0"_L1, ReaderManagerPlugInType::PCSC);

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
			mReaderInfos += ReaderInfo("Identiv CLOUD 3700 F Contactless Reader 0"_L1, ReaderManagerPlugInType::PCSC);

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
			mReaderInfos += ReaderInfo("Governikus Special Reader"_L1, ReaderManagerPlugInType::PCSC);

			ReaderModel readerModel;
			QCOMPARE(readerModel.rowCount(), 1);
		}


		void test_translation()
		{
			ReaderModel readerModel;
			QSignalSpy spy(&readerModel, &ReaderModel::fireModelChanged);
			readerModel.onTranslationChanged();
			QCOMPARE(spy.count(), 1);
		}


};


QTEST_GUILESS_MAIN(test_ReaderModel)
#include "test_ReaderModel.moc"
