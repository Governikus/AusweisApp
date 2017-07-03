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
		void statusIndCtor()
		{
			BluetoothMessageStatusInd msg;
			QCOMPARE(msg.getBluetoothMsgId(), BluetoothMsgId::StatusInd);
		}


		void statusIndStatusChange()
		{
			BluetoothMessageParameterStatusChange param(QByteArray(1, Enum<BluetoothStatusChange>::getValue(BluetoothStatusChange::CardRecovered)));
			BluetoothMessageStatusInd msg;
			msg.copyParameter(param);

			QCOMPARE(msg.getStatusChange(), BluetoothStatusChange::CardRecovered);
		}


};

QTEST_GUILESS_MAIN(test_BluetoothMessage)
#include "test_BluetoothMessage.moc"
