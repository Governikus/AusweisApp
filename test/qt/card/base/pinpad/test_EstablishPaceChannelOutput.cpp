/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Tests for card EstablishPACEChannelOutput
 */

#include <QtCore>
#include <QtTest>

#include "TestFileHelper.h"
#include "pinpad/EstablishPaceChannelOutput.h"


using namespace governikus;


class test_EstablishPaceChannelOutput
	: public QObject
{
	Q_OBJECT

	const QByteArray mEfCardAccess = "3181c1300d060804007f00070202020201023012060a04007f000702020302020201020201413012060a04007f0007020204020202010202010d301c060904007f000702020302300c060704007f0007010202010d020141302a060804007f0007020206161e687474703a2f2f6273692e62756e642e64652f6369662f6e70612e786d6c303e060804007f000702020831323012060a04007f00070202030202020102020145301c060904007f000702020302300c060704007f0007010202010d020145";
	const QByteArray mIdIcc = "24e41d62b8c848226b86fcc6c7657577dca47ad2bf21573617bae84807f85c6b";
	const QByteArray mCarCurr = "4445435643416549443030313033";
	const QByteArray mCarPrev = "4445435643416549443030313032";

	const QByteArray mCarCurrAscii = "DECVCAeID00103";
	const QByteArray mCarPrevAscii = "DECVCAeID00102";

	private Q_SLOTS:
		void parse()
		{
			QByteArray hexBytes("00000000"
								"fa00"
								"9000"
								"c400 [CARD]"
								"0e [CCAR]"
								"00"
								"2000 [PICC]");
			hexBytes.replace("[CARD]", mEfCardAccess);
			hexBytes.replace("[CCAR]", mCarCurr);
			hexBytes.replace("[PICC]", mIdIcc);

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(channelOutput.parse(QByteArray::fromHex(hexBytes)));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::OK);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("9000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex(mEfCardAccess));
			QCOMPARE(channelOutput.getCarCurr(), mCarCurrAscii);
			QVERIFY(channelOutput.getCarPrev().isEmpty());
			QCOMPARE(channelOutput.getIdIcc(), QByteArray::fromHex(mIdIcc));
		}


		void parseEmpty()
		{
			EstablishPaceChannelOutput channelOutput;
			QVERIFY(!channelOutput.parse(QByteArray()));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("0000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QVERIFY(channelOutput.getCarCurr().isEmpty());
			QVERIFY(channelOutput.getCarPrev().isEmpty());
			QVERIFY(channelOutput.getIdIcc().isEmpty());
		}


		void parseEmptyValues()
		{
			QByteArray hexBytes("00000000"
								"0700"
								"0000"
								"0000"
								"00"
								"00"
								"0000");

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(!channelOutput.parse(QByteArray::fromHex(hexBytes)));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::OK);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("0000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QVERIFY(channelOutput.getCarCurr().isEmpty());
			QVERIFY(channelOutput.getCarPrev().isEmpty());
			QVERIFY(channelOutput.getIdIcc().isEmpty());
		}


		void parseIncompleteWrongPIN()
		{
			QByteArray bytes = QByteArray::fromHex("C26306F0"
												   "0000");

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(channelOutput.parse(bytes));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::INVALID_PASSWORD);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("0000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QVERIFY(channelOutput.getCarCurr().isEmpty());
			QVERIFY(channelOutput.getCarPrev().isEmpty());
			QVERIFY(channelOutput.getIdIcc().isEmpty());
		}


		void parseResultCode_data()
		{
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<CardReturnCode>("code");

			QTest::newRow("NoError") << QByteArray("00000000") << CardReturnCode::OK;

			QTest::newRow("InconsistentLengthsInInput") << QByteArray("010000d0") << CardReturnCode::COMMAND_FAILED;
			QTest::newRow("UnexpectedDataInInput") << QByteArray("020000d0") << CardReturnCode::COMMAND_FAILED;
			QTest::newRow("UnexpectedCombinationOfDataInInput") << QByteArray("030000d0") << CardReturnCode::COMMAND_FAILED;
			QTest::newRow("SyntaxErrorInTLVResponse") << QByteArray("010000E0") << CardReturnCode::COMMAND_FAILED;
			QTest::newRow("UnexpectedOrMissingObjectInTLVResponse") << QByteArray("020000E0") << CardReturnCode::COMMAND_FAILED;
			QTest::newRow("UnknownPasswordID") << QByteArray("030000E0") << CardReturnCode::COMMAND_FAILED;
			QTest::newRow("WrongAuthenticationToken") << QByteArray("060000E0") << CardReturnCode::COMMAND_FAILED;

			QTest::newRow("CertificateChainForTerminalAuthenticationCannotBeBuilt") << QByteArray("070000E0") << CardReturnCode::UNKNOWN;
			QTest::newRow("UnexpectedDataStructureInResponseToChipAuthentication") << QByteArray("080000E0") << CardReturnCode::UNKNOWN;
			QTest::newRow("PassiveAuthenticationFailed") << QByteArray("090000E0") << CardReturnCode::UNKNOWN;
			QTest::newRow("IncorrectTokenForChipAuthentication") << QByteArray("a00000e0") << CardReturnCode::UNKNOWN;

			QTest::newRow("GeneralAuthenticateStep1_4_Warning - Wrong PIN 1 time") << QByteArray("c26306f0") << CardReturnCode::INVALID_PASSWORD;
			QTest::newRow("GeneralAuthenticateStep1_4_Warning - Wrong PIN 2 time") << QByteArray("c16306f0") << CardReturnCode::INVALID_PASSWORD;
			QTest::newRow("GeneralAuthenticateStep1_4_Warning - Wrong PIN 3 time") << QByteArray("c06306f0") << CardReturnCode::INVALID_PASSWORD;
			QTest::newRow("GeneralAuthenticateStep1_4_Warning - Wrong CAN/PUK") << QByteArray("006306f0") << CardReturnCode::INVALID_PASSWORD;

			QTest::newRow("CommunicationAbort") << QByteArray("010010f0") << CardReturnCode::COMMAND_FAILED;
			QTest::newRow("NoCard") << QByteArray("020010f0") << CardReturnCode::CARD_NOT_FOUND;
			QTest::newRow("Abort") << QByteArray("010020f0") << CardReturnCode::CANCELLATION_BY_USER;
			QTest::newRow("Timeout") << QByteArray("020020f0") << CardReturnCode::INPUT_TIME_OUT;
		}


		void parseResultCode()
		{
			QFETCH(QByteArray, data);
			QFETCH(CardReturnCode, code);

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(channelOutput.parseResultCode(QByteArray::fromHex(data)));

			QCOMPARE(channelOutput.getPaceReturnCode(), code);
		}


		void parseFromCcid()
		{
			QByteArray hexBytes("30 82 01 1d"
								"a1 06 04 04 00000000"
								"a2 04 04 02 9000"
								"a3 81 c4 [CARD]"
								"a4 22 04 20 [PICC]"
								"a5 10 04 0e [CCAR]"
								"a6 10 04 0e [PCAR]"
								"9000");
			hexBytes.replace("[CARD]", mEfCardAccess);
			hexBytes.replace("[PICC]", mIdIcc);
			hexBytes.replace("[CCAR]", mCarCurr);
			hexBytes.replace("[PCAR]", mCarPrev);

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes)));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::OK);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("9000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex(mEfCardAccess));
			QCOMPARE(channelOutput.getIdIcc(), QByteArray::fromHex(mIdIcc));
			QCOMPARE(channelOutput.getCarCurr(), mCarCurrAscii);
			QCOMPARE(channelOutput.getCarPrev(), mCarPrevAscii);
		}


		void parseFromCcid2()
		{
			QByteArray hexBytes("30 81 f9"
								"a1 06 04 04 00000000"
								"a2 04 04 02 9000"
								"a3 81 c4 [CARD]"
								"a4 22 04 20 [PICC]"
								"9000");
			hexBytes.replace("[CARD]", mEfCardAccess);
			hexBytes.replace("[PICC]", mIdIcc);

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes)));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::OK);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("9000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex(mEfCardAccess));
			QCOMPARE(channelOutput.getIdIcc(), QByteArray::fromHex(mIdIcc));
			QVERIFY(channelOutput.getCarCurr().isEmpty());
			QVERIFY(channelOutput.getCarPrev().isEmpty());
		}


		void parseCrapCcid()
		{
			QByteArray hexBytes = QByteArray("30 0C"
											 "04 04 F0200001"
											 "04 02 9000"
											 "31 00"
											 "9000");

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(!channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes)));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("0000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QVERIFY(channelOutput.getIdIcc().isEmpty());
			QVERIFY(channelOutput.getCarCurr().isEmpty());
			QVERIFY(channelOutput.getCarPrev().isEmpty());
		}


		void parseFromCcid_onlyMandatoryElements()
		{
			QByteArray hexBytes = QByteArray("30 12"
											 "A1 06 04 04 F0200001"
											 "A2 04 04 02 9000"
											 "A3 02 31 00"
											 "9000");

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes)));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("9000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QCOMPARE(channelOutput.getIdIcc(), QByteArray());
			QCOMPARE(channelOutput.getCarCurr(), QByteArray());
			QCOMPARE(channelOutput.getCarPrev(), QByteArray());
		}


		void parseFromCcid_allElements()
		{
			QByteArray hexBytes = QByteArray("30 5A"
											 "A1 06 04 04 F0200001"
											 "A2 04 04 02 9000"
											 "A3 02 31 00"
											 "A4 22 04 20 [PICC]"
											 "A5 10 04 0e [CCAR]"
											 "A6 10 04 0e [PCAR]"
											 "9000");
			hexBytes.replace("[PICC]", mIdIcc);
			hexBytes.replace("[CCAR]", mCarCurr);
			hexBytes.replace("[PCAR]", mCarPrev);

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes)));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("9000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QCOMPARE(channelOutput.getIdIcc(), QByteArray::fromHex(mIdIcc));
			QCOMPARE(channelOutput.getCarCurr(), mCarCurrAscii);
			QCOMPARE(channelOutput.getCarPrev(), mCarPrevAscii);

		}


		/*!
		 * When the ASN.1 structure is invalid we still try to return the error code
		 */
		void parseFromCcid_invalid_ASN1()
		{
			QByteArray hexBytes = QByteArray("30 10"
											 "a1 06 04 04 f00663c2"
											 "a2 04 04 02 0000"
											 "a3 00"
											 "9000");

			EstablishPaceChannelOutput channelOutput;
			QTest::ignoreMessage(QtWarningMsg, "Determine at least PACE return code by regular expression");
			QVERIFY(channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes)));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::INVALID_PASSWORD);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("0000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QVERIFY(channelOutput.getIdIcc().isEmpty());
			QVERIFY(channelOutput.getCarCurr().isEmpty());
			QVERIFY(channelOutput.getCarPrev().isEmpty());
		}


		void toResultCode_data()
		{
			QTest::addColumn<CardReturnCode>("code");
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("UNKNOWN") << CardReturnCode::UNKNOWN << QByteArray("020000d0");
			QTest::newRow("UNDEFINED") << CardReturnCode::UNDEFINED << QByteArray("020000d0");
			QTest::newRow("NEW_PIN_MISMATCH") << CardReturnCode::NEW_PIN_MISMATCH << QByteArray("020000d0");
			QTest::newRow("NEW_PIN_INVALID_LENGTH") << CardReturnCode::NEW_PIN_INVALID_LENGTH << QByteArray("020000d0");
			QTest::newRow("PIN_BLOCKED") << CardReturnCode::PIN_BLOCKED << QByteArray("020000d0");
			QTest::newRow("PIN_NOT_BLOCKED") << CardReturnCode::PIN_NOT_BLOCKED << QByteArray("020000d0");
			QTest::newRow("PUK_INOPERATIVE") << CardReturnCode::PUK_INOPERATIVE << QByteArray("020000d0");
			QTest::newRow("UNEXPECTED_TRANSMIT_STATUS") << CardReturnCode::UNEXPECTED_TRANSMIT_STATUS << QByteArray("020000d0");
			QTest::newRow("PROTOCOL_ERROR") << CardReturnCode::PROTOCOL_ERROR << QByteArray("020000d0");

			QTest::newRow("INVALID_CAN") << CardReturnCode::INVALID_CAN << QByteArray("006306f0");
			QTest::newRow("INVALID_PASSWORD") << CardReturnCode::INVALID_PASSWORD << QByteArray("006306f0");
			QTest::newRow("INVALID_PIN") << CardReturnCode::INVALID_PIN << QByteArray("006306f0");
			QTest::newRow("INVALID_PIN_2") << CardReturnCode::INVALID_PIN_2 << QByteArray("006306f0");
			QTest::newRow("INVALID_PIN_3") << CardReturnCode::INVALID_PIN_3 << QByteArray("006306f0");
			QTest::newRow("INVALID_PUK") << CardReturnCode::INVALID_PUK << QByteArray("006306f0");

			QTest::newRow("OK") << CardReturnCode::OK << QByteArray("00000000");
			QTest::newRow("OK_PUK") << CardReturnCode::OK_PUK << QByteArray("00000000");

			QTest::newRow("CARD_NOT_FOUND") << CardReturnCode::CARD_NOT_FOUND << QByteArray("020010f0");
			QTest::newRow("RETRY_ALLOWED") << CardReturnCode::RESPONSE_EMPTY << QByteArray("020010f0");

			QTest::newRow("INPUT_TIME_OUT") << CardReturnCode::INPUT_TIME_OUT << QByteArray("020020f0");

			QTest::newRow("COMMAND_FAILED") << CardReturnCode::COMMAND_FAILED << QByteArray("010010f0");

			QTest::newRow("CANCELLATION_BY_USER") << CardReturnCode::CANCELLATION_BY_USER << QByteArray("010020f0");
		}


		void toResultCode()
		{
			QFETCH(CardReturnCode, code);
			QFETCH(QByteArray, data);

			EstablishPaceChannelOutput channelOutput(code);
			QCOMPARE(channelOutput.toResultCode(), QByteArray::fromHex(data));
		}


		void toOutputData()
		{
			QByteArray hexBytes = QByteArray("9000"
											 "02 00 3100"
											 "0e [CCAR]"
											 "0e [PCAR]"
											 "20 00 [PICC]");
			hexBytes.replace("[CCAR]", mCarCurr);
			hexBytes.replace("[PCAR]", mCarPrev);
			hexBytes.replace("[PICC]", mIdIcc);

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(channelOutput.parseOutputData(QByteArray::fromHex(hexBytes)));

			EstablishPaceChannelOutput channelOutput2;
			QVERIFY(channelOutput2.parseOutputData(channelOutput.toOutputData()));

			QCOMPARE(channelOutput2, channelOutput);

			QCOMPARE(channelOutput2.getPaceReturnCode(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(channelOutput2.getStatusMseSetAt(), QByteArray::fromHex("9000"));
			QCOMPARE(channelOutput2.getEfCardAccess(), QByteArray::fromHex("3100"));
			QCOMPARE(channelOutput2.getIdIcc(), QByteArray::fromHex(mIdIcc));
			QCOMPARE(channelOutput2.getCarCurr(), QByteArray("DECVCAeID00103"));
			QCOMPARE(channelOutput2.getCarPrev(), QByteArray("DECVCAeID00102"));
		}


		void toCcid()
		{
			QByteArray hexBytes = QByteArray("30 5A"
											 "A1 06 04 04 F0200001"
											 "A2 04 04 02 9000"
											 "A3 02 31 00"
											 "A4 22 04 20 [PICC]"
											 "A5 10 04 0e [CCAR]"
											 "A6 10 04 0e [PCAR]"
											 "9000");
			hexBytes.replace("[PICC]", mIdIcc);
			hexBytes.replace("[CCAR]", mCarCurr);
			hexBytes.replace("[PCAR]", mCarPrev);

			EstablishPaceChannelOutput channelOutput;
			QVERIFY(channelOutput.parseFromCcid(QByteArray::fromHex(hexBytes)));

			EstablishPaceChannelOutput channelOutput2;
			QVERIFY(channelOutput2.parseFromCcid(channelOutput.toCcid()));

			QCOMPARE(channelOutput2, channelOutput);

			QCOMPARE(channelOutput2.getPaceReturnCode(), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(channelOutput2.getStatusMseSetAt(), QByteArray::fromHex("9000"));
			QCOMPARE(channelOutput2.getEfCardAccess(), QByteArray::fromHex("3100"));
			QCOMPARE(channelOutput2.getIdIcc(), QByteArray::fromHex(mIdIcc));
			QCOMPARE(channelOutput2.getCarCurr(), mCarCurrAscii);
			QCOMPARE(channelOutput2.getCarPrev(), mCarPrevAscii);
		}


		void toOutputData_default()
		{
			QByteArray hexBytes = QByteArray("0000"
											 "02 00 3100");

			EstablishPaceChannelOutput channelOutput;
			QCOMPARE(channelOutput.toOutputData(), QByteArray::fromHex(hexBytes));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("0000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QVERIFY(channelOutput.getIdIcc().isEmpty());
			QVERIFY(channelOutput.getCarCurr().isEmpty());
			QVERIFY(channelOutput.getCarPrev().isEmpty());
		}


		void toCcid_default()
		{
			QByteArray hexBytes = QByteArray("30 12"
											 "A1 06 04 04 F0100001"
											 "A2 04 04 02 0000"
											 "A3 02 31 00"
											 "9000");

			EstablishPaceChannelOutput channelOutput;
			QCOMPARE(channelOutput.toCcid(), QByteArray::fromHex(hexBytes));

			QCOMPARE(channelOutput.getPaceReturnCode(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(channelOutput.getStatusMseSetAt(), QByteArray::fromHex("0000"));
			QCOMPARE(channelOutput.getEfCardAccess(), QByteArray::fromHex("3100"));
			QVERIFY(channelOutput.getIdIcc().isEmpty());
			QVERIFY(channelOutput.getCarCurr().isEmpty());
			QVERIFY(channelOutput.getCarPrev().isEmpty());
		}


		void toOutputData_ReturnCode_ErrorCode_data()
		{
			QTest::addColumn<CardReturnCode>("cardReturnCodeIn");
			QTest::addColumn<CardReturnCode>("cardReturnCodeOut");

			QTest::newRow("INVALID_CAN") << CardReturnCode::INVALID_CAN << CardReturnCode::INVALID_PASSWORD;
			QTest::newRow("INVALID_PIN") << CardReturnCode::INVALID_PASSWORD << CardReturnCode::INVALID_PASSWORD;
			QTest::newRow("INVALID_PIN") << CardReturnCode::INVALID_PIN << CardReturnCode::INVALID_PASSWORD;
			QTest::newRow("INVALID_PUK") << CardReturnCode::INVALID_PUK << CardReturnCode::INVALID_PASSWORD;

			QTest::newRow("OK") << CardReturnCode::OK << CardReturnCode::OK;
			QTest::newRow("CANCELLATION_BY_USER") << CardReturnCode::CANCELLATION_BY_USER << CardReturnCode::CANCELLATION_BY_USER;
			QTest::newRow("INPUT_TIME_OUT") << CardReturnCode::INPUT_TIME_OUT << CardReturnCode::INPUT_TIME_OUT;
			QTest::newRow("COMMAND_FAILED") << CardReturnCode::COMMAND_FAILED << CardReturnCode::COMMAND_FAILED;
			QTest::newRow("CARD_NOT_FOUND") << CardReturnCode::CARD_NOT_FOUND << CardReturnCode::CARD_NOT_FOUND;
			QTest::newRow("UNKNOWN") << CardReturnCode::UNKNOWN << CardReturnCode::COMMAND_FAILED;
		}


		void toOutputData_ReturnCode_ErrorCode()
		{
			QFETCH(CardReturnCode, cardReturnCodeIn);
			QFETCH(CardReturnCode, cardReturnCodeOut);

			EstablishPaceChannelOutput channelOutput;
			channelOutput.setPaceReturnCode(cardReturnCodeIn);
			QCOMPARE(channelOutput.getPaceReturnCode(), cardReturnCodeIn);

			EstablishPaceChannelOutput channelOutput2;
			QVERIFY(channelOutput2.parseResultCode(channelOutput.toResultCode()));
			QCOMPARE(channelOutput2.getPaceReturnCode(), cardReturnCodeOut);
		}


		void toCcid_ReturnCode_ErrorCode_data()
		{
			QTest::addColumn<CardReturnCode>("cardReturnCodeIn");
			QTest::addColumn<CardReturnCode>("cardReturnCodeOut");

			QTest::newRow("INVALID_CAN") << CardReturnCode::INVALID_CAN << CardReturnCode::INVALID_PASSWORD;
			QTest::newRow("INVALID_PIN") << CardReturnCode::INVALID_PASSWORD << CardReturnCode::INVALID_PASSWORD;
			QTest::newRow("INVALID_PIN") << CardReturnCode::INVALID_PIN << CardReturnCode::INVALID_PASSWORD;
			QTest::newRow("INVALID_PUK") << CardReturnCode::INVALID_PUK << CardReturnCode::INVALID_PASSWORD;

			QTest::newRow("OK") << CardReturnCode::OK << CardReturnCode::OK;
			QTest::newRow("CANCELLATION_BY_USER") << CardReturnCode::CANCELLATION_BY_USER << CardReturnCode::CANCELLATION_BY_USER;
			QTest::newRow("INPUT_TIME_OUT") << CardReturnCode::INPUT_TIME_OUT << CardReturnCode::INPUT_TIME_OUT;
			QTest::newRow("COMMAND_FAILED") << CardReturnCode::COMMAND_FAILED << CardReturnCode::COMMAND_FAILED;
			QTest::newRow("CARD_NOT_FOUND") << CardReturnCode::CARD_NOT_FOUND << CardReturnCode::CARD_NOT_FOUND;
			QTest::newRow("UNKNOWN") << CardReturnCode::UNKNOWN << CardReturnCode::COMMAND_FAILED;
		}


		void toCcid_ReturnCode_ErrorCode()
		{
			QFETCH(CardReturnCode, cardReturnCodeIn);
			QFETCH(CardReturnCode, cardReturnCodeOut);

			EstablishPaceChannelOutput channelOutput;
			channelOutput.setPaceReturnCode(cardReturnCodeIn);
			QCOMPARE(channelOutput.getPaceReturnCode(), cardReturnCodeIn);

			EstablishPaceChannelOutput channelOutput2;
			QVERIFY(channelOutput2.parseFromCcid(channelOutput.toCcid()));
			QCOMPARE(channelOutput2.getPaceReturnCode(), cardReturnCodeOut);
		}


		void emptyValuesOutputData_data()
		{
			QTest::addColumn<QByteArray>("stat");
			QTest::addColumn<QByteArray>("card");
			QTest::addColumn<QByteArray>("ccar");
			QTest::addColumn<QByteArray>("pcar");
			QTest::addColumn<QByteArray>("picc");

			QTest::newRow("stat") << QByteArray() << mEfCardAccess << mCarCurr << mCarPrev << mIdIcc;
			QTest::newRow("card") << QByteArray("9000") << QByteArray() << mCarCurr << mCarPrev << mIdIcc;
			QTest::newRow("ccar") << QByteArray("9000") << mEfCardAccess << QByteArray() << mCarPrev << mIdIcc;
			QTest::newRow("pcar") << QByteArray("9000") << mEfCardAccess << mCarCurr << QByteArray() << mIdIcc;
			QTest::newRow("picc") << QByteArray("9000") << mEfCardAccess << mCarCurr << mCarPrev << QByteArray();
			QTest::newRow("all") << QByteArray() << QByteArray() << QByteArray() << QByteArray() << QByteArray();
		}


		void emptyValuesOutputData()
		{
			QFETCH(QByteArray, stat);
			QFETCH(QByteArray, card);
			QFETCH(QByteArray, ccar);
			QFETCH(QByteArray, pcar);
			QFETCH(QByteArray, picc);

			EstablishPaceChannelOutput channelOutput1;
			channelOutput1.setStatusMseSetAt(QByteArray::fromHex(stat));
			channelOutput1.setEfCardAccess(QByteArray::fromHex(card));
			channelOutput1.setCarCurr(QByteArray::fromHex(ccar));
			channelOutput1.setCarPrev(QByteArray::fromHex(pcar));
			channelOutput1.setIdIcc(QByteArray::fromHex(picc));

			EstablishPaceChannelOutput channelOutput2;
			QVERIFY(channelOutput2.parseOutputData(channelOutput1.toOutputData()));

			QCOMPARE(channelOutput2, channelOutput1);
		}


		void emptyValuesCcid_data()
		{
			QTest::addColumn<QByteArray>("stat");
			QTest::addColumn<QByteArray>("card");
			QTest::addColumn<QByteArray>("picc");
			QTest::addColumn<QByteArray>("ccar");
			QTest::addColumn<QByteArray>("pcar");

			QTest::newRow("stat") << QByteArray() << mEfCardAccess << mIdIcc << mCarCurr << mCarPrev;
			QTest::newRow("card") << QByteArray("9000") << QByteArray() << mIdIcc << mCarCurr << mCarPrev;
			QTest::newRow("picc") << QByteArray("9000") << mEfCardAccess << QByteArray() << mCarCurr << mCarPrev;
			QTest::newRow("ccar") << QByteArray("9000") << mEfCardAccess << mIdIcc << QByteArray() << mCarPrev;
			QTest::newRow("pcar") << QByteArray("9000") << mEfCardAccess << mIdIcc << mCarCurr << QByteArray();
		}


		void emptyValuesCcid()
		{
			QFETCH(QByteArray, stat);
			QFETCH(QByteArray, card);
			QFETCH(QByteArray, picc);
			QFETCH(QByteArray, ccar);
			QFETCH(QByteArray, pcar);

			EstablishPaceChannelOutput channelOutput1(CardReturnCode::OK);
			channelOutput1.setStatusMseSetAt(QByteArray::fromHex(stat));
			channelOutput1.setEfCardAccess(QByteArray::fromHex(card));
			channelOutput1.setIdIcc(QByteArray::fromHex(picc));
			channelOutput1.setCarCurr(QByteArray::fromHex(ccar));
			channelOutput1.setCarPrev(QByteArray::fromHex(pcar));

			EstablishPaceChannelOutput channelOutput2;
			QVERIFY(channelOutput2.parseFromCcid(channelOutput1.toCcid()));

			QCOMPARE(channelOutput2, channelOutput1);
		}


		void outputDataWrongSize_data()
		{
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("efCardAccess") << QByteArray::fromHex("9000020061");
			QTest::newRow("carCurr") << QByteArray::fromHex("900000000261");
			QTest::newRow("carPrev") << QByteArray::fromHex("90000000000261");
			QTest::newRow("idIcc") << QByteArray::fromHex("900000000000020061");
		}


		void outputDataWrongSize()
		{
			QFETCH(QByteArray, data);

			EstablishPaceChannelOutput output;
			QTest::ignoreMessage(QtDebugMsg, "Decapsulation of command failed. Wrong size.");
			QVERIFY(!output.parseOutputData(data));
		}


};

QTEST_GUILESS_MAIN(test_EstablishPaceChannelOutput)
#include "test_EstablishPaceChannelOutput.moc"
