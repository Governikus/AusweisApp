/*!
 * \brief Unit tests for \InitializeFramework
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "TestFileHelper.h"
#include "paos/retrieve/StartPaosResponse.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;

class test_StartPAOSResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parsing_data()
		{
			QTest::addColumn<QString>("filename");
			QTest::addColumn<ECardApiResult::Major>("major");
			QTest::addColumn<ECardApiResult::Minor>("minor");
			QTest::addColumn<QString>("message");
			QTest::addColumn<int>("mRemainingDays");
			QTest::addColumn<int>("mRemainingAttempts");
			QTest::addColumn<QString>("mBlockingCode");

			QTest::newRow("Major") << ":paos/StartPAOSResponse1.xml" << ECardApiResult::Major::Ok << ECardApiResult::Minor::null << QString() << -1 << -1 << QString();
			QTest::newRow("MajorMinor") << ":paos/StartPAOSResponse2.xml" << ECardApiResult::Major::Error << ECardApiResult::Minor::DP_Timeout_Error << QString() << -1 << -1 << QString();
			QTest::newRow("MajorMinorMessage") << ":paos/StartPAOSResponse3.xml" << ECardApiResult::Major::Error << ECardApiResult::Minor::DP_Timeout_Error << QString("Detail message") << -1 << -1 << QString();
			QTest::newRow("AdditionalInformation") << ":paos/StartPAOSResponse4.xml" << ECardApiResult::Major::Ok << ECardApiResult::Minor::null << QString() << 30 << 9 << QString("LASTWAGEN");
		}


		void parsing()
		{
			QFETCH(QString, filename);
			QFETCH(ECardApiResult::Major, major);
			QFETCH(ECardApiResult::Minor, minor);
			QFETCH(QString, message);
			QFETCH(int, mRemainingDays);
			QFETCH(int, mRemainingAttempts);
			QFETCH(QString, mBlockingCode);

			QByteArray content = TestFileHelper::readFile(filename);
			StartPaosResponse startPaosResponse(content);
			QCOMPARE(startPaosResponse.getResult().getMajor(), major);
			QCOMPARE(startPaosResponse.getResult().getMinor(), minor);
			QCOMPARE(startPaosResponse.getResult().getMessage(), message);
			QCOMPARE(startPaosResponse.getRemainingDays(), mRemainingDays);
			QCOMPARE(startPaosResponse.getRemainingAttempts(), mRemainingAttempts);
			QCOMPARE(startPaosResponse.getBlockingCode(), mBlockingCode);
		}


};

QTEST_GUILESS_MAIN(test_StartPAOSResponse)
#include "test_StartPAOSResponse.moc"
