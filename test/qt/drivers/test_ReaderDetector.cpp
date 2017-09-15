/*!
 * \brief Unit tests for \ref ReaderDetector
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "DriverParser.h"
#include "FileDestination.h"
#include "ReaderDetector.h"
#include "TestFileHelper.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>

using namespace governikus;


class ReaderDetectorMock
	: public ReaderDetector
{
	Q_OBJECT

	public:
		ReaderDetectorMock(const QVector<QPair<uint, uint> >& pDevIds);

		virtual ~ReaderDetectorMock();

		QVector<QPair<uint, uint> > attachedDevIds() const override;

	private:
		const QVector<QPair<uint, uint> > mDevIds;

		QSharedPointer<DriverSettings> loadTestDrivers();
};


ReaderDetectorMock::ReaderDetectorMock(const QVector<QPair<uint, uint> >& pDevIds)
	: ReaderDetector(loadTestDrivers()), mDevIds(pDevIds)
{
}


ReaderDetectorMock::~ReaderDetectorMock()
{
}


QVector<QPair<uint, uint> > ReaderDetectorMock::attachedDevIds() const
{
	return mDevIds;
}


QSharedPointer<DriverSettings> ReaderDetectorMock::loadTestDrivers()
{
	QByteArray data = TestFileHelper::readFile(FileDestination::getPath("default-supported-devices.json"));

	DriverParser parser;
	return parser.parse(data);
}


namespace
{
#if defined(Q_OS_WIN)
static const QString KOMFORT_DRIVER_URL("https://appl.governikus-asp.de/ausweisapp2/driver/bc_7_2_3.exe");
#elif defined(Q_OS_OSX)
static const QString KOMFORT_DRIVER_URL = QSysInfo::MacintoshVersion >= QSysInfo::MV_10_11 ?
		QStringLiteral("https://appl.governikus-asp.de/ausweisapp2/driver/01_pcsc-cyberjack_3.99.5final.SP08-universal-osx10.11-signed.pkg") :
		QStringLiteral("https://appl.governikus-asp.de/ausweisapp2/driver/pcsc-cyberjack_3.99.5final.SP07-universal-signed.pkg");
#elif defined(Q_OS_LINUX)
static const QString KOMFORT_DRIVER_URL("https://www.reiner-sct.com/support/download/treiber-und-software/cyberjack/rfid-komfort-linux.html");
#else
static const QString KOMFORT_DRIVER_URL("SHOULD NOT BE TESTING THIS");
#endif
}


class test_ReaderDetector
	: public QObject
{
	Q_OBJECT

	private:
		void verify_REINER_cyberJack_RFID_komfort(const QSharedPointer<DeviceDescriptor>& dev)
		{
			QCOMPARE(dev->getReaderType(), ReaderType::REINER_cyberJack_RFID_komfort);
			QCOMPARE(dev->getVendorId(), static_cast<uint>(0x0C4B));
			QCOMPARE(dev->getProductId(), static_cast<uint>(0x0501));
			QCOMPARE(dev->getName(), QStringLiteral("REINER SCT cyberJack RFID komfort"));
			QCOMPARE(dev->getDriverUrl(), KOMFORT_DRIVER_URL);
		}


		void verify_REINER_cyberJack_RFID_komfort(const QSharedPointer<const Driver>& driver)
		{
			QCOMPARE(driver->getReaderType(), QStringLiteral("REINER_cyberJack_RFID_komfort"));
			QCOMPARE(driver->getVendorId(), static_cast<uint>(0x0C4B));
			QCOMPARE(driver->getProductId(), static_cast<uint>(0x0501));
			QCOMPARE(driver->getName(), QStringLiteral("REINER SCT cyberJack RFID komfort"));
			QCOMPARE(driver->getUrl(), KOMFORT_DRIVER_URL);
		}


		QStringList incompleteKeyValuePairs(int skipIndex, bool skipOnlyValue)
		{
			const QStringList keys = {
				"ReaderType", "VendorId", "ProductId", "Name", "Drivers"
			};
			const QStringList vals = {
				"REINER_cyberJack_RFID_komfort", "0x0C4B", "0x0501",
				"REINER SCT cyberJack RFID komfort",
				"        [\n"
				"          {\n"
				"              \"Platforms\": [\"WV_WINDOWS7\", \"WV_WINDOWS8\", \"WV_WINDOWS8_1\", \"WV_WINDOWS10\"],\n"
				"              \"URL\": \"https://appl.governikus-asp.de/ausweisapp2/driver/bc_7_2_3.exe\"\n"
				"          },\n"
				"          {\n"
				"              \"Platforms\": [\"MV_10_9\", \"MV_10_10\"],\n"
				"              \"URL\": \"https://appl.governikus-asp.de/ausweisapp2/driver/pcsc-cyberjack_3.99.5final.SP07-universal-signed.pkg\"\n"
				"          },\n"
				"          {\n"
				"              \"Platforms\": [\"MV_10_11\", \"MV_10_12\"],\n"
				"              \"URL\": \"https://appl.governikus-asp.de/ausweisapp2/driver/01_pcsc-cyberjack_3.99.5final.SP08-universal-osx10.11-signed.pkg\"\n"
				"          },\n"
				"          {\n"
				"              \"Platforms\": [\"LINUX\"],\n"
				"              \"URL\": \"https://www.reiner-sct.com/support/download/treiber-und-software/cyberjack/rfid-komfort-linux.html\"\n"
				"          }\n"
				"        ]\n"
			};
			QStringList result;
			for (int index = 0; index < keys.size(); index++)
			{
				if (index != skipIndex || skipOnlyValue)
				{
					const QString key = keys.at(index);
					const QString val = index != skipIndex ? vals.at(index) : QString();
					result += QString("\"%1\": \"%2\"\n").arg(key, val);
				}
			}

			return result;
		}


		QString jsonSingleDeviceString(const QStringList& keyValuePairs)
		{
			const QString dataPrefix = QStringLiteral("\"SupportedDevices\": [ { ");
			const QString dataSuffix = QStringLiteral(" } ]");

			return dataPrefix + keyValuePairs.join(QStringLiteral(",\n")) + dataSuffix;
		}


		QString jsonDataString(const QString& pDateIssued, const QString& pDevices)
		{
			return QStringLiteral("{ %1, %2\n}").arg(pDateIssued, pDevices);
		}


	private Q_SLOTS:
		void initTestCase()
		{
#if !defined(Q_OS_WIN) && !defined(Q_OS_OSX) && !defined(Q_OS_LINUX)
			QSKIP("Test only relevant on Windows, MacOsX and Linux platforms");
#endif
		}


		void noAttachedDeviceWithVIDAndPID_noAttachedDevicesFound()
		{
			ReaderDetectorMock readerDetector({});

			QCOMPARE(readerDetector.getAttachedDevices({}).size(), 0);
		}


		void nonSupportedDeviceAttached_noAttachedDevicesFound()
		{
			ReaderDetectorMock readerDetector({
						{0x413C, 0x2107}
					});

			QCOMPARE(readerDetector.getAttachedDevices({}).size(), 0);
		}


		void supportedDeviceAttachedButNoDriverInstalled_attachedDeviceFound()
		{
			ReaderDetectorMock readerDetector({
						{0x0C4B, 0x0501}
					});
			const auto devs = readerDetector.getAttachedDevices({});

			QCOMPARE(devs.size(), 1);
			QCOMPARE(devs.first()->hasDriver(), false);
			verify_REINER_cyberJack_RFID_komfort(devs.first());
		}


		void supportedDeviceAttachedAndDriverInstalled_attachedDeviceFound()
		{
			static const QString READER_NAME("REINER SCT cyberJack RFID komfort");
			static const ReaderType READER_TYPE = ReaderType::REINER_cyberJack_RFID_komfort;
			static const ReaderManagerPlugInType PLUGIN_TYPE = ReaderManagerPlugInType::UNKNOWN;

			ReaderDetectorMock readerDetector({
						{0x0C4B, 0x0501}
					});

			const auto devs = readerDetector.getAttachedDevices({ReaderInfo(PLUGIN_TYPE, READER_NAME, READER_TYPE)});

			QCOMPARE(devs.size(), 1);
			QCOMPARE(devs.first()->hasDriver(), true);
			verify_REINER_cyberJack_RFID_komfort(devs.first());
		}


		void invalidJsonDocument_parseReturnsNullDeviceInfo()
		{
			const QByteArray data = QStringLiteral("INVALID JSON INPUT").toUtf8();

			QVERIFY(DriverParser().parse(data).isNull());
		}


		void validJsonDocumentWithoutIssueDate_parseReturnsNullDeviceInfo()
		{
			const QByteArray data = QStringLiteral("{ \"SupportedDevices\": [] }").toUtf8();

			QVERIFY(DriverParser().parse(data).isNull());
		}


		void validJsonDocumentWithoutSupportedDevices_parseReturnsNullDeviceInfo()
		{
			const QByteArray data = QStringLiteral("{ \"IssueDate\": \"2015-11-03T12:00:00+1:00\" }").toUtf8();

			QVERIFY(DriverParser().parse(data).isNull());
		}


		void validJsonDocumentWithInvalidIssueDate_parseReturnsNullDeviceInfo()
		{
			const QString issueDatePair("\"IssueDate\": \"\"");
			const QString devicesPair("\"SupportedDevices\": []");
			const QByteArray data = jsonDataString(issueDatePair, devicesPair).toUtf8();

			QVERIFY(DriverParser().parse(data).isNull());
		}


		void validJsonDocumentWithInvalidSupportedDevices_parseReturnsNullDeviceInfo()
		{
			const QString issueDatePair("\"IssueDate\": \"2015-11-03T12:00:00+1:00\"");
			const QString devicesPair("\"SupportedDevices\": \"\"");
			const QByteArray data = QStringLiteral("{ %1, %2 }").arg(issueDatePair, devicesPair).toUtf8();

			QVERIFY(DriverParser().parse(data).isNull());
		}


		void validEmptyJsonData_parseReturnsValidEmptyDriverInfo()
		{
			const QString dateKey("IssueDate");
			const QString dateVal("2015-11-03T12:00:00+1:00");
			const QString issueDatePair = QStringLiteral("\"%1\": \"%2\"").arg(dateKey, dateVal);
			const QString devicesPair("\"SupportedDevices\": []");
			const QByteArray data = jsonDataString(issueDatePair, devicesPair).toUtf8();

			DriverParser parser;
			const auto settings = parser.parse(data);

			QVERIFY(!settings.isNull());
			QCOMPARE(settings->getIssueDate(), QDateTime::fromString(dateVal, Qt::ISODate));
			QCOMPARE(settings->getDrivers().size(), 0);
		}


		void validJsonDocumentWithOneValidEntry_parseOkAndOneCorrectDeviceInfo()
		{
			const QByteArray data = QStringLiteral("{"
												   "  \"IssueDate\": \"2015-11-03T12:00:00+1:00\","
												   "  \"SupportedDevices\":\n"
												   "  [\n"
												   "    {\n"
												   "        \"ReaderType\": \"REINER_cyberJack_RFID_komfort\",\n"
												   "        \"VendorId\": \"0x0C4B\",\n"
												   "        \"ProductId\": \"0x0501\",\n"
												   "        \"Name\": \"REINER SCT cyberJack RFID komfort\",\n"
												   "        \"Drivers\":\n"
												   "        [\n"
												   "          {\n"
												   "              \"Platforms\": [\"WV_WINDOWS7\", \"WV_WINDOWS8\", \"WV_WINDOWS8_1\", \"WV_WINDOWS10\"],\n"
												   "              \"URL\": \"https://appl.governikus-asp.de/ausweisapp2/driver/bc_7_2_3.exe\"\n"
												   "          },\n"
												   "          {\n"
												   "              \"Platforms\": [\"MV_10_9\", \"MV_10_10\"],\n"
												   "              \"URL\": \"https://appl.governikus-asp.de/ausweisapp2/driver/pcsc-cyberjack_3.99.5final.SP07-universal-signed.pkg\"\n"
												   "          },\n"
												   "          {\n"
												   "              \"Platforms\": [\"MV_10_11\", \"MV_10_12\"],\n"
												   "              \"URL\": \"https://appl.governikus-asp.de/ausweisapp2/driver/01_pcsc-cyberjack_3.99.5final.SP08-universal-osx10.11-signed.pkg\"\n"
												   "          },\n"
												   "          {\n"
												   "              \"Platforms\": [\"LINUX\"],\n"
												   "              \"URL\": \"https://www.reiner-sct.com/support/download/treiber-und-software/cyberjack/rfid-komfort-linux.html\"\n"
												   "          }\n"
												   "        ]\n"
												   "    }\n"
												   "  ]\n"
												   "}").toUtf8();

			DriverParser parser;
			const auto settings = parser.parse(data);

			QVERIFY(!settings.isNull());
			QCOMPARE(settings->getDrivers().size(), 1);
			verify_REINER_cyberJack_RFID_komfort(settings->getDrivers().first());
		}


		void validJsonDocumentWithOneInvalidEntry_parseErrorAndEmptyDeviceInfo()
		{
			const QString issueDatePair("\"IssueDate\": \"2015-11-03T12:00:00+1:00\"");

			// Check that a missing property is reported as a parse error.
			for (int skipIndex = 0; skipIndex < 5; skipIndex++)
			{
				// Key with index skipIndex is not in the test Json data.
				const QString devicesPair = jsonSingleDeviceString(incompleteKeyValuePairs(skipIndex,
								/* skipOnlyValue */false));
				const QByteArray data = jsonDataString(issueDatePair, devicesPair).toUtf8();

				QVERIFY(DriverParser().parse(data).isNull());
			}

			// Check that a property with an empty value is reported as a parse error.
			for (int skipIndex = 0; skipIndex < 5; skipIndex++)
			{
				// Key with index skipIndex is in the test Json data but has the invalid value ""
				const QString devicesPair = jsonSingleDeviceString(incompleteKeyValuePairs(skipIndex,
								/* skipOnlyValue */true));
				const QByteArray data = jsonDataString(issueDatePair, devicesPair).toUtf8();

				QVERIFY(DriverParser().parse(data).isNull());
			}
		}


};

QTEST_GUILESS_MAIN(test_ReaderDetector)
#include "test_ReaderDetector.moc"
