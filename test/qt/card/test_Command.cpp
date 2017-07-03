/*!
 * test_Command.cpp
 *
 * \brief Tests for card commands
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "Commands.h"

#include "TestFileHelper.h"

using namespace governikus;

class test_Command
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


		void testToBigEndian_fatInt()
		{
			QByteArray value = toBigEndian(static_cast<int>(0xFFEE));

			QCOMPARE(value.size(), 2);
			QCOMPARE(value.at(0), char(0xFF));
			QCOMPARE(value.at(1), char(0xEE));
		}


		void testToBigEndian_int()
		{
			QByteArray value = toBigEndian(static_cast<int>(0x11));

			QCOMPARE(value.size(), 1);
			QCOMPARE(value.at(0), static_cast<char>(0x11));
		}


		void testToBigEndian_zero()
		{
			QByteArray value = toBigEndian(static_cast<int>(0x00));

			QCOMPARE(value.size(), 1);
			QCOMPARE(value.at(0), static_cast<char>(0x00));
		}


		void testToBigEndian_long()
		{
			QByteArray value = toBigEndian(static_cast<long>(0x11));

			QCOMPARE(value.size(), 1);
			QCOMPARE(value.at(0), static_cast<char>(0x11));
		}


		void testPinModifyBuilder_createChangeEidPinCommandData()
		{
			PinModifyBuilder builder;
			quint8 timeout = 77;
			auto timeoutBytes = QByteArray::fromHex(QByteArray::number(timeout, 16));
			QCOMPARE(builder.createChangeEidPinCommandData(timeout), QByteArray::fromHex("82000000000606010202070400010200000004000000002C0203").prepend(timeoutBytes).prepend(timeoutBytes));
		}


};

QTEST_GUILESS_MAIN(test_Command)
#include "test_Command.moc"
