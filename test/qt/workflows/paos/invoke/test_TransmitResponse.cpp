/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref TransmitResponse
 */

#include "paos/invoke/TransmitResponse.h"

#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
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
			responseWithApdu.setMessageId("dummy"_L1);
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
			data.replace(QRegularExpression("<wsa:MessageID>.*</wsa:MessageID>"_L1), "<wsa:MessageID>STRIP ME</wsa:MessageID>"_L1);
			QCOMPARE(data, QString::fromLatin1(TestFileHelper::readFile(":/paos/TransmitResponse.xml"_L1)));
		}


};

QTEST_GUILESS_MAIN(test_TransmitResponse)
#include "test_TransmitResponse.moc"
