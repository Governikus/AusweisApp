/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "FileProvider.h"

#include <QtTest/QtTest>

using namespace governikus;

class test_FileProvider
	: public QObject
{
	Q_OBJECT

	private:
		const QString mSection;
		const QString mName1;
		const QString mName2;
		const QString mDefaultPath;

	private Q_SLOTS:
		void testProviderReturnsMeaningfulUpdatableFiles()
		{
			const auto fileProvider = Env::getSingleton<FileProvider>();
			const QSharedPointer<UpdatableFile> updatableFile1 = fileProvider->getFile(mSection, mName1, mDefaultPath);
			QVERIFY(updatableFile1);
			QCOMPARE(updatableFile1->lookupPath(), QStringLiteral(":/updatable-files/reader/img_ACS_ACR1252U.png"));

			const QSharedPointer<UpdatableFile> updatableFile2 = fileProvider->getFile(mSection, mName2, mDefaultPath);
			QVERIFY(updatableFile2);
			QCOMPARE(updatableFile2->lookupPath(), mDefaultPath);
		}


		void testProviderReturnsUniqueUpdatableFiles()
		{
			const auto fileProvider = Env::getSingleton<FileProvider>();
			const QSharedPointer<UpdatableFile> updatableFile11 = fileProvider->getFile(mSection, mName1, mDefaultPath);
			const QSharedPointer<UpdatableFile> updatableFile12 = fileProvider->getFile(mSection, mName1, mDefaultPath);
			const QSharedPointer<UpdatableFile> updatableFile2 = fileProvider->getFile(mSection, mName2, mDefaultPath);

			QVERIFY(updatableFile11);
			QVERIFY(updatableFile12);
			QVERIFY(updatableFile2);

			QVERIFY(updatableFile11.data() == updatableFile12.data());
			QVERIFY(updatableFile11.data() != updatableFile2.data());
		}


	public:
		test_FileProvider()
			: mSection("reader")
			, mName1("img_ACS_ACR1252U.png")
			, mName2("img_ACS_ACR1252V.png")
			, mDefaultPath(":/images/reader/default_no_reader.png")
		{
		}


};

QTEST_GUILESS_MAIN(test_FileProvider)
#include "test_FileProvider.moc"
