/*!
 * \brief Unit tests for \ref DisconnectResponse
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CardReturnCode.h"
#include "paos/invoke/DisconnectResponse.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_DisconnectResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void type()
		{
			DisconnectResponse elem;
			QCOMPARE(elem.mType, PaosType::DISCONNECT_RESPONSE);
		}


		void marshall()
		{
			DisconnectResponse response;
			QByteArray elem = response.marshall();

			QVERIFY(elem.contains("<DisconnectResponse "));
			QVERIFY(elem.contains("</DisconnectResponse>"));

			QVERIFY(elem.contains("<Result xmlns=\"urn:oasis:names:tc:dss:1.0:core:schema\">"));
			QVERIFY(elem.contains("</Result>"));

			QVERIFY(elem.contains("<ResultMajor>http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok</ResultMajor>"));

			DisconnectResponse responseWithSlot;
			responseWithSlot.setResult(ECardApiResult(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CARD_NOT_FOUND)));
			responseWithSlot.setSlotHandle("huhu");
			elem = responseWithSlot.marshall();

			QVERIFY(elem.contains("<ResultMajor>http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error</ResultMajor>"));
			QVERIFY(elem.contains("<ResultMinor>http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownError</ResultMinor>"));
			QVERIFY(elem.contains("<ResultMessage xml:lang=\"en\">Card does not exist</ResultMessage>"));
			QVERIFY(elem.contains("<SlotHandle>huhu</SlotHandle>"));
		}


};

QTEST_GUILESS_MAIN(test_DisconnectResponse)
#include "test_DisconnectResponse.moc"
