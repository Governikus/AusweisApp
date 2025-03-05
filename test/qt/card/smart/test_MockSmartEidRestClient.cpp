/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "mock/MockSmartEidRestClient.h"

#include "HttpResponse.h"
#include "MockHttpServer.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTest>
#include <QUuid>

#include <eid_applet_results.h>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(HttpResponse)
Q_DECLARE_METATYPE(InitializeResult)
Q_DECLARE_METATYPE(PersonalizationResult)
Q_DECLARE_METATYPE(GenericDataResult)


class test_MockSmartEidRestClient
	: public QObject
{
	Q_OBJECT

	constexpr static auto SMART_EID_MOCK_URL = "AUSWEISAPP2_SMART_EID_MOCK_URL";
	MockHttpServer mServer;

	private Q_SLOTS:
		void init()
		{
			QVERIFY(qputenv(SMART_EID_MOCK_URL, mServer.getAddress().toString().toUtf8()));
		}


		void cleanup()
		{
			mServer.reset();
		}


		void test_preserveUrlPath()
		{
			QVERIFY(qputenv(SMART_EID_MOCK_URL, mServer.getAddress("/foo"_L1).toString().toUtf8()));

			MockSmartEidRestClient restInterface;
			QCOMPARE(restInterface.isEnabled(), true);

			mServer.addMock("/foo/persotest", HttpResponse(HTTP_STATUS_OK, R"({"SessionId":"df40fa42-a40a-402b-bfa1-766f2eeac326","preparePersonalizationData":"deadbeef"})"));
			const auto& createResult = restInterface.createSession();
			QCOMPARE(createResult.mResult, EidServiceResult::SUCCESS);
			QCOMPARE(createResult.mPreparePersonalizationData, "deadbeef");

			mServer.addMock("/foo/persotest/df40fa42-a40a-402b-bfa1-766f2eeac326", HttpResponse(HTTP_STATUS_OK, R"({"commandList":["bar"]})"));
			QCOMPARE(restInterface.sendPersonalizationCommand("bar").mResult, EidServiceResult::SUCCESS);

			mServer.addMock("/foo/persotest/df40fa42-a40a-402b-bfa1-766f2eeac326", HttpResponse(HTTP_STATUS_OK), HTTP_DELETE);
			QCOMPARE(restInterface.deleteSession().mResult, EidServiceResult::SUCCESS);
		}


		void test_isEnabled_data()
		{
			QTest::addColumn<bool>("enabled");

			QTest::addRow("enabled") << true;
			QTest::addRow("disabled") << false;
		}


		void test_isEnabled()
		{
			QFETCH(bool, enabled);

			if (!enabled)
			{
				qunsetenv(SMART_EID_MOCK_URL);
			}

			MockSmartEidRestClient restInterface;
			QCOMPARE(restInterface.isEnabled(), enabled);
		}


		void test_createSession_data()
		{
			QTest::addColumn<HttpResponse>("response");
			QTest::addColumn<InitializeResult>("expectedResult");

			QTest::addRow("Success - Empty Data") << HttpResponse(HTTP_STATUS_OK) << InitializeResult {EidServiceResult::ERROR};
			QTest::addRow("Success - With data") << HttpResponse(HTTP_STATUS_OK, R"({"SessionId":"df40fa42-a40a-402b-bfa1-766f2eeac326","preparePersonalizationData":"YWJjZA=="})") << InitializeResult {EidServiceResult::SUCCESS, "YWJjZA=="};
			QTest::addRow("Success - Session ID format with braces") << HttpResponse(HTTP_STATUS_OK, R"({"SessionId":"{df40fa42-a40a-402b-bfa1-766f2eeac326}","preparePersonalizationData":"YWJjZA=="})") << InitializeResult {EidServiceResult::SUCCESS, "YWJjZA=="};

			QTest::addRow("Error - too many requests") << HttpResponse(HTTP_STATUS_TOO_MANY_REQUESTS) << InitializeResult {EidServiceResult::ERROR};
			QTest::addRow("Error - Unexpected Status Code") << HttpResponse(HTTP_STATUS_INTERNAL_SERVER_ERROR) << InitializeResult {EidServiceResult::ERROR};

			QTest::addRow("False Input - no field SessionId") << HttpResponse(HTTP_STATUS_OK, "{preparePersonalizationData: \"YWJjZA==\"}", "application/json") << InitializeResult {EidServiceResult::ERROR};
			QTest::addRow("False Input - no field prePersonalizationData") << HttpResponse(HTTP_STATUS_OK, "{SessionId: \"abcd-abcd-abcd-abcd\"}", "application/json") << InitializeResult {EidServiceResult::ERROR};
			QTest::addRow("False Input - invalid json") << HttpResponse(HTTP_STATUS_OK, "this is no json", "application/json") << InitializeResult {EidServiceResult::ERROR};
			QTest::addRow("False Input - invalid SessionId") << HttpResponse(HTTP_STATUS_OK, R"({"SessionId":"not a valid uuid"})") << InitializeResult {EidServiceResult::ERROR};
			QTest::addRow("False Input - preparePersonalizationData as JSON") << HttpResponse(HTTP_STATUS_OK, R"({"SessionId":"{df40fa42-a40a-402b-bfa1-766f2eeac326}","preparePersonalizationData":{"foo":"bar", "baz":"foobar"}})") << InitializeResult {EidServiceResult::ERROR};
			QTest::addRow("False Input - preparePersonalizationData as malformed JSON") << HttpResponse(HTTP_STATUS_OK, R"({"SessionId":"{df40fa42-a40a-402b-bfa1-766f2eeac326}","preparePersonalizationData":{this is:malformed}})") << InitializeResult {EidServiceResult::ERROR};
		}


		void test_createSession()
		{
			QFETCH(HttpResponse, response);
			QFETCH(InitializeResult, expectedResult);
			mServer.addMock("/persotest", response);

			MockSmartEidRestClient restInterface;
			QVERIFY(restInterface.isEnabled());
			const auto& result = restInterface.createSession();
			QCOMPARE(result.mResult, expectedResult.mResult);
			QCOMPARE(result.mPreparePersonalizationData, expectedResult.mPreparePersonalizationData);
		}


		void test_sendPersonalizationCommand_data()
		{
			QTest::addColumn<bool>("createSession");
			QTest::addColumn<HttpResponse>("response");
			QTest::addColumn<GenericDataResult>("expectedResult");

			QTest::addRow("Valid session, 1 command in response") << true << HttpResponse(HTTP_STATUS_OK, R"({"commandList":["abcd"]})") << GenericDataResult {EidServiceResult::SUCCESS, "abcd"};

			QTest::addRow("Invalid session") << false << HttpResponse(HTTP_STATUS_NOT_FOUND) << GenericDataResult {EidServiceResult::ERROR};
			QTest::addRow("Invalid input") << true << HttpResponse(HTTP_STATUS_BAD_REQUEST) << GenericDataResult {EidServiceResult::ERROR};
			QTest::addRow("Unexpected status code") << true << HttpResponse(HTTP_STATUS_INTERNAL_SERVER_ERROR) << GenericDataResult {EidServiceResult::ERROR};

			QTest::addRow("Valid session, 0 commands in response") << true << HttpResponse(HTTP_STATUS_OK, R"()") << GenericDataResult {EidServiceResult::ERROR};
			QTest::addRow("Valid session, 2 commands in response") << true << HttpResponse(HTTP_STATUS_OK, R"({"commandList":["abcd","defg"]}])") << GenericDataResult {EidServiceResult::ERROR};
		}


		void test_sendPersonalizationCommand()
		{
			QFETCH(bool, createSession);
			QFETCH(HttpResponse, response);
			QFETCH(GenericDataResult, expectedResult);

			mServer.addMock("/persotest/df40fa42-a40a-402b-bfa1-766f2eeac326", response);

			MockSmartEidRestClient restInterface;
			QVERIFY(restInterface.isEnabled());
			if (createSession)
			{
				mServer.addMock("/persotest", HttpResponse(HTTP_STATUS_OK, R"({"SessionId":"df40fa42-a40a-402b-bfa1-766f2eeac326","preparePersonalizationData":"deadbeef"})"));
				const auto& createResult = restInterface.createSession();
				QCOMPARE(createResult.mResult, EidServiceResult::SUCCESS);
				QCOMPARE(createResult.mPreparePersonalizationData, "deadbeef");
			}
			const auto& result = restInterface.sendPersonalizationCommand("foo bar");
			QCOMPARE(result.mResult, expectedResult.mResult);
			QCOMPARE(result.mData, expectedResult.mData);
		}


		void test_deleteSession_data()
		{
			QTest::addColumn<bool>("createSession");
			QTest::addColumn<HttpResponse>("response");
			QTest::addColumn<PersonalizationResult>("expectedResult");

			QTest::addRow("Valid session") << true << HttpResponse(HTTP_STATUS_OK) << PersonalizationResult {EidServiceResult::SUCCESS};
			QTest::addRow("Invalid session") << true << HttpResponse(HTTP_STATUS_NOT_FOUND) << PersonalizationResult {EidServiceResult::ERROR};
			QTest::addRow("Nonexistent session") << false << HttpResponse(HTTP_STATUS_NOT_FOUND) << PersonalizationResult {EidServiceResult::ERROR};
			QTest::addRow("Unexpected status code") << true << HttpResponse(HTTP_STATUS_INTERNAL_SERVER_ERROR) << PersonalizationResult {EidServiceResult::ERROR};
		}


		void test_deleteSession()
		{
			QFETCH(bool, createSession);
			QFETCH(HttpResponse, response);
			QFETCH(PersonalizationResult, expectedResult);

			mServer.addMock("/persotest/df40fa42-a40a-402b-bfa1-766f2eeac326", response, HTTP_DELETE);

			MockSmartEidRestClient restInterface;
			QVERIFY(restInterface.isEnabled());
			if (createSession)
			{
				mServer.addMock("/persotest", HttpResponse(HTTP_STATUS_OK, R"({"SessionId":"df40fa42-a40a-402b-bfa1-766f2eeac326","preparePersonalizationData":"deadbeef"})"));
				const auto& createResult = restInterface.createSession();
				QCOMPARE(createResult.mResult, EidServiceResult::SUCCESS);
				QCOMPARE(createResult.mPreparePersonalizationData, "deadbeef");
			}
			const auto& result = restInterface.deleteSession();
			QCOMPARE(result.mResult, expectedResult.mResult);
			QCOMPARE(std::string(), expectedResult.mInitPIN);
		}


		void test_callCreateTwice()
		{
			MockSmartEidRestClient restInterface;
			QVERIFY(restInterface.isEnabled());

			mServer.addMock("/persotest", HttpResponse(HTTP_STATUS_OK, R"({"SessionId":"a90b3add-5c1b-4350-b7cd-4c5c81833b5a","preparePersonalizationData":"deadbeef"})"));
			const auto& result1 = restInterface.createSession();
			QCOMPARE(result1.mResult, EidServiceResult::SUCCESS);
			QCOMPARE(result1.mPreparePersonalizationData, "deadbeef");

			QTest::ignoreMessage(QtWarningMsg, "Existing Session \"a90b3add-5c1b-4350-b7cd-4c5c81833b5a\" was not deleted!");
			mServer.addMock("/persotest", HttpResponse(HTTP_STATUS_OK, R"({"SessionId":"df40fa42-a40a-402b-bfa1-766f2eeac326","preparePersonalizationData":"deadbeef"})"));
			const auto& result2 = restInterface.createSession();
			QCOMPARE(result2.mResult, EidServiceResult::SUCCESS);
			QCOMPARE(result2.mPreparePersonalizationData, "deadbeef");
		}


};

QTEST_GUILESS_MAIN(test_MockSmartEidRestClient)
#include "test_MockSmartEidRestClient.moc"
