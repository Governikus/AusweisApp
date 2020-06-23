/*!
 * \brief Unit tests for \ref ChainBuilder
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include <algorithm>
#include <QtCore>
#include <QtTest>

#include "asn1/ChainBuilder.h"


using namespace governikus;


class test_ChainBuilder
	: public QObject
{
	Q_OBJECT


	/*
	 * Der einfacheren Testbarkeit halber werden Ketten aus QStrings statt
	 * Zertifikatsketten gebildet.
	 *
	 * Die QStrings sind jeweils zwei Zeichen lang. Das erste Zeichen entspricht
	 * dem Issuer, das zweite Zeichen entspricht dem Subject.
	 * Die Verkettungsvorschrift ist durch die Methode test_ChainBuilder::isChild
	 * definiert.
	 */
	static bool isChild(const QByteArray& pChild, const QByteArray& pParent)
	{
		if (pChild.at(1) == pChild.at(0))
		{
			// selbst signierte Zertifikate haben kein anderes Issuer-Zert
			return false;
		}
		return pChild.at(0) == pParent.at(1);
	}

	private Q_SLOTS:
		void testEmpty()
		{
			const QVector<QByteArray> allElements;
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QVERIFY(chainBuilder.getChains().isEmpty());
		}


		void testOneShortChain()
		{
			const QVector<QByteArray> allElements({"AB"});
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 1);
			QCOMPARE(chainBuilder.getChains().at(0).size(), 1);
		}


		void testManyShortChain()
		{
			const QVector<QByteArray> allElements({"AB", "AC", "AD", "AE"});
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 4);
			QCOMPARE(chainBuilder.getChains().at(0).size(), 1);
			QCOMPARE(chainBuilder.getChains().at(1).size(), 1);
			QCOMPARE(chainBuilder.getChains().at(2).size(), 1);
			QCOMPARE(chainBuilder.getChains().at(3).size(), 1);
		}


		void testShortChainWithDuplicates()
		{
			const QVector<QByteArray> allElements({"AB", "AC", "AB", "AC", "AC", "AB"});
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 2);
			QCOMPARE(chainBuilder.getChains().at(0).size(), 1);
			QCOMPARE(chainBuilder.getChains().at(1).size(), 1);
		}


		void testOneLongChain()
		{
			const QVector<QByteArray> allElements({"AB", "BC", "CD", "DE", "EF", "FG"});
			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 1);
			QCOMPARE(chainBuilder.getChains().at(0).size(), 6);
		}


		void testOneLongChainWithDuplicates()
		{
			const QVector<QByteArray> allElements({"AB", "BC", "BC", "CD", "BC", "CD", "DE", "DE", "EF", "BC", "FG"});
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
			const QVector<QByteArray> allElements({"AA", "AB", "BC", "CD", "DE", "BB"});

			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 2);
			QVERIFY(chainBuilder.getChains().contains(QVector<QByteArray>({"AA", "AB", "BC", "CD", "DE"})));
			QVERIFY(chainBuilder.getChains().contains(QVector<QByteArray>({"BB", "BC", "CD", "DE"})));
		}


		void testManyLeafs()
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
			const QVector<QByteArray> allElements({"AA", "AB", "BC", "CD", "DE", "CF"});

			ChainBuilder<QByteArray> chainBuilder(allElements, &test_ChainBuilder::isChild);

			QCOMPARE(chainBuilder.getChains().size(), 2);
			QVERIFY(chainBuilder.getChains().contains(QVector<QByteArray>({"AA", "AB", "BC", "CD", "DE"})));
			QVERIFY(chainBuilder.getChains().contains(QVector<QByteArray>({"AA", "AB", "BC", "CF"})));
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
				QVector<QByteArray> allElements({"AA", "AB", "BC", "CD", "DE", "BB", "CF"});
				const QVector<QByteArray> chain1({"AA", "AB", "BC", "CD", "DE"});
				const QVector<QByteArray> chain2({"BB", "BC", "CD", "DE"});
				const QVector<QByteArray> chain3({"AA", "AB", "BC", "CF"});
				const QVector<QByteArray> chain4({"BB", "BC", "CF"});

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
