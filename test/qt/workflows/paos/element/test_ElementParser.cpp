/**
 * Copyright (c) 2022-2026 Governikus Service GmbH, Germany
 */

#include "paos/element/ElementParser.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_ElementParser
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_nextElementNameEquals()
		{
			const auto dummyXmlData = "<rootElement>"
									  "	<subElement>hello</subElement>"
									  "</rootElement>";

			ElementParser parser = ElementParser(QSharedPointer<QXmlStreamReader>::create(dummyXmlData), true);
			QCOMPARE(parser.readElementText(), QLatin1String("hello"));
		}


		void test_parserFailed()
		{
			ElementParser parser = ElementParser(QSharedPointer<QXmlStreamReader>::create(), true);

			QVERIFY(!parser.parserFailed());
			parser.setParserFailed();
			QVERIFY(parser.parserFailed());
		}


};

QTEST_GUILESS_MAIN(test_ElementParser)
#include "test_ElementParser.moc"
