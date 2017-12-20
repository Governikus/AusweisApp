/*!
 * \brief Tests for card commands
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
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
