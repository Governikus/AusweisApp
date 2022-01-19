/*!
 * \brief Unit tests for \ref RequestType
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include "paos/RequestType.h"

using namespace governikus;

class test_RequestType
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void type()
		{
			RequestType request(PaosType::UNKNOWN);
			QCOMPARE(request.mType, PaosType::UNKNOWN);

			RequestType request2(PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE);
			QCOMPARE(request2.mType, PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE);
		}


		void emptyMember()
		{
			RequestType request(PaosType::UNKNOWN);
			QCOMPARE(request.getMessageId(), QString());
			QCOMPARE(request.getRelatesTo(), QString());
		}


};

QTEST_GUILESS_MAIN(test_RequestType)
#include "test_RequestType.moc"
