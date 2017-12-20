/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "paos/MessageIdHandler.h"

using namespace governikus;

class test_MessageIdHandler
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testCreateNewMsgId()
		{
			MessageIdHandler handler;

			QString myMsgId = handler.createNewMsgId();

			QVERIFY(myMsgId.startsWith("urn:uuid:"));
			QVERIFY(!QUuid(myMsgId.mid(QString("urn:uuid:").size())).isNull());
			QCOMPARE(myMsgId.size(), 9 + 36);
		}


		void testRemoteMsgId()
		{
			MessageIdHandler handler;
			QString remoteMsgId("123456789");
			handler.setRemoteMsgId(remoteMsgId);

			QCOMPARE(handler.getRemoteMsgId(), remoteMsgId);
			QVERIFY(handler.createNewMsgId().startsWith(QLatin1String("urn:uuid:")));
		}


};

QTEST_GUILESS_MAIN(test_MessageIdHandler)
#include "test_MessageIdHandler.moc"
