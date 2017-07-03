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
		context->setRequiredAccessRights({AccessRight::READ_DG01, AccessRight::READ_DG04});
		context->setOptionalAccessRights({AccessRight::AGE_VERIFICATION, AccessRight::READ_DG05});
		return context;
	}


	private Q_SLOTS:
		void state()
		{
			MessageDispatcher dispatcher;
			dispatcher.init(getContextWithChat());

			QCOMPARE(dispatcher.processStateChange("StateEditAccessRights"), QByteArray("{\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8,0],\"optional\":[12,0],\"required\":[11,8]}}"));
		}


		void getAccessRightsBadState()
		{
			MessageDispatcher dispatcher;

			QByteArray msg = QByteArray("{\"cmd\": \"GET_ACCESS_RIGHTS\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"GET_ACCESS_RIGHTS\",\"msg\":\"BAD_STATE\"}"));

			dispatcher.init(getContextWithChat());
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"GET_ACCESS_RIGHTS\",\"msg\":\"BAD_STATE\"}"));
		}


		void acceptAccessRights()
		{
			const auto& context = getContextWithChat();
			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QVERIFY(!context->isStateApproved());
			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());

			QVERIFY(!context->isStateApproved());
			QByteArray msg = QByteArray("{\"cmd\": \"GET_ACCESS_RIGHTS\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8,0],\"optional\":[12,0],\"required\":[11,8]}}"));

			msg = QByteArray("{\"cmd\": \"ACCEPT\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
			QVERIFY(context->isStateApproved());
		}


		void getAccessRights()
		{
			MessageDispatcher dispatcher;
			dispatcher.init(getContextWithChat());

			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());
			QByteArray msg = QByteArray("{\"cmd\": \"GET_ACCESS_RIGHTS\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8,0],\"optional\":[12,0],\"required\":[11,8]}}"));
		}


		void setAccessRights()
		{
			MessageDispatcher dispatcher;
			dispatcher.init(getContextWithChat());
			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());

			QByteArray msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": [8,\"11\"]}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Entry in 'raw' data needs to be integer\",\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8,0],\"optional\":[12,0],\"required\":[11,8]}}"));

			msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": [0, 123]}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Entry in 'raw' data is invalid\",\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8,0],\"optional\":[12,0],\"required\":[11,8]}}"));

			msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": [12]}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8],\"optional\":[12,0],\"required\":[11,8]}}"));

			msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": [0, 11]}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Entry in 'raw' data is invalid\",\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8],\"optional\":[12,0],\"required\":[11,8]}}"));

			msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": [0,12]}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8,0],\"optional\":[12,0],\"required\":[11,8]}}"));

			msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": [0]}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[11,8,0],\"optional\":[12,0],\"required\":[11,8]}}"));

			msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": [0,12]}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8,0],\"optional\":[12,0],\"required\":[11,8]}}"));

			msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": []}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[11,8],\"optional\":[12,0],\"required\":[11,8]}}"));
		}


		void setAccessRightsWithoutChat()
		{
			const auto& context = getContextWithChat();
			context->setOptionalAccessRights({});

			MessageDispatcher dispatcher;
			dispatcher.init(context);
			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());

			QByteArray msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": [0]}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"No optional access rights available\",\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[11,8],\"optional\":[],\"required\":[11,8]}}"));

			context->setOptionalAccessRights({AccessRight::AGE_VERIFICATION});
			msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": [0]}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[11,8,0],\"optional\":[0],\"required\":[11,8]}}"));

			context->setRequiredAccessRights({AccessRight::AGE_VERIFICATION});
			context->setOptionalAccessRights({});
			msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": [0]}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"No optional access rights available\",\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[0],\"optional\":[],\"required\":[0]}}"));
		}


		void crap()
		{
			MessageDispatcher dispatcher;
			dispatcher.init(getContextWithChat());
			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());

			QByteArray msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"raw\": null}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Invalid 'raw' data\",\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8,0],\"optional\":[12,0],\"required\":[11,8]}}"));

			msg = QByteArray("{\"cmd\": \"SET_ACCESS_RIGHTS\", \"RAW\": []}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"'raw' cannot be undefined\",\"msg\":\"ACCESS_RIGHTS\",\"raw\":{\"effective\":[12,11,8,0],\"optional\":[12,0],\"required\":[11,8]}}"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerAccessRights)
#include "test_MsgHandlerAccessRights.moc"
