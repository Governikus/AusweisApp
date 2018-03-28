/*!
 * \brief Unit tests for \ref PaosCreator
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "paos/invoke/PaosCreator.h"

#include "LogHandler.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

namespace
{
struct test_PaosCreatorDummy
	: public PaosCreator
{
	QString mText;
	bool mNamespace = false;

	virtual QDomElement getDocumentStructure() override
	{
		if (mNamespace)
		{
			return createTextElement(Namespace::SOAP, "content", mText);
		}
		else
		{
			return createTextElement("content", mText);
		}
	}


	QByteArray getData(QDomElement& pElement)
	{
		mDoc.appendChild(pElement);
		return mDoc.toByteArray();
	}


};
}


class test_PaosCreator
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
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


		void createHeaderElement()
		{
			test_PaosCreatorDummy creator;
			auto elem = creator.createHeaderElement(QString(), "something");
			QByteArray data = creator.getData(elem);
			QVERIFY(!data.contains("RelatesTo>"));
			QVERIFY(data.contains("<wsa:MessageID>something</wsa:MessageID>"));

			test_PaosCreatorDummy creator2;
			elem = creator2.createHeaderElement("first one", "second one");
			data = creator.getData(elem);
			QVERIFY(data.contains("<wsa:RelatesTo>first one</wsa:RelatesTo>"));
			QVERIFY(data.contains("<wsa:MessageID>second one</wsa:MessageID>"));
		}


		void namespaces()
		{
			static int count = 8;

			QCOMPARE(PaosCreator::mNamespace.count(), count);
			QCOMPARE(PaosCreator::mNamespacePrefix.count(), count + 1); // + DEFAULT

			QCOMPARE(PaosCreator::mNamespace.value(PaosCreator::Namespace::ADDRESSING), QString("http://www.w3.org/2005/03/addressing"));
			QCOMPARE(PaosCreator::mNamespacePrefix.value(PaosCreator::Namespace::ADDRESSING), QString("wsa"));

			QCOMPARE(PaosCreator::getNamespace(PaosCreator::Namespace::ADDRESSING), QString("http://www.w3.org/2005/03/addressing"));
			QCOMPARE(PaosCreator::getNamespacePrefix(PaosCreator::Namespace::ADDRESSING, "suffix"), QString("wsa:suffix"));
			QCOMPARE(PaosCreator::getNamespacePrefix(PaosCreator::Namespace::ADDRESSING), QString("xmlns:wsa"));
			QCOMPARE(PaosCreator::getNamespaceType(PaosCreator::Namespace::ADDRESSING, "test"), QString("wsa:test"));
		}


};

QTEST_GUILESS_MAIN(test_PaosCreator)
#include "test_PaosCreator.moc"
