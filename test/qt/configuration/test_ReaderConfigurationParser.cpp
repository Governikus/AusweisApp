/*!
 * \brief Unit tests for \ref ReaderDetector
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderConfigurationParser.h"

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


class test_ReaderConfigurationParser
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
				"              \"Platforms\": [{\"os\": \"win\"}],\n"
				"              \"URL\": \"https://www.reiner-sct.com/support/support-anfrage/?os=Windows&productGroup=77304735&product=77304822&q=driver#choice5\"\n"
				"          },\n"
				"          {\n"
				"              \"Platforms\": [{\"os\": \"mac\", \"max\": \"10.10\"}],\n"
				"              \"URL\": \"https://www.reiner-sct.com/support/support-anfrage/?os=MacOS&productGroup=77304735&product=77304822&q=driver#choice5\"\n"
				"          },\n"
				"          {\n"
				"              \"Platforms\": [{\"os\": \"mac\", \"min\": \"10.11\"}],\n"
				"              \"URL\": \"https://www.reiner-sct.com/support/support-anfrage/?os=MacOS&productGroup=77304735&product=77304822&q=driver#choice5\"\n"
				"          },\n"
				"          {\n"
				"              \"Platforms\": [{\"os\": \"unknown\"}],\n"
				"              \"URL\": \"https://www.reiner-sct.com/support/support-anfrage/?os=Linux&productGroup=77304735&product=77304822&q=driver#choice5\"\n"
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


		QString jsonDataString(const QString& pDevices)
		{
			return QStringLiteral("{ %1\n}").arg(pDevices);
		}


	private Q_SLOTS:
		void invalidJsonDocument_parseReturnsNullDeviceInfo()
		{
			const QByteArray data = QByteArrayLiteral("INVALID JSON INPUT");

			QCOMPARE(ReaderConfigurationParser::parse(data).size(), 0);
		}


		void validJsonDocument_parseReturnsNullDeviceInfo()
		{
			const QByteArray data = QByteArrayLiteral("{ \"SupportedDevices\": [] }");

			QCOMPARE(ReaderConfigurationParser::parse(data).size(), 0);
		}


		void validJsonDocumentWithInvalidSupportedDevices_parseReturnsNullDeviceInfo()
		{
			const QByteArray data = QByteArrayLiteral("{ \"SupportedDevices\": \"\" }");

			QCOMPARE(ReaderConfigurationParser::parse(data).size(), 0);
		}


		void validJsonDocumentWithOneValidEntry_parseOkAndOneCorrectDeviceInfo()
		{
			const QByteArray data = QByteArrayLiteral("{"
													  "  \"SupportedDevices\":\n"
													  "  [\n"
													  "    {\n"
													  "        \"ReaderType\": \"REINER_cyberJack_RFID_komfort\",\n"
													  "        \"VendorId\": \"0x0C4B\",\n"
													  "        \"ProductId\": \"0x0501\",\n"
													  "        \"Name\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Pattern\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "          {\n"
													  "              \"Platforms\": [{\"os\": \"win\"}],\n"
													  "              \"URL\": \"https://www.reiner-sct.com/support/support-anfrage/?os=Windows&productGroup=77304735&product=77304822&q=driver#choice5\"\n"
													  "          },\n"
													  "          {\n"
													  "              \"Platforms\": [{\"os\": \"mac\", \"max\": \"10.10\"}],\n"
													  "              \"URL\": \"https://www.reiner-sct.com/support/support-anfrage/?os=MacOS&productGroup=77304735&product=77304822&q=driver#choice5\"\n"
													  "          },\n"
													  "          {\n"
													  "              \"Platforms\": [{\"os\": \"mac\", \"min\": \"10.11\"}],\n"
													  "              \"URL\": \"https://www.reiner-sct.com/support/support-anfrage/?os=MacOS&productGroup=77304735&product=77304822&q=driver#choice5\"\n"
													  "          },\n"
													  "          {\n"
													  "              \"Platforms\": [{\"os\": \"unknown\"}],\n"
													  "              \"URL\": \"https://www.reiner-sct.com/support/support-anfrage/?os=Linux&productGroup=77304735&product=77304822&q=driver#choice5\"\n"
													  "          }\n"
													  "        ]\n"
													  "    }\n"
													  "  ]\n"
													  "}");

			const auto configuration = ReaderConfigurationParser::parse(data);

			QCOMPARE(configuration.size(), 1);
			verify_REINER_cyberJack_RFID_komfort(configuration.first());
		}


		void validJsonDocumentWithOneInvalidEntry_parseErrorAndEmptyDeviceInfo()
		{
			// Check that a missing property is reported as a parse error.
			for (int skipIndex = 0; skipIndex < 5; skipIndex++)
			{
				// Key with index skipIndex is not in the test Json data.
				const QString devicesPair = jsonSingleDeviceString(incompleteKeyValuePairs(skipIndex,
						/* skipOnlyValue */ false));
				const QByteArray data = jsonDataString(devicesPair).toUtf8();

				QCOMPARE(ReaderConfigurationParser::parse(data).size(), 0);
			}

			// Check that a property with an empty value is reported as a parse error.
			for (int skipIndex = 0; skipIndex < 5; skipIndex++)
			{
				// Key with index skipIndex is in the test Json data but has the invalid value ""
				const QString devicesPair = jsonSingleDeviceString(incompleteKeyValuePairs(skipIndex,
						/* skipOnlyValue */ true));
				const QByteArray data = jsonDataString(devicesPair).toUtf8();

				QCOMPARE(ReaderConfigurationParser::parse(data).size(), 0);
			}
		}


		void parserAcceptsJSONWithValidReaderIdentification()
		{
			const QByteArray data = QByteArrayLiteral("{"
													  "  \"SupportedDevices\":\n"
													  "  [\n"
													  "    {\n"
													  "        \"ReaderType\": \"REINER_cyberJack_RFID_komfort\",\n"
													  "        \"VendorId\": \"0x0C4B\",\n"
													  "        \"ProductId\": \"0x0501\",\n"
													  "        \"Name\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Pattern\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "        ]\n"
													  "    },\n"
													  "    {\n"
													  "        \"ReaderType\": \"SCM_SDI011\",\n"
													  "        \"VendorId\": \"0x04E6\",\n"
													  "        \"ProductId\": \"0x512B\",\n"
													  "        \"Name\": \"SDI011 Contactless Reader\",\n"
													  "        \"Pattern\": \"SDI011 (USB )?(Smart Card|Contactless) Reader\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "        ]\n"
													  "    }\n"
													  "  ]\n"
													  "}");

			QCOMPARE(ReaderConfigurationParser::parse(data).size(), 2);
		}


		void parserAcceptsJSONWithMultipleEmptyPatternsInReadersWithDifferentNames()
		{
			const QByteArray data = QByteArrayLiteral("{"
													  "  \"SupportedDevices\":\n"
													  "  [\n"
													  "    {\n"
													  "        \"ReaderType\": \"REINER_cyberJack_RFID_komfort\",\n"
													  "        \"VendorId\": \"0x0C4B\",\n"
													  "        \"ProductId\": \"0x0501\",\n"
													  "        \"Name\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Pattern\": \"\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "        ]\n"
													  "    },\n"
													  "    {\n"
													  "        \"ReaderType\": \"SCM_SDI011\",\n"
													  "        \"VendorId\": \"0x04E6\",\n"
													  "        \"ProductId\": \"0x512B\",\n"
													  "        \"Name\": \"SDI011 Contactless Reader\",\n"
													  "        \"Pattern\": \"\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "        ]\n"
													  "    }\n"
													  "  ]\n"
													  "}");

			QCOMPARE(ReaderConfigurationParser::parse(data).size(), 2);
		}


		void parserRejectsJSONDataWithRepeatedUSBId()
		{
			const QByteArray data = QByteArrayLiteral("{"
													  "  \"SupportedDevices\":\n"
													  "  [\n"
													  "    {\n"
													  "        \"ReaderType\": \"REINER_cyberJack_RFID_komfort\",\n"
													  "        \"VendorId\": \"0x0C4B\",\n"
													  "        \"ProductId\": \"0x0501\",\n"
													  "        \"Name\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Pattern\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "        ]\n"
													  "    },\n"
													  "    {\n"
													  "        \"ReaderType\": \"SCM_SDI011\",\n"
													  "        \"VendorId\": \"0x0C4B\",\n"
													  "        \"ProductId\": \"0x0501\",\n"
													  "        \"Name\": \"SDI011 Contactless Reader\",\n"
													  "        \"Pattern\": \"SDI011 (USB )?(Smart Card|Contactless) Reader\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "        ]\n"
													  "    }\n"
													  "  ]\n"
													  "}");

			QCOMPARE(ReaderConfigurationParser::parse(data).size(), 0);
		}


		void parserRejectsJSONDataWithRepeatedReaderName()
		{
			const QByteArray data = QByteArrayLiteral("{"
													  "  \"SupportedDevices\":\n"
													  "  [\n"
													  "    {\n"
													  "        \"ReaderType\": \"REINER_cyberJack_RFID_komfort\",\n"
													  "        \"VendorId\": \"0x0C4B\",\n"
													  "        \"ProductId\": \"0x0501\",\n"
													  "        \"Name\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Pattern\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "        ]\n"
													  "    },\n"
													  "    {\n"
													  "        \"ReaderType\": \"SCM_SDI011\",\n"
													  "        \"VendorId\": \"0x04E6\",\n"
													  "        \"ProductId\": \"0x512B\",\n"
													  "        \"Name\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Pattern\": \"SDI011 (USB )?(Smart Card|Contactless) Reader\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "        ]\n"
													  "    }\n"
													  "  ]\n"
													  "}");

			QCOMPARE(ReaderConfigurationParser::parse(data).size(), 0);
		}


		void parserRejectsJSONDataWithRepeatedPattern()
		{
			const QByteArray data = QByteArrayLiteral("{"
													  "  \"SupportedDevices\":\n"
													  "  [\n"
													  "    {\n"
													  "        \"ReaderType\": \"REINER_cyberJack_RFID_komfort\",\n"
													  "        \"VendorId\": \"0x0C4B\",\n"
													  "        \"ProductId\": \"0x0501\",\n"
													  "        \"Name\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Pattern\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "        ]\n"
													  "    },\n"
													  "    {\n"
													  "        \"ReaderType\": \"SCM_SDI011\",\n"
													  "        \"VendorId\": \"0x04E6\",\n"
													  "        \"ProductId\": \"0x512B\",\n"
													  "        \"Name\": \"SDI011 Contactless Reader\",\n"
													  "        \"Pattern\": \"REINER SCT cyberJack RFID komfort\",\n"
													  "        \"Drivers\":\n"
													  "        [\n"
													  "        ]\n"
													  "    }\n"
													  "  ]\n"
													  "}");

			QCOMPARE(ReaderConfigurationParser::parse(data).size(), 0);
		}


};

QTEST_GUILESS_MAIN(test_ReaderConfigurationParser)
#include "test_ReaderConfigurationParser.moc"
