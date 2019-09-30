/*!
 * \brief Unit tests for \ref AppUpdater
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "AppUpdater.h"
#include "Downloader.h"

#include "Env.h"
#include "GlobalStatus.h"
#include "MockDownloader.h"
#include "SecureStorage.h"
#include "VersionNumber.h"

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
} // namespace

using namespace governikus;

class test_AppUpdater
	: public QObject
{
	Q_OBJECT

	private:
		AppUpdater mAppUpdater;
		MockDownloader mDownloader;
		QUrl mAppCastLocation;
		QUrl mReleaseNoteLocation;
		QJsonDocument mJsonDocument;

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
			auto itemArray = pDocument.object()["items"].toArray();
			int i = 0;
			for (auto item : itemArray)
			{
				QJsonObject itemObject = item.toObject();
				itemObject[pField] = pValue;
				itemArray.replace(i, itemObject);
				i++;
			}
			QJsonObject newRootObject;
			newRootObject[QLatin1String("items")] = itemArray;
			pDocument.setObject(newRootObject);
		}


		QJsonValue getJsonItemField(QJsonDocument& pDocument, const QString& pField)
		{
			#ifdef Q_OS_WIN
			QString platform = "win";
			#endif

			#ifdef Q_OS_MACOS
			QString platform = "mac";
			#endif

			#if !defined(Q_OS_MACOS) && !defined(Q_OS_WIN)
			QString platform = "src";
			#endif

			auto itemArray = pDocument.object()["items"].toArray();
			for (auto item : itemArray)
			{
				QJsonObject itemObject = item.toObject();
				if (itemObject["platform"] == platform)
				{
					return itemObject[pField];
				}
			}
			return QString("NO_INFO");
		}


	private Q_SLOTS:
		void initTestCase()
		{
			Env::set(Downloader::staticMetaObject, &mDownloader);

			const auto* secureStorage = Env::getSingleton<SecureStorage>();
			mAppCastLocation = VersionNumber::getApplicationVersion().isDeveloperVersion() ? secureStorage->getAppcastBetaUpdateUrl() : secureStorage->getAppcastUpdateUrl();

			mJsonDocument = QJsonDocument::fromJson(test_jsonData);
			mReleaseNoteLocation = getJsonItemField(mJsonDocument, "notes").toString();
		}


		void init()
		{
			mDownloader.setError(GlobalStatus::Code::No_Error);
			mDownloader.setTestData(mAppCastLocation, test_jsonData);
			mDownloader.setTestData(mReleaseNoteLocation, test_releaseNotes);
		}


		void cleanup()
		{
		}


		void testNewAppUpdateVersion()
		{
			QJsonDocument document = QJsonDocument::fromJson(test_jsonData);
			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppUpdateCheckFinished);

			mAppUpdater.checkAppUpdate();

			checkResult(spy, true, GlobalStatus::Code::No_Error);
			AppUpdateData updateData = mAppUpdater.getUpdateData();

			QCOMPARE(updateData.getDate(), QDateTime::fromString(getJsonItemField(document, "date").toString(), Qt::ISODate));
			QCOMPARE(updateData.getVersion(), getJsonItemField(document, "version").toString());
			QCOMPARE(updateData.getNotesUrl(), QUrl(getJsonItemField(document, "notes").toString()));
			QCOMPARE(updateData.getUrl(), QUrl(getJsonItemField(document, "url").toString()));
			QCOMPARE(updateData.getChecksumUrl(), QUrl(getJsonItemField(document, "checksum").toString()));
			QCOMPARE(updateData.getSize(), getJsonItemField(document, "size").toInt());
			QVERIFY(mAppUpdater.getUpdateData().getNotes() != QString());
		}


		void testJSONParsingError()
		{
			QByteArray data("!{no json data]");
			mDownloader.setTestData(mAppCastLocation, data);

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppUpdateCheckFinished);

			mAppUpdater.checkAppUpdate();

			checkResult(spy, false, GlobalStatus::Code::Downloader_Data_Corrupted);
		}


		void testPlatformNotSupported()
		{
			QByteArray data("{\"items\":[]}");
			mDownloader.setTestData(mAppCastLocation, data);
			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppUpdateCheckFinished);

			mAppUpdater.checkAppUpdate();

			checkResult(spy, false, GlobalStatus::Code::No_Error);
		}


		void testSkipCurrentVersion()
		{
			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppUpdateCheckFinished);
			mAppUpdater.skipVersion(getJsonItemField(mJsonDocument, "version").toString());

			mAppUpdater.checkAppUpdate();

			checkResult(spy, false, GlobalStatus::Code::No_Error);
			mAppUpdater.skipVersion("");
		}


		void testSkipCurrentVersionButDoNotRespectIt()
		{
			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppUpdateCheckFinished);
			mAppUpdater.skipVersion(getJsonItemField(mJsonDocument, "version").toString());

			mAppUpdater.checkAppUpdate(true);

			checkResult(spy, true, GlobalStatus::Code::No_Error);
			mAppUpdater.skipVersion("");
		}


		void testNoNewVersion()
		{
			QJsonDocument document = QJsonDocument::fromJson(test_jsonData);
			setJsonItemField(document, QString("version"), QString(" "));
			QByteArray newJson = document.toJson();
			mDownloader.setTestData(mAppCastLocation, newJson);

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppUpdateCheckFinished);

			mAppUpdater.checkAppUpdate();

			checkResult(spy, false, GlobalStatus::Code::No_Error);
		}


		void testJSONDownloadFailed()
		{
			mDownloader.setError(GlobalStatus::Code::Downloader_File_Not_Found);

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppUpdateCheckFinished);

			mAppUpdater.checkAppUpdate();

			checkResult(spy, false, GlobalStatus::Code::Downloader_File_Not_Found);
		}


		void testReleaseNoteDownloadFailed()
		{
			QJsonDocument document = QJsonDocument::fromJson(test_jsonData);
			setJsonItemField(document, QString("notes"), QString("httb://notarealurl.org"));
			QByteArray newJson = document.toJson();
			mDownloader.setTestData(mAppCastLocation, newJson);

			QSignalSpy spy(&mAppUpdater, &AppUpdater::fireAppUpdateCheckFinished);

			mAppUpdater.checkAppUpdate();

			checkResult(spy, true, GlobalStatus::Code::No_Error);
			QCOMPARE(mAppUpdater.getUpdateData().getNotes(), QString());
		}


};

QTEST_GUILESS_MAIN(test_AppUpdater)
#include "test_AppUpdatr.moc"
