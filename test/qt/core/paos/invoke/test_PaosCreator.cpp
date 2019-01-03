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
	const QString mTag = QStringLiteral("content");

	virtual void createBodyElement() override
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
