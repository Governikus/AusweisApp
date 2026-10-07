/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "paos/retrieve/DidAuthenticateMessage.h"
#include "paos/retrieve/PaosParser.h"

#include "TestParserHelper.h"

#include <QtTest>
#include <TestFileHelper.h>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_DidAuthenticateParser
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


		void parseXml()
		{
			const auto elementParser = TestParserHelper::create(":/paos/DIDAuthenticateEAC1.xml"_L1);
			auto* pm = PaosParser().parse(elementParser).release();
			const std::unique_ptr<DidAuthenticateMessage> didAuthMsg(static_cast<DidAuthenticateMessage*>(pm));
			QVERIFY(didAuthMsg);
			QCOMPARE(didAuthMsg->getConnectionHandle().getCardApplication(), "4549445F49534F5F32343732375F42415345"_L1);
			QCOMPARE(didAuthMsg->getConnectionHandle().getContextHandle(), "4549445F4946445F434F4E544558545F42415345"_L1);
			QCOMPARE(didAuthMsg->getConnectionHandle().getIfdName(), "REINER SCT cyberJack RFID komfort USB 52"_L1);
			QCOMPARE(didAuthMsg->getConnectionHandle().getSlotHandle(), "37343139303333612D616163352D343331352D386464392D656166393664636661653361"_L1);
			QCOMPARE(didAuthMsg->getConnectionHandle().getSlotIndex(), "0"_L1);
			QCOMPARE(didAuthMsg->getDidName(), "PIN"_L1);
		}


		void checkUniqueEntries_data()
		{
			QTest::addColumn<QByteArray>("replaceIdentifier");
			QTest::addColumn<QByteArray>("replaceContent");
			QTest::addColumn<QString>("templateXml");

			const QString templ1(":/paos/DIDAuthenticateEAC1_template.xml"_L1);
			const QString templ2(":/paos/DIDAuthenticateEAC1_template2.xml"_L1);

			QTest::newRow("DIDName") << QByteArray("<!-- DIDNAME -->") << QByteArray("PIN") << templ1;

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

			const auto& parser = TestParserHelper::create(content);
			auto* pm = PaosParser().parse(parser).release();
			const std::unique_ptr<DidAuthenticateMessage> didAuthMsg(static_cast<DidAuthenticateMessage*>(pm));
			QVERIFY(!didAuthMsg);

			const QByteArray duplicateUniqueElement = "Duplicate unique element: \"" + tag + "\"";
			QVERIFY(Env::getSingleton<LogHandler>()->getBacklog().contains(duplicateUniqueElement));
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateParser)
#include "test_DidAuthenticateParser.moc"
