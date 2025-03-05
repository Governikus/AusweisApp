/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "FailureCode.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(FailureCode::FailureInfoMap)


class test_FailureCode
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_getReason()
		{
			FailureCode failureCode(FailureCode::Reason::User_Cancelled);
			QCOMPARE(failureCode.getReason(), FailureCode::Reason::User_Cancelled);
		}


		void test_isEqual()
		{
			const FailureCode failureCode {FailureCode::Reason::User_Cancelled};
			const FailureCode comparisonCode {FailureCode::Reason::Card_Removed};

			QVERIFY(failureCode == failureCode);
			QVERIFY(failureCode != comparisonCode);
		}


		void test_getFailureInfoMap_data()
		{
			QTest::addColumn<FailureCode::FailureInfoMap>("failureInfoMap");

			QTest::newRow("Empty") << FailureCode::FailureInfoMap();
			QTest::newRow("One entry") << FailureCode::FailureInfoMap({
						{FailureCode::Info::State_Name, "MyStateName"_L1}
					});
			QTest::newRow("Two entries") << FailureCode::FailureInfoMap({
						{FailureCode::Info::State_Name, "MyStateName"_L1},
						{FailureCode::Info::Card_Return_Code, "MyReturnCode"_L1}
					});
		}


		void test_getFailureInfoMap()
		{
			QFETCH(FailureCode::FailureInfoMap, failureInfoMap);
			const FailureCode failureCode {FailureCode::Reason::User_Cancelled, failureInfoMap};

			QVERIFY(failureCode.getFailureInfoMap() == failureInfoMap);
		}


};

QTEST_GUILESS_MAIN(test_FailureCode)
#include "test_FailureCode.moc"
