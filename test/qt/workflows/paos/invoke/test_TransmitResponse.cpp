/*!
 * \brief Unit tests for \ref TransmitResponse
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "paos/invoke/TransmitResponse.h"

#include "TestFileHelper.h"

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
			QCOMPARE(elem.mType, PaosType::TRANSMIT_RESPONSE);
		}


		void marshall()
		{
			TransmitResponse response;
			QByteArray elem = response.marshall();

			QVERIFY(elem.contains("<TransmitResponse "));
			QVERIFY(elem.contains("</TransmitResponse>"));
			QVERIFY(!elem.contains("OutputAPDU"));

			TransmitResponse responseWithApdu;
			responseWithApdu.setMessageId("dummy");
			responseWithApdu.setOutputApdus(QByteArrayList{"bla"});
			elem = responseWithApdu.marshall();

			QVERIFY(elem.contains("<OutputAPDU>bla</OutputAPDU>"));
		}


		void checkTemplate()
		{
			TransmitResponse response;
			response.setResult(ECardApiResult(GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation));
			response.setOutputApdus(QByteArrayList{"a", "b", "c"});
			auto data = QString::fromLatin1(response.marshall());
			data.replace(QRegularExpression("<wsa:MessageID>.*</wsa:MessageID>"), "<wsa:MessageID>STRIP ME</wsa:MessageID>");
			QCOMPARE(data, QString::fromLatin1(TestFileHelper::readFile(":/paos/TransmitResponse.xml")));
		}


};

QTEST_GUILESS_MAIN(test_TransmitResponse)
#include "test_TransmitResponse.moc"
