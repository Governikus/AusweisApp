/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref VersionNumber.
 */

#include "VersionNumber.h"

#include <QtTest>

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


		void distance_data()
		{
			QTest::addColumn<int>("distance");

			QTest::newRow("1.99+432") << 432;
			QTest::newRow("1.99+98-default-et43t") << 98;
			QTest::newRow("1.99+1-stable") << 1;
			QTest::newRow("1.99broken+77-stable") << 77;
			QTest::newRow("1.91.2.2.2.2") << -1;
			QTest::newRow("1.91.2.2.2.2-5") << -1;
			QTest::newRow("1.0+-stable") << -1;
			QTest::newRow("1.0+5stable") << -1;
			QTest::newRow("1.91.2.7+534533") << 534533;
			QTest::newRow("1.91.2.7+12++4243+2+2-default") << -1;
			QTest::newRow("1.91.2.7+12++4243+2+2default") << -1;
		}


		void distance()
		{
			QFETCH(int, distance);

			const auto& version = QString::fromLatin1(QTest::currentDataTag());
			QCOMPARE(VersionNumber(version).getDistance(), distance);
		}


		void branch_data()
		{
			QTest::addColumn<QString>("branch");

			QTest::newRow("1.99+432") << QString();
			QTest::newRow("1.99+98-default-et43t") << QString("default");
			QTest::newRow("1.99+1-stable") << QString("stable");
			QTest::newRow("1.99+98-default-et43t+") << QString("default");
			QTest::newRow("1.99+98-default-draft-et43t+") << QString("default");
		}


		void branch()
		{
			QFETCH(QString, branch);

			const auto& version = QString::fromLatin1(QTest::currentDataTag());
			QCOMPARE(VersionNumber(version).getBranch(), branch);
		}


		void revision_data()
		{
			QTest::addColumn<QString>("revision");

			QTest::newRow("1.99+432") << QString();
			QTest::newRow("1.99+98-default-et43t") << QString("et43t");
			QTest::newRow("1.99+1-stable") << QString();
			QTest::newRow("1.99+98-default-draft-et43t+") << QString("et43t+");
		}


		void revision()
		{
			QFETCH(QString, revision);

			const auto& version = QString::fromLatin1(QTest::currentDataTag());
			QCOMPARE(VersionNumber(version).getRevision(), revision);
		}


		void isDraft_data()
		{
			QTest::addColumn<bool>("draft");

			QTest::newRow("1.5.0+16-default-secret") << true;
			QTest::newRow("1.6.0+1-draft-t34t53+") << true;
			QTest::newRow("1.6.0+12-stable-abc123") << false;
		}


		void isDraft()
		{
			QFETCH(bool, draft);

			const auto& version = QString::fromLatin1(QTest::currentDataTag());
			QCOMPARE(VersionNumber(version).isDraft(), draft);
		}


		void isDeveloper_data()
		{
			QTest::addColumn<bool>("developer");
			QTest::addColumn<bool>("beta");

			QTest::newRow("") << true << false;
			QTest::newRow("1.5.0+16-default-secret") << true << false;
			QTest::newRow("1.6.0+1-draft-t34t53+") << true << false;
			QTest::newRow("1.5.0") << false << false;
			QTest::newRow("1.6.0") << false << false;
			QTest::newRow("1.5.0+0") << true << false;
			QTest::newRow("1.6.0+0") << true << false;
			QTest::newRow("1.6.0+422312-stable-2143eg435") << true << false;
			QTest::newRow("1.9.0+422312-stable-2143eg435") << true << false;
			QTest::newRow("3.28.1") << false << false;
			QTest::newRow("3.28.1+23-default") << true << false;
			QTest::newRow("  3.28.1+23-default   ") << true << false;
			QTest::newRow("    1.10.0      ") << false << false;
			QTest::newRow("1.100.0") << true << true;
			QTest::newRow("1.100.100") << true << true;
			QTest::newRow("1.200.0") << true << true;
			QTest::newRow("1.200.199") << true << true;
			QTest::newRow("2.99.0") << false << false;
			QTest::newRow("2.1.0") << false << false;
			QTest::newRow("2.1.100") << true << true;
			QTest::newRow("2.1.199") << true << true;
			QTest::newRow("2.2.0") << false << false;
			QTest::newRow("2.2.100") << true << true;
			QTest::newRow("2.2.100+0") << true << true;
			QTest::newRow("2.3.0+0") << true << false;
		}


		void isDeveloper()
		{
			QFETCH(bool, developer);
			QFETCH(bool, beta);

			const auto& version = VersionNumber(QString::fromLatin1(QTest::currentDataTag()));
			QCOMPARE(version.isDeveloperVersion(), developer);
			QCOMPARE(version.isBetaVersion(), beta);
		}


		void toString()
		{
			VersionNumber number1("1.6.0+422312-stable-2143eg435");
			QCOMPARE(number1.getVersionNumber().toString(), QString("1.6.0"));
		}


		void debugStream()
		{
			QTest::ignoreMessage(QtDebugMsg, "1.6.0+422312-stable-2143eg435");
			VersionNumber number1("1.6.0+422312-stable-2143eg435");
			qDebug() << number1;
		}


};

QTEST_GUILESS_MAIN(test_VersionNumber)
#include "test_VersionNumber.moc"
