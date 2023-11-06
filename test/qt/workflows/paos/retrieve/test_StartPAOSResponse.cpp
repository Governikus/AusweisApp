/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \InitializeFramework
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

			QTest::newRow("Major") << ":paos/StartPAOSResponse1.xml" << ECardApiResult::Major::Ok << ECardApiResult::Minor::null << QString();
			QTest::newRow("MajorMinor") << ":paos/StartPAOSResponse2.xml" << ECardApiResult::Major::Error << ECardApiResult::Minor::DP_Timeout_Error << QString();
			QTest::newRow("MajorMinorMessage") << ":paos/StartPAOSResponse3.xml" << ECardApiResult::Major::Error << ECardApiResult::Minor::DP_Timeout_Error << QString("Detail message");
		}


		void parsing()
		{
			QFETCH(QString, filename);
			QFETCH(ECardApiResult::Major, major);
			QFETCH(ECardApiResult::Minor, minor);
			QFETCH(QString, message);

			QByteArray content = TestFileHelper::readFile(filename);
			StartPaosResponse startPaosResponse(content);
			QCOMPARE(startPaosResponse.getResult().getMajor(), major);
			QCOMPARE(startPaosResponse.getResult().getMinor(), minor);
			QCOMPARE(startPaosResponse.getResult().getMessage(), message);
		}


};

QTEST_GUILESS_MAIN(test_StartPAOSResponse)
#include "test_StartPAOSResponse.moc"
