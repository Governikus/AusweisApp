/*!
 * \brief Unit tests for \ref BluetoothMessageParameterCardReaderStatus
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "messages/parameter/BluetoothMessageParameterCardReaderStatus.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_BluetoothMessageParameterCardReaderStatus
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_GetStatusChange()
		{
			const QByteArray value("value");
			BluetoothMessageParameterCardReaderStatus status(value);

			QCOMPARE(status.getStatusChange(), BluetoothStatusChange::Unknown);

			status.mCardReaderStatus = BluetoothCardReaderStatus::CardInserted;
			QCOMPARE(status.getStatusChange(), BluetoothStatusChange::CardInserted);

			status.mCardReaderStatus = BluetoothCardReaderStatus::CardRemoved;
			QCOMPARE(status.getStatusChange(), BluetoothStatusChange::CardRemoved);
		}


		void test_GetReaderStatus()
		{
			const QByteArray value("value");
			BluetoothMessageParameterCardReaderStatus status(value);

			QCOMPARE(status.getCardReaderStatus(), BluetoothCardReaderStatus::Unknown);

			status.mCardReaderStatus = BluetoothCardReaderStatus::CardInserted;
			QCOMPARE(status.getCardReaderStatus(), BluetoothCardReaderStatus::CardInserted);
		}


		void test_ToStringValue()
		{
			const QByteArray value("value");
			BluetoothMessageParameterCardReaderStatus status(value);

			QCOMPARE(status.toStringValue(), QString("Unknown"));
		}


};

QTEST_GUILESS_MAIN(test_BluetoothMessageParameterCardReaderStatus)
#include "test_BluetoothMessageParameterCardReaderStatus.moc"
