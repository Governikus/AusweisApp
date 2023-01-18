/*!
 * \brief Unit tests for \ref PaosCreator
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "paos/invoke/PaosCreator.h"

#include "LogHandler.h"

#include <QtCore>
#include <QtTest>

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

	void createBodyElement() override
	{
		if (mNamespace)
		{
			mWriter.writeTextElement(getNamespaceType(Namespace::SOAP, mTag), mText);
		}
		else
		{
			mWriter.writeTextElement(mTag, mText);
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

			creator.mText = "some test";
			elem = creator.marshall();
			QVERIFY(elem.contains("<content></content>")); // marshall() can be called once!

			test_PaosCreatorDummy creator2;
			creator2.mText = "hello again";
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
			QTest::addColumn<QString>("suffix");
			QTest::addColumn<QString>("namespaceString");
			QTest::addColumn<QString>("prefixString");
			QTest::addColumn<QString>("type");

			QTest::newRow("addressing") << PaosCreator::Namespace::ADDRESSING << QString("test") << QString("http://www.w3.org/2005/03/addressing") << QString("wsa:test") << QString("wsa:test");
			QTest::newRow("dss") << PaosCreator::Namespace::DSS << QString() << QString("urn:oasis:names:tc:dss:1.0:core:schema") << QString("xmlns:dss") << QString("dss:");
			QTest::newRow("ecard") << PaosCreator::Namespace::ECARD << QString("ecard") << QString("http://www.bsi.bund.de/ecard/api/1.1") << QString("ecard:ecard") << QString("ecard:ecard");
			QTest::newRow("paos") << PaosCreator::Namespace::PAOS << QString("") << QString("urn:liberty:paos:2006-08") << QString("paos:") << QString("paos:");
			QTest::newRow("techschema") << PaosCreator::Namespace::TECHSCHEMA << QString(" ") << QString("urn:iso:std:iso-iec:24727:tech:schema") << QString("iso: ") << QString("iso: ");
			QTest::newRow("xsd") << PaosCreator::Namespace::XSD << QString() << QString("http://www.w3.org/2001/XMLSchema") << QString("xmlns:xsd") << QString("xsd:");
			QTest::newRow("xsi") << PaosCreator::Namespace::XSI << QString() << QString("http://www.w3.org/2001/XMLSchema-instance") << QString("xmlns:xsi") << QString("xsi:");
			QTest::newRow("soap") << PaosCreator::Namespace::SOAP << QString("test") << QString("http://schemas.xmlsoap.org/soap/envelope/") << QString("soap:test") << QString("soap:test");
		}


		void namespaces()
		{
			QFETCH(PaosCreator::Namespace, namespaceName);
			QFETCH(QString, suffix);
			QFETCH(QString, namespaceString);
			QFETCH(QString, prefixString);
			QFETCH(QString, type);

			QCOMPARE(PaosCreator::getNamespace(namespaceName), namespaceString);
			QCOMPARE(PaosCreator::getNamespacePrefix(namespaceName, suffix), prefixString);
			QCOMPARE(PaosCreator::getNamespaceType(namespaceName, suffix), type);
		}


};

QTEST_GUILESS_MAIN(test_PaosCreator)
#include "test_PaosCreator.moc"
