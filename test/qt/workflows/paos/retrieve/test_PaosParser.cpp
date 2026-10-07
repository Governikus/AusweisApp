/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "paos/retrieve/PaosParser.h"

#include "TestParserHelper.h"

#include <QtTest>


using namespace governikus;


class test_PaosParser
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initialData()
		{
			const auto& elementParser = TestParserHelper::create(QByteArray());
			PaosParser parser;
			QCOMPARE(parser.parse(elementParser), nullptr);
			QCOMPARE(parser.getParser(), elementParser);
			QCOMPARE(parser.mMessageID, QLatin1String());
			QCOMPARE(parser.mRelatesTo, QLatin1String());
		}


};

QTEST_GUILESS_MAIN(test_PaosParser)
#include "test_PaosParser.moc"
