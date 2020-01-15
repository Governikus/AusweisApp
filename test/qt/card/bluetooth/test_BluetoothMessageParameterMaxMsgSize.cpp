/*!
 * \brief Unit tests for \ref BluetoothMessageParameterMaxMsgSize *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/parameter/BluetoothMessageParameterMaxMsgSize.h"
#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_BluetoothMessageParameterMaxMsgSize
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		/*
		 * The value is a 2 byte big endian. So perform some tests.
		 */
		void createWithByteArray()
		{
			BluetoothMessageParameterMaxMsgSize param(QByteArray::fromHex("0100"));
			QCOMPARE(param.getMaxMsgSize(), static_cast<unsigned int>(256));
			QCOMPARE(param.getValue(), QByteArray::fromHex("0100"));
		}


		void createWithByteArray_tooBig()
		{
			BluetoothMessageParameterMaxMsgSize param(QByteArray::fromHex("010000"));
			QCOMPARE(param.getMaxMsgSize(), static_cast<unsigned int>(0));
			QCOMPARE(param.getValue(), QByteArray(""));
		}


		void createWithInt()
		{
			BluetoothMessageParameterMaxMsgSize param(256);
			QCOMPARE(param.getValue().toHex(), QByteArray("0100"));
			QCOMPARE(param.getMaxMsgSize(), static_cast<unsigned int>(256));
		}


		void createWithInt_tooBig()
		{
			BluetoothMessageParameterMaxMsgSize param(0x010000);
			QCOMPARE(param.getValue().toHex(), QByteArray(""));
			QCOMPARE(param.getMaxMsgSize(), static_cast<unsigned int>(0));
		}


};

QTEST_GUILESS_MAIN(test_BluetoothMessageParameterMaxMsgSize)
#include "test_BluetoothMessageParameterMaxMsgSize.moc"
