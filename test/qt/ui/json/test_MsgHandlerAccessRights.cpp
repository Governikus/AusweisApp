/*!
 * \brief Unit tests for \ref MsgHandlerAccessRights
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerAccessRights.h"

#include "AppSettings.h"
#include "InternalActivationContext.h"
#include "MessageDispatcher.h"
#include "VolatileSettings.h"
#include "states/StateEditAccessRights.h"

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


	QSharedPointer<TestAuthContext> getContextWithChat(bool pCanAllowed = false)
	{
		Env::getSingleton<AppSettings>()->getGeneralSettings().setEnableCanAllowed(pCanAllowed);
		QSharedPointer<ActivationContext> activationContext(new InternalActivationContext(QUrl("http://dummy")));
		QSharedPointer<TestAuthContext> context(new TestAuthContext(activationContext, ":/paos/DIDAuthenticateEAC1.xml"));
		context->setRequiredAccessRights({AccessRight::READ_DG01, AccessRight::READ_DG04, AccessRight::READ_DG17});
		context->setOptionalAccessRights({AccessRight::AGE_VERIFICATION, AccessRight::READ_DG05});
		Env::getSingleton<AppSettings>()->getGeneralSettings().setEnableCanAllowed(false);
		return context;
	}


	[[nodiscard]] QByteArray getAux() const
	{
		const QDate today = QDateTime::currentDateTimeUtc().date();
		const QDate ageVerificationDate = QDate(1992, 12, 6);

		const QByteArray age =
				(today.month() > ageVerificationDate.month() || (today.month() == ageVerificationDate.month() && today.day() >= ageVerificationDate.day()))
				? QByteArray::number(today.year() - ageVerificationDate.year())
				: QByteArray::number(today.year() - ageVerificationDate.year() - 1);

		QByteArray aux(R"({"aux":{"ageVerificationDate":"%1","communityId":"02760400110000","requiredAge":"%2","validityDate":"2013-12-06"},)");
		aux.replace(QByteArrayLiteral("%1"), ageVerificationDate.toString(Qt::ISODate).toLatin1());
		aux.replace(QByteArrayLiteral("%2"), age);
		return aux;
	}

	private Q_SLOTS:
		void init()
		{
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);
		}


		void nonExistingTransactionInfo()
		{
			QSharedPointer<ActivationContext> activationContext(new InternalActivationContext(QUrl("http://dummy")));
			QSharedPointer<TestAuthContext> context(new TestAuthContext(activationContext, ":/paos/DIDAuthenticateEAC1_2.xml"));
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::AUTH);

			QCOMPARE(dispatcher.processStateChange(AbstractState::getClassName<StateEditAccessRights>()),
					QByteArray(R"({"chat":{"effective":["WriteAddress","WriteCommunityID","WriteResidencePermitI","WriteResidencePermitII","ResidencePermitII","ResidencePermitI","CommunityID","Address","BirthName","Nationality","PlaceOfBirth","DateOfBirth","DoctoralDegree","ArtisticName","FamilyName","GivenNames","ValidUntil","IssuingCountry","DocumentType","PinManagement","CanAllowed","Pseudonym"],"optional":["WriteAddress","WriteCommunityID","WriteResidencePermitI","WriteResidencePermitII","ResidencePermitII","ResidencePermitI","CommunityID","Address","BirthName","Nationality","PlaceOfBirth","DateOfBirth","DoctoralDegree","ArtisticName","FamilyName","GivenNames","ValidUntil","IssuingCountry","DocumentType","PinManagement","CanAllowed","Pseudonym"],"required":[]},"msg":"ACCESS_RIGHTS"})"));
		}


		void state()
		{
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(getContextWithChat()), MsgType::AUTH);

			QCOMPARE(dispatcher.processStateChange(AbstractState::getClassName<StateEditAccessRights>()),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));
		}


		void getAccessRightsBadState()
		{
			MessageDispatcher dispatcher;

			const auto& msg = QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"GET_ACCESS_RIGHTS","msg":"BAD_STATE"})"));

			QCOMPARE(dispatcher.init(getContextWithChat()), MsgType::AUTH);
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"GET_ACCESS_RIGHTS","msg":"BAD_STATE"})"));
		}


		void acceptAccessRights()
		{
			const auto& context = getContextWithChat();
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::AUTH);

			QVERIFY(!context->isStateApproved());
			QVERIFY(!QByteArray(dispatcher.processStateChange(AbstractState::getClassName<StateEditAccessRights>())).isEmpty());

			QVERIFY(!context->isStateApproved());
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "ACCEPT"}   )")), QByteArray());
			QVERIFY(context->isStateApproved());
		}


		void getAccessRights()
		{
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(getContextWithChat()), MsgType::AUTH);

			QVERIFY(!QByteArray(dispatcher.processStateChange(AbstractState::getClassName<StateEditAccessRights>())).isEmpty());
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));
		}


		void getAccessRightsCanAllowed()
		{
			MessageDispatcher dispatcher;
			auto context = getContextWithChat(true);
			QCOMPARE(dispatcher.init(context), MsgType::AUTH);

			QVERIFY(!QByteArray(dispatcher.processStateChange(AbstractState::getClassName<StateEditAccessRights>())).isEmpty());
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","CanAllowed","AgeVerification"],"optional":["FamilyName","CanAllowed","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["FamilyName","AgeVerification"]}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","CanAllowed","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["FamilyName","AgeVerification", "CanAllowed"]}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","CanAllowed","AgeVerification"],"optional":["FamilyName","CanAllowed","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));
		}


		void setAccessRights_data()
		{
			QTest::addColumn<QByteArray>("cmd");
			QTest::addColumn<QByteArray>("msg");

			QTest::newRow("chat_invalid") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["8",11]}   )")
										  << getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data is invalid","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_needs_be_string") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": [8,"11"]}   )")
												  << getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data needs to be string","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_unknown_id") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["y", "123"]}   )")
											 << getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data is invalid","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_invalid_and_needs_be_string") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["y", 123]}   )")
															  << getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data is invalid","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_set_optional") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["FamilyName"]}   )")
											   << getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_mixed_valid_and_required") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification", "GivenNames"]}   )")
														   << getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data is not available","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_both_optional") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification", "FamilyName"]}   )")
												<< getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_single_optional") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification"]}   )")
												  << getAux() + QByteArray(R"("chat":{"effective":["Address","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("chat_disable_optional") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": []}   )")
												   << getAux() + QByteArray(R"("chat":{"effective":["Address","GivenNames","DocumentType"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");
		}


		void setAccessRights()
		{
			QFETCH(QByteArray, cmd);
			QFETCH(QByteArray, msg);

			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(getContextWithChat()), MsgType::AUTH);
			QVERIFY(!QByteArray(dispatcher.processStateChange(AbstractState::getClassName<StateEditAccessRights>())).isEmpty());

			// check original state
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			// check cmd
			QCOMPARE(dispatcher.processCommand(cmd), msg);
		}


		void setAccessRightsWithMultipleCmds()
		{
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(getContextWithChat()), MsgType::AUTH);
			QVERIFY(!QByteArray(dispatcher.processStateChange(AbstractState::getClassName<StateEditAccessRights>())).isEmpty());

			// check original state
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			// check cmds
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["FamilyName"]}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			// 11 is not valid, 0 is valid ... we do not accept partial valid values!
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification", "GivenNames"]}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Entry in 'chat' data is not available","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));
		}


		void setAccessRightsWithoutChat()
		{
			const auto& context = getContextWithChat();
			context->setOptionalAccessRights({});

			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::AUTH);
			QVERIFY(!QByteArray(dispatcher.processStateChange(AbstractState::getClassName<StateEditAccessRights>())).isEmpty());

			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification"]}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","GivenNames","DocumentType"],"optional":[],"required":["Address","GivenNames","DocumentType"]},"error":"No optional access rights available","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			context->setOptionalAccessRights({AccessRight::AGE_VERIFICATION});
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification"]}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","GivenNames","DocumentType","AgeVerification"],"optional":["AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			context->setRequiredAccessRights({AccessRight::AGE_VERIFICATION, AccessRight::READ_DG17});
			context->setOptionalAccessRights({});
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": ["AgeVerification"]}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","AgeVerification"],"optional":[],"required":["Address","AgeVerification"]},"error":"No optional access rights available","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));
		}


		void smartCardAllowed()
		{
			const auto& context = getContextWithChat();
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::AUTH);
			QVERIFY(!QByteArray(dispatcher.processStateChange(AbstractState::getClassName<StateEditAccessRights>())).isEmpty());

			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));

			context->setAcceptedEidTypes({AcceptedEidType::CARD_CERTIFIED});
			QCOMPARE(dispatcher.processCommand(QByteArray(R"(   {"cmd": "GET_ACCESS_RIGHTS"}   )")),
					getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})"));
		}


		void crap_data()
		{
			QTest::addColumn<QByteArray>("cmd");
			QTest::addColumn<QByteArray>("msg");

			QTest::newRow("chat_null") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "chat": null}   )")
									   << getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"Invalid 'chat' data","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");

			QTest::newRow("CHAT") << QByteArray(R"(   {"cmd": "SET_ACCESS_RIGHTS", "CHAT": []}   )")
								  << getAux() + QByteArray(R"("chat":{"effective":["Address","FamilyName","GivenNames","DocumentType","AgeVerification"],"optional":["FamilyName","AgeVerification"],"required":["Address","GivenNames","DocumentType"]},"error":"'chat' cannot be undefined","msg":"ACCESS_RIGHTS","transactionInfo":"this is a test for TransactionInfo"})");
		}


		void crap()
		{
			QFETCH(QByteArray, cmd);
			QFETCH(QByteArray, msg);

			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(getContextWithChat()), MsgType::AUTH);
			QVERIFY(!QByteArray(dispatcher.processStateChange(AbstractState::getClassName<StateEditAccessRights>())).isEmpty());

			QCOMPARE(dispatcher.processCommand(cmd), msg);
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerAccessRights)
#include "test_MsgHandlerAccessRights.moc"
