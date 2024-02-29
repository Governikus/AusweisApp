/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for class \ref Downloader
 */

#include "Downloader.h"

#include "Env.h"
#include "MockNetworkManager.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_Downloader
	: public QObject
{
	Q_OBJECT

	private:
		QTemporaryDir mCacheDir;
		MockNetworkManager mMockNetworkManager;

		void verifySuccessReply(const QSignalSpy& pSpy, const QUrl& pUrl, const QDateTime& pTimestamp, const QByteArray& pData)
		{
			QCOMPARE(pSpy.count(), 1);

			const QList<QVariant>& arguments = pSpy.first();
			const QVariant updateUrlVariant = arguments.at(0);
			QVERIFY(updateUrlVariant.userType() == QMetaType::QUrl);
			const QUrl url = updateUrlVariant.toUrl();
			QCOMPARE(url, pUrl);

			const QVariant timestampVariant = arguments.at(1);
			QVERIFY(timestampVariant.userType() == QMetaType::QDateTime);
			QCOMPARE(timestampVariant.toDateTime(), pTimestamp);

			const QVariant dataVariant = arguments.at(2);
			QVERIFY(dataVariant.userType() == QMetaType::QByteArray);
			QCOMPARE(dataVariant.toByteArray(), pData);
		}


		void verifyFailedReply(const QSignalSpy& pSpy, const QUrl& pUrl, GlobalStatus::Code pErrorCode)
		{
			QCOMPARE(pSpy.count(), 1);

			const QList<QVariant>& arguments = pSpy.first();
			const QVariant updateUrlVariant = arguments.at(0);
			QVERIFY(updateUrlVariant.userType() == QMetaType::QUrl);
			const QUrl url = updateUrlVariant.toUrl();
			QCOMPARE(url, pUrl);

			const QVariant errorCodeVariant = arguments.at(1);
			QVERIFY(errorCodeVariant.canConvert<GlobalStatus::Code>());
			QCOMPARE(errorCodeVariant.value<GlobalStatus::Code>(), pErrorCode);
		}


		void verifyUnnecessaryDownloadReply(const QSignalSpy& pSpy, const QUrl& pUrl)
		{
			QCOMPARE(pSpy.count(), 1);

			const QList<QVariant>& arguments = pSpy.first();
			const QVariant updateUrlVariant = arguments.at(0);
			QVERIFY(updateUrlVariant.userType() == QMetaType::QUrl);
			const QUrl url = updateUrlVariant.toUrl();
			QCOMPARE(url, pUrl);
		}

	private Q_SLOTS:
		void initTestCase()
		{
			mCacheDir.setAutoRemove(true);
		}


		void init()
		{
			Env::set(NetworkManager::staticMetaObject, &mMockNetworkManager);
		}


		void cleanup()
		{
			Env::clear();
		}


		void downloadExistingFile()
		{
			const QByteArray fileContent("Some icon data");
			const QDateTime timestampOnServer(QDate(2017, 6, 1), QTime(12, 00, 0, 0));
			auto* const reply = new MockNetworkReply(fileContent, HTTP_STATUS_OK);
			reply->setFileModificationTimestamp(timestampOnServer);
			mMockNetworkManager.setNextReply(reply);

			auto* const downloader = Env::getSingleton<Downloader>();
			QSignalSpy spy(downloader, &Downloader::fireDownloadSuccess);

			const QUrl url("http://server/reader/icons/icon.png"_L1);
			downloader->download(url);

			mMockNetworkManager.fireFinished();

			verifySuccessReply(spy, url, timestampOnServer, fileContent);
		}


		void downloadEmptyFile()
		{
			const QByteArray fileContent;
			const QDateTime timestampOnServer(QDate(2017, 6, 1), QTime(12, 00, 0, 0));
			auto* const reply = new MockNetworkReply(fileContent, HTTP_STATUS_OK);
			reply->setFileModificationTimestamp(timestampOnServer);
			mMockNetworkManager.setNextReply(reply);

			auto* const downloader = Env::getSingleton<Downloader>();
			QSignalSpy spy(downloader, &Downloader::fireDownloadFailed);

			const QUrl url("http://server/reader/icons/icon.png"_L1);
			downloader->download(url);

			mMockNetworkManager.fireFinished();

			verifyFailedReply(spy, url, GlobalStatus::Code::Network_Other_Error);
		}


		void downloadNonExistingFile()
		{
			MockNetworkReply* const reply = new MockNetworkReply(QByteArray(), HTTP_STATUS_NOT_FOUND);
			mMockNetworkManager.setNextReply(reply);

			auto* const downloader = Env::getSingleton<Downloader>();
			QSignalSpy spy(downloader, &Downloader::fireDownloadFailed);

			const QUrl url("http://server/reader/icons/icon.png"_L1);
			downloader->download(url);

			mMockNetworkManager.fireFinished();

			verifyFailedReply(spy, url, GlobalStatus::Code::Downloader_File_Not_Found);
		}


		void abortDownload()
		{
			MockNetworkReply* const reply = new MockNetworkReply(QByteArray(), HTTP_STATUS_OK);
			mMockNetworkManager.setNextReply(reply);

			auto* const downloader = Env::getSingleton<Downloader>();
			QSignalSpy spy(downloader, &Downloader::fireDownloadFailed);

			const QUrl urlCurrent("http://server/current"_L1);
			const QUrl urlurlNext("http://server/next"_L1);

			QTest::ignoreMessage(QtDebugMsg, "Try abort of download: QUrl(\"http://server/current\")");
			QVERIFY(!downloader->abort(urlCurrent));
			QCOMPARE(spy.count(), 0);

			QTest::ignoreMessage(QtDebugMsg, "Download: QUrl(\"http://server/current\")");
			downloader->download(urlCurrent);

			QTest::ignoreMessage(QtDebugMsg, "Try abort of download: QUrl(\"http://server/next\")");
			QVERIFY(!downloader->abort(urlurlNext));
			QCOMPARE(spy.count(), 0);

			QTest::ignoreMessage(QtDebugMsg, "Download: QUrl(\"http://server/next\")");
			QTest::ignoreMessage(QtDebugMsg, "A download is already in progress... delaying.");
			downloader->download(urlurlNext);

			QTest::ignoreMessage(QtDebugMsg, "Try abort of download: QUrl(\"http://server/next\")");
			QTest::ignoreMessage(QtDebugMsg, "Remove pending request");
			QVERIFY(downloader->abort(urlurlNext));
			verifyFailedReply(spy, urlurlNext, GlobalStatus::Code::Downloader_Aborted);

			spy.clear();
			QTest::ignoreMessage(QtDebugMsg, "Try abort of download: QUrl(\"http://server/current\")");
			QTest::ignoreMessage(QtDebugMsg, "Operation aborted"); // from MockNetworkReply
			QTest::ignoreMessage(QtDebugMsg, "Current download aborted");
			QVERIFY(downloader->abort(urlCurrent));
			verifyFailedReply(spy, urlCurrent, GlobalStatus::Code::Downloader_Aborted);
		}


		void conditionalDownloadOfNewerFile()
		{
			const QByteArray fileContent("Some icon data");
			auto* const reply = new MockNetworkReply(fileContent, HTTP_STATUS_OK);
			const QDateTime timestampOnServer(QDate(2017, 7, 1), QTime(12, 00, 0, 0), QTimeZone::utc());
			reply->setFileModificationTimestamp(timestampOnServer);
			mMockNetworkManager.setNextReply(reply);

			auto* const downloader = Env::getSingleton<Downloader>();
			QSignalSpy spy(downloader, &Downloader::fireDownloadSuccess);

			const QUrl url("http://server/reader/icons/icon.png"_L1);
			const QDateTime timestampInCache(QDate(2017, 6, 1), QTime(12, 00, 0, 0), QTimeZone::utc());
			downloader->download(url, timestampInCache);

			mMockNetworkManager.fireFinished();

			verifySuccessReply(spy, url, timestampOnServer, fileContent);

			const QNetworkRequest lastRequest = mMockNetworkManager.getLastRequest();
			QCOMPARE(lastRequest.header(QNetworkRequest::IfModifiedSinceHeader).toDateTime(), timestampInCache);
			QCOMPARE(lastRequest.rawHeader(QByteArray("If-Modified-Since")), QLocale::c().toString(timestampInCache, QStringLiteral("ddd, dd MMM yyyy hh:mm:ss 'GMT'")).toLatin1());
			QCOMPARE(lastRequest.rawHeader(QByteArray("If-Modified-Since")), QByteArray("Thu, 01 Jun 2017 12:00:00 GMT"));
		}


		void conditionalDownloadOfOlderFile()
		{
			MockNetworkReply* const reply = new MockNetworkReply(QByteArray(), HTTP_STATUS_NOT_MODIFIED);
			const QDateTime timestampOnServer(QDate(2017, 6, 1), QTime(12, 00, 0, 0), QTimeZone::utc());
			reply->setFileModificationTimestamp(timestampOnServer);
			mMockNetworkManager.setNextReply(reply);

			auto* const downloader = Env::getSingleton<Downloader>();
			QSignalSpy spy(downloader, &Downloader::fireDownloadUnnecessary);

			const QUrl url("http://server/reader/icons/icon.png"_L1);
			const QDateTime timestampInCache(QDate(2017, 7, 1), QTime(12, 00, 0, 0), QTimeZone::utc());
			downloader->download(url, timestampInCache);

			mMockNetworkManager.fireFinished();

			verifyUnnecessaryDownloadReply(spy, url);

			const QNetworkRequest lastRequest = mMockNetworkManager.getLastRequest();
			QCOMPARE(lastRequest.header(QNetworkRequest::IfModifiedSinceHeader).toDateTime(), timestampInCache);
			QCOMPARE(lastRequest.rawHeader(QByteArray("If-Modified-Since")), QLocale::c().toString(timestampInCache, QStringLiteral("ddd, dd MMM yyyy hh:mm:ss 'GMT'")).toLatin1());
			QCOMPARE(lastRequest.rawHeader(QByteArray("If-Modified-Since")), QByteArray("Sat, 01 Jul 2017 12:00:00 GMT"));
		}


};

QTEST_GUILESS_MAIN(test_Downloader)
#include "test_Downloader.moc"
