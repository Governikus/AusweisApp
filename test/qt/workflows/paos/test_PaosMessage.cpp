/*!
 * \brief Unit tests for \ref PaosMessage
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "paos/PaosMessage.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_PaosMessage
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void type()
		{
			PaosMessage msg(PaosType::UNKNOWN);
			QCOMPARE(msg.mType, PaosType::UNKNOWN);

			PaosMessage msg2(PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE);
			QCOMPARE(msg2.mType, PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE);
		}


		void emptyMembers()
		{
			PaosMessage msg(PaosType::UNKNOWN);
			QCOMPARE(msg.getMessageId(), QString());
			QCOMPARE(msg.getRelatesTo(), QString());
		}


		void handleWSAddressingHeaders()
		{
			PaosMessage msg(PaosType::UNKNOWN);
			QCOMPARE(msg.handleWSAddressingHeaders(QStringLiteral("elem name"), QStringLiteral("some value"), QXmlStreamAttributes()), false);
			QCOMPARE(msg.getMessageId(), QString());
			QCOMPARE(msg.getRelatesTo(), QString());

			QCOMPARE(msg.handleWSAddressingHeaders(QStringLiteral("MessageID"), QStringLiteral("msg value"), QXmlStreamAttributes()), true);
			QCOMPARE(msg.getMessageId(), QStringLiteral("msg value"));
			QCOMPARE(msg.getRelatesTo(), QString());

			QCOMPARE(msg.handleWSAddressingHeaders(QStringLiteral("RelatesTo"), QStringLiteral("relates value"), QXmlStreamAttributes()), true);
			QCOMPARE(msg.getMessageId(), QStringLiteral("msg value"));
			QCOMPARE(msg.getRelatesTo(), QStringLiteral("relates value"));
		}


};

QTEST_GUILESS_MAIN(test_PaosMessage)
#include "test_PaosMessage.moc"
