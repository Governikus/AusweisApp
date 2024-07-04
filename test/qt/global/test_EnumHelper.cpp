/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for EnumHelper.
 */

#include "EnumHelper.h"
#include "LogHandler.h"

#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
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

defineEnumType(TestEnum2, FIRST = 0xFF, SECOND = 0x01, THIRD = 0xAA)

defineTypedEnumType(TestEnum3, char, FIRST = static_cast<char>(0xFF), SECOND = static_cast<char>(0x01), THIRD = static_cast<char>(0xAA))

defineEnumTypeQmlExposed(TestEnum4, A, B, C)
} // namespace governikus

class test_EnumHelper
	: public QObject
{
	Q_OBJECT

	private:
		void testBadConverion(const int pValue, const QString& pExpectedOutput)
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			auto badEnumValue = static_cast<TestEnum1>(pValue);
			QCOMPARE(Enum<TestEnum1>::getName(badEnumValue), QLatin1String());

			QCOMPARE(logSpy.count(), 1);
			auto result = logSpy.takeFirst();
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
			QVERIFY(0xFF == TestEnum2::FIRST);
			QVERIFY(0xFF != TestEnum2::SECOND);

			QVERIFY(!(0xFF != TestEnum2::FIRST));
			QVERIFY(!(0xFF == TestEnum2::SECOND));
		}


		void operatorString()
		{
			QString str = "this is a dummy: "_L1;
			str += TestEnum1::FIRST;
			QCOMPARE(str, QStringLiteral("this is a dummy: FIRST"));

			QString str2 = "other text"_L1 + TestEnum1::FIRST + str + TestEnum1::SECOND + " test"_L1;
			QCOMPARE(str, QStringLiteral("this is a dummy: FIRST"));
			QCOMPARE(str2, QStringLiteral("other textFIRSTthis is a dummy: FIRSTSECOND test"));
		}


		void operatorDebug()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			qDebug() << TestEnum1::FIRST;
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, "FIRST"_L1));

			logSpy.takeFirst();
			QCOMPARE(logSpy.count(), 0);

			qDebug() << QList<TestEnum1>({TestEnum1::FIRST, TestEnum1::SECOND});
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, "(FIRST, SECOND)"_L1));
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
			const QList<TestEnum2>& list1 = Enum<TestEnum2>::getList();
			QCOMPARE(list1.size(), 3);

			const QList<TestEnum2>& list2 = Enum<TestEnum2>::getList();
			QCOMPARE(list2.size(), 3);

			QCOMPARE(list1, list2);

			QCOMPARE(list1[0], TestEnum2::FIRST);
			QCOMPARE(list1[1], TestEnum2::SECOND);
			QCOMPARE(list1[2], TestEnum2::THIRD);
		}


		void isValue()
		{
			QVERIFY(!Enum<TestEnum1>::isValue(999));
			QVERIFY(!Enum<TestEnum1>::isValue(static_cast<char>(999)));
			QVERIFY(Enum<TestEnum1>::isValue(0));
			QVERIFY(Enum<TestEnum1>::isValue('\0'));

			QVERIFY(!Enum<TestEnum2>::isValue(-69));
			QVERIFY(!Enum<TestEnum2>::isValue(0xBB));
			QVERIFY(!Enum<TestEnum2>::isValue(static_cast<char>(0xBB)));
			QVERIFY(!Enum<TestEnum2>::isValue(-1));
			QVERIFY(Enum<TestEnum2>::isValue(0xFF));
			QVERIFY(!Enum<TestEnum2>::isValue(static_cast<char>(0xFF)));
			QVERIFY(Enum<TestEnum2>::isValue(1));
			QVERIFY(Enum<TestEnum2>::isValue(static_cast<char>(0x01)));
			QVERIFY(!Enum<TestEnum2>::isValue(-86));
			QVERIFY(Enum<TestEnum2>::isValue(0xAA));
			QVERIFY(!Enum<TestEnum2>::isValue(static_cast<char>(0xAA)));

			QVERIFY(!Enum<TestEnum3>::isValue(-69));
			QVERIFY(!Enum<TestEnum3>::isValue(static_cast<char>(0xBB)));
			QVERIFY(Enum<TestEnum3>::isValue(-1));
			QVERIFY(Enum<TestEnum3>::isValue(static_cast<char>(0xFF)));
			QVERIFY(Enum<TestEnum3>::isValue(1));
			QVERIFY(Enum<TestEnum3>::isValue(static_cast<char>(0x01)));
			QVERIFY(Enum<TestEnum3>::isValue(-86));
			QVERIFY(Enum<TestEnum3>::isValue(static_cast<char>(0xAA)));
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

			QString value = "FIRST"_L1;
			QVERIFY(Enum<TestEnum1>::fromString(value, TestEnum1::THIRD) != TestEnum1::THIRD);
		}


		void checkQHash()
		{
			QMap<TestEnum1, QByteArray> dummy;
			dummy.insert(TestEnum1::FIRST, QByteArray("value 1"));
			dummy.insert(TestEnum1::SECOND, QByteArray("value 2"));
			dummy.insert(TestEnum1::THIRD, QByteArray("value 3"));

			QCOMPARE(dummy.size(), 3);
			QCOMPARE(dummy.value(TestEnum1::FIRST), QByteArray("value 1"));
			QCOMPARE(dummy.value(TestEnum1::SECOND), QByteArray("value 2"));
			QCOMPARE(dummy.value(TestEnum1::THIRD), QByteArray("value 3"));
			QVERIFY(dummy.contains(TestEnum1::FIRST));
			QVERIFY(dummy.contains(TestEnum1::SECOND));
			QVERIFY(dummy.contains(TestEnum1::THIRD));
			dummy.remove(TestEnum1::SECOND);
			QVERIFY(dummy.contains(TestEnum1::FIRST));
			QVERIFY(!dummy.contains(TestEnum1::SECOND));
			QVERIFY(dummy.contains(TestEnum1::THIRD));

			QSet<TestEnum2> dummySet;

			dummySet << TestEnum2::FIRST;
			QVERIFY(dummySet.contains(TestEnum2::FIRST));
			QVERIFY(!dummySet.contains(TestEnum2::SECOND));
			QVERIFY(!dummySet.contains(TestEnum2::THIRD));

			dummySet << TestEnum2::SECOND;
			QVERIFY(dummySet.contains(TestEnum2::FIRST));
			QVERIFY(dummySet.contains(TestEnum2::SECOND));
			QVERIFY(!dummySet.contains(TestEnum2::THIRD));

			dummySet << TestEnum2::THIRD;
			QVERIFY(dummySet.contains(TestEnum2::FIRST));
			QVERIFY(dummySet.contains(TestEnum2::SECOND));
			QVERIFY(dummySet.contains(TestEnum2::THIRD));

			dummySet.remove(TestEnum2::SECOND);
			QVERIFY(dummySet.contains(TestEnum2::FIRST));
			QVERIFY(!dummySet.contains(TestEnum2::SECOND));
			QVERIFY(dummySet.contains(TestEnum2::THIRD));
		}


		void enumProperty()
		{
			QCOMPARE(EnumTestEnum1::staticMetaObject.classInfoCount(), 0);

			QCOMPARE(EnumTestEnum4::staticMetaObject.classInfoCount(), 1);
			const auto meta = EnumTestEnum4::staticMetaObject.classInfo(0);
			QCOMPARE(meta.name(), "QML.Element");
			QCOMPARE(meta.value(), "TestEnum4");
		}


};

QTEST_GUILESS_MAIN(test_EnumHelper)
#include "test_EnumHelper.moc"
