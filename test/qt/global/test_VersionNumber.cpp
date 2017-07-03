/*!
 * \brief Unit tests for \ref VersionNumber.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "VersionNumber.h"

#include <QtTest/QtTest>

using namespace governikus;

class test_VersionNumber
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void compare()
		{
			VersionNumber number1("1.2.3.4.5");
			VersionNumber number2("1.2.3.4.5");
			QCOMPARE(number1, number2);

			VersionNumber number3("4.5");
			QVERIFY(number1 != number3);
			QVERIFY(number1 < number3);
			QVERIFY(number3 > number1);

			VersionNumber number4("1.12.0+123");
			VersionNumber number5("1.12.0+124");
			QVERIFY(number4 < number5);
			QVERIFY(number5 > number4);

			VersionNumber number6("");
			QVERIFY(number1 != number6);


			VersionNumber number7("1.12.0+123-default-abc123");
			VersionNumber number8("1.12.0+123-default-abc123");
			QVERIFY(number7 == number8);
			QVERIFY(!(number7 < number8));
			QVERIFY(!(number7 > number8));
			QVERIFY(!(number8 < number7));
			QVERIFY(!(number8 > number7));

			VersionNumber number9("1.12.0+123-stable-xyz123");
			QVERIFY(number7 != number9);
			QVERIFY(!(number7 < number9));
			QVERIFY(!(number7 > number9));
			QVERIFY(!(number9 < number7));
			QVERIFY(!(number9 > number7));

			VersionNumber number10("1.01.0+123-default-abc123");
			VersionNumber number11("1.1.0+123-default-abc123");
			QVERIFY(number10 == number11);
			QVERIFY(!(number10 < number11));
			QVERIFY(!(number11 > number10));
		}


		void parser()
		{
			VersionNumber number("1.10.3.8");
			QCOMPARE(number.getVersionNumber().majorVersion(), 1);
			QCOMPARE(number.getVersionNumber().minorVersion(), 10);
			QCOMPARE(number.getVersionNumber().microVersion(), 3);

			VersionNumber numberSpace("  1.20.0  ");
			QCOMPARE(numberSpace.getVersionNumber().majorVersion(), 1);
			QCOMPARE(numberSpace.getVersionNumber().minorVersion(), 20);
			QCOMPARE(numberSpace.getVersionNumber().microVersion(), 0);
		}


		void distance()
		{
			VersionNumber number1("1.99+432");
			QCOMPARE(number1.getDistance(), 432);

			VersionNumber number2("1.99+98-default-et43t");
			QCOMPARE(number2.getDistance(), 98);

			VersionNumber number3("1.99+1-stable");
			QCOMPARE(number3.getDistance(), 1);

			VersionNumber number4("1.91.2.2.2.2");
			QCOMPARE(number4.getDistance(), -1);

			VersionNumber number5("1.91.2.7+534533");
			QCOMPARE(number5.getDistance(), 534533);

			VersionNumber number6("1.91.2.7+12++4243+2+2-default");
			QCOMPARE(number6.getDistance(), -1);

			VersionNumber number7("1.91.2.7+12++4243+2+2default");
			QCOMPARE(number7.getDistance(), -1);
		}


		void branch()
		{
			VersionNumber number1("1.99+432");
			QCOMPARE(number1.getBranch(), QString());

			VersionNumber number2("1.99+98-default-et43t");
			QCOMPARE(number2.getBranch(), QString("default"));

			VersionNumber number3("1.99+1-stable");
			QCOMPARE(number3.getBranch(), QString("stable"));

			VersionNumber number4("1.99+98-default-et43t+");
			QCOMPARE(number4.getBranch(), QString("default"));

			VersionNumber number5("1.99+98-default-draft-et43t+");
			QCOMPARE(number5.getBranch(), QString("default"));
		}


		void revision()
		{
			VersionNumber number1("1.99+432");
			QCOMPARE(number1.getRevision(), QString());

			VersionNumber number2("1.99+98-default-et43t");
			QCOMPARE(number2.getRevision(), QString("et43t"));

			VersionNumber number3("1.99+1-stable");
			QCOMPARE(number3.getRevision(), QString());

			VersionNumber number4("1.99+98-default-draft-et43t+");
			QCOMPARE(number4.getRevision(), QString("et43t+"));
		}


		void isDraft()
		{
			VersionNumber number1("1.5.0+16-default-secret");
			QVERIFY(number1.isDraft());

			VersionNumber number2("1.6.0+1-draft-t34t53+");
			QVERIFY(number2.isDraft());

			VersionNumber number3("1.6.0+12-stable-abc123");
			QVERIFY(!number3.isDraft());
		}


		void isDeveloper()
		{
			VersionNumber number1("1.5.0");
			QVERIFY(number1.isDeveloperVersion());

			VersionNumber number2("1.6.0");
			QVERIFY(!number2.isDeveloperVersion());

			VersionNumber number3("1.5.0+0");
			QVERIFY(number3.isDeveloperVersion());

			VersionNumber number4("1.6.0+0");
			QVERIFY(number4.isDeveloperVersion());

			VersionNumber number5("1.6.0+422312-stable-2143eg435");
			QVERIFY(number5.isDeveloperVersion());

			VersionNumber number6("1.9.0+422312-stable-2143eg435");
			QVERIFY(number6.isDeveloperVersion());

			VersionNumber number7("3.28.1");
			QVERIFY(!number7.isDeveloperVersion());

			VersionNumber number8("3.28.1+23-default");
			QVERIFY(number8.isDeveloperVersion());

			VersionNumber number9("  3.28.1+23-default   ");
			QVERIFY(number9.isDeveloperVersion());

			VersionNumber number10("    1.10.0      ");
			QVERIFY(!number10.isDeveloperVersion());
		}


		void toString()
		{
			VersionNumber number1("1.6.0+422312-stable-2143eg435");
			QCOMPARE(number1.getVersionNumber().toString(), QString("1.6.0"));
		}


};

QTEST_GUILESS_MAIN(test_VersionNumber)
#include "test_VersionNumber.moc"
