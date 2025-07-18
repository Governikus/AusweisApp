/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>
#include <algorithm>

#include "asn1/ChainBuilder.h"


using namespace governikus;


class test_ChainBuilder
	: public QObject
{
	Q_OBJECT


	/*
	 * For easier testability, chains of QByteArray are created instead of
	 * certificate chains.
	 *
	 * The QByteArray are each two characters long. The first character
	 * represents the issuer, and the second character represents the subject.
	 * The chaining rule is defined by the method test_ChainBuilder::isChild.
	 */
	static bool isChild(const QByteArray& pChild, const QByteArray& pParent)
	{
		if (pChild.at(1) == pChild.at(0))
		{
			// Self-signed certificates do not have an external issuer certificate.
			return false;
		}
		return pChild.at(0) == pParent.at(1);
	}

	private Q_SLOTS:
		void testEmpty()
		{
			const QList<QByteArray> allElements;
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QVERIFY(chainBuilder.getChains().isEmpty());
		}


		void testOneShortChain()
		{
			const QList<QByteArray> allElements({"AB"});
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 1);
			QCOMPARE(chainBuilder.getChains().at(0).size(), 1);
		}


		void testManyShortChain()
		{
			const QList<QByteArray> allElements({"AB", "AC", "AD", "AE"});
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 4);
			QCOMPARE(chainBuilder.getChains().at(0).size(), 1);
			QCOMPARE(chainBuilder.getChains().at(1).size(), 1);
			QCOMPARE(chainBuilder.getChains().at(2).size(), 1);
			QCOMPARE(chainBuilder.getChains().at(3).size(), 1);
		}


		void testShortChainWithDuplicates()
		{
			const QList<QByteArray> allElements({"AB", "AC", "AB", "AC", "AC", "AB"});
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 2);
			QCOMPARE(chainBuilder.getChains().at(0).size(), 1);
			QCOMPARE(chainBuilder.getChains().at(1).size(), 1);
		}


		void testOneLongChain()
		{
			const QList<QByteArray> allElements({"AB", "BC", "CD", "DE", "EF", "FG"});
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 1);
			QCOMPARE(chainBuilder.getChains().at(0).size(), 6);
		}


		void testOneLongChainWithDuplicates()
		{
			const QList<QByteArray> allElements({"AB", "BC", "BC", "CD", "BC", "CD", "DE", "DE", "EF", "BC", "FG"});
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 1);
			QCOMPARE(chainBuilder.getChains().at(0).size(), 6);
		}


		void testManyRoots()
		{
			/*
			 * AA
			 * |
			 * AB     BB
			 * |   /
			 * BC
			 * |
			 * CD
			 * |
			 * DE
			 */
			const QList<QByteArray> allElements({"AA", "AB", "BC", "CD", "DE", "BB"});

			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 2);
			QVERIFY(chainBuilder.getChains().contains(QList<QByteArray>({"AA", "AB", "BC", "CD", "DE"})));
			QVERIFY(chainBuilder.getChains().contains(QList<QByteArray>({"BB", "BC", "CD", "DE"})));
		}


		void testManyLeaves()
		{
			/*
			 * AA
			 * |
			 * AB
			 * |
			 * BC
			 * |   \
			 * CD    CF
			 * |
			 * DE
			 */
			const QList<QByteArray> allElements({"AA", "AB", "BC", "CD", "DE", "CF"});

			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 2);
			QVERIFY(chainBuilder.getChains().contains(QList<QByteArray>({"AA", "AB", "BC", "CD", "DE"})));
			QVERIFY(chainBuilder.getChains().contains(QList<QByteArray>({"AA", "AB", "BC", "CF"})));
		}


		void testAllCombinations()
		{
			/*
			 * AA
			 * |
			 * AB     BB
			 * |   /
			 * BC
			 * |   \
			 * CD    CF
			 * |
			 * DE
			 */
			QBENCHMARK
			{
				QList<QByteArray> allElements({"AA", "AB", "BC", "CD", "DE", "BB", "CF"});
				const QList<QByteArray> chain1({"AA", "AB", "BC", "CD", "DE"});
				const QList<QByteArray> chain2({"BB", "BC", "CD", "DE"});
				const QList<QByteArray> chain3({"AA", "AB", "BC", "CF"});
				const QList<QByteArray> chain4({"BB", "BC", "CF"});

				do
				{
					ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

					QCOMPARE(chainBuilder.getChains().size(), 4);
					QVERIFY(chainBuilder.getChains().contains(chain1));
					QVERIFY(chainBuilder.getChains().contains(chain2));
					QVERIFY(chainBuilder.getChains().contains(chain3));
					QVERIFY(chainBuilder.getChains().contains(chain4));
				}
				while (std::next_permutation(allElements.begin(), allElements.end()));
			}
		}


};


QTEST_GUILESS_MAIN(test_ChainBuilder)
#include "test_ChainBuilder.moc"
