/*
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include <QtTest>

#include "mock/eid_applet_interface_mock.h"

#include <eid_applet_interface.h>

using namespace governikus;

Q_DECLARE_METATYPE(EidUpdateInfo)
Q_DECLARE_METATYPE(EidStatus)
Q_DECLARE_METATYPE(EidServiceResult)

class test_eid_applet_interface_mock
	: public QObject
{
	Q_OBJECT

	constexpr static auto SMART_EID_APPLET_MOCK = "AUSWEISAPP2_SMART_EID_MOCK_URL";

	private Q_SLOTS:
		void testInitialReturnValues_data()
		{
			QTest::addColumn<bool>("restInterfaceEnabled");
			QTest::addColumn<EidUpdateInfo>("updateInfo");
			QTest::addColumn<EidStatus>("eidStatus");
			QTest::addColumn<EidServiceResult>("installResult");
			QTest::addColumn<EidServiceResult>("deleteSmartResult");
			QTest::addColumn<EidServiceResult>("deletePersonalizationResult");

			QTest::addRow("Rest Enabled") << true << EidUpdateInfo::UP_TO_DATE << EidStatus::NO_PERSONALIZATION << EidServiceResult::SUCCESS << EidServiceResult::SUCCESS << EidServiceResult::SUCCESS;
			QTest::addRow("Rest disabled") << false << EidUpdateInfo::UNAVAILABLE << EidStatus::UNAVAILABLE << EidServiceResult::UNSUPPORTED << EidServiceResult::UNSUPPORTED << EidServiceResult::UNSUPPORTED;
		}


		void testInitialReturnValues()
		{
			QFETCH(bool, restInterfaceEnabled);
			QFETCH(EidUpdateInfo, updateInfo);
			QFETCH(EidStatus, eidStatus);
			QFETCH(EidServiceResult, installResult);
			QFETCH(EidServiceResult, deleteSmartResult);
			QFETCH(EidServiceResult, deletePersonalizationResult);

			if (restInterfaceEnabled)
			{
				QVERIFY(qputenv(SMART_EID_APPLET_MOCK, "http://localhost:12345"));
			}
			else
			{
				QVERIFY(qunsetenv(SMART_EID_APPLET_MOCK));
			}
			initMock();

			QCOMPARE(getUpdateInfo(), updateInfo);
			QCOMPARE(getSmartEidStatus(), eidStatus);
			QCOMPARE(installSmartEid(ProgressHandler()), installResult);
			QCOMPARE(deleteSmartEid(ProgressHandler()), deleteSmartResult);
			QCOMPARE(deletePersonalization(), deletePersonalizationResult);
		}


};

QTEST_GUILESS_MAIN(test_eid_applet_interface_mock)
#include "test_eid_applet_interface_mock.moc"
