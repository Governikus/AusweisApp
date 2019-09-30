/*!
 * \brief Unit tests for \ref BluetoothMessage
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "messages/BluetoothMessage.h"
#include "messages/BluetoothMessageStatusInd.h"

#include <QtCore>
#include <QtTest>

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


		void toData()
		{
			BluetoothMessage msg(BluetoothMsgId::ConnectRequest);
			const QSharedPointer<BluetoothMessageParameter> maxMsgSize(new BluetoothMessageParameter(BluetoothParamId::MaxMsgSize, QByteArray::fromHex("111111")));
			const QSharedPointer<BluetoothMessageParameter> connectionStatus(new BluetoothMessageParameter(BluetoothParamId::ConnectionStatus, QByteArray::fromHex("101010")));
			msg.addParameter(maxMsgSize);
			msg.addParameter(connectionStatus);
			QCOMPARE(msg.toData().toHex(), QByteArray("0002000000000003111111000100000310101000"));
		}


		void toString()
		{
			BluetoothMessage msg(BluetoothMsgId::ConnectRequest);
			const QSharedPointer<BluetoothMessageParameter> maxMsgSize(new BluetoothMessageParameter(BluetoothParamId::MaxMsgSize, QByteArray::fromHex("111111")));
			const QSharedPointer<BluetoothMessageParameter> connectionStatus(new BluetoothMessageParameter(BluetoothParamId::ConnectionStatus, QByteArray::fromHex("101010")));
			msg.addParameter(maxMsgSize);
			msg.addParameter(connectionStatus);
			QCOMPARE(msg.toString(), QString("ConnectRequest | Parameter: MaxMsgSize | Value: 111111 | Parameter: ConnectionStatus | Value: 101010"));
		}


};

QTEST_GUILESS_MAIN(test_BluetoothMessage)
#include "test_BluetoothMessage.moc"
