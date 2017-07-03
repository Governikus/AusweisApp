/*!
 * test_EstablishPACEChannelOutput.cpp
 *
 * \brief Tests for card EstablishPACEChannelOutput
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "EstablishPACEChannel.h"
#include "TestFileHelper.h"


using namespace governikus;


class test_EstablishPACEChannelOutput
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseEstablishPACEChannelOutput()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/EstablishPACEChannelOutput.hex"));

			EstablishPACEChannelOutput channelOutput;
			channelOutput.parse(bytes, PACE_PIN_ID::PACE_PIN);
			QCOMPARE(channelOutput.getCARcurr(), QByteArray("DETESTeID00004"));
			QVERIFY(channelOutput.getCARprev().isEmpty());
			QVERIFY(channelOutput.getEfCardAccess().toHex().startsWith(QByteArray("3181c1300d0608040")));
		}


		void parseEstablishPACEChannelOutput_wrongPIN()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/EstablishPACEChannelOutput_wrongPIN.hex"));

			EstablishPACEChannelOutput channelOutput;
			channelOutput.parse(bytes, PACE_PIN_ID::PACE_PIN);
			QVERIFY(channelOutput.getCARcurr().isEmpty());
			QVERIFY(channelOutput.getCARprev().isEmpty());
			QVERIFY(channelOutput.getEfCardAccess().isEmpty());
		}


		void parseEstablishPACEChannelOutput_fromCcid()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/EstablishPACEChannelOutput_fromCcid.hex"));

			EstablishPACEChannelOutput channelOutput;
			channelOutput.parseFromCcid(bytes, PACE_PIN_ID::PACE_PIN);
			QCOMPARE(CardReturnCode::OK, channelOutput.getPaceReturnCode());
			QCOMPARE(channelOutput.getCARcurr(), QByteArray("DECVCAeID00103"));
			QCOMPARE(channelOutput.getCARprev(), QByteArray("DECVCAeID00102"));
			QVERIFY(!channelOutput.getEfCardAccess().isEmpty());
			QVERIFY(!channelOutput.getIDicc().isEmpty());
		}


		void parseEstablishPACEChannelOutput_fromCcid2()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/EstablishPACEChannelOutput_fromCcid2.hex"));

			EstablishPACEChannelOutput channelOutput;
			channelOutput.parseFromCcid(bytes, PACE_PIN_ID::PACE_PIN);
			QCOMPARE(CardReturnCode::OK, channelOutput.getPaceReturnCode());
			QVERIFY(channelOutput.getCARcurr().isEmpty());
			QVERIFY(channelOutput.getCARprev().isEmpty());
			QVERIFY(!channelOutput.getEfCardAccess().isEmpty());
			QVERIFY(!channelOutput.getIDicc().isEmpty());
		}


		void parseEstablishPACEChannelOutput_fromCcid_parseCrap()
		{
			QByteArray hexBytes = QByteArray("30 0C"
											 "            04 04 F0200001"
											 "            04 02 9000"
											 "            31 00"
					//											 "            A4 22 04 20 24e41d62b8c848226b86fcc6c7657577dca47ad2bf21573617bae84807f85c6b"
					//											 "            A5 10 040e4445435643416549443030313033"
					//											 "            A6 10 040e4445435643416549443030313032"
											 "            9000" // ReturnCode
					);

			EstablishPACEChannelOutput channelOutput;
			channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes), PACE_PIN_ID::PACE_PIN);
			QCOMPARE(CardReturnCode::UNKNOWN, channelOutput.getPaceReturnCode());
			QCOMPARE(channelOutput.getCARcurr(), QByteArray());
			QCOMPARE(channelOutput.getCARprev(), QByteArray());
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray());
			QCOMPARE(channelOutput.getIDicc(), QByteArray());
		}


		void parseEstablishPACEChannelOutput_fromCcid_onlyMandatoryElements()
		{
			QByteArray hexBytes = QByteArray("30 12"
											 "            A1 06 04 04 F0200001"
											 "            A2 04 04 02 9000"
											 "            A3 02 31 00"
					//											 "            A4 22 04 20 24e41d62b8c848226b86fcc6c7657577dca47ad2bf21573617bae84807f85c6b"
					//											 "            A5 10 040e4445435643416549443030313033"
					//											 "            A6 10 040e4445435643416549443030313032"
											 "            9000" // ReturnCode
					);

			EstablishPACEChannelOutput channelOutput;
			channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes), PACE_PIN_ID::PACE_PIN);
			QCOMPARE(CardReturnCode::CANCELLATION_BY_USER, channelOutput.getPaceReturnCode());
			QCOMPARE(channelOutput.getCARcurr(), QByteArray());
			QCOMPARE(channelOutput.getCARprev(), QByteArray());
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QCOMPARE(channelOutput.getIDicc(), QByteArray());
		}


		void parseEstablishPACEChannelOutput_fromCcid_allElements()
		{
			QByteArray hexBytes = QByteArray("30 5A"
											 "            A1 06 04 04 F0200001"
											 "            A2 04 04 02 9000"
											 "            A3 02 31 00"
											 "            A4 22 04 20 24e41d62b8c848226b86fcc6c7657577dca47ad2bf21573617bae84807f85c6b"
											 "            A5 10 04 0e 4445435643416549443030313033"
											 "            A6 10 04 0e 4445435643416549443030313032"
											 "            9000" // ReturnCode
					);

			EstablishPACEChannelOutput channelOutput;
			channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes), PACE_PIN_ID::PACE_PIN);
			QCOMPARE(CardReturnCode::CANCELLATION_BY_USER, channelOutput.getPaceReturnCode());
			QCOMPARE(channelOutput.getCARcurr(), QByteArray("DECVCAeID00103"));
			QCOMPARE(channelOutput.getCARprev(), QByteArray("DECVCAeID00102"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QCOMPARE(channelOutput.getIDicc(), QByteArray::fromHex("24e41d62b8c848226b86fcc6c7657577dca47ad2bf21573617bae84807f85c6b"));
		}


		/*!
		 * Workaround for erroneous firmware implementation of reader cyberJack wave:
		 * When entering a wrong PIN, the reader returns an invalid ASN.1 structure, where the element
		 *
		 * ESTABLISHPACECHANNELOUTPUT.efCardAccess
		 *
		 * is treated as IMPLICIT TAGGED. That's wrong.
		 */
		void parseEstablishPACEChannelOutput_fromCcid_cyberJackWave_workaround()
		{
			QByteArray hexBytes = QByteArray("30 10"
											 "		a1 06 04 04 f00663c2"
											 "		a2 04 04 02 0000"
											 "		a3 00"
											 "9000" // ReturnCode
					);
			EstablishPACEChannelOutput channelOutput;
			channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes), PACE_PIN_ID::PACE_PIN);
			QCOMPARE(CardReturnCode::INVALID_PIN, channelOutput.getPaceReturnCode());
		}


};

QTEST_GUILESS_MAIN(test_EstablishPACEChannelOutput)
#include "test_EstablishPACEChannelOutput.moc"
