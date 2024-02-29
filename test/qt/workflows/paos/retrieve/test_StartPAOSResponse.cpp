/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \InitializeFramework
 */

#include "TestFileHelper.h"
#include "paos/retrieve/StartPaosResponse.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_StartPAOSResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parsing_data()
		{
			QTest::addColumn<QLatin1String>("filename");
			QTest::addColumn<ECardApiResult::Major>("major");
			QTest::addColumn<ECardApiResult::Minor>("minor");
			QTest::addColumn<QLatin1String>("message");

			QTest::newRow("Major") << ":paos/StartPAOSResponse1.xml"_L1 << ECardApiResult::Major::Ok << ECardApiResult::Minor::null << QLatin1String();
			QTest::newRow("MajorMinor") << ":paos/StartPAOSResponse2.xml"_L1 << ECardApiResult::Major::Error << ECardApiResult::Minor::DP_Timeout_Error << QLatin1String();
			QTest::newRow("MajorMinorMessage") << ":paos/StartPAOSResponse3.xml"_L1 << ECardApiResult::Major::Error << ECardApiResult::Minor::DP_Timeout_Error << "Detail message"_L1;
		}


		void parsing()
		{
			QFETCH(QLatin1String, filename);
			QFETCH(ECardApiResult::Major, major);
			QFETCH(ECardApiResult::Minor, minor);
			QFETCH(QLatin1String, message);

			QByteArray content = TestFileHelper::readFile(filename);
			StartPaosResponse startPaosResponse(content);
			QCOMPARE(startPaosResponse.getResult().getMajor(), major);
			QCOMPARE(startPaosResponse.getResult().getMinor(), minor);
			QCOMPARE(startPaosResponse.getResult().getMessage(), message);
		}


};

QTEST_GUILESS_MAIN(test_StartPAOSResponse)
#include "test_StartPAOSResponse.moc"
