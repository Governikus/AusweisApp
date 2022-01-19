/*!
 * \brief Unit tests for PskManager.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "PskManager.h"

#include <QtTest>

using namespace governikus;

class test_PskManager
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isSecure()
		{
			QVERIFY(!PskManager::getInstance().isSecureRandomPsk());
			QVERIFY(!PskManager::getInstance().generatePsk().isEmpty());
			QVERIFY(PskManager::getInstance().isSecureRandomPsk());
		}


		void generatePskWithoutClientPartial()
		{
			QByteArray prev;
			for (int i = 0; i < 100; ++i)
			{
				const auto& tmp = PskManager::getInstance().generatePsk();
				QVERIFY(prev != tmp);
				QCOMPARE(tmp, PskManager::getInstance().getPsk());
				QVERIFY(PskManager::getInstance().isSecureRandomPsk());
				prev = tmp;
			}
		}


};

QTEST_GUILESS_MAIN(test_PskManager)
#include "test_PskManager.moc"
