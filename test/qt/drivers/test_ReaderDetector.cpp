/*!
 * \brief Unit tests for \ref ReaderDetector
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDetector.h"

#include "MockReaderDetector.h"
#include "ResourceLoader.h"

#include <QtTest>

using namespace governikus;


namespace
{
#if defined(Q_OS_WIN)
const QLatin1String KOMFORT_DRIVER_URL("https://www.reiner-sct.com/support/support-anfrage/?os=Windows&productGroup=77304735&product=77304822&q=driver#choice5");
#elif defined(Q_OS_MACOS)
const QLatin1String KOMFORT_DRIVER_URL("https://www.reiner-sct.com/support/support-anfrage/?os=MacOS&productGroup=77304735&product=77304822&q=driver#choice5");
#else
const QLatin1String KOMFORT_DRIVER_URL("https://www.reiner-sct.com/support/support-anfrage/?os=Linux&productGroup=77304735&product=77304822&q=driver#choice5");
#endif
} // namespace


class test_ReaderDetector
	: public QObject
{
	Q_OBJECT

	private:
		void verify_REINER_cyberJack_RFID_komfort(const ReaderConfigurationInfo& info)
		{
			QCOMPARE(info.getVendorId(), static_cast<uint>(0x0C4B));
			QCOMPARE(info.getProductId(), static_cast<uint>(0x0501));
			QCOMPARE(info.getName(), QStringLiteral("REINER SCT cyberJack RFID komfort"));
			QCOMPARE(info.getPattern(), QStringLiteral("REINER SCT cyberJack RFID komfort"));
			QCOMPARE(info.getUrl(), KOMFORT_DRIVER_URL);
		}


	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void noAttachedDeviceWithVIDAndPID_noAttachedDevicesFound()
		{
			const QVector<UsbId> pDevIds({});
			MockReaderDetector readerDetector(pDevIds);

			QCOMPARE(readerDetector.getAttachedSupportedDevices().size(), 0);
		}


		void nonSupportedDeviceAttached_noAttachedDevicesFound()
		{
			const QVector<UsbId> pDevIds({UsbId(0x413C, 0x2107)});
			MockReaderDetector readerDetector(pDevIds);

			QCOMPARE(readerDetector.getAttachedSupportedDevices().size(), 0);
		}


		void supportedDeviceAttachedButNoDriverInstalled_attachedDeviceFound()
		{
			const QVector<UsbId> pDevIds({UsbId(0x0C4B, 0x0501)});
			MockReaderDetector readerDetector(pDevIds);
			const auto devs = readerDetector.getAttachedSupportedDevices();

			QCOMPARE(devs.size(), 1);
			verify_REINER_cyberJack_RFID_komfort(devs.first());
		}


};

QTEST_GUILESS_MAIN(test_ReaderDetector)
#include "test_ReaderDetector.moc"
