/*!
 * \brief Tests for card EstablishPACEChannelOutput
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "EstablishPaceChannelOutput.h"
#include "TestFileHelper.h"


using namespace governikus;


class test_EstablishPaceChannelOutput
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseEstablishPaceChannelOutput()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/EstablishPaceChannelOutput.hex"));

			EstablishPaceChannelOutput channelOutput;
			channelOutput.parse(bytes, PacePasswordId::PACE_PIN);
			QCOMPARE(channelOutput.getCARcurr(), QByteArray("DETESTeID00004"));
			QVERIFY(channelOutput.getCARprev().isEmpty());
			QVERIFY(channelOutput.getEfCardAccess().toHex().startsWith(QByteArray("3181c1300d0608040")));
		}


		void parseEstablishPaceChannelOutput_wrongPIN()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/EstablishPaceChannelOutput_wrongPIN.hex"));

			EstablishPaceChannelOutput channelOutput;
			channelOutput.parse(bytes, PacePasswordId::PACE_PIN);
			QVERIFY(channelOutput.getCARcurr().isEmpty());
			QVERIFY(channelOutput.getCARprev().isEmpty());
			QVERIFY(channelOutput.getEfCardAccess().isEmpty());
		}


		void parseEstablishPaceChannelOutput_fromCcid()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/EstablishPaceChannelOutput_fromCcid.hex"));

			EstablishPaceChannelOutput channelOutput;
			channelOutput.parseFromCcid(bytes, PacePasswordId::PACE_PIN);
			QCOMPARE(CardReturnCode::OK, channelOutput.getPaceReturnCode());
			QCOMPARE(channelOutput.getCARcurr(), QByteArray("DECVCAeID00103"));
			QCOMPARE(channelOutput.getCARprev(), QByteArray("DECVCAeID00102"));
			QVERIFY(!channelOutput.getEfCardAccess().isEmpty());
			QVERIFY(!channelOutput.getIDicc().isEmpty());
		}


		void parseEstablishPaceChannelOutput_fromCcid2()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/EstablishPaceChannelOutput_fromCcid2.hex"));

			EstablishPaceChannelOutput channelOutput;
			channelOutput.parseFromCcid(bytes, PacePasswordId::PACE_PIN);
			QCOMPARE(CardReturnCode::OK, channelOutput.getPaceReturnCode());
			QVERIFY(channelOutput.getCARcurr().isEmpty());
			QVERIFY(channelOutput.getCARprev().isEmpty());
			QVERIFY(!channelOutput.getEfCardAccess().isEmpty());
			QVERIFY(!channelOutput.getIDicc().isEmpty());
		}


		void parseEstablishPaceChannelOutput_fromCcid_parseCrap()
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

			EstablishPaceChannelOutput channelOutput;
			channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes), PacePasswordId::PACE_PIN);
			QCOMPARE(CardReturnCode::UNKNOWN, channelOutput.getPaceReturnCode());
			QCOMPARE(channelOutput.getMseStatusSetAt(), QByteArray());
			QCOMPARE(channelOutput.getCARcurr(), QByteArray());
			QCOMPARE(channelOutput.getCARprev(), QByteArray());
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray());
			QCOMPARE(channelOutput.getIDicc(), QByteArray());
		}


		void parseEstablishPaceChannelOutput_fromCcid_onlyMandatoryElements()
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

			EstablishPaceChannelOutput channelOutput;
			channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes), PacePasswordId::PACE_PIN);
			QCOMPARE(CardReturnCode::CANCELLATION_BY_USER, channelOutput.getPaceReturnCode());
			QCOMPARE(channelOutput.getMseStatusSetAt(), QByteArray::fromHex("9000"));
			QCOMPARE(channelOutput.getCARcurr(), QByteArray());
			QCOMPARE(channelOutput.getCARprev(), QByteArray());
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QCOMPARE(channelOutput.getIDicc(), QByteArray());
		}


		void parseEstablishPaceChannelOutput_fromCcid_allElements()
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

			EstablishPaceChannelOutput channelOutput;
			channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes), PacePasswordId::PACE_PIN);
			QCOMPARE(CardReturnCode::CANCELLATION_BY_USER, channelOutput.getPaceReturnCode());
			QCOMPARE(channelOutput.getMseStatusSetAt(), QByteArray::fromHex("9000"));
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
		void parseEstablishPaceChannelOutput_fromCcid_cyberJackWave_workaround()
		{
			QByteArray hexBytes = QByteArray("30 10"
											 "		a1 06 04 04 f00663c2"
											 "		a2 04 04 02 0000"
											 "		a3 00"
											 "9000" // ReturnCode
					);
			EstablishPaceChannelOutput channelOutput;
			channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes), PacePasswordId::PACE_PIN);
			QCOMPARE(CardReturnCode::INVALID_PIN, channelOutput.getPaceReturnCode());
		}


		void toCcid()
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

			EstablishPaceChannelOutput channelOutput;
			channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes), PacePasswordId::PACE_PIN);

			EstablishPaceChannelOutput channelOutput2;
			channelOutput2.parseFromCcid(channelOutput.toCcid(), PacePasswordId::PACE_PIN);

			QCOMPARE(channelOutput2.getPaceReturnCode(), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(channelOutput2.getPaceReturnCode(), channelOutput.getPaceReturnCode());

			QCOMPARE(channelOutput2.getMseStatusSetAt(), QByteArray::fromHex("9000"));
			QCOMPARE(channelOutput2.getMseStatusSetAt(), channelOutput.getMseStatusSetAt());

			QCOMPARE(channelOutput2.getCARcurr(), QByteArray("DECVCAeID00103"));
			QCOMPARE(channelOutput2.getCARcurr(), channelOutput.getCARcurr());

			QCOMPARE(channelOutput2.getCARprev(), QByteArray("DECVCAeID00102"));
			QCOMPARE(channelOutput2.getCARprev(), channelOutput.getCARprev());

			QCOMPARE(channelOutput2.getEfCardAccess(), QByteArray::fromHex("3100"));
			QCOMPARE(channelOutput2.getEfCardAccess(), channelOutput.getEfCardAccess());

			QCOMPARE(channelOutput2.getIDicc(), QByteArray::fromHex("24e41d62b8c848226b86fcc6c7657577dca47ad2bf21573617bae84807f85c6b"));
			QCOMPARE(channelOutput2.getIDicc(), channelOutput.getIDicc());
		}


		void toCcid_ReturnCode_ErrorCode_data()
		{
			QTest::addColumn<CardReturnCode>("cardReturnCode");
			QTest::addColumn<PacePasswordId>("pacePasswordId");

			QTest::newRow("INVALID_CAN") << CardReturnCode::INVALID_CAN << PacePasswordId::PACE_CAN;
			QTest::newRow("INVALID_PIN") << CardReturnCode::INVALID_PIN << PacePasswordId::PACE_PIN;
			QTest::newRow("INVALID_PUK") << CardReturnCode::INVALID_PUK << PacePasswordId::PACE_PUK;

			QTest::newRow("OK") << CardReturnCode::OK << PacePasswordId::PACE_PIN;
			QTest::newRow("CANCELLATION_BY_USER") << CardReturnCode::CANCELLATION_BY_USER << PacePasswordId::PACE_PIN;
			QTest::newRow("INPUT_TIME_OUT") << CardReturnCode::INPUT_TIME_OUT << PacePasswordId::PACE_PIN;
			QTest::newRow("COMMAND_FAILED") << CardReturnCode::COMMAND_FAILED << PacePasswordId::PACE_PIN;
			QTest::newRow("CARD_NOT_FOUND") << CardReturnCode::CARD_NOT_FOUND << PacePasswordId::PACE_PIN;
		}


		void toCcid_ReturnCode_ErrorCode()
		{
			QFETCH(CardReturnCode, cardReturnCode);
			QFETCH(PacePasswordId, pacePasswordId);

			EstablishPaceChannelOutput channelOutput;
			channelOutput.setPaceReturnCode(cardReturnCode);

			EstablishPaceChannelOutput channelOutput2;
			channelOutput2.parseFromCcid(channelOutput.toCcid(), pacePasswordId);

			QCOMPARE(channelOutput2.getPaceReturnCode(), cardReturnCode);
			QCOMPARE(channelOutput2.getPaceReturnCode(), channelOutput.getPaceReturnCode());
		}


};

QTEST_GUILESS_MAIN(test_EstablishPaceChannelOutput)
#include "test_EstablishPaceChannelOutput.moc"
