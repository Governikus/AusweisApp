/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "UpdatableFile.h"

#include "Downloader.h"
#include "Env.h"
#include "MockDownloader.h"

#include <QtTest>

using namespace governikus;

class test_UpdatableFile
	: public QObject
{
	Q_OBJECT

	private:
		const QString mSection;
		const QChar mSep;

		void verifySectionCacheFolder(UpdatableFile& pUpdatableFile)
		{
			QDir folder(pUpdatableFile.getSectionCachePath());
			QVERIFY(folder.exists());
			QVERIFY(pUpdatableFile.getSectionCachePath().endsWith(mSection));
		}


		void touchFileInCache(const QString& pFilename, UpdatableFile& pUpdatableFile)
		{
			verifySectionCacheFolder(pUpdatableFile);

			const QString filePath = pUpdatableFile.getSectionCachePath() + mSep + pFilename;
			QFile file(filePath);
			QVERIFY(!file.exists());
			QVERIFY(file.open(QIODevice::WriteOnly));
			file.close();
			QVERIFY(file.exists());
		}


		void removeFileFromCache(const QString& pFilename, UpdatableFile& pUpdatableFile)
		{
			verifySectionCacheFolder(pUpdatableFile);

			const QString filePath = pUpdatableFile.getSectionCachePath() + mSep + pFilename;
			QFile file(filePath);
			QVERIFY(file.exists());
			QVERIFY(file.remove());
			QVERIFY(!file.exists());
		}


	private Q_SLOTS:
		void cleanup()
		{
			Env::clear();
		}


		void testFileOnlyInCache()
		{
			const QString filename("img_ACS_ACR1252V.png");
			const QString filenameInCache = filename + QStringLiteral("_20170601102132");
			const QDate timestampDate(2017, 6, 1);
			const QTime timestampTime(10, 21, 32);
			const QDateTime timestamp(timestampDate, timestampTime);
			UpdatableFile updatableFile(mSection, filename);
			touchFileInCache(filenameInCache, updatableFile);

			QCOMPARE(updatableFile.getName(), filename);
			QCOMPARE(updatableFile.lookupPath(), updatableFile.getSectionCachePath() + mSep + filenameInCache);
			QCOMPARE(updatableFile.cacheTimestamp(), timestamp);

			removeFileFromCache(filenameInCache, updatableFile);
		}


		void testFileOnlyInBundle()
		{
			const QString filename("img_ACS_ACR1252U.png");
			UpdatableFile updatableFile(mSection, filename);

			verifySectionCacheFolder(updatableFile);
			QCOMPARE(updatableFile.getName(), filename);
			QCOMPARE(updatableFile.lookupPath(), QStringLiteral(":/updatable-files/reader/img_ACS_ACR1252U.png"));
			QVERIFY(!updatableFile.cacheTimestamp().isValid());
		}


		void testFileInCacheAndInBundle()
		{
			const QString filename("img_ACS_ACR1252U.png");
			const QString filenameInCache = filename + QStringLiteral("_20170601102132");
			UpdatableFile updatableFile(mSection, filename);
			touchFileInCache(filenameInCache, updatableFile);

			QCOMPARE(updatableFile.getName(), filename);
			QCOMPARE(updatableFile.lookupPath(), updatableFile.getSectionCachePath() + mSep + filenameInCache);

			removeFileFromCache(filenameInCache, updatableFile);
		}


		void testMoreThanOneVersionInCache()
		{
			const QString filename("img_ACS_ACR1252U.png");
			const QString filenameInCache1 = filename + QStringLiteral("_20170710120015");
			const QString filenameInCache2 = filename + QStringLiteral("_20170601102132");
			UpdatableFile updatableFile(mSection, filename);
			touchFileInCache(filenameInCache1, updatableFile);
			touchFileInCache(filenameInCache2, updatableFile);

			QCOMPARE(updatableFile.getName(), filename);
			QCOMPARE(updatableFile.lookupPath(), updatableFile.getSectionCachePath() + mSep + filenameInCache1);

			removeFileFromCache(filenameInCache1, updatableFile);
			removeFileFromCache(filenameInCache2, updatableFile);
		}


		void testFileNeitherInCacheNorInBundle()
		{
			const QString filename("img_ACS_ACR1252V.png");
			UpdatableFile updatableFile(mSection, filename, QStringLiteral("DEFAULT_TEST"));

			verifySectionCacheFolder(updatableFile);
			QCOMPARE(updatableFile.getName(), filename);
			QCOMPARE(updatableFile.lookupPath(), QStringLiteral("DEFAULT_TEST"));
		}


		void testIsDirty()
		{
			const QString filename("img_ACS_ACR1252U.png");
			const QString dirtyFilename = filename + QStringLiteral(".dirty");
			UpdatableFile updatableFile(mSection, filename);

			QVERIFY(!updatableFile.isDirty());

			touchFileInCache(dirtyFilename, updatableFile);
			QVERIFY(updatableFile.isDirty());

			removeFileFromCache(dirtyFilename, updatableFile);
			QVERIFY(!updatableFile.isDirty());
		}


		void testMarkDirty()
		{
			const QString filename("img_ACS_ACR1252U.png");
			const QString dirtyFilename = filename + QStringLiteral(".dirty");
			UpdatableFile updatableFile(mSection, filename);

			QVERIFY(!updatableFile.isDirty());

			updatableFile.markDirty();
			QVERIFY(updatableFile.isDirty());

			removeFileFromCache(dirtyFilename, updatableFile);
			QVERIFY(!updatableFile.isDirty());
		}


		void testClearDirty()
		{
			const QString filename("img_ACS_ACR1252U.png");
			const QString dirtyFilename = filename + QStringLiteral(".dirty");
			UpdatableFile updatableFile(mSection, filename);

			touchFileInCache(dirtyFilename, updatableFile);
			QVERIFY(updatableFile.isDirty());

			updatableFile.clearDirty();
			QVERIFY(!updatableFile.isDirty());
		}


		void testEmptyName()
		{
			const QString filename;
			const QString dirtyFilename = filename + QStringLiteral(".dirty");
			UpdatableFile updatableFile(mSection, filename, QStringLiteral("DEFAULT"));

			QVERIFY(!updatableFile.isDirty());
			QCOMPARE(updatableFile.lookupPath(), QStringLiteral("DEFAULT"));

			updatableFile.markDirty();
			QVERIFY(!updatableFile.isDirty());
			QCOMPARE(updatableFile.lookupPath(), QStringLiteral("DEFAULT"));

			updatableFile.clearDirty();
			QVERIFY(!updatableFile.isDirty());
			QCOMPARE(updatableFile.lookupPath(), QStringLiteral("DEFAULT"));

			touchFileInCache(dirtyFilename, updatableFile);
			QVERIFY(!updatableFile.isDirty());
			QCOMPARE(updatableFile.lookupPath(), QStringLiteral("DEFAULT"));

			removeFileFromCache(dirtyFilename, updatableFile);
			QVERIFY(!updatableFile.isDirty());
			QCOMPARE(updatableFile.lookupPath(), QStringLiteral("DEFAULT"));
		}


		void testFileIsDownloadedOnDirty()
		{
			MockDownloader downloader;
			Env::set(Downloader::staticMetaObject, &downloader);

			const QString filename("img_ACS_ACR1252U.png");
			const QString filenameInCache = filename + QLatin1Char('_') + downloader.getTimeStampString();

			UpdatableFile updatableFile(mSection, filename);
			QUrl updateUrl = updatableFile.updateUrl(mSection, filename);
			QByteArray testData = QByteArray("Testdata");
			downloader.setTestData(updateUrl, testData);

			updatableFile.markDirty();

			QSignalSpy spy(&updatableFile, &UpdatableFile::fireUpdated);
			QCOMPARE(updatableFile.lookupPath(), QStringLiteral(":/updatable-files/reader/img_ACS_ACR1252U.png"));

			QCOMPARE(spy.count(), 1);
			QVERIFY(!updatableFile.isDirty());
			QCOMPARE(updatableFile.lookupPath(), updatableFile.getSectionCachePath() + mSep + filenameInCache);
		}


		void testFileIsCreatedAfterUpdate()
		{
			MockDownloader downloader;
			Env::set(Downloader::staticMetaObject, &downloader);

			const QString filename("img_updatetest.png");

			UpdatableFile updatableFile(mSection, filename);
			QSignalSpy spy(&updatableFile, &UpdatableFile::fireUpdated);
			QUrl updateUrl = updatableFile.updateUrl(mSection, filename);
			QByteArray testData = "Testdata";
			downloader.setTestData(updateUrl, testData);

			updatableFile.update();

			QCOMPARE(spy.count(), 1);
			const QString fileName = updatableFile.getName() + QLatin1Char('_') + downloader.getTimeStampString();
			const QString filePath = updatableFile.getSectionCachePath() + "/" + fileName;
			QFile testfile(filePath);
			QVERIFY(testfile.exists());
			QVERIFY(testfile.open(QIODevice::ReadOnly));
			QCOMPARE(testfile.readAll(), downloader.getTestData(updateUrl));
			testfile.close();

			removeFileFromCache(fileName, updatableFile);
		}


		void testNoFileIsCreatedAfterFailedUpdate()
		{
			MockDownloader downloader(GlobalStatus::Code::Downloader_File_Not_Found);
			Env::set(Downloader::staticMetaObject, &downloader);

			const QString filename("img_updatetest.png");

			UpdatableFile updatableFile(mSection, filename);
			QSignalSpy spy(&updatableFile, &UpdatableFile::fireUpdated);

			updatableFile.update();

			QCOMPARE(spy.count(), 0);
			const QString filePath = updatableFile.getSectionCachePath() + "/" + updatableFile.getName() + QLatin1Char('_') + downloader.getTimeStampString();
			QFile testfile(filePath);
			QVERIFY(!testfile.exists());
		}


	public:
		test_UpdatableFile()
			: mSection("reader")
			, mSep('/')
		{
		}


};

QTEST_GUILESS_MAIN(test_UpdatableFile)
#include "test_UpdatableFile.moc"
