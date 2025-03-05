/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AppUpdater.h"
#include "Downloader.h"

#include "Env.h"
#include "GlobalStatus.h"
#include "MockDownloader.h"
#include "VersionNumber.h"

#include <QDir>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QtTest>

namespace
{
const char* test_jsonData = "{"
							"	\"items\":"
							"	["
							"		{"
							"			\"date\": \"2017-10-25T15:20:25\","
							"			\"platform\": \"mac\","
							"			\"version\": \"1.13.3+284-default-a2f3eafc47e9\","
							"			\"url\": \"https://ausweisapp-ci.govkg.de/job/default_Appcast/lastSuccessfulBuild/artifact/build/AusweisApp2-1.13.3+284-default-a2f3eafc47e9.dmg\","
							"			\"size\": 16531848,"
							"			\"checksum\": \"https://ausweisapp-ci.govkg.de/job/default_Appcast/lastSuccessfulBuild/artifact/build/AusweisApp2-1.13.3+284-default-a2f3eafc47e9.dmg.sha256\","
							"			\"notes\": \"https://ausweisapp-ci.govkg.de/job/default_Appcast/lastSuccessfulBuild/artifact/build/ReleaseNotes.html#1.13.3+284-default-a2f3eafc47e9\""
							"		}"
							",		{"
							"			\"date\": \"2017-10-25T15:20:25\","
							"			\"platform\": \"win\","
							"			\"version\": \"1.13.3+285-default-8b1156a4acc5\","
							"			\"url\": \"https://ausweisapp-ci.govkg.de/job/default_Appcast/lastSuccessfulBuild/artifact/build/AusweisApp2-1.13.3+285-default-8b1156a4acc5.msi\","
							"			\"size\": 17633280,"
							"			\"checksum\": \"https://ausweisapp-ci.govkg.de/job/default_Appcast/lastSuccessfulBuild/artifact/build/AusweisApp2-1.13.3+285-default-8b1156a4acc5.msi.sha256\","
							"			\"notes\": \"https://ausweisapp-ci.govkg.de/job/default_Appcast/lastSuccessfulBuild/artifact/build/ReleaseNotes.html#1.13.3+285-default-8b1156a4acc5\""
							"		}"
							",		{"
							"			\"date\": \"2017-10-25T15:20:25\","
							"			\"platform\": \"src\","
							"			\"version\": \"1.13.3+277-default-54ea820b42b0.tar.gz\","
							"			\"url\": \"https://ausweisapp-ci.govkg.de/job/default_Appcast/lastSuccessfulBuild/artifact/build/AusweisApp2-1.13.3+277-default-54ea820b42b0.tar.gz\","
							"			\"size\": 8687552,"
							"			\"checksum\": \"https://ausweisapp-ci.govkg.de/job/default_Appcast/lastSuccessfulBuild/artifact/build/AusweisApp2-1.13.3+277-default-54ea820b42b0.tar.gz.sha256\","
							"			\"notes\": \"https://ausweisapp-ci.govkg.de/job/default_Appcast/lastSuccessfulBuild/artifact/build/ReleaseNotes.html#1.13.3+277-default-54ea820b42b0.tar.gz\""
							"		}"
							""
							"	]"
							"}";
const char* test_releaseNotes = "<head><body>Release Notes</head></body>";

const char* test_package = "BelaFarinRod\n";

const char* test_checksum = "85b1e5ba7c499ab30959febf8d9ea39d08c863617995d7dce20ddc42cd0eb291  ";

} // namespace

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_AppUpdatr
	: public QObject
{
	Q_OBJECT

	private:
		AppUpdater mAppUpdater;
		MockDownloader mDownloader;
		QUrl mAppCastLocation;
		QUrl mReleaseNoteLocation;
		QUrl mAppPackage;
		QUrl mChecksum;
		QJsonDocument mJsonDocument;

		void checkDlResult(QSignalSpy& pSpy, GlobalStatus::Code pErrorCode)
		{
			QCOMPARE(pSpy.count(), 1);

			const QList<QVariant>& arguments = pSpy.first();
			const QVariant errorCodeVariant = arguments.at(0);
			QVERIFY(errorCodeVariant.canConvert<GlobalStatus>());
			GlobalStatus errorCode = errorCodeVariant.value<GlobalStatus>();
			QCOMPARE(errorCode.getStatusCode(), pErrorCode);
		}


		void checkResult(QSignalSpy& pSpy, bool pSuccess, GlobalStatus::Code pErrorCode)
		{
			QCOMPARE(pSpy.count(), 1);

			const QList<QVariant>& arguments = pSpy.first();
			const QVariant successVariant = arguments.at(0);
			QVERIFY(successVariant.userType() == QMetaType::Bool);
			const bool success = successVariant.toBool();
			QCOMPARE(success, pSuccess);

			const QVariant errorCodeVariant = arguments.at(1);
			QVERIFY(errorCodeVariant.canConvert<GlobalStatus>());
			GlobalStatus errorCode = errorCodeVariant.value<GlobalStatus>();
			QCOMPARE(errorCode.getStatusCode(), pErrorCode);
		}


		void setJsonItemField(QJsonDocument& pDocument, const QString& pField, const QString& pValue)
		{
			auto itemArray = pDocument.object()["items"_L1].toArray();
			int i = 0;
			for (auto item : itemArray)
			{
				QJsonObject itemObject = item.toObject();
				itemObject[pField] = pValue;
				itemArray.replace(i, itemObject);
				i++;
			}
			QJsonObject newRootObject;
			newRootObject["items"_L1] = itemArray;
			pDocument.setObject(newRootObject);
		}


		QJsonValue getJsonItemField(QJsonDocument& pDocument, const QString& pField)
		{
#ifdef Q_OS_WIN
			const auto platform = "win"_L1;
#endif

#ifdef Q_OS_MACOS
			const auto platform = "mac"_L1;
#endif

#if !defined(Q_OS_MACOS) && !defined(Q_OS_WIN)
			const auto platform = "src"_L1;
#endif

			auto itemArray = pDocument.object()["items"_L1].toArray();
			for (auto item : itemArray)
			{
				QJsonObject itemObject = item.toObject();
				if (itemObject["platform"_L1] == platform)
				{
					return itemObject[pField];
				}
			}
			return "NO_INFO"_L1;
		}

	private Q_SLOTS:
		void initTestCase()
		{
			Env::set(Downloader::staticMetaObject, &mDownloader);

			mAppCastLocation = mAppUpdater.mAppUpdateJsonUrl;
			mJsonDocument = QJsonDocument::fromJson(test_jsonData);
			mReleaseNoteLocation = getJsonItemField(mJsonDocument, "notes"_L1).toString();
		}


		void init()
		{
			mDownloader.setError(GlobalStatus::Code::No_Error);
			mDownloader.setTestData(mAppCastLocation, test_jsonData);
			mDownloader.setTestData(mReleaseNoteLocation, test_releaseNotes);

			mAppPackage = getJsonItemField(mJsonDocument, "url"_L1).toString();
			mDownloader.setTestData(mAppPackage, test_package);

			mChecksum = getJsonItemField(mJsonDocument, "checksum"_L1).toString();
			QByteArray checksum(test_checksum);
			checksum += mAppPackage.fileName().toUtf8();
			mDownloader.setTestData(mChecksum, checksum);
		}


		void testHashAlgos_data()
		{
			QTest::addColumn<QCryptographicHash::Algorithm>("algo");

			QTest::newRow("") << QCryptographicHash::Sha256;
			QTest::newRow("  ") << QCryptographicHash::Sha256;
			QTest::newRow("#crap#") << QCryptographicHash::Sha256;
			QTest::newRow("sha256") << QCryptographicHash::Sha256;
			QTest::newRow("SHA256") << QCryptographicHash::Sha256;
			QTest::newRow("sha1") << QCryptographicHash::Sha1;
			QTest::newRow("SHA1") << QCryptographicHash::Sha1;
			QTest::newRow("md5") << QCryptographicHash::Md5;
			QTest::newRow("mD5") << QCryptographicHash::Md5;
			QTest::newRow("sha 384") << QCryptographicHash::Sha256; // whitespace not stripped
			QTest::newRow("SHA 384") << QCryptographicHash::Sha256; // whitespace not stripped
			QTest::newRow("sHa512") << QCryptographicHash::Sha512;
		}


		void testHashAlgos()
		{
			QFETCH(QCryptographicHash::Algorithm, algo);

			const QByteArray extension(QTest::currentDataTag());
			QCOMPARE(AppUpdater::getHashAlgo(extension), algo);
		}


		void testWrongChecksumVersion()
		{
			QTemporaryDir dir;
			QVERIFY(dir.isValid());
			mAppUpdater.setDownloadPath(dir.path());

			mDownloader.setTestData(mChecksum, "broken");

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppcastCheckFinished);

			QVERIFY(mAppUpdater.checkAppUpdate());
			checkResult(spy, true, GlobalStatus::Code::No_Error);

			QSignalSpy spyDownload(&mAppUpdater, &AppUpdater::fireAppDownloadFinished);
			QVERIFY(mAppUpdater.downloadUpdate());

			checkDlResult(spyDownload, GlobalStatus::Code::Downloader_Data_Corrupted);

			QVERIFY(!mAppUpdater.getUpdateData().isChecksumValid());
			QCOMPARE(mAppUpdater.getUpdateData().getChecksum(), QByteArray("broken"));

			const QString expectedDest = QDir::toNativeSeparators(dir.path() + '/'_L1 + mAppPackage.fileName());
			QCOMPARE(mAppUpdater.getUpdateData().getUpdatePackagePath(), expectedDest);
			QVERIFY(QFile::exists(expectedDest));
		}


		void testCorrectChecksumVersionAndReDownload()
		{
			QTemporaryDir dir;
			QVERIFY(dir.isValid());
			mAppUpdater.setDownloadPath(dir.path());

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppcastCheckFinished);

			QVERIFY(mAppUpdater.checkAppUpdate());
			checkResult(spy, true, GlobalStatus::Code::No_Error);
			QCOMPARE(mAppUpdater.getUpdateData().getChecksum(), QByteArray());
			QCOMPARE(mAppUpdater.getUpdateData().getUpdatePackagePath(), QString());

			QSignalSpy spyDownload(&mAppUpdater, &AppUpdater::fireAppDownloadFinished);
			QVERIFY(mAppUpdater.downloadUpdate());

			checkDlResult(spyDownload, GlobalStatus::Code::No_Error);

			const QByteArray expectedChecksum("85b1e5ba7c499ab30959febf8d9ea39d08c863617995d7dce20ddc42cd0eb291");
			QCOMPARE(mAppUpdater.getUpdateData().getChecksum(), expectedChecksum);

			const QString expectedDest = QDir::toNativeSeparators(dir.path() + '/'_L1 + mAppPackage.fileName());
			QCOMPARE(mAppUpdater.getUpdateData().getUpdatePackagePath(), expectedDest);
			QVERIFY(QFile::exists(expectedDest));

			QVERIFY(mAppUpdater.getUpdateData().isChecksumValid());

			// trigger re-download but use downloaded package
			spyDownload.clear();
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Checksum file downloaded successfully: .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Data written to file: .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, "Verify checksum with algorithm: QCryptographicHash::Sha256");
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Package already exists: .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, "Re-use valid package...");
			QVERIFY(mAppUpdater.downloadUpdate());
			checkDlResult(spyDownload, GlobalStatus::Code::No_Error);

			// invalidate downloaded package
			QFile expectedFile(expectedDest);
			QVERIFY(expectedFile.open(QIODevice::Append));
			expectedFile.write("break");
			expectedFile.close();

			// trigger re-download and replace package
			spyDownload.clear();
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Checksum file downloaded successfully: .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Data written to file: .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, "Verify checksum with algorithm: QCryptographicHash::Sha256");
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Package already exists: .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, "Checksum of package invalid...");
			QTest::ignoreMessage(QtDebugMsg, "Download package...");
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Package downloaded successfully: .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Data written to file: .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, "Verify checksum with algorithm: QCryptographicHash::Sha256");
			QVERIFY(mAppUpdater.downloadUpdate());
			checkDlResult(spyDownload, GlobalStatus::Code::No_Error);
			QVERIFY(!mAppUpdater.abortDownload());
		}


		void testNewAppUpdateVersion()
		{
			QJsonDocument document = QJsonDocument::fromJson(test_jsonData);
			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppcastCheckFinished);

			QVERIFY(mAppUpdater.checkAppUpdate());

			checkResult(spy, true, GlobalStatus::Code::No_Error);
			AppUpdateData updateData = mAppUpdater.getUpdateData();

			QCOMPARE(updateData.getDate(), QDateTime::fromString(getJsonItemField(document, "date"_L1).toString(), Qt::ISODate));
			QCOMPARE(updateData.getVersion(), getJsonItemField(document, "version"_L1).toString());
			QCOMPARE(updateData.getNotesUrl(), QUrl(getJsonItemField(document, "notes"_L1).toString()));
			QCOMPARE(updateData.getUrl(), QUrl(getJsonItemField(document, "url"_L1).toString()));
			QCOMPARE(updateData.getChecksumUrl(), QUrl(getJsonItemField(document, "checksum"_L1).toString()));
			QCOMPARE(updateData.getSize(), getJsonItemField(document, "size"_L1).toInt());
			QVERIFY(mAppUpdater.getUpdateData().getNotes() != QString());
		}


		void testJSONParsingError()
		{
			QByteArray data("!{no json data]");
			mDownloader.setTestData(mAppCastLocation, data);

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppcastCheckFinished);

			QVERIFY(mAppUpdater.checkAppUpdate());

			checkResult(spy, false, GlobalStatus::Code::Downloader_Data_Corrupted);
		}


		void testPlatformNotSupported()
		{
			QByteArray data("{\"items\":[]}");
			mDownloader.setTestData(mAppCastLocation, data);
			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppcastCheckFinished);

			QVERIFY(mAppUpdater.checkAppUpdate());

			checkResult(spy, false, GlobalStatus::Code::Downloader_Missing_Platform);
		}


		void testNoNewVersion()
		{
			QJsonDocument document = QJsonDocument::fromJson(test_jsonData);
			setJsonItemField(document, "version"_L1, " "_L1);
			QByteArray newJson = document.toJson();
			mDownloader.setTestData(mAppCastLocation, newJson);

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppcastCheckFinished);

			QVERIFY(mAppUpdater.checkAppUpdate());

			checkResult(spy, false, GlobalStatus::Code::No_Error);
		}


		void testJSONDownloadFailed()
		{
			mDownloader.setError(GlobalStatus::Code::Downloader_File_Not_Found);

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppcastCheckFinished);

			QVERIFY(mAppUpdater.checkAppUpdate());

			checkResult(spy, false, GlobalStatus::Code::Downloader_File_Not_Found);
		}


		void testReleaseNoteDownloadFailed()
		{
			QJsonDocument document = QJsonDocument::fromJson(test_jsonData);
			setJsonItemField(document, "notes"_L1, "httb://notarealurl.org"_L1);
			mDownloader.setTestData(mAppCastLocation, document.toJson());

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppcastCheckFinished);

			QVERIFY(mAppUpdater.checkAppUpdate());

			checkResult(spy, true, GlobalStatus::Code::Downloader_File_Not_Found);
			QCOMPARE(mAppUpdater.getUpdateData().getNotes(), QString());
		}


		void testCheckSumDownloadFailed()
		{
			QJsonDocument document = QJsonDocument::fromJson(test_jsonData);
			setJsonItemField(document, "checksum"_L1, "httb://notarealurl.org/checksum"_L1);
			mDownloader.setTestData(mAppCastLocation, document.toJson());

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppcastCheckFinished);
			QVERIFY(mAppUpdater.checkAppUpdate());
			checkResult(spy, true, GlobalStatus::Code::No_Error);

			QSignalSpy spyPackage(&mAppUpdater, &AppUpdater::fireAppDownloadFinished);
			QVERIFY(mAppUpdater.downloadUpdate());
			checkDlResult(spyPackage, GlobalStatus::Code::Downloader_File_Not_Found);
			QCOMPARE(mAppUpdater.getUpdateData().getChecksum(), QByteArray());
			QCOMPARE(mAppUpdater.getUpdateData().getUpdatePackagePath(), QString());
		}


		void testPackageDownloadFailed()
		{
			QJsonDocument document = QJsonDocument::fromJson(test_jsonData);
			setJsonItemField(document, "url"_L1, "httb://notarealurl.org/nonexisting"_L1);
			mDownloader.setTestData(mAppCastLocation, document.toJson());

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppcastCheckFinished);
			QVERIFY(mAppUpdater.checkAppUpdate());
			checkResult(spy, true, GlobalStatus::Code::No_Error);

			QSignalSpy spyPackage(&mAppUpdater, &AppUpdater::fireAppDownloadFinished);
			QVERIFY(mAppUpdater.downloadUpdate());
			checkDlResult(spyPackage, GlobalStatus::Code::Downloader_File_Not_Found);
			QCOMPARE(mAppUpdater.getUpdateData().getChecksum(), QByteArray(test_checksum).trimmed());
			QCOMPARE(mAppUpdater.getUpdateData().getUpdatePackagePath(), QString());
		}


};

QTEST_GUILESS_MAIN(test_AppUpdatr)
#include "test_AppUpdatr.moc"
