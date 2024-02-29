/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Tests for SecureMessagingCommand
 */

#include "apdu/SecureMessagingCommand.h"

#include "LogHandler.h"

#include <QSignalSpy>
#include <QtCore>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(CommandApdu)


class test_SecureMessagingCommand
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

			SecureMessagingCommand command((CommandApdu()));
			QVERIFY(!command.isValid());
			QCOMPARE(command.getEncryptedData(), QByteArray());
			QCOMPARE(command.getEncryptedDataObjectEncoded(), QByteArray());
			QCOMPARE(command.getExpectedLength(), 0);
			QCOMPARE(command.getExpectedLengthObjectEncoded(), QByteArray());
			QCOMPARE(command.getMac(), QByteArray());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("No data to decrypt"_L1));
		}


		void plaintext_data()
		{
			QTest::addColumn<CommandApdu>("input");

			QTest::newRow("No Data, No LE") << CommandApdu(Ins::MSE_SET, 0, 0);
			QTest::newRow("LE only") << CommandApdu(Ins::MSE_SET, 0, 0, QByteArray(), 24);
			QTest::newRow("Data only") << CommandApdu(Ins::MSE_SET, 0, 0, QByteArray::fromHex("05060708"));
			QTest::newRow("Data and LE") << CommandApdu(Ins::MSE_SET, 0, 0, QByteArray::fromHex("05060708"), 24);
		}


		void plaintext()
		{
			QFETCH(CommandApdu, input);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			SecureMessagingCommand command(input);
			QVERIFY(!command.isValid());
			QCOMPARE(command.getEncryptedData(), QByteArray());
			QCOMPARE(command.getEncryptedDataObjectEncoded(), QByteArray());
			QCOMPARE(command.getExpectedLength(), 0);
			QCOMPARE(command.getExpectedLengthObjectEncoded(), QByteArray());
			QCOMPARE(command.getMac(), QByteArray());

			QCOMPARE(logSpy.count(), 1);
			if (input.getData().isEmpty())
			{
				QVERIFY(logSpy.at(0).at(0).toString().contains("No data to decrypt"_L1));
			}
			else
			{
				QVERIFY(logSpy.at(0).at(0).toString().contains("Error on decoding mac"_L1));
			}
		}


		void noPaddingContentIndicator()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			SecureMessagingCommand command(CommandApdu(Ins::MSE_SET, 0, 0, QByteArray::fromHex("8704050607088E080102030405060708")));
			QVERIFY(!command.isValid());

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Error on decoding encrypted data"_L1));
		}


		void wrongMac_data()
		{
			QTest::addColumn<CommandApdu>("input");

			QTest::newRow("No Mac - No Data, No LE") << CommandApdu(Ins::MSE_SET, 0, 0);
			QTest::newRow("No Mac - LE only") << CommandApdu(Ins::MSE_SET, 0, 0, QByteArray::fromHex("970108"));
			QTest::newRow("No Mac - Data only") << CommandApdu(Ins::MSE_SET, 0, 0, QByteArray::fromHex("87050105060708"));
			QTest::newRow("No Mac - Data and LE") << CommandApdu(Ins::MSE_SET, 0, 0, QByteArray::fromHex("87050105060708970108"));

			QTest::newRow("To Short Mac - No Data, No LE") << CommandApdu(Ins::MSE_SET, 0, 0, QByteArray::fromHex("0E0701020304050607"));
			QTest::newRow("To Short Mac - LE only") << CommandApdu(Ins::MSE_SET, 0, 0, QByteArray::fromHex("9701080E0701020304050607"));
			QTest::newRow("To Short Mac - Data only") << CommandApdu(Ins::MSE_SET, 0, 0, QByteArray::fromHex("870501050607080E0701020304050607"));
			QTest::newRow("To Short Mac - Data and LE") << CommandApdu(Ins::MSE_SET, 0, 0, QByteArray::fromHex("870501050607089701080E0701020304050607"));
		}


		void wrongMac()
		{
			QFETCH(CommandApdu, input);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			SecureMessagingCommand command(input);
			QVERIFY(!command.isValid());

			QCOMPARE(logSpy.count(), 1);
			if (input.getData().isEmpty())
			{
				QVERIFY(logSpy.at(0).at(0).toString().contains("No data to decrypt"_L1));
			}
			else
			{
				QVERIFY(logSpy.at(0).at(0).toString().contains("Error on decoding mac"_L1));
			}
		}


		void secureMessagingCommand_data()
		{
			QTest::addColumn<QByteArray>("encryptedDataHeader");
			QTest::addColumn<QByteArray>("encryptedData");
			QTest::addColumn<QByteArray>("expectedLengthHeader");
			QTest::addColumn<QByteArray>("expectedLength");

			QTest::newRow("No Data, No LE") << QByteArray() << QByteArray() << QByteArray() << QByteArray();
			QTest::newRow("LE only") << QByteArray() << QByteArray() << QByteArray::fromHex("9701") << QByteArray::fromHex("08");
			QTest::newRow("Data only") << QByteArray::fromHex("870501") << QByteArray::fromHex("05060708") << QByteArray() << QByteArray();
			QTest::newRow("Data and LE") << QByteArray::fromHex("870501") << QByteArray::fromHex("05060708") << QByteArray::fromHex("9701") << QByteArray::fromHex("08");
		}


		void secureMessagingCommand()
		{
			QFETCH(QByteArray, encryptedDataHeader);
			QFETCH(QByteArray, encryptedData);
			QFETCH(QByteArray, expectedLengthHeader);
			QFETCH(QByteArray, expectedLength);

			const QByteArray data = encryptedDataHeader + encryptedData + expectedLengthHeader + expectedLength + QByteArray::fromHex("8E080102030405060708");
			SecureMessagingCommand command(CommandApdu(Ins::MSE_SET, 0, 0, data));

			QCOMPARE(command.getEncryptedData(), encryptedData);
			QCOMPARE(command.getEncryptedDataObjectEncoded(), encryptedDataHeader + encryptedData);
			QCOMPARE(command.getExpectedLength(), expectedLength.isEmpty() ? 0 : expectedLength.at(0));
			QCOMPARE(command.getExpectedLengthObjectEncoded(), expectedLengthHeader + expectedLength);
			QCOMPARE(command.getMac(), QByteArray::fromHex("0102030405060708"));
		}


		void expectedLength_data()
		{
			QTest::addColumn<QByteArray>("encodedExpectedLength");
			QTest::addColumn<int>("expectedLength");

			QTest::newRow("Short LE") << QByteArray::fromHex("970108") << 8;
			QTest::newRow("Short Max LE") << QByteArray::fromHex("970100") << 256;
			QTest::newRow("Long LE") << QByteArray::fromHex("97020101") << 257;
			QTest::newRow("Long Max LE") << QByteArray::fromHex("97020000") << 65536;
		}


		void expectedLength()
		{
			QFETCH(QByteArray, encodedExpectedLength);
			QFETCH(int, expectedLength);

			const QByteArray data = encodedExpectedLength + QByteArray::fromHex("8E080102030405060708");
			SecureMessagingCommand command(CommandApdu(Ins::MSE_SET, 0, 0, data));
			QCOMPARE(command.getExpectedLength(), expectedLength);
		}


};

QTEST_GUILESS_MAIN(test_SecureMessagingCommand)
#include "test_SecureMessagingCommand.moc"
