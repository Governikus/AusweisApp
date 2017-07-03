/*!
 * \brief Unit tests for \ref StartPaos
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "paos/invoke/StartPaos.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_StartPaos
	: public QObject
{
	Q_OBJECT

	QString getValue(const QDomElement& pElement, const QString& pName)
	{
		return pElement.elementsByTagName(pName).at(0).firstChild().nodeValue();
	}


	private Q_SLOTS:
		void initTestCase()
		{
			QCoreApplication::setApplicationVersion("123.456.789");
		}


		void type()
		{
			StartPaos elem("session");
			elem.setMessageId("dummy");
			QCOMPARE(elem.mType, PaosType::STARTPAOS);
		}


		void marshall()
		{
			StartPaos startPaos("session");
			startPaos.setMessageId("dummy");
			QByteArray elem = startPaos.marshall();

			QVERIFY(elem.contains("<StartPAOS xmlns=\"urn:iso:std:iso-iec:24727:tech:schema\">"));
			QVERIFY(elem.contains("</StartPAOS>"));

			QVERIFY(elem.contains("<SessionIdentifier>session</SessionIdentifier>"));

			QVERIFY(elem.contains("<ConnectionHandle "));
			QVERIFY(elem.contains("</ConnectionHandle>"));

			QVERIFY(elem.contains("<CardApplication>e80704007f00070302</CardApplication>"));

			QVERIFY(elem.contains("<SlotHandle>00</SlotHandle>"));

			QVERIFY(elem.contains("<SupportedAPIVersions>"));
			QVERIFY(elem.contains("</SupportedAPIVersions>"));

			QVERIFY(elem.contains("<Major>1</Major>"));
			QVERIFY(elem.contains("<Minor>1</Minor>"));
			QVERIFY(elem.contains("<Subminor>4</Subminor>"));
		}


		void elements()
		{
			StartPaos ctor("session123");
			ctor.setMessageId("dummy");

			QCOMPARE(ctor.createSessionIdentifierElement().nodeName(), QString("SessionIdentifier"));
			QCOMPARE(ctor.createSessionIdentifierElement().firstChild().nodeValue(), QString("session123"));

			auto elem = ctor.createConnectionHandleElement();
			QCOMPARE(getValue(elem, "CardApplication"), QString("e80704007f00070302"));
			QCOMPARE(getValue(elem, "SlotHandle"), QString("00"));
		}


		void userAgentElement()
		{
			StartPaos elem("session123");
			elem.setMessageId("dummy");
			QCOMPARE(getValue(elem.createUserAgentElement(), "Name"), QString("Test_core_paos_invoke_StartPaos"));
		}


		void supportedAPIVersionsElement()
		{
			StartPaos elem("session123");
			elem.setMessageId("dummy");
			QCOMPARE(getValue(elem.createSupportedAPIVersionsElement(), "Major"), QString("1"));
			QCOMPARE(getValue(elem.createSupportedAPIVersionsElement(), "Minor"), QString("1"));
			QCOMPARE(getValue(elem.createSupportedAPIVersionsElement(), "Subminor"), QString("4"));
		}


};

QTEST_GUILESS_MAIN(test_StartPaos)
#include "test_StartPaos.moc"
