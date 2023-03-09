/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "eid_applet_interface.h"

#include "mock/eid_applet_interface_mock.h"

#include <QtTest>

using namespace governikus;

class test_EidAppletService
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void cleanup()
		{
			setEphemeralResult();
		}


		void getSetSmartEidStatus()
		{
			QCOMPARE(getSmartEidStatus(), EidStatus::UNAVAILABLE);
			setSmartEidStatus(EidStatus::PERSONALIZED);
			QCOMPARE(getSmartEidStatus(), EidStatus::PERSONALIZED);
			QCOMPARE(getSmartEidStatus(), EidStatus::PERSONALIZED);
		}


		void getSetAPDUCommand_data()
		{
			QTest::addColumn<bool>("ephemeral");

			QTest::newRow("true") << true;
			QTest::newRow("false") << false;
		}


		void getSetAPDUCommand()
		{
			QFETCH(bool, ephemeral);
			setEphemeralResult(ephemeral);

			auto performResult = performAPDUCommand(std::string());
			QCOMPARE(performResult.mResult, EidServiceResult::UNDEFINED);
			QCOMPARE(performResult.mData, std::string());

			const GenericDataResult result({EidServiceResult::INFO, std::string("dummy")});
			setApduCommandResult(result);
			if (!ephemeral)
			{
				// just to check if assert is disabled
				setApduCommandResult(result);
			}

			performResult = performAPDUCommand(std::string());
			QCOMPARE(performResult.mResult, EidServiceResult::INFO);
			QCOMPARE(performResult.mData, std::string("dummy"));

			performResult = performAPDUCommand(std::string());
			if (ephemeral)
			{
				QCOMPARE(performResult.mResult, EidServiceResult::UNDEFINED);
				QCOMPARE(performResult.mData, std::string());
			}
			else
			{
				QCOMPARE(performResult.mResult, EidServiceResult::INFO);
				QCOMPARE(performResult.mData, std::string("dummy"));
			}
		}


		void getSetPersonalization_data()
		{
			QTest::addColumn<bool>("ephemeral");

			QTest::newRow("true") << true;
			QTest::newRow("false") << false;
		}


		void getSetPersonalization()
		{
			QFETCH(bool, ephemeral);
			setEphemeralResult(ephemeral);

			auto performResult = performPersonalization(std::string());
			QCOMPARE(performResult.mResult, EidServiceResult::UNDEFINED);
			QCOMPARE(performResult.mData, std::string());

			const GenericDataResult result({EidServiceResult::INFO, std::string("dummy")});
			setPersonalizationResult(result);
			if (!ephemeral)
			{
				// just to check if assert is disabled
				setPersonalizationResult(result);
			}

			performResult = performPersonalization(std::string());
			QCOMPARE(performResult.mResult, EidServiceResult::INFO);
			QCOMPARE(performResult.mData, std::string("dummy"));

			performResult = performPersonalization(std::string());
			if (ephemeral)
			{
				QCOMPARE(performResult.mResult, EidServiceResult::UNDEFINED);
				QCOMPARE(performResult.mData, std::string());
			}
			else
			{
				QCOMPARE(performResult.mResult, EidServiceResult::INFO);
				QCOMPARE(performResult.mData, std::string("dummy"));
			}
		}


		void getSetPrePersonalization_data()
		{
			QTest::addColumn<bool>("ephemeral");

			QTest::newRow("true") << true;
			QTest::newRow("false") << false;
		}


		void getSetPrePersonalization()
		{
			QFETCH(bool, ephemeral);
			setEphemeralResult(ephemeral);

			auto performResult = initializePersonalization(std::string(), std::string());
			QCOMPARE(performResult.mResult, EidServiceResult::UNDEFINED);
			QCOMPARE(performResult.mPreparePersonalizationData, std::string());

			const InitializeResult result({EidServiceResult::INFO, std::string("data")});
			setInitializePersonalizationResult(result);
			if (!ephemeral)
			{
				// just to check if assert is disabled
				setInitializePersonalizationResult(result);
			}

			performResult = initializePersonalization(std::string(), std::string());
			QCOMPARE(performResult.mResult, EidServiceResult::INFO);
			QCOMPARE(performResult.mPreparePersonalizationData, std::string("data"));

			performResult = initializePersonalization(std::string(), std::string());
			if (ephemeral)
			{
				QCOMPARE(performResult.mResult, EidServiceResult::UNDEFINED);
				QCOMPARE(performResult.mPreparePersonalizationData, std::string());
			}
			else
			{
				QCOMPARE(performResult.mResult, EidServiceResult::INFO);
				QCOMPARE(performResult.mPreparePersonalizationData, std::string("data"));
			}
		}


};

QTEST_GUILESS_MAIN(test_EidAppletService)
#include "test_eid_applet_service.moc"
