/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref MsgHandlerInternalError
 */

#include "messages/MsgHandlerInternalError.h"

#include <QtTest>

using namespace governikus;

class test_MsgHandlerInternalError
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void defaultCtor()
		{
			MsgHandlerInternalError msg;
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"INTERNAL_ERROR\"}"));
		}


		void stringCtor()
		{
			MsgHandlerInternalError msg1(QStringLiteral("dummy"));
			QCOMPARE(msg1.toJson(), QByteArray("{\"error\":\"dummy\",\"msg\":\"INTERNAL_ERROR\"}"));

			QString tmp;
			MsgHandlerInternalError msg2(tmp);
			QCOMPARE(msg2.toJson(), QByteArray("{\"msg\":\"INTERNAL_ERROR\"}"));
		}


		void latin1Ctor()
		{
			MsgHandlerInternalError msg1(QLatin1String("dummy"));
			QCOMPARE(msg1.toJson(), QByteArray("{\"error\":\"dummy\",\"msg\":\"INTERNAL_ERROR\"}"));

			QLatin1String tmp;
			MsgHandlerInternalError msg2(tmp);
			QCOMPARE(msg2.toJson(), QByteArray("{\"msg\":\"INTERNAL_ERROR\"}"));
		}


		void msgVoid()
		{
			MsgHandler msg = MsgHandler::Void;
			QCOMPARE(msg.toJson(), QByteArray(R"({"msg":"VOID"})"));
			QCOMPARE(msg.getOutput(), QByteArray());
			QVERIFY(msg.isVoid());
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerInternalError)
#include "test_MsgHandlerInternalError.moc"
