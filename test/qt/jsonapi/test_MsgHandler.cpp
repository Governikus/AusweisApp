/*!
 * \brief Unit tests for \ref MsgHandler
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandler.h"

#include <QtTest>

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

}


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

			dummy["unused"] = "this will be ignored";
			msg.setRequest(dummy);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"INVALID\"}"));

			dummy["request"] = "hello world";
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
