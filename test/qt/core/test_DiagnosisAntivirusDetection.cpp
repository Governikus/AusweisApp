/*!
 * \brief Unit tests for \ref DiagnosisAntivirusDetection
 *
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisAntivirusDetection.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

class test_DiagnosisAntivirusDetection
	: public QObject
{
	Q_OBJECT
	QSharedPointer<DiagnosisAntivirusDetection> mAntivirusDetection;

	private:
		const QString mTestFileLocation = QStringLiteral(":/core/diagnosis/");
		QString getTestData(const QString& pFilename)
		{
			QString filePath = mTestFileLocation + pFilename;
			QByteArray rawData = TestFileHelper::readFile(filePath);
			return QString::fromUtf8(rawData);
		}

	private Q_SLOTS:
		void init()
		{
			mAntivirusDetection.reset(new DiagnosisAntivirusDetection());
		}


		void cleanup()
		{
			mAntivirusDetection.clear();
		}


		void test_parsingNullAntivirusInformation()
		{
			QSignalSpy spy(mAntivirusDetection.data(), &DiagnosisAntivirusDetection::fireAntivirusInformationChanged);
			QVERIFY(mAntivirusDetection->getAntivirusInformations().empty());
			QCOMPARE(spy.count(), 0);
			mAntivirusDetection->parseAntivirInfos("");
			QVERIFY(mAntivirusDetection->getAntivirusInformations().empty());
			QCOMPARE(spy.count(), 1);
		}


		void test_parsingOneAntivirusInformation()
		{
			QSignalSpy spy(mAntivirusDetection.data(), &DiagnosisAntivirusDetection::fireAntivirusInformationChanged);
			QVERIFY(mAntivirusDetection->getAntivirusInformations().empty());
			QCOMPARE(spy.count(), 0);

			const QString& fileContent = getTestData(QStringLiteral("antivir_one_antivirus.txt"));
			mAntivirusDetection->parseAntivirInfos(fileContent);
			auto antivirusInfos = mAntivirusDetection->getAntivirusInformations();
			QVERIFY(!antivirusInfos.empty());
			QCOMPARE(antivirusInfos[0]->getDisplayName(), QString("Windows Defender"));
			QCOMPARE(antivirusInfos[0]->getLastUpdate(), QString("Mon, 26 Nov 2018 10:34:23 GMT"));
			QCOMPARE(antivirusInfos[0]->getExePath(), QString("windowsdefender://"));
			QCOMPARE(spy.count(), 1);
		}


		void test_parsingOneAntivirusInformationMissingTimestamp()
		{
			QSignalSpy spy(mAntivirusDetection.data(), &DiagnosisAntivirusDetection::fireAntivirusInformationChanged);
			QVERIFY(mAntivirusDetection->getAntivirusInformations().empty());
			QCOMPARE(spy.count(), 0);

			const QString& fileContent = getTestData(QStringLiteral("antivir_one_antivirus_missing_timestamp.txt"));
			mAntivirusDetection->parseAntivirInfos(fileContent);
			auto antivirusInfos = mAntivirusDetection->getAntivirusInformations();
			QVERIFY(!antivirusInfos.empty());
			QCOMPARE(antivirusInfos[0]->getDisplayName(), QString("Windows Defender"));
			QCOMPARE(antivirusInfos[0]->getLastUpdate(), QString(""));
			QCOMPARE(antivirusInfos[0]->getExePath(), QString("windowsdefender://"));
			QCOMPARE(spy.count(), 1);
		}


		void test_parsingTwoAntivirusInformation()
		{
			QSignalSpy spy(mAntivirusDetection.data(), &DiagnosisAntivirusDetection::fireAntivirusInformationChanged);
			QVERIFY(mAntivirusDetection->getAntivirusInformations().empty());
			QCOMPARE(spy.count(), 0);

			const QString& fileContent = getTestData(QStringLiteral("antivir_two_antivirus.txt"));
			mAntivirusDetection->parseAntivirInfos(fileContent);
			auto antivirusInfos = mAntivirusDetection->getAntivirusInformations();
			QVERIFY(!antivirusInfos.empty());
			QCOMPARE(antivirusInfos[0]->getDisplayName(), QString("BullGuard Antivirus"));
			QCOMPARE(antivirusInfos[0]->getLastUpdate(), QString("Fri, 30 Nov 2018 15:04:13 GMT"));
			QCOMPARE(antivirusInfos[0]->getExePath(), QString("C:\\Program Files\\BullGuard Ltd\\BullGuard\\BullGuard.exe"));
			QCOMPARE(antivirusInfos[1]->getDisplayName(), QString("Windows Defender"));
			QCOMPARE(antivirusInfos[1]->getLastUpdate(), QString("Mon, 26 Nov 2018 10:34:23 GMT"));
			QCOMPARE(antivirusInfos[1]->getExePath(), QString("windowsdefender://"));
			QCOMPARE(spy.count(), 1);
		}


		void test_parsingTwoBrokenAntivirusInformation()
		{
			QSignalSpy spy(mAntivirusDetection.data(), &DiagnosisAntivirusDetection::fireAntivirusInformationChanged);
			QVERIFY(mAntivirusDetection->getAntivirusInformations().empty());
			QCOMPARE(spy.count(), 0);

			const QString& fileContent = getTestData(QStringLiteral("antivir_two_broken_antivirus.txt"));
			mAntivirusDetection->parseAntivirInfos(fileContent);
			auto antivirusInfos = mAntivirusDetection->getAntivirusInformations();
			QVERIFY(antivirusInfos.empty());
			QCOMPARE(spy.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_DiagnosisAntivirusDetection)
#include "test_DiagnosisAntivirusDetection.moc"
