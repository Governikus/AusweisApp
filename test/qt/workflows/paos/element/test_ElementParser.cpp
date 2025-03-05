/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "paos/element/ElementParser.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

const auto dummyXmlData = "<rootElement>"
						  "	<subElement></subElement>"
						  "</rootElement>";

class test_ElementParser
	: public QObject
{
	Q_OBJECT

	private:
		ElementParser mParser = ElementParser(QSharedPointer<QXmlStreamReader>::create());

		bool nextElementNameEquals(const QString& pName)
		{
			if (mParser.readNextStartElement())
			{
				return mParser.getElementName() == pName;
			}

			return false;
		}

	private Q_SLOTS:
		void test_initData_verifyClearData()
		{
			mParser.initData(dummyXmlData);

			QVERIFY(nextElementNameEquals("rootElement"_L1));

			mParser.initData(dummyXmlData);

			QVERIFY(nextElementNameEquals("rootElement"_L1));
			QVERIFY(nextElementNameEquals("subElement"_L1));

		}


		void test_initData_verifyClearParserError()
		{
			mParser.setParserFailed();
			QVERIFY(mParser.parserFailed());

			mParser.initData(dummyXmlData);
			QVERIFY(!mParser.parserFailed());
		}


};

QTEST_GUILESS_MAIN(test_ElementParser)
#include "test_ElementParser.moc"
