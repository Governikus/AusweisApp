/*!
 * \brief Unit tests for \DidAuthenticateEACAdditionalInputType
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "paos/retrieve/DidAuthenticateEacAdditional.h"

#include "Env.h"
#include "LogHandler.h"
#include "paos/retrieve/DidAuthenticateEacAdditionalParser.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_DidAuthenticateEacAdditional
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void parse()
		{
			QByteArray content = TestFileHelper::readFile(":paos/DIDAuthenticateEACAdditionalInput.xml");
			PaosMessage* paosMessage = DidAuthenticateEacAdditionalParser().parse(content);

			QScopedPointer<DIDAuthenticateEACAdditional> eacAdd(static_cast<DIDAuthenticateEACAdditional*>(paosMessage));

			QVERIFY(eacAdd);
			QCOMPARE(eacAdd->getSignature(), QString("86E24ACCE25E2B4623A3558EED7AA65658FEE70CC7B46CB9D34BD50B2F27E1E2221473D5B871497563693E0F28BE9C8896062A9C02CE3959AD4BD58DEA0E984B"));
		}


		void noSignature()
		{
			QByteArray content = TestFileHelper::readFile(":paos/DIDAuthenticateEACAdditionalInput_noSignature.xml");
			PaosMessage* paosMessage = DidAuthenticateEacAdditionalParser().parse(content);

			QVERIFY(paosMessage == nullptr);
		}


		void manySignature()
		{
			QByteArray content = TestFileHelper::readFile(":paos/DIDAuthenticateEACAdditionalInput_manySignature.xml");
			PaosMessage* paosMessage = DidAuthenticateEacAdditionalParser().parse(content);

			QVERIFY(paosMessage == nullptr);
		}


		void checkUniqueEntries_data()
		{
			QTest::addColumn<QByteArray>("replaceIdentifier");
			QTest::addColumn<QByteArray>("replaceContent");
			QTest::addColumn<QString>("templateXml");

			const QString templ1(":/paos/DIDAuthenticateEACAdditionalInput_template.xml");
			const QString templ2(":/paos/DIDAuthenticateEACAdditionalInput_template2.xml");

			QTest::newRow("ConnectionHandle") << QByteArray("<!-- CONNECTIONHANDLE -->") << QByteArray("<ns3:SlotHandle>3189905F82823FB6F8EF871E315CC9F0</ns3:SlotHandle>") << templ1;
			QTest::newRow("DIDName") << QByteArray("<!-- DIDNAME -->") << QByteArray("PIN") << templ1;
			QTest::newRow("Signature") << QByteArray("<!-- SIGNATURE -->") << QByteArray("86E24ACCE25E2B4623A3558EED7AA65658FEE70CC7B46CB9D34BD50B2F27E1E2221473D5B871497563693E0F28BE9C8896062A9C02CE3959AD4BD58DEA0E984B") << templ1;

			QTest::newRow("ContextHandle") << QByteArray("<!-- CONTEXTHANDLE -->") << QByteArray("4549445F4946445F434F4E544558545F42415345") << templ2;
			QTest::newRow("IFDName") << QByteArray("<!-- IFDNAME -->") << QByteArray("REINER SCT cyberJack RFID komfort USB 52") << templ2;
			QTest::newRow("SlotIndex") << QByteArray("<!-- SLOTINDEX -->") << QByteArray("0") << templ2;
			QTest::newRow("CardApplication") << QByteArray("<!-- CARDAPPLICATION -->") << QByteArray("4549445F49534F5F32343732375F42415345") << templ2;
			QTest::newRow("SlotHandle") << QByteArray("<!-- SLOTHANDLE -->") << QByteArray("37343139303333612D616163352D343331352D386464392D656166393664636661653361") << templ2;
		}


		void checkUniqueEntries()
		{
			QFETCH(QByteArray, replaceIdentifier);
			QFETCH(QByteArray, replaceContent);
			QFETCH(QString, templateXml);

			const QByteArray tag = QByteArray(QTest::currentDataTag());
			const QByteArray data = '<' + tag + '>' + replaceContent + "</" + tag + '>';

			QByteArray content = TestFileHelper::readFile(templateXml);
			content = content.replace(replaceIdentifier, data + data);

			PaosMessage* paosMessage = DidAuthenticateEacAdditionalParser().parse(content);
			QVERIFY(paosMessage == nullptr);

			const QByteArray duplicateUniqueElement = "Duplicate unique element: \"" + tag + "\"";
			QVERIFY(Env::getSingleton<LogHandler>()->getBacklog().contains(duplicateUniqueElement));
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateEacAdditional)
#include "test_DidAuthenticateEacAdditional.moc"
