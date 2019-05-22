/*!
 * \brief Unit tests for \ref Initializer
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "Initializer.h"

#include <QtTest/QtTest>

using namespace governikus;

class test_Initializer
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			Initializer::getInstance().mRegisteredFunctions.clear();
		}


		void simpleInit()
		{
			auto& instance = Initializer::getInstance();
			QCOMPARE(instance.mRegisteredFunctions.size(), size_t(0));

			bool flipMe = false;
			instance.add([&flipMe] {
						flipMe = true;
					});

			QCOMPARE(instance.mRegisteredFunctions.size(), size_t(1));
			QVERIFY(flipMe);
		}


		void entryInit()
		{
			auto& instance = Initializer::getInstance();
			QCOMPARE(instance.mRegisteredFunctions.size(), size_t(0));

			bool flipMeX = false;
			bool flipMeY = false;
			Initializer::Entry X([&flipMeX] {
						flipMeX = true;
					});
			Q_UNUSED(X)

			QCOMPARE(instance.mRegisteredFunctions.size(), size_t(1));
			QVERIFY(flipMeX);

			static Initializer::Entry Y([&flipMeY] {
						flipMeY = true;
					});
			Q_UNUSED(Y)

			QCOMPARE(instance.mRegisteredFunctions.size(), size_t(2));
			QVERIFY(flipMeX);
			QVERIFY(flipMeY);
		}


};

QTEST_GUILESS_MAIN(test_Initializer)
#include "test_Initializer.moc"
