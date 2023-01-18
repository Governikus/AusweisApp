/*!
 * \brief Unit tests for \ref StartPaos
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "paos/invoke/StartPaos.h"

#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_StartPaos
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			QCoreApplication::setApplicationVersion("123.456.789");
		}


		void type()
		{
			StartPaos elem("session");
			QCOMPARE(elem.mType, PaosType::STARTPAOS);
		}


		void marshall()
		{
			StartPaos startPaos("session");
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
			QVERIFY(elem.contains("<Subminor>5</Subminor>"));
		}


		void checkTemplate()
		{
			StartPaos startPaos("abcd");
			startPaos.setRelatedMessageId("urn:uuid:dummy");
			auto data = QString::fromLatin1(startPaos.marshall());
			data.replace(QRegularExpression("<wsa:MessageID>.*</wsa:MessageID>"), "<wsa:MessageID>STRIP ME</wsa:MessageID>");
			QCOMPARE(data, QString::fromLatin1(TestFileHelper::readFile(":/paos/StartPAOS.xml")));
		}


};

QTEST_GUILESS_MAIN(test_StartPaos)
#include "test_StartPaos.moc"
