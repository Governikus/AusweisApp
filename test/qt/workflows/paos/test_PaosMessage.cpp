/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
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


};

QTEST_GUILESS_MAIN(test_PaosMessage)
#include "test_PaosMessage.moc"
