/*!
 * test_BluetoothMessage.cpp
 * \brief Unit tests for \ref BluetoothMessage
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "messages/BluetoothMessageStatusInd.h"
#include <QtCore/QtCore>
#include <QtTest/QtTest>

using namespace governikus;

class test_BluetoothMessage
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void get()
		{
			BluetoothMessage msg(BluetoothMsgId::StatusInd);
			const BluetoothMessageStatusInd* obj = msg.get<BluetoothMessageStatusInd>();
			QVERIFY(obj != nullptr);
		}


		void statusIndCtor()
		{
			BluetoothMessageStatusInd msg;
			QCOMPARE(msg.getBluetoothMsgId(), BluetoothMsgId::StatusInd);
		}


		void statusIndStatusChange()
		{
			BluetoothMessageParameterStatusChange param(getEnumByteValue(BluetoothStatusChange::CardRecovered));
			BluetoothMessageStatusInd msg;
			msg.copyParameter(param);

			QVERIFY(msg.getParamStatusChange()->get<BluetoothMessageParameterStatusChange>() != nullptr);
			QCOMPARE(msg.getStatusChange(), BluetoothStatusChange::CardRecovered);
		}


};

QTEST_GUILESS_MAIN(test_BluetoothMessage)
#include "test_BluetoothMessage.moc"
