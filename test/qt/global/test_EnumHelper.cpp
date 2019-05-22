/*!
 * \brief Unit tests for EnumHelper.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "EnumHelper.h"
#include "LogHandler.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

namespace
{
#ifdef Q_OS_WIN
const QLatin1String lineBreak("\r\n");
#else
const QLatin1Char lineBreak('\n');
#endif
} // namespace

namespace governikus
{
defineEnumType(TestEnum1, FIRST, SECOND, THIRD)

defineEnumType(TestEnum2, FIRST = 0xff, SECOND = 0x01, THIRD = 0xaa)
} // namespace governikus

class test_EnumHelper
	: public QObject
{
	Q_OBJECT

	private:
		void testBadConverion(const int pValue, const QString& pExpectedOutput)
		{
			QSignalSpy spy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			TestEnum1 badEnumValue = static_cast<TestEnum1>(pValue);
			QCOMPARE(Enum<TestEnum1>::getName(badEnumValue), QLatin1String());

			QCOMPARE(spy.count(), 1);
			auto result = spy.takeFirst();
			QVERIFY(result.at(0).toString().endsWith(pExpectedOutput));
		}


	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void operatorEquals()
		{
			QVERIFY(0xff == TestEnum2::FIRST);
			QVERIFY(0xff != TestEnum2::SECOND);

			QVERIFY(!(0xff != TestEnum2::FIRST));
			QVERIFY(!(0xff == TestEnum2::SECOND));
		}


		void operatorString()
		{
			QString str = "this is a dummy: ";
			str += TestEnum1::FIRST;
			QCOMPARE(str, QStringLiteral("this is a dummy: FIRST"));

			QString str2 = "other text" + TestEnum1::FIRST + str + TestEnum1::SECOND + " test";
			QCOMPARE(str, QStringLiteral("this is a dummy: FIRST"));
			QCOMPARE(str2, QStringLiteral("other textFIRSTthis is a dummy: FIRSTSECOND test"));
		}


		void operatorDebug()
		{
			QSignalSpy spy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			qDebug() << TestEnum1::FIRST;

			QCOMPARE(spy.count(), 1);
			auto result = spy.takeFirst();
			QVERIFY(result.at(0).toString().endsWith(QStringLiteral("FIRST") + lineBreak));
		}


		void allGetter()
		{
			QCOMPARE(Enum<TestEnum1>::getCount(), 3);
			QCOMPARE(Enum<TestEnum1>::getName(), QStringLiteral("TestEnum1"));
			QCOMPARE(Enum<TestEnum1>::getName(TestEnum1::FIRST), QStringLiteral("FIRST"));
			QCOMPARE(Enum<TestEnum1>::getName(TestEnum1::SECOND), QStringLiteral("SECOND"));
			QCOMPARE(Enum<TestEnum1>::getName(TestEnum1::THIRD), QStringLiteral("THIRD"));

			testBadConverion(6, QStringLiteral("UNKNOWN 0x6") + lineBreak);
			testBadConverion(255, QStringLiteral("UNKNOWN 0xff") + lineBreak);
			testBadConverion(365, QStringLiteral("UNKNOWN 0x16d") + lineBreak);
			testBadConverion(2147483647, QStringLiteral("UNKNOWN 0x7fffffff") + lineBreak);

			QCOMPARE(getEnumName(TestEnum1::SECOND), QStringLiteral("SECOND"));
			QCOMPARE(getEnumName(EnumTestEnum1::TestEnum1::SECOND), QStringLiteral("SECOND"));
		}


		void list()
		{
			const QVector<TestEnum2>& list1 = Enum<TestEnum2>::getList();
			QCOMPARE(list1.size(), 3);

			const QVector<TestEnum2>& list2 = Enum<TestEnum2>::getList();
			QCOMPARE(list2.size(), 3);

			QCOMPARE(list1, list2);
			QVERIFY(&list1 == &list2);

			QCOMPARE(list1[0], TestEnum2::FIRST);
			QCOMPARE(list1[1], TestEnum2::SECOND);
			QCOMPARE(list1[2], TestEnum2::THIRD);
		}


		void isValue()
		{
			QVERIFY(!Enum<TestEnum1>::isValue(static_cast<int>(999)));
			QVERIFY(Enum<TestEnum1>::isValue(static_cast<int>(0)));

			QVERIFY(!Enum<TestEnum2>::isValue(static_cast<int>(0xbb)));
			QVERIFY(Enum<TestEnum2>::isValue(static_cast<int>(0xFF)));
			QVERIFY(Enum<TestEnum2>::isValue(static_cast<int>(0xaa)));

			QVERIFY(!Enum<TestEnum1>::isValue(char(999)));
			QVERIFY(Enum<TestEnum1>::isValue('\0'));

			QVERIFY(!Enum<TestEnum2>::isValue(char(0xbb)));
			QVERIFY(Enum<TestEnum2>::isValue(char(0xff)));
			QVERIFY(Enum<TestEnum2>::isValue(char(0xaa)));
		}


		void fromString()
		{
			QCOMPARE(Enum<TestEnum1>::fromString("SECOND", TestEnum1::THIRD), TestEnum1::SECOND);
			QVERIFY(Enum<TestEnum1>::fromString("SECOND", TestEnum1::THIRD) != TestEnum1::FIRST);

			QCOMPARE(Enum<TestEnum1>::fromString("FIRST", TestEnum1::THIRD), TestEnum1::FIRST);
			QVERIFY(Enum<TestEnum1>::fromString("FIRST", TestEnum1::THIRD) != TestEnum1::SECOND);

			QVERIFY(Enum<TestEnum1>::fromString("first", TestEnum1::THIRD) != TestEnum1::FIRST);
			QVERIFY(Enum<TestEnum1>::fromString("second", TestEnum1::THIRD) != TestEnum1::SECOND);

			QVERIFY(Enum<TestEnum1>::fromString("first", TestEnum1::THIRD) != TestEnum1::SECOND);
			QVERIFY(Enum<TestEnum1>::fromString("second", TestEnum1::THIRD) != TestEnum1::FIRST);

			QCOMPARE(Enum<TestEnum1>::fromString("abc", TestEnum1::THIRD), TestEnum1::THIRD);
			QVERIFY(Enum<TestEnum1>::fromString("FIRST", TestEnum1::THIRD) != TestEnum1::THIRD);

			QString value = "FIRST";
			QVERIFY(Enum<TestEnum1>::fromString(value, TestEnum1::THIRD) != TestEnum1::THIRD);
		}


};

QTEST_GUILESS_MAIN(test_EnumHelper)
#include "test_EnumHelper.moc"
