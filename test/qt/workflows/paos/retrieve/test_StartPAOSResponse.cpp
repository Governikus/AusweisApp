/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "paos/retrieve/StartPaosResponse.h"

#include "paos/retrieve/PaosParser.h"

#include "TestParserHelper.h"

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

			auto parser = TestParserHelper::create(filename);
			auto* pm = PaosParser().parse(parser).release();
			const std::unique_ptr<StartPaosResponse> startPaosResponseMessage(static_cast<StartPaosResponse*>(pm));

			QCOMPARE(startPaosResponseMessage->mType, PaosType::STARTPAOS_RESPONSE);
			QCOMPARE(startPaosResponseMessage->getResult().getMajor(), major);
			QCOMPARE(startPaosResponseMessage->getResult().getMinor(), minor);
			QCOMPARE(startPaosResponseMessage->getResult().getMessage(), message);
		}


};

QTEST_GUILESS_MAIN(test_StartPAOSResponse)
#include "test_StartPAOSResponse.moc"
