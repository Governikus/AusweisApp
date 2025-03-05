/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#include "paos/invoke/PaosCreator.h"

#include "LogHandler.h"

#include <QtCore>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(PaosCreator::Namespace)

namespace
{
struct test_PaosCreatorDummy
	: public PaosCreator
{
	QString mText;
	bool mNamespace = false;
	const QString mTag = QStringLiteral("content");

	void createBodyElement(QXmlStreamWriter& pWriter) override
	{
		if (mNamespace)
		{
			pWriter.writeTextElement(getNamespaceType(Namespace::SOAP, mTag), mText);
		}
		else
		{
			pWriter.writeTextElement(mTag, mText);
		}
	}


};
} // namespace


class test_PaosCreator
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


		void marshall()
		{
			test_PaosCreatorDummy creator;
			QByteArray elem = creator.marshall();

			QVERIFY(elem.contains("<content></content>"));

			creator.mText = "some test"_L1;
			elem = creator.marshall();
			QVERIFY(elem.contains("<content></content>")); // marshall() can be called once!

			test_PaosCreatorDummy creator2;
			creator2.mText = "hello again"_L1;
			creator2.mNamespace = true;
			elem = creator2.marshall();
			QVERIFY(elem.contains("<soap:content>hello again</soap:content>"));
		}


		void checkRelatesTo_data()
		{
			QTest::addColumn<QString>("relatesTo");

			QTest::newRow("empty") << QString();
			QTest::newRow("related") << QStringLiteral("first one");
		}


		void checkRelatesTo()
		{
			QFETCH(QString, relatesTo);

			test_PaosCreatorDummy creator;
			creator.setRelatedMessageId(relatesTo);
			QByteArray data = creator.marshall();

			QCOMPARE(data.contains("RelatesTo>"), !relatesTo.isNull());
			QVERIFY(data.contains("<wsa:MessageID>urn:uuid:"));
		}


		void namespaces_data()
		{
			QTest::addColumn<PaosCreator::Namespace>("namespaceName");
			QTest::addColumn<QLatin1String>("suffix");
			QTest::addColumn<QLatin1String>("namespaceString");
			QTest::addColumn<QLatin1String>("prefixString");
			QTest::addColumn<QLatin1String>("type");

			QTest::newRow("addressing") << PaosCreator::Namespace::ADDRESSING << "test"_L1 << "http://www.w3.org/2005/03/addressing"_L1 << "wsa:test"_L1 << "wsa:test"_L1;
			QTest::newRow("dss") << PaosCreator::Namespace::DSS << QLatin1String() << "urn:oasis:names:tc:dss:1.0:core:schema"_L1 << "xmlns:dss"_L1 << "dss:"_L1;
			QTest::newRow("ecard") << PaosCreator::Namespace::ECARD << "ecard"_L1 << "http://www.bsi.bund.de/ecard/api/1.1"_L1 << "ecard:ecard"_L1 << "ecard:ecard"_L1;
			QTest::newRow("paos") << PaosCreator::Namespace::PAOS << ""_L1 << "urn:liberty:paos:2006-08"_L1 << "paos:"_L1 << "paos:"_L1;
			QTest::newRow("techschema") << PaosCreator::Namespace::TECHSCHEMA << " "_L1 << "urn:iso:std:iso-iec:24727:tech:schema"_L1 << "iso: "_L1 << "iso: "_L1;
			QTest::newRow("xsd") << PaosCreator::Namespace::XSD << QLatin1String() << "http://www.w3.org/2001/XMLSchema"_L1 << "xmlns:xsd"_L1 << "xsd:"_L1;
			QTest::newRow("xsi") << PaosCreator::Namespace::XSI << QLatin1String() << "http://www.w3.org/2001/XMLSchema-instance"_L1 << "xmlns:xsi"_L1 << "xsi:"_L1;
			QTest::newRow("soap") << PaosCreator::Namespace::SOAP << "test"_L1 << "http://schemas.xmlsoap.org/soap/envelope/"_L1 << "soap:test"_L1 << "soap:test"_L1;
		}


		void namespaces()
		{
			QFETCH(PaosCreator::Namespace, namespaceName);
			QFETCH(QLatin1String, suffix);
			QFETCH(QLatin1String, namespaceString);
			QFETCH(QLatin1String, prefixString);
			QFETCH(QLatin1String, type);

			QCOMPARE(PaosCreator::getNamespace(namespaceName), namespaceString);
			QCOMPARE(PaosCreator::getNamespacePrefix(namespaceName, suffix), prefixString);
			QCOMPARE(PaosCreator::getNamespaceType(namespaceName, suffix), type);
		}


};

QTEST_GUILESS_MAIN(test_PaosCreator)
#include "test_PaosCreator.moc"
