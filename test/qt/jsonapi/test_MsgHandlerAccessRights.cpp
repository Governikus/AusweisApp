/*!
 * \brief Unit tests for \ref MsgHandlerAccessRights
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "messages/MsgHandlerAccessRights.h"

#include "InternalActivationContext.h"
#include "MessageDispatcher.h"

#include "TestAuthContext.h"
#include <QtTest>

using namespace governikus;

class test_MsgHandlerAccessRights
	: public QObject
{
	Q_OBJECT

	QSharedPointer<CHAT> getChat(const std::initializer_list<AccessRight>& pList)
	{
		auto chat = newObject<CHAT>();
		chat->setAccessRights(QSet<AccessRight>(pList));
		return chat;
	}


	QSharedPointer<TestAuthContext> getContextWithChat()
	{
		QSharedPointer<TestAuthContext> context(new TestAuthContext(new InternalActivationContext(QUrl("http://dummy")), ":/paos/DIDAuthenticateEAC1.xml"));
		context->setRequiredAccessRights({AccessRight::READ_DG01, AccessRight::READ_DG04, AccessRight::READ_DG17});
		context->setOptionalAccessRights({AccessRight::AGE_VERIFICATION, AccessRight::READ_DG05});
		return context;
	}


#define AUX R"({"aux":{"ageVerificationDate":"1992-12-06","communityId":"02760400110000","requiredAge":"24","validityDate":"2013-12-06"},)"

	private Q_SLOTS:
		void nonExistingTransactionInfo()
		{
			QSharedPointer<TestAuthContext> context(new TestAuthContext(new InternalActivationContext(QUrl("http://dummy")), ":/paos/DIDAuthenticateEAC1_2.xml"));
			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QCOMPARE(dispatcher.processStateChange("StateEditAccessRights"),
					QByteArray(R"({"chat":{"effective":["ResidencePermitI","Address","BirthName","Nationality","PlaceOfBirth","DateOfBirth","DoctoralDegree","ArtisticName","FamilyName","GivenNames","ValidUntil","IssuingCountry","DocumentType","Pseudonym"],"optional":["ResidencePermitI","Address","BirthName","Nationality","PlaceOfBirth","DateOfBirth","DoctoralDegree","ArtisticName","FamilyName","GivenNames","ValidUntil","IssuingCountry","DocumentType","Pseudonym"],"required":[]},"msg":"ACCESS_RIGHTS"})"));
		}


		void state()
		{
			MessageDispatcher dispatcher;
			dispatcher.init(getContextWithChat());

			QCOMPARE(dispatcher.processStateChange("StateEditAccessRights"),
					QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));
		}


		void getAccessRightsBadState()
		{
			MessageDispatcher dispatcher;

			const auto& msg = QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"GET_ACCESS_RIGHTS","msg":"BAD_STATE"})"));

			dispatcher.init(getContextWithChat());
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"GET_ACCESS_RIGHTS","msg":"BAD_STATE"})"));
		}


		void acceptAccessRights()
		{
			const auto& context = getContextWithChat();
			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QVERIFY(!context->isStateApproved());
			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());

			QVERIFY(!context->isStateApproved());
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "ACCEPT"}   )")), QByteArray());
			QVERIFY(context->isStateApproved());
		}


		void getAccessRights()
		{
			MessageDispatcher dispatcher;
			dispatcher.init(getContextWithChat());

			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));
		}


		void setAccessRights_data()
		{
			QTest::addColumn<QByteArray>("cmd");
			QTest::addColumn<QByteArray>("msg");

			QTest::newRow("chat_invalid") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["8",11]}   )")
										  << QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data needs to be string","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_needs_be_string") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": [8,"11"]}   )")
												  << QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data is invalid","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_unknown_id") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["y", "123"]}   )")
											 << QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data is invalid","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_set_optional") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["FamilyName"]}   )")
											   << QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_mixed_valid_and_required") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification", "GivenNames"]}   )")
														   << QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data is not available","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_both_optional") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification", "FamilyName"]}   )")
												<< QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_single_optional") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification"]}   )")
												  << QByteArray(AUX R"("chat":{"effective":["Address","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_disable_optional") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": []}   )")
												   << QByteArray(AUX R"("chat":{"effective":["Address","GivenNames","DocumentType"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");
		}


		void setAccessRights()
		{
			QFETCH(QByteArray, cmd);
			QFETCH(QByteArray, msg);

			MessageDispatcher dispatcher;
			dispatcher.init(getContextWithChat());
			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());

			// check original state
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			// check cmd
			QCOMPARE(dispatcher.processCommand(cmd), msg);
		}


		void setAccessRightsWithMultipleCmds()
		{
			MessageDispatcher dispatcher;
			dispatcher.init(getContextWithChat());
			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());

			// check original state
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			// check cmds
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["FamilyName"]}   )")),
					QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			// 11 is not valid, 0 is valid ... we do not accept partial valid values!
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification", "GivenNames"]}   )")),
					QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data is not available","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));
		}


		void setAccessRightsWithoutChat()
		{
			const auto& context = getContextWithChat();
			context->setOptionalAccessRights({});

			MessageDispatcher dispatcher;
			dispatcher.init(context);
			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());

			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification"]}   )")),
					QByteArray(AUX R"("chat":{"effective":["Address","GivenNames","DocumentType"],"optional":[],"required":["Address","GivenNames","DocumentType"]},"error":"No optional access rights available","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			context->setOptionalAccessRights({AccessRight::AGE_VERIFICATION});
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification"]}   )")),
					QByteArray(AUX R"("chat":{"effective":["Address","GivenNames","DocumentType","AgeVerification"],"optional":["AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			context->setRequiredAccessRights({AccessRight::AGE_VERIFICATION, AccessRight::READ_DG17});
			context->setOptionalAccessRights({});
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification"]}   )")),
					QByteArray(AUX R"("chat":{"effective":["Address","AgeVerification"],"optional":[],"required":["Address","AgeVerification"]},"error":"No optional access rights available","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));
		}


		void crap_data()
		{
			QTest::addColumn<QByteArray>("cmd");
			QTest::addColumn<QByteArray>("msg");

			QTest::newRow("chat_null") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": null}   )")
									   << QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Invalid 'chat' data","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("CHAT") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "CHAT": []}   )")
								  << QByteArray(AUX R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"'chat' cannot be undefined","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");
		}


		void crap()
		{
			QFETCH(QByteArray, cmd);
			QFETCH(QByteArray, msg);

			MessageDispatcher dispatcher;
			dispatcher.init(getContextWithChat());
			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());

			QCOMPARE(dispatcher.processCommand(cmd), msg);
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerAccessRights)
#include "test_MsgHandlerAccessRights.moc"
