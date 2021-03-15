/*!
 * \brief Unit tests for \ref ReaderConfigurationParser
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderConfigurationParser.h"

#include <QtTest>

using namespace governikus;


class test_ReaderConfigurationEntryParser
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void matchPlatformCurrentSupported()
		{
			ReaderConfigurationParser::EntryParser parser(QJsonValue::Null);
			QByteArray content = R"(  [{"os": "win", "min": "6.1"}, {"os": "mac", "min": "10.9"}, {"os": "unknown"}]  )";
			const auto& doc = QJsonDocument::fromJson(content);
			QVERIFY(!doc.isNull());
			const auto& data = doc.array();

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion(QOperatingSystemVersion::Windows, 6, 0))); // Vista
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::Windows7));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::Windows8));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::Windows8_1));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::Windows10));

			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion(QOperatingSystemVersion::Unknown, 1))); // linux

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 9)));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 10, 0)));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 10, 8)));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::OSXMavericks));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::OSXYosemite));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::OSXElCapitan));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::MacOSSierra));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::MacOSHighSierra));
		}


		void matchPlatformOtherOS()
		{
			ReaderConfigurationParser::EntryParser parser(QJsonValue::Null);
			QByteArray content = R"(  [{"os": "win"}]  )";
			const auto& doc = QJsonDocument::fromJson(content);
			QVERIFY(!doc.isNull());
			const auto& data = doc.array();

			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion(QOperatingSystemVersion::Windows, 6, 0))); // Vista
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::Windows10));

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion(QOperatingSystemVersion::Unknown, 1))); // linux
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXMavericks));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::MacOSSierra));
		}


		void matchPlatformMinMax()
		{
			ReaderConfigurationParser::EntryParser parser(QJsonValue::Null);
			QByteArray content = R"(  [{"os": "mac", "min": "10.10", "max": "10.11"}]  )";
			const auto& doc = QJsonDocument::fromJson(content);
			QVERIFY(!doc.isNull());
			const auto& data = doc.array();

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::Windows10));

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXMavericks));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::OSXYosemite));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::OSXElCapitan));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::MacOSSierra));
		}


		void matchPlatformMultipleMinMax()
		{
			ReaderConfigurationParser::EntryParser parser(QJsonValue::Null);
			QByteArray content = R"(  [{"os": "mac", "min": "10.10", "max": "10.10"}, {"os": "mac", "min": "10.13", "max": "10.13"}]  )";
			const auto& doc = QJsonDocument::fromJson(content);
			QVERIFY(!doc.isNull());
			const auto& data = doc.array();

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::Windows10));

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXMavericks));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::OSXYosemite));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXElCapitan));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::MacOSSierra));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 10, 13)));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 10, 14)));
		}


		void matchPlatformNothing()
		{
			ReaderConfigurationParser::EntryParser parser(QJsonValue::Null);
			QByteArray content = R"(  [{"os": "typo"}, {"os": "typo", "min": "10.10", "max": "10.11"}]  )";
			const auto& doc = QJsonDocument::fromJson(content);
			QVERIFY(!doc.isNull());
			const auto& data = doc.array();

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion(QOperatingSystemVersion::Unknown, 1))); // linux
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::Windows10));

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXMavericks));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXYosemite));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXElCapitan));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::MacOSSierra));
		}


		void matchPlatformInsideVersion()
		{
			ReaderConfigurationParser::EntryParser parser(QJsonValue::Null);
			QByteArray content = R"(  [{"os": "typo"}, {"os": "mac", "min": "10.10", "max": "10.10"}]  )";
			const auto& doc = QJsonDocument::fromJson(content);
			QVERIFY(!doc.isNull());
			const auto& data = doc.array();

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXMavericks));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::OSXYosemite));
			auto macOs10_10_1 = QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 10, 10, 1);
			QVERIFY(parser.matchPlatform(data, macOs10_10_1));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXElCapitan));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::MacOSSierra));
		}


		void matchPlatformInsideVersionWithZero()
		{
			ReaderConfigurationParser::EntryParser parser(QJsonValue::Null);
			QByteArray content = R"(  [{"os": "typo"}, {"os": "mac", "min": "10.10", "max": "10.10.0"}]  )";
			const auto& doc = QJsonDocument::fromJson(content);
			QVERIFY(!doc.isNull());
			const auto& data = doc.array();

			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXMavericks));
			QVERIFY(parser.matchPlatform(data, QOperatingSystemVersion::OSXYosemite));
			auto macOs10_10_1 = QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 10, 10, 1);
			QVERIFY(!parser.matchPlatform(data, macOs10_10_1));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::OSXElCapitan));
			QVERIFY(!parser.matchPlatform(data, QOperatingSystemVersion::MacOSSierra));
		}


};

QTEST_GUILESS_MAIN(test_ReaderConfigurationEntryParser)
#include "test_ReaderConfigurationEntryParser.moc"
