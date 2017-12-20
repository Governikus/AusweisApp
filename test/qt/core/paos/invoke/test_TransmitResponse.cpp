/*!
 * \brief Unit tests for \ref TransmitResponse
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "paos/invoke/TransmitResponse.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_TransmitResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void type()
		{
			TransmitResponse elem;
			elem.setMessageId("dummy");
			QCOMPARE(elem.mType, PaosType::TRANSMIT_RESPONSE);
		}


		void marshall()
		{
			TransmitResponse response;
			response.setMessageId("dummy");
			QByteArray elem = response.marshall();

			QVERIFY(elem.contains("<TransmitResponse "));
			QVERIFY(elem.contains("</TransmitResponse>"));
			QVERIFY(!elem.contains("OutputAPDU"));

			TransmitResponse responseWithApdu;
			responseWithApdu.setMessageId("dummy");
			responseWithApdu.setOutputApdus(QByteArrayList() << "bla");
			elem = responseWithApdu.marshall();

			QVERIFY(elem.contains("<OutputAPDU>bla</OutputAPDU>"));
		}


		void elements()
		{
			TransmitResponse elem;
			elem.setMessageId("dummy");

			QCOMPARE(elem.createTransmitResponse().nodeName(), QString("TransmitResponse"));
			QVERIFY(elem.createTransmitResponse().elementsByTagName("OutputAPDU").isEmpty());

			elem.setOutputApdus(QByteArrayList() << "bla");
			QVERIFY(!elem.createTransmitResponse().elementsByTagName("OutputAPDU").isEmpty());
		}


};

QTEST_GUILESS_MAIN(test_TransmitResponse)
#include "test_TransmitResponse.moc"
