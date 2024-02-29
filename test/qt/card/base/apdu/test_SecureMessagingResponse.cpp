/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Tests for SecureMessagingResponse
 */

#include "apdu/SecureMessagingResponse.h"

#include "LogHandler.h"

#include <QSignalSpy>
#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(ResponseApdu)


class test_SecureMessagingResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void noData()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray bytes = QByteArray::fromHex("");

			SecureMessagingResponse response((ResponseApdu(bytes)));
			QVERIFY(!response.isValid());
			QCOMPARE(response.getEncryptedData(), QByteArray());
			QCOMPARE(response.getEncryptedDataObjectEncoded(), QByteArray());
			QCOMPARE(response.getMac(), QByteArray());
			QCOMPARE(response.getSecuredStatusCode(), StatusCode::INVALID_SM_OBJECTS);
			QCOMPARE(response.getSecuredStatusCodeBytes(), QByteArray());
			QCOMPARE(response.getSecuredStatusCodeObjectEncoded(), QByteArray());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("No data to decrypt"_L1));
		}


		void secureMessagingDataObjectsAreIncorrect()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray bytes = QByteArray::fromHex("6988");

			SecureMessagingResponse response((ResponseApdu(bytes)));
			QVERIFY(!response.isValid());
			QCOMPARE(response.getEncryptedData(), QByteArray());
			QCOMPARE(response.getEncryptedDataObjectEncoded(), QByteArray());
			QCOMPARE(response.getMac(), QByteArray());
			QCOMPARE(response.getSecuredStatusCode(), StatusCode::INVALID_SM_OBJECTS);
			QCOMPARE(response.getSecuredStatusCodeBytes(), QByteArray());
			QCOMPARE(response.getSecuredStatusCodeObjectEncoded(), QByteArray());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("No data to decrypt"_L1));
		}


		void noPaddingContentIndicator()
		{
			QByteArray bytes = QByteArray::fromHex("87060F1020304050990290008E0811213141516171819000");

			SecureMessagingResponse response((ResponseApdu(bytes)));
			QVERIFY(!response.isValid());
		}


		void noProcessingStatus()
		{
			QByteArray bytes = QByteArray::fromHex("87060110203040508E0811213141516171819000");

			SecureMessagingResponse response((ResponseApdu(bytes)));
			QVERIFY(!response.isValid());
		}


		void processingStatusWithWrongLength()
		{
			QByteArray bytes = QByteArray::fromHex("870601102030405099039090008E0811213141516171819000");

			SecureMessagingResponse response((ResponseApdu(bytes)));
			QVERIFY(!response.isValid());
		}


		void noMac()
		{
			QByteArray bytes = QByteArray::fromHex("990290009000");

			SecureMessagingResponse response((ResponseApdu(bytes)));
			QVERIFY(!response.isValid());
		}


		void macWithWrongLength()
		{
			QByteArray bytes = QByteArray::fromHex("990290008E07112131415161719000");

			SecureMessagingResponse response((ResponseApdu(bytes)));
			QVERIFY(!response.isValid());
		}


		void secureMessagingResponseNoCryptogram()
		{
			QByteArray bytes = QByteArray::fromHex("990290008E0811213141516171819000");

			SecureMessagingResponse response((ResponseApdu(bytes)));
			QCOMPARE(response.getEncryptedData(), QByteArray());
			QCOMPARE(response.getEncryptedDataObjectEncoded(), QByteArray());
			QCOMPARE(response.getSecuredStatusCode(), StatusCode::SUCCESS);
			QCOMPARE(response.getSecuredStatusCodeBytes(), QByteArray::fromHex("9000"));
			QCOMPARE(response.getSecuredStatusCodeObjectEncoded(), QByteArray::fromHex("99029000"));
			QCOMPARE(response.getMac(), QByteArray::fromHex("1121314151617181"));
		}


		void secureMessagingResponse()
		{
			QByteArray bytes = QByteArray::fromHex("8706011020304050990290008E0811213141516171819000");

			SecureMessagingResponse response((ResponseApdu(bytes)));
			QCOMPARE(response.getEncryptedData(), QByteArray::fromHex("1020304050"));
			QCOMPARE(response.getEncryptedDataObjectEncoded(), QByteArray::fromHex("8706011020304050"));
			QCOMPARE(response.getSecuredStatusCode(), StatusCode::SUCCESS);
			QCOMPARE(response.getSecuredStatusCodeBytes(), QByteArray::fromHex("9000"));
			QCOMPARE(response.getSecuredStatusCodeObjectEncoded(), QByteArray::fromHex("99029000"));
			QCOMPARE(response.getMac(), QByteArray::fromHex("1121314151617181"));
		}


};

QTEST_GUILESS_MAIN(test_SecureMessagingResponse)
#include "test_SecureMessagingResponse.moc"
