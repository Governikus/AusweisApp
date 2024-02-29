/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UpdatableFile.h"

#include "Downloader.h"
#include "Env.h"
#include "MockDownloader.h"

#include <QTimeZone>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_UpdatableFile
	: public QObject
{
	Q_OBJECT

	private:
		static constexpr auto mSection = QLatin1String("reader");
		static constexpr auto mSep = QLatin1Char('/');

		static void verifySectionCacheFolder(UpdatableFile& pUpdatableFile)
		{
			QDir folder(pUpdatableFile.getSectionCachePath());
			QVERIFY(folder.exists());
			QVERIFY(pUpdatableFile.getSectionCachePath().endsWith(mSection));
		}


		[[nodiscard]] static auto touchFileInCache(const QString& pFilename, UpdatableFile& pUpdatableFile)
		{
			verifySectionCacheFolder(pUpdatableFile);

			const auto& check = [](const auto& pFilePath){
						QFile file(pFilePath);
						QVERIFY(!file.exists());
						QVERIFY(file.open(QIODevice::WriteOnly));
						file.close();
						QVERIFY(file.exists());
					};
			check(pUpdatableFile.getSectionCachePath() + mSep + pFilename);

			return qScopeGuard([pFilename, &pUpdatableFile]{
					removeFileFromCache(pFilename, pUpdatableFile);
				});

		}


		static void removeFileFromCache(const QString& pFilename, UpdatableFile& pUpdatableFile)
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
			const QString filename("img_ACS_ACR1252V.png"_L1);
			const QString filenameInCache = filename + QStringLiteral("_20170601102132MST");
			const QDate timestampDate(2017, 6, 1);
			const QTime timestampTime(10, 21, 32);
			const QDateTime timestamp(timestampDate, timestampTime, QTimeZone("MST"));
			UpdatableFile updatableFile(mSection, filename);
			const auto guard = touchFileInCache(filenameInCache, updatableFile);

			QCOMPARE(updatableFile.getName(), filename);
			QCOMPARE(updatableFile.lookupPath(), updatableFile.getSectionCachePath() + mSep + filenameInCache);
			QCOMPARE(updatableFile.cacheTimestamp(), timestamp);

			QStringList paths;
			QVERIFY(updatableFile.forEachLookupPath([&paths](const QString& pPath) -> bool {
					paths += pPath;
					return true;
				}));
			QCOMPARE(paths, QStringList({updatableFile.getSectionCachePath() + mSep + filenameInCache}));
		}


		void testFileOnlyInBundle()
		{
			const QString filename("img_ACS_ACR1252U.png"_L1);
			const QString expectedPath = ":/updatable-files/reader/"_L1 + filename;
			UpdatableFile updatableFile(mSection, filename);

			verifySectionCacheFolder(updatableFile);
			QCOMPARE(updatableFile.getName(), filename);
			QCOMPARE(updatableFile.lookupPath(), expectedPath);
			QVERIFY(!updatableFile.cacheTimestamp().isValid());
		}


		void testFileInCacheAndInBundle()
		{
			const QString filename("img_ACS_ACR1252U.png"_L1);
			const QString expectedPath = ":/updatable-files/reader/"_L1 + filename;
			const QString filenameInCache = filename + QStringLiteral("_20170601102132GMT");
			UpdatableFile updatableFile(mSection, filename);
			const auto guard = touchFileInCache(filenameInCache, updatableFile);

			QCOMPARE(updatableFile.getName(), filename);
			QCOMPARE(updatableFile.lookupPath(), updatableFile.getSectionCachePath() + mSep + filenameInCache);

			QStringList paths;
			QVERIFY(!updatableFile.forEachLookupPath([&paths](const QString& pPath) -> bool {
					paths += pPath;
					return false;
				}));
			QCOMPARE(paths, QStringList({updatableFile.getSectionCachePath() + mSep + filenameInCache, expectedPath}));
		}


		void testMoreThanOneVersionInCache()
		{
			const QString filename("img_ACS_ACR1252U.png"_L1);
			const QString filenameInCache1 = filename + QStringLiteral("_20170710120015WAST");
			const QString filenameInCache2 = filename + QStringLiteral("_20170601102132UTC");
			UpdatableFile updatableFile(mSection, filename);
			const auto guard1 = touchFileInCache(filenameInCache1, updatableFile);
			const auto guard2 = touchFileInCache(filenameInCache2, updatableFile);

			QCOMPARE(updatableFile.getName(), filename);
			QCOMPARE(updatableFile.lookupPath(), updatableFile.getSectionCachePath() + mSep + filenameInCache1);
		}


		void testFileNeitherInCacheNorInBundle()
		{
			const QString filename("img_ACS_ACR1252V.png"_L1);
			UpdatableFile updatableFile(mSection, filename, QStringLiteral("DEFAULT_TEST"));

			verifySectionCacheFolder(updatableFile);
			QCOMPARE(updatableFile.getName(), filename);
			QCOMPARE(updatableFile.lookupPath(), QStringLiteral("DEFAULT_TEST"));
		}


		void testIsDirty()
		{
			const QString filename("img_ACS_ACR1252U.png"_L1);
			const QString dirtyFilename = filename + QStringLiteral(".dirty");
			UpdatableFile updatableFile(mSection, filename);

			QVERIFY(!updatableFile.isDirty());

			{
				const auto guard = touchFileInCache(dirtyFilename, updatableFile);
				QVERIFY(updatableFile.isDirty());
			}

			QVERIFY(!updatableFile.isDirty());
		}


		void testMarkDirty()
		{
			const QString filename("img_ACS_ACR1252U.png"_L1);
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
			const QString filename("img_ACS_ACR1252U.png"_L1);
			const QString dirtyFilename = filename + QStringLiteral(".dirty");
			UpdatableFile updatableFile(mSection, filename);

			auto guard = touchFileInCache(dirtyFilename, updatableFile);
			QVERIFY(updatableFile.isDirty());

			guard.dismiss();
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

			{
				const auto guard = touchFileInCache(dirtyFilename, updatableFile);
				QVERIFY(!updatableFile.isDirty());
				QCOMPARE(updatableFile.lookupPath(), QStringLiteral("DEFAULT"));
			}

			QVERIFY(!updatableFile.isDirty());
			QCOMPARE(updatableFile.lookupPath(), QStringLiteral("DEFAULT"));
		}


		void testFileIsDownloadedOnDirty()
		{
			MockDownloader downloader;
			Env::set(Downloader::staticMetaObject, &downloader);

			const QString filename("img_ACS_ACR1252U.png"_L1);
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

			const QString filename("img_updatetest.png"_L1);

			UpdatableFile updatableFile(mSection, filename);
			QSignalSpy spy(&updatableFile, &UpdatableFile::fireUpdated);
			QUrl updateUrl = updatableFile.updateUrl(mSection, filename);
			QByteArray testData = "Testdata";
			downloader.setTestData(updateUrl, testData);

			updatableFile.update();

			QCOMPARE(spy.count(), 1);
			const QString fileName = updatableFile.getName() + QLatin1Char('_') + downloader.getTimeStampString();
			const QString filePath = updatableFile.getSectionCachePath() + QLatin1Char('/') + fileName;
			const auto guard = qScopeGuard([&fileName, &updatableFile]{
					removeFileFromCache(fileName, updatableFile);
				});
			QFile testfile(filePath);
			QVERIFY(testfile.exists());
			QVERIFY(testfile.open(QIODevice::ReadOnly));
			QCOMPARE(testfile.readAll(), downloader.getTestData(updateUrl));
			testfile.close();
		}


		void testNoFileIsCreatedAfterFailedUpdate()
		{
			MockDownloader downloader(GlobalStatus::Code::Downloader_File_Not_Found);
			Env::set(Downloader::staticMetaObject, &downloader);

			const QString filename("img_updatetest.png"_L1);

			UpdatableFile updatableFile(mSection, filename);
			QSignalSpy spy(&updatableFile, &UpdatableFile::fireUpdated);

			updatableFile.update();

			QCOMPARE(spy.count(), 0);
			const QString filePath = updatableFile.getSectionCachePath() + QLatin1Char('/') + updatableFile.getName() + QLatin1Char('_') + downloader.getTimeStampString();
			QFile testfile(filePath);
			QVERIFY(!testfile.exists());
		}


};

QTEST_GUILESS_MAIN(test_UpdatableFile)
#include "test_UpdatableFile.moc"
