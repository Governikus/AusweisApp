/*!
 * \brief Unit tests for \ref BluetoothMessageParser
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "messages/BluetoothMessageParser.h"
#include "messages/BluetoothMessageStatusInd.h"
#include "messages/parameter/BluetoothMessageParameterApduResponse.h"
#include "messages/parameter/BluetoothMessageParameterStatusChange.h"
#include <QtCore/QtCore>
#include <QtTest/QtTest>

using namespace governikus;

class test_BluetoothMessageParser
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


		void transferApduResponse()
		{
			QByteArray response;
			response =
					QByteArray::fromHex("060200000200000100000000050000fe3081f9a106040400000000a20404029000a381c43181c1300d060804007f00070202020201023012060a04007f000702020302020201020201413012060a04007f0007020204020202010202010d301c060904007f000702020302300c060704007f0007010202010d020141302a060804007f0007020206161e687474703a2f2f6273692e62756e642e64652f6369662f6e70612e786d6c303e060804007f000702020831323012060a04007f00070202030202020102020145301c060904007f000702020302300c060704007f0007010202010d020145a42204209ee959c9f063c6e144b463e04fa19ef44da0f5059d817a7db2240bbee8b218a290000000");
			BluetoothMessageParser parser1(response);
			QCOMPARE(parser1.getMessages().size(), 1);
			auto message = parser1.getMessages().at(0);

			QCOMPARE(message->getBluetoothMsgId(), BluetoothMsgId::TransferApduResponse);
			QCOMPARE(message->mMessageParameter.size(), 2);

			const auto& params = message->mMessageParameter;
			QCOMPARE(params.first()->getParameterId(), BluetoothParamId::ResultCode);
			QCOMPARE(params.first()->getValue(), QByteArray(1, Enum<BluetoothResultCode>::getValue(BluetoothResultCode::Ok)));

			const auto param = params.last();
			QCOMPARE(param->getParameterId(), BluetoothParamId::ResponseAPDU);
			const auto paramApdu = param.staticCast<const BluetoothMessageParameterApduResponse>();
			QCOMPARE(paramApdu->getResponseApdu(),
					QByteArray::fromHex("3081f9a106040400000000a20404029000a381c43181c1300d060804007f00070202020201023012060a04007f000702020302020201020201413012060a04007f0007020204020202010202010d301c060904007f000702020302300c060704007f0007010202010d020141302a060804007f0007020206161e687474703a2f2f6273692e62756e642e64652f6369662f6e70612e786d6c303e060804007f000702020831323012060a04007f00070202030202020102020145301c060904007f000702020302300c060704007f0007010202010d020145a42204209ee959c9f063c6e144b463e04fa19ef44da0f5059d817a7db2240bbee8b218a29000"));
		}


		void messageToStringCasted()
		{
			QByteArray response;
			response = QByteArray::fromHex("110100000800000102000000");
			BluetoothMessageParser parser1(response);
			QCOMPARE(parser1.getMessages().size(), 1);
			auto message = parser1.getMessages().at(0);

			QCOMPARE(message->getBluetoothMsgId(), BluetoothMsgId::StatusInd);
			auto msgInd = message.staticCast<const BluetoothMessageStatusInd>();
			QCOMPARE(msgInd->toString(), QString("StatusInd | Parameter: StatusChange | Value: CardNotAccessible"));
			QCOMPARE(msgInd->getStatusChange(), BluetoothStatusChange::CardNotAccessible);
		}


		void messageToString()
		{
			QByteArray response;
			response = QByteArray::fromHex("110100000800000104000000110100000800000103000000");
			BluetoothMessageParser parser1(response);
			QCOMPARE(parser1.getMessages().size(), 2);
			QCOMPARE(parser1.getMessages().at(0)->toString(), QString("StatusInd | Parameter: StatusChange | Value: CardInserted"));
			QCOMPARE(parser1.getMessages().at(1)->toString(), QString("StatusInd | Parameter: StatusChange | Value: CardRemoved"));
		}


		void parseAtrReponse()
		{
			QByteArray response;

			response = QByteArray::fromHex("0802000002000001000000000600000f4b8a80018031f873f741e08290007500");
			BluetoothMessageParser parser1(response);
			QCOMPARE(parser1.getMessages().size(), 1);
			QCOMPARE(parser1.getRemainingBytes().size(), 0);

			QCOMPARE(parser1.getMessages()[0]->getBluetoothMsgId(), BluetoothMsgId::TransferAtrResponse);
			QCOMPARE(parser1.getMessages()[0]->mMessageParameter.size(), 2);

			const auto& params = parser1.getMessages()[0]->mMessageParameter;
			QCOMPARE(params.first()->getParameterId(), BluetoothParamId::ResultCode);
			QCOMPARE(params.first()->getValue(), QByteArray(1, Enum<BluetoothResultCode>::getValue(BluetoothResultCode::Ok)));

			QCOMPARE(params.last()->getParameterId(), BluetoothParamId::ATR);
			QCOMPARE(params.last()->getValue(), QByteArray::fromHex("4b8a80018031f873f741e082900075"));

			QCOMPARE(parser1.getMessages()[0]->toString(),
					QString("TransferAtrResponse | Parameter: ResultCode | Value: Ok | Parameter: ATR | Value: 4b8a80018031f873f741e082900075"));
		}


		void parseCardReaderStatusIndMultipleMessages()
		{
			QByteArray response;

			response = QByteArray::fromHex("110100000800000104000000110100000800000103000000");
			BluetoothMessageParser parser1(response);
			QCOMPARE(parser1.getMessages().size(), 2);
			QCOMPARE(parser1.getRemainingBytes().size(), 0);

			QCOMPARE(parser1.getMessages()[0]->getBluetoothMsgId(), BluetoothMsgId::StatusInd);
			QCOMPARE(parser1.getMessages()[0]->mMessageParameter.size(), 1);

			const auto& params = parser1.getMessages()[0]->mMessageParameter;
			QCOMPARE(params.first()->getParameterId(), BluetoothParamId::StatusChange);
			QCOMPARE(params.first()->getValue(), QByteArray(1, Enum<BluetoothStatusChange>::getValue(BluetoothStatusChange::CardInserted)));

			QCOMPARE(parser1.getMessages()[1]->getBluetoothMsgId(), BluetoothMsgId::StatusInd);
			QCOMPARE(parser1.getMessages()[1]->mMessageParameter.size(), 1);

			const auto& paramsInd = parser1.getMessages()[1]->mMessageParameter;
			QCOMPARE(paramsInd.first()->getParameterId(), BluetoothParamId::StatusChange);
			QCOMPARE(paramsInd.first()->getValue(), QByteArray(1, Enum<BluetoothStatusChange>::getValue(BluetoothStatusChange::CardRemoved)));
		}


		void parseCardReaderStatusInd()
		{
			QByteArray response;

			response = QByteArray::fromHex("110100000800000104000000");
			BluetoothMessageParser parser1(response);
			QCOMPARE(parser1.getMessages().size(), 1);
			QCOMPARE(parser1.getRemainingBytes().size(), 0);
			QCOMPARE(parser1.getMessages()[0]->getBluetoothMsgId(), BluetoothMsgId::StatusInd);
			QCOMPARE(parser1.getMessages()[0]->mMessageParameter.size(), 1);

			const auto& params = parser1.getMessages()[0]->mMessageParameter;
			QCOMPARE(params.first()->getParameterId(), BluetoothParamId::StatusChange);
			QCOMPARE(params.first()->getValue(), QByteArray(1, Enum<BluetoothStatusChange>::getValue(BluetoothStatusChange::CardInserted)));

			response = QByteArray::fromHex("110100000800000103000000");
			BluetoothMessageParser parser2(response);
			QCOMPARE(parser2.getMessages().size(), 1);
			QCOMPARE(parser2.getRemainingBytes().size(), 0);
			QCOMPARE(parser2.getMessages()[0]->getBluetoothMsgId(), BluetoothMsgId::StatusInd);
			QCOMPARE(parser2.getMessages()[0]->mMessageParameter.size(), 1);

			const auto& paramsStatus = parser2.getMessages()[0]->mMessageParameter;
			QCOMPARE(paramsStatus.first()->getParameterId(), BluetoothParamId::StatusChange);
			QCOMPARE(paramsStatus.first()->getValue(), QByteArray(1, Enum<BluetoothStatusChange>::getValue(BluetoothStatusChange::CardRemoved)));
		}


		void parseDisconnectResponse()
		{
			BluetoothMessageParser parser(QByteArray::fromHex("03000000"));
			QCOMPARE(parser.getMessages().size(), 1);
			QCOMPARE(parser.getMessages().at(0)->getBluetoothMsgId(), BluetoothMsgId::DisconnectResponse);
			QVERIFY(parser.getRemainingBytes().isEmpty());
		}


		void parseMessageParametersAsMultipleOfFour()
		{
			BluetoothMessageParser parser = BluetoothMessageParser(QByteArray::fromHex("060200000200000100000000050001153082010fa106040400000000a20404029000a381b63181b3300d060804007f00070202020201023012060a04007f000702020302020201020201413012060a04007f0007"));
			QCOMPARE(parser.getMessages().size(), 0);
			QCOMPARE(parser.getRemainingBytes().isEmpty(), false);

			parser = BluetoothMessageParser(QByteArray(parser.getRemainingBytes()).append(QByteArray::fromHex("02020302020201020201453012060a04007f0007020204020202010202010d301c060904007f00070202")));
			QCOMPARE(parser.getMessages().size(), 0);
			QCOMPARE(parser.getRemainingBytes().isEmpty(), false);

			parser = BluetoothMessageParser(QByteArray(parser.getRemainingBytes()).append(QByteArray::fromHex("0302300c060704007f0007010202010d020141301c060904007f000702020302300c060704007f0007010202010d020145302a060804007f0007020206161e687474703a2f2f6273692e62756e642e64652f6369")));
			QCOMPARE(parser.getMessages().size(), 0);
			QCOMPARE(parser.getRemainingBytes().isEmpty(), false);

			parser = BluetoothMessageParser(QByteArray(parser.getRemainingBytes()).append(QByteArray::fromHex("662f6e70612e786d6ca422042065828281cc36f1bf6056748fa23ae95f7d73c6b52e5a5277d1bf48a66fd9a6c9a510040e4445435643416549443030313033a610040e4445435643416549443030313032900000")));
			QCOMPARE(parser.getMessages().size(), 0);
			QCOMPARE(parser.getRemainingBytes().isEmpty(), false);

			parser = BluetoothMessageParser(QByteArray(parser.getRemainingBytes()).append(QByteArray::fromHex("0000")));
			QCOMPARE(parser.getMessages().size(), 1);
			QCOMPARE(parser.getRemainingBytes().isEmpty(), true);

			parser = BluetoothMessageParser(QByteArray(parser.getRemainingBytes()).append(QByteArray::fromHex("0602000002000001000000000500000a37eb6102ba764d9f90000000")));
			QCOMPARE(parser.getMessages().size(), 1);
			QCOMPARE(parser.getRemainingBytes().isEmpty(), true);
		}


};

QTEST_GUILESS_MAIN(test_BluetoothMessageParser)
#include "test_BluetoothMessageParser.moc"
