/*!
 * \brief Unit tests for \ref AppUpdateData
 *
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

#include "AppUpdateData.h"

#include <QtTest>

using namespace governikus;

class test_AppUpdatData
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testChecksumValid()
		{
			AppUpdateData data;
			QVERIFY(!data.isChecksumValid());
			data.setChecksum(QByteArray("broken"), QCryptographicHash::Sha1);
			QVERIFY(!data.isChecksumValid());

			const QString dummyFile(":/card/efCardAccess.hex");
			QTest::ignoreMessage(QtDebugMsg, "Verify checksum with algorithm: QCryptographicHash::Sha1");
			data.setUpdatePackagePath(dummyFile);
			QVERIFY(!data.isChecksumValid());

			QByteArray hash("5084c7186a2d52d16d52c6c9d809918451d872f3eb3c4452f739938012b1331f9b78e9212d7e4abb947c059b2d8b141fb694dcc130349167e061bb8a9132af9e");
			hash += QByteArray("  ");
			hash += QFileInfo(dummyFile).fileName().toLatin1();

			QTest::ignoreMessage(QtDebugMsg, "Verify checksum with algorithm: QCryptographicHash::Sha1");
			data.setChecksum(hash, QCryptographicHash::Sha1);
			QVERIFY(!data.isChecksumValid());

			QTest::ignoreMessage(QtDebugMsg, "Verify checksum with algorithm: QCryptographicHash::Sha512");
			data.setChecksum(hash, QCryptographicHash::Sha512);
			QVERIFY(data.isChecksumValid());
			data.setChecksum(hash, QCryptographicHash::Sha512);
			QVERIFY(data.isChecksumValid());

			data.setChecksum(QByteArray(), QCryptographicHash::Sha512);
			QVERIFY(!data.isChecksumValid());
		}


		void test_default()
		{
			AppUpdateData data;
			QCOMPARE(data.isValid(), false);
			QCOMPARE(data.getParsingResult(), GlobalStatus::Code::No_Error);
		}


		void test_error()
		{
			AppUpdateData data(GlobalStatus::Code::Downloader_Data_Corrupted);
			QCOMPARE(data.isValid(), false);
			QCOMPARE(data.getParsingResult(), GlobalStatus::Code::Downloader_Data_Corrupted);
		}


		void test_notes()
		{
			AppUpdateData data;
			QCOMPARE(data.getNotes(), QString());

			data.setNotes(QStringLiteral("<head><body>Release Notes</head></body>"));
			QCOMPARE(data.getNotes(), QStringLiteral("<head><body>Release Notes</head></body>"));
			QCOMPARE(data.isValid(), false);
		}


		void test_parsing()
		{
			const QByteArray jsonData(R"({)"
									  R"(	"items":)"
									  R"(	[)"
									  R"(		{)"
									  R"(			"date": "2017-10-25T15:20:25",)"
									  R"(			"platform": "mac",)"
									  R"(			"minimum_platform": "0.0",)"
									  R"(			"version": "1.22.0",)"
									  R"(			"url": "https://www.example.com/aa2.zip",)"
									  R"(			"size": 1337,)"
									  R"(			"checksum": "https://www.example.com/aa2.zip.sha256",)"
									  R"(			"notes": "https://www.example.com/ReleaseNotes.html#1.22.0")"
									  R"(		})"
									  R"(,		{)"
									  R"(			"date": "2017-10-25T15:20:25",)"
									  R"(			"platform": "win",)"
									  R"(			"minimum_platform": "0.0",)"
									  R"(			"version": "1.22.0",)"
									  R"(			"url": "https://www.example.com/aa2.zip",)"
									  R"(			"size": 1337,)"
									  R"(			"checksum": "https://www.example.com/aa2.zip.sha256",)"
									  R"(			"notes": "https://www.example.com/ReleaseNotes.html#1.22.0")"
									  R"(		})"
									  R"(,		{)"
									  R"(			"date": "2017-10-25T15:20:25",)"
									  R"(			"platform": "src",)"
									  R"(			"minimum_platform": "0.0",)"
									  R"(			"version": "1.22.0",)"
									  R"(			"url": "https://www.example.com/aa2.zip",)"
									  R"(			"size": 1337,)"
									  R"(			"checksum": "https://www.example.com/aa2.zip.sha256",)"
									  R"(			"notes": "https://www.example.com/ReleaseNotes.html#1.22.0")"
									  R"(		})"
									  R"(	])"
									  R"(})");
			AppUpdateData data(jsonData);
			QCOMPARE(data.isValid(), true);
			QCOMPARE(data.getParsingResult(), GlobalStatus::Code::No_Error);

			QCOMPARE(data.isCompatible(), true);
			QCOMPARE(data.getDate(), QDateTime(QDate(2017, 10, 25), QTime(15, 20, 25)));
			QCOMPARE(data.getVersion(), QStringLiteral("1.22.0"));
			QCOMPARE(data.getUrl(), QUrl(QStringLiteral("https://www.example.com/aa2.zip")));
			QCOMPARE(data.getSize(), 1337);
			QCOMPARE(data.getChecksumUrl(), QUrl(QStringLiteral("https://www.example.com/aa2.zip.sha256")));
			QCOMPARE(data.getNotesUrl(), QUrl(QStringLiteral("https://www.example.com/ReleaseNotes.html#1.22.0")));
			QCOMPARE(data.getNotes(), QString());
		}


		void test_incompatible()
		{
			const QByteArray jsonData(R"({)"
									  R"(	"items":)"
									  R"(	[)"
									  R"(		{)"
									  R"(			"date": "2017-10-25T15:20:25",)"
									  R"(			"platform": "mac",)"
									  R"(			"minimum_platform": "999.0",)"
									  R"(			"version": "1.22.0",)"
									  R"(			"url": "https://www.example.com/aa2.zip",)"
									  R"(			"size": 1337,)"
									  R"(			"checksum": "https://www.example.com/aa2.zip.sha256",)"
									  R"(			"notes": "https://www.example.com/ReleaseNotes.html#1.22.0")"
									  R"(		})"
									  R"(,		{)"
									  R"(			"date": "2017-10-25T15:20:25",)"
									  R"(			"platform": "win",)"
									  R"(			"minimum_platform": "999.0",)"
									  R"(			"version": "1.22.0",)"
									  R"(			"url": "https://www.example.com/aa2.zip",)"
									  R"(			"size": 1337,)"
									  R"(			"checksum": "https://www.example.com/aa2.zip.sha256",)"
									  R"(			"notes": "https://www.example.com/ReleaseNotes.html#1.22.0")"
									  R"(		})"
									  R"(,		{)"
									  R"(			"date": "2017-10-25T15:20:25",)"
									  R"(			"platform": "src",)"
									  R"(			"minimum_platform": "999.0",)"
									  R"(			"version": "1.22.0",)"
									  R"(			"url": "https://www.example.com/aa2.zip",)"
									  R"(			"size": 1337,)"
									  R"(			"checksum": "https://www.example.com/aa2.zip.sha256",)"
									  R"(			"notes": "https://www.example.com/ReleaseNotes.html#1.22.0")"
									  R"(		})"
									  R"(	])"
									  R"(})");
			AppUpdateData data(jsonData);
			QCOMPARE(data.isValid(), true);
			QCOMPARE(data.getParsingResult(), GlobalStatus::Code::No_Error);

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
			QCOMPARE(data.isCompatible(), false);
#else
			QCOMPARE(data.isCompatible(), true);
#endif
		}


		void test_invalid_data()
		{
			QTest::addColumn<QByteArray>("jsonData");
			QTest::addColumn<GlobalStatus::Code>("code");

			QTest::newRow("corrupted1") << QByteArray("") << GlobalStatus::Code::Downloader_Data_Corrupted;
			QTest::newRow("corrupted2") << QByteArray("{") << GlobalStatus::Code::Downloader_Data_Corrupted;
			QTest::newRow("corrupted3") << QByteArray("{}") << GlobalStatus::Code::Downloader_Data_Corrupted;
			QTest::newRow("corrupted4") << QByteArray(R"({)"
													  R"(	"date": "2017-10-25T15:20:25",)"
													  R"(	"platform": "mac",)"
													  R"(	"version": "1.22.0",)"
													  R"(	"url": "https://www.example.com/aa2.zip",)"
													  R"(	"size": 1337,)"
													  R"(	"checksum": "https://www.example.com/aa2.zip.sha256",)"
													  R"(	"notes": "https://www.example.com/ReleaseNotes.html#1.22.0")"
													  R"(})") << GlobalStatus::Code::Downloader_Data_Corrupted;
			QTest::newRow("corrupted5") << QByteArray(R"({)"
													  R"(	"items":)"
													  R"(	{)"
													  R"(		"date": "2017-10-25T15:20:25",)"
													  R"(		"platform": "mac",)"
													  R"(		"version": "1.22.0",)"
													  R"(		"url": "https://www.example.com/aa2.zip",)"
													  R"(		"size": 1337,)"
													  R"(		"checksum": "https://www.example.com/aa2.zip.sha256",)"
													  R"(		"notes": "https://www.example.com/ReleaseNotes.html#1.22.0")"
													  R"(	})"
													  R"(})") << GlobalStatus::Code::Downloader_Data_Corrupted;
			QTest::newRow("corrupted6") << QByteArray(R"({)"
													  R"(	"items":)"
													  R"(	[)"
													  R"(	])"
													  R"(})") << GlobalStatus::Code::Downloader_Missing_Platform;
			QTest::newRow("corrupted7") << QByteArray(R"({)"
													  R"(	"items":)"
													  R"(	[)"
													  R"(		{)"
													  R"(			"date": "2017-10-25T15:20:25",)"
													  R"(			"platform": "foo",)"
													  R"(			"version": "1.22.0",)"
													  R"(			"url": "https://www.example.com/aa2.zip",)"
													  R"(			"size": 1337,)"
													  R"(			"checksum": "https://www.example.com/aa2.zip.sha256",)"
													  R"(			"notes": "https://www.example.com/ReleaseNotes.html#1.22.0")"
													  R"(		})"
													  R"(	])"
													  R"(})") << GlobalStatus::Code::Downloader_Missing_Platform;
		}


		void test_invalid()
		{
			QFETCH(QByteArray, jsonData);
			QFETCH(GlobalStatus::Code, code);

			AppUpdateData data(jsonData);
			QCOMPARE(data.isValid(), false);
			QCOMPARE(data.getParsingResult(), code);
		}


};

QTEST_GUILESS_MAIN(test_AppUpdatData)
#include "test_AppUpdatData.moc"
