/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref DiagnosisAntivirusDetection
 */

#include "DiagnosisAntivirusDetection.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
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
			const QString filePath = QStringLiteral(":/core/diagnosis/") + pFilename;
			const QByteArray rawData = TestFileHelper::readFile(filePath);
			QString stringData = QString::fromUtf8(rawData);

			Q_ASSERT(stringData.count("\r\n"_L1) == 0);
			const qsizetype linebreaks = stringData.count('\n'_L1);
			stringData.replace('\n'_L1, "\r\n"_L1);
			Q_ASSERT(stringData.count("\r\n"_L1) == linebreaks);

			return stringData;
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
			mAntivirusDetection->parseAntivirInfos(QString());
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
			QCOMPARE(antivirusInfos[0]->getDisplayName(), "Windows Defender"_L1);
			QCOMPARE(antivirusInfos[0]->getLastUpdate(), "Mon, 26 Nov 2018 10:34:23 GMT"_L1);
			QCOMPARE(antivirusInfos[0]->getExePath(), "windowsdefender://"_L1);
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
			QCOMPARE(antivirusInfos[0]->getDisplayName(), "Windows Defender"_L1);
			QCOMPARE(antivirusInfos[0]->getLastUpdate(), QString());
			QCOMPARE(antivirusInfos[0]->getExePath(), "windowsdefender://"_L1);
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
			QCOMPARE(antivirusInfos[0]->getDisplayName(), "BullGuard Antivirus"_L1);
			QCOMPARE(antivirusInfos[0]->getLastUpdate(), "Fri, 30 Nov 2018 15:04:13 GMT"_L1);
			QCOMPARE(antivirusInfos[0]->getExePath(), "C:\\Program Files\\BullGuard Ltd\\BullGuard\\BullGuard.exe"_L1);
			QCOMPARE(antivirusInfos[1]->getDisplayName(), "Windows Defender"_L1);
			QCOMPARE(antivirusInfos[1]->getLastUpdate(), "Mon, 26 Nov 2018 10:34:23 GMT"_L1);
			QCOMPARE(antivirusInfos[1]->getExePath(), "windowsdefender://"_L1);
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
