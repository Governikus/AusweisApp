/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref StartPaos
 */

#include "paos/invoke/StartPaos.h"

#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_StartPaos
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			QCoreApplication::setApplicationVersion("123.456.789"_L1);
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
			startPaos.setRelatedMessageId("urn:uuid:dummy"_L1);
			auto data = QString::fromLatin1(startPaos.marshall());
			data.replace(QRegularExpression("<wsa:MessageID>.*</wsa:MessageID>"_L1), "<wsa:MessageID>STRIP ME</wsa:MessageID>"_L1);
			QCOMPARE(data, QString::fromLatin1(TestFileHelper::readFile(":/paos/StartPAOS.xml"_L1)));
		}


};

QTEST_GUILESS_MAIN(test_StartPaos)
#include "test_StartPaos.moc"
