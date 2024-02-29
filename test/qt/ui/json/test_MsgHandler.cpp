/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref MsgHandler
 */

#include "messages/MsgHandler.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

namespace
{
class MsgTest
	: public MsgHandler
{
	public:
		MsgTest(MsgType pType = MsgType::INVALID) : MsgHandler(pType)
		{
		}


		void testVoid(bool pVoid)
		{
			setVoid(pVoid);
		}


};

} // namespace


class test_MsgHandler
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void invalidate()
		{
			MsgTest msg;
			QCOMPARE(msg.isVoid(), false);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"INVALID\"}"));
			msg.testVoid(true);
			QCOMPARE(msg.isVoid(), true);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"INVALID\"}"));
		}


		void setRequest()
		{
			MsgTest msg;
			QJsonObject dummy;

			dummy["unused"_L1] = "this will be ignored"_L1;
			msg.setRequest(dummy);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"INVALID\"}"));

			dummy["request"_L1] = "hello world"_L1;
			msg.setRequest(dummy);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"INVALID\",\"request\":\"hello world\"}"));
		}


		void toJson()
		{
			MsgTest msg;
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"INVALID\"}"));
		}


		void getOutput()
		{
			MsgTest msg;
			QCOMPARE(msg.getOutput(), QByteArray("{\"msg\":\"INVALID\"}"));

			msg.testVoid(true);
			QCOMPARE(msg.getOutput(), QByteArray());

			msg.testVoid(false);
			QCOMPARE(msg.getOutput(), QByteArray("{\"msg\":\"INVALID\"}"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandler)
#include "test_MsgHandler.moc"
