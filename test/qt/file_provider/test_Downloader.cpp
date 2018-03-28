/*!
 * \brief Unit tests for class \ref Downloader
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "Downloader.h"

#include "Env.h"
#include "MockNetworkManager.h"

#include <QtTest>

using namespace governikus;

class test_Downloader
	: public QObject
{
	Q_OBJECT

	private:
		QTemporaryDir mCacheDir;

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
			Env::set(NetworkManager::staticMetaObject, std::make_shared<MockNetworkManager>());
		}


		void cleanup()
		{
			Env::clear();
		}


		void downloadExistingFile()
		{
			const QByteArray fileContent("Some icon data");
			const QDateTime timestampOnServer(QDate(2017, 6, 1), QTime(12, 00, 0, 0));
			MockNetworkReply* const reply = new MockNetworkReply(fileContent, HttpStatusCode::OK);
			reply->setFileModificationTimestamp(timestampOnServer);
			Env::getSingleton<NetworkManager, MockNetworkManager>()->setNextReply(reply);

			Downloader* const downloader = Env::getSingleton<Downloader>();
			QSignalSpy spy(downloader, &Downloader::fireDownloadSuccess);

			const QUrl url("http://server/reader/icons/icon.png");
			downloader->download(url);

			Env::getSingleton<NetworkManager, MockNetworkManager>()->fireFinished();

			verifySuccessReply(spy, url, timestampOnServer, fileContent);
		}


		void downloadNonExistingFile()
		{
			MockNetworkReply* const reply = new MockNetworkReply(QByteArray(), HttpStatusCode::NOT_FOUND);
			Env::getSingleton<NetworkManager, MockNetworkManager>()->setNextReply(reply);

			Downloader* const downloader = Env::getSingleton<Downloader>();
			QSignalSpy spy(downloader, &Downloader::fireDownloadFailed);

			const QUrl url("http://server/reader/icons/icon.png");
			downloader->download(url);

			Env::getSingleton<NetworkManager, MockNetworkManager>()->fireFinished();

			verifyFailedReply(spy, url, GlobalStatus::Code::Downloader_File_Not_Found);
		}


		void conditionalDownloadOfNewerFile()
		{
			const QByteArray fileContent("Some icon data");
			MockNetworkReply* const reply = new MockNetworkReply(fileContent, HttpStatusCode::OK);
			const QDateTime timestampOnServer(QDate(2017, 7, 1), QTime(12, 00, 0, 0));
			reply->setFileModificationTimestamp(timestampOnServer);
			Env::getSingleton<NetworkManager, MockNetworkManager>()->setNextReply(reply);

			Downloader* const downloader = Env::getSingleton<Downloader>();
			QSignalSpy spy(downloader, &Downloader::fireDownloadSuccess);

			const QUrl url("http://server/reader/icons/icon.png");
			const QDateTime timestampInCache(QDate(2017, 6, 1), QTime(12, 00, 0, 0));
			downloader->downloadIfNew(url, timestampInCache);

			Env::getSingleton<NetworkManager, MockNetworkManager>()->fireFinished();

			verifySuccessReply(spy, url, timestampOnServer, fileContent);

			QNetworkRequest* const lastRequest = Env::getSingleton<NetworkManager, MockNetworkManager>()->getLastRequest();
			QVERIFY(lastRequest);
			QCOMPARE(lastRequest->rawHeader(QByteArray("If-Modified-Since")), QLocale::c().toString(timestampInCache, QStringLiteral("ddd, dd MMM yyyy hh:mm:ss 'GMT'")).toLatin1());
			QCOMPARE(lastRequest->rawHeader(QByteArray("If-Modified-Since")), QByteArray("Thu, 01 Jun 2017 12:00:00 GMT"));
		}


		void conditionalDownloadOfOlderFile()
		{
			MockNetworkReply* const reply = new MockNetworkReply(QByteArray(), HttpStatusCode::NOT_MODIFIED);
			const QDateTime timestampOnServer(QDate(2017, 6, 1), QTime(12, 00, 0, 0));
			reply->setFileModificationTimestamp(timestampOnServer);
			Env::getSingleton<NetworkManager, MockNetworkManager>()->setNextReply(reply);

			Downloader* const downloader = Env::getSingleton<Downloader>();
			QSignalSpy spy(downloader, &Downloader::fireDownloadUnnecessary);

			const QUrl url("http://server/reader/icons/icon.png");
			const QDateTime timestampInCache(QDate(2017, 7, 1), QTime(12, 00, 0, 0));
			downloader->downloadIfNew(url, timestampInCache);

			Env::getSingleton<NetworkManager, MockNetworkManager>()->fireFinished();

			verifyUnnecessaryDownloadReply(spy, url);

			QNetworkRequest* const lastRequest = Env::getSingleton<NetworkManager, MockNetworkManager>()->getLastRequest();
			QVERIFY(lastRequest);
			QCOMPARE(lastRequest->rawHeader(QByteArray("If-Modified-Since")), QLocale::c().toString(timestampInCache, QStringLiteral("ddd, dd MMM yyyy hh:mm:ss 'GMT'")).toLatin1());
			QCOMPARE(lastRequest->rawHeader(QByteArray("If-Modified-Since")), QByteArray("Sat, 01 Jul 2017 12:00:00 GMT"));
		}


};

QTEST_GUILESS_MAIN(test_Downloader)
#include "test_Downloader.moc"
