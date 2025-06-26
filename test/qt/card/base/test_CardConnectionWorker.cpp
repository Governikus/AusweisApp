/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnectionWorker.h"

#include "MockReader.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_CardConnectionWorker
	: public QObject
{
	Q_OBJECT
	QSharedPointer<CardConnectionWorker> mWorker;
	QScopedPointer<MockReader> mReader;

	void setCard()
	{
		QList<TransmitConfig> transmitConfigs;
		transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6982")));
		transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("9000")));
		MockCardConfig cardConfig(transmitConfigs);
		mReader->setCard(cardConfig);
	}

	private Q_SLOTS:
		void init()
		{
			mReader.reset(new MockReader(QStringLiteral("Reader"), ReaderManagerPluginType::PCSC));
			mWorker = CardConnectionWorker::create(mReader.data());
		}


		void cleanup()
		{
			mWorker.clear();
			mReader.reset();
		}


		void test_Transmit()
		{
			const CommandApdu emptyCommandApdu(QByteArray(""));

			//no card
			QCOMPARE(mWorker->transmit(emptyCommandApdu), ResponseApduResult({CardReturnCode::CARD_NOT_FOUND}));

			setCard();

			//no secure messaging
			ResponseApdu apdu(QByteArray::fromHex("6982"));
			QCOMPARE(mWorker->transmit(emptyCommandApdu), ResponseApduResult({CardReturnCode::OK, apdu}));
		}


		void test_EstablishPaceChannel_data()
		{
			QTest::addColumn<int>("retryCounter");
			QTest::addColumn<PacePasswordId>("pacePasswordId");
			QTest::addColumn<CardReturnCode>("cardReturnCode");

			QTest::newRow("PIN RT 3") << 3 << PacePasswordId::PACE_PIN << CardReturnCode::INVALID_PIN;
			QTest::newRow("PIN RT 2") << 2 << PacePasswordId::PACE_PIN << CardReturnCode::INVALID_PIN_2;
			QTest::newRow("PIN RT 1") << 1 << PacePasswordId::PACE_PIN << CardReturnCode::INVALID_PIN_3;
			QTest::newRow("PIN RT 0") << 0 << PacePasswordId::PACE_PIN << CardReturnCode::INVALID_PIN;

			QTest::newRow("CAN RT 3") << 3 << PacePasswordId::PACE_CAN << CardReturnCode::INVALID_CAN;
			QTest::newRow("CAN RT 2") << 2 << PacePasswordId::PACE_CAN << CardReturnCode::INVALID_CAN;
			QTest::newRow("CAN RT 1") << 1 << PacePasswordId::PACE_CAN << CardReturnCode::INVALID_CAN;
			QTest::newRow("CAN RT 0") << 0 << PacePasswordId::PACE_CAN << CardReturnCode::INVALID_CAN;

			QTest::newRow("PUK RT 3") << 3 << PacePasswordId::PACE_PUK << CardReturnCode::INVALID_PUK;
			QTest::newRow("PUK RT 2") << 2 << PacePasswordId::PACE_PUK << CardReturnCode::INVALID_PUK;
			QTest::newRow("PUK RT 1") << 1 << PacePasswordId::PACE_PUK << CardReturnCode::INVALID_PUK;
			QTest::newRow("PUK RT 0") << 0 << PacePasswordId::PACE_PUK << CardReturnCode::INVALID_PUK;

			QTest::newRow("MRZ RT 3") << 3 << PacePasswordId::PACE_MRZ << CardReturnCode::UNKNOWN;
			QTest::newRow("MRZ RT 2") << 2 << PacePasswordId::PACE_MRZ << CardReturnCode::UNKNOWN;
			QTest::newRow("MRZ RT 1") << 1 << PacePasswordId::PACE_MRZ << CardReturnCode::UNKNOWN;
			QTest::newRow("MRZ RT 0") << 0 << PacePasswordId::PACE_MRZ << CardReturnCode::UNKNOWN;
		}


		void test_EstablishPaceChannel()
		{
			QFETCH(int, retryCounter);
			QFETCH(PacePasswordId, pacePasswordId);
			QFETCH(CardReturnCode, cardReturnCode);

			const QByteArray password("111111");
			const QByteArray chat = QByteArray::fromHex("7F4C12060904007F00070301020253050000000F0F");
			const QByteArray certDescription = QByteArray::fromHex("30 8202A4"
																   "        06 0A 04007F00070301030103"
																   "        A1 0E 0C0C442D547275737420476D6248"
																   "        A3 3A 0C38476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E"
																   "        A5 820248"
																   "            04 820244 4E616D652C20416E7363687269667420756E6420452D4D61696C2D4164726573736520646573204469656E737465616E626965746572733A0D0A476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E0D0A57696C68656C6D73747261C39F652034332F3433670D0A3130313137204265726C696E0D0A6265726C696E406764762E64650D0A0D0A4765736368C3A46674737A7765636B3A0D0A2D52656769737472696572756E6720756E64204C6F67696E20616D204744562D4D616B6C6572706F7274616C2D0D0A0D0A48696E7765697320617566206469652066C3BC722064656E204469656E737465616E626965746572207A757374C3A46E646967656E205374656C6C656E2C20646965206469652045696E68616C74756E672064657220566F7273636872696674656E207A756D20446174656E73636875747A206B6F6E74726F6C6C696572656E3A0D0A4265726C696E6572204265617566747261677465722066C3BC7220446174656E73636875747A20756E6420496E666F726D6174696F6E7366726569686569740D0A416E20646572205572616E696120342D31300D0A3130373837204265726C696E0D0A3033302F3133382038392D300D0A6D61696C626F7840646174656E73636875747A2D6265726C696E2E64650D0A687474703A2F2F7777772E646174656E73636875747A2D6265726C696E2E64650D0A416E737072656368706172746E65723A2044722E20416C6578616E64657220446978");

			//no card
			QCOMPARE(mWorker->establishPaceChannel(PacePasswordId::PACE_PIN, password, chat, certDescription).getPaceReturnCode(), CardReturnCode::CARD_NOT_FOUND);

			setCard();

			QByteArray startMsg("Starting PACE for %1");
			startMsg.replace("%1"_ba, Enum<PacePasswordId>::getName(pacePasswordId));

			QByteArray basicMsg("Finished PACE for %1 with result PROTOCOL_ERROR");
			basicMsg.replace("%1"_ba, Enum<PacePasswordId>::getName(pacePasswordId));

			//basic reader
			QTest::ignoreMessage(QtInfoMsg, startMsg.data());
			QTest::ignoreMessage(QtInfoMsg, basicMsg.data());
			QCOMPARE(mWorker->establishPaceChannel(pacePasswordId, password, chat, certDescription).getPaceReturnCode(), CardReturnCode::PROTOCOL_ERROR);

			QByteArray comfortMsg("Finished PACE for %1 with result %2");
			comfortMsg.replace("%1"_ba, Enum<PacePasswordId>::getName(pacePasswordId));
			comfortMsg.replace("%2"_ba, Enum<CardReturnCode>::getName(cardReturnCode));

			//comfort reader
			mReader->setInfoBasicReader(false);
			mReader->setInfoCardInfo(CardInfo(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), retryCounter));
			QTest::ignoreMessage(QtInfoMsg, startMsg.data());
			if (pacePasswordId == PacePasswordId::PACE_PIN)
			{
				QTest::ignoreMessage(QtWarningMsg, "Add missing StatusCodeMseSet in EstablishPaceChannelOutput for Reiner SCT reader with pin pad that do not follow PCSC Part 10 IFDs with Secure PIN Entry Capabilities - AMENDMENT 1.1");
			}
			QTest::ignoreMessage(QtInfoMsg, comfortMsg.data());
			QCOMPARE(mWorker->establishPaceChannel(pacePasswordId, QByteArray(), chat, certDescription).getPaceReturnCode(), cardReturnCode);
		}


		void test_DestroyPaceChannel()
		{
			//no card
			QCOMPARE(mWorker->destroyPaceChannel(), CardReturnCode::CARD_NOT_FOUND);

			setCard();

			//basic reader
			QTest::ignoreMessage(QtInfoMsg, "Destroying PACE channel");
			QTest::ignoreMessage(QtDebugMsg, "Destroying PACE channel with invalid command causing 6700 as return code");
			QCOMPARE(mWorker->destroyPaceChannel(), CardReturnCode::OK);

			//comfort reader
			mReader->setInfoBasicReader(false);
			QTest::ignoreMessage(QtInfoMsg, "Destroying PACE channel");
			QTest::ignoreMessage(QtWarningMsg, "Destruction of PACE channel not supported");
			QCOMPARE(mWorker->destroyPaceChannel(), CardReturnCode::COMMAND_FAILED);
		}


		void test_SetEidPin()
		{
			const QByteArray newPin("111111");

			//no card
			QCOMPARE(mWorker->setEidPin(newPin, 5), ResponseApduResult({CardReturnCode::CARD_NOT_FOUND}));

			setCard();

			//basic reader
			QTest::ignoreMessage(QtWarningMsg, "Modify PIN failed");
			QCOMPARE(mWorker->setEidPin(newPin, 5), ResponseApduResult({CardReturnCode::OK, ResponseApdu(QByteArray::fromHex("6982"))}));

			//comfort reader
			mReader->setInfoBasicReader(false);
			QTest::ignoreMessage(QtWarningMsg, "Setting eID PIN is not supported");
			QCOMPARE(mWorker->setEidPin(QByteArray(), 5), ResponseApduResult({CardReturnCode::COMMAND_FAILED}));
		}


		void test_UpdateRetryCounter()
		{
			//no card
			QCOMPARE(mWorker->updateRetryCounter(), CardReturnCode::CARD_NOT_FOUND);
		}


		void test_readFile_data()
		{
			QTest::addColumn<QList<TransmitConfig>>("responses");
			QTest::addColumn<QByteArray>("expectedFileContent");

			QTest::newRow("short file - success") << QList<TransmitConfig>({
						TransmitConfig(CardReturnCode::OK, QByteArray(8, 0) + QByteArray::fromHex("9000")),
						TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("9000")),
						TransmitConfig(CardReturnCode::COMMAND_FAILED, QByteArray())
					}) << QByteArray(8, 0);

			QTest::newRow("short file - end of file") << QList<TransmitConfig>({
						TransmitConfig(CardReturnCode::OK, QByteArray(8, 0) + QByteArray::fromHex("6282")),
						TransmitConfig(CardReturnCode::COMMAND_FAILED, QByteArray())
					}) << QByteArray(8, 0);

			QTest::newRow("short file - illegal offset") << QList<TransmitConfig>({
						TransmitConfig(CardReturnCode::OK, QByteArray(256, 0) + QByteArray::fromHex("9000")),
						TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6B00")),
						TransmitConfig(CardReturnCode::COMMAND_FAILED, QByteArray())
					}) << QByteArray(256, 0);

			QTest::newRow("long file - success") << QList<TransmitConfig>({
						TransmitConfig(CardReturnCode::OK, QByteArray(256, 0) + QByteArray::fromHex("9000")),
						TransmitConfig(CardReturnCode::OK, QByteArray(8, 0) + QByteArray::fromHex("9000")),
						TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("9000")),
						TransmitConfig(CardReturnCode::COMMAND_FAILED, QByteArray())
					}) << QByteArray(264, 0);

			QTest::newRow("long file - end of file") << QList<TransmitConfig>({
						TransmitConfig(CardReturnCode::OK, QByteArray(256, 0) + QByteArray::fromHex("9000")),
						TransmitConfig(CardReturnCode::OK, QByteArray(8, 0) + QByteArray::fromHex("6282")),
						TransmitConfig(CardReturnCode::COMMAND_FAILED, QByteArray())
					}) << QByteArray(264, 0);

			QTest::newRow("long file - illegal offset") << QList<TransmitConfig>({
						TransmitConfig(CardReturnCode::OK, QByteArray(256, 0) + QByteArray::fromHex("9000")),
						TransmitConfig(CardReturnCode::OK, QByteArray(256, 0) + QByteArray::fromHex("9000")),
						TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6B00")),
						TransmitConfig(CardReturnCode::COMMAND_FAILED, QByteArray())
					}) << QByteArray(512, 0);
		}


		void test_readFile()
		{
			QFETCH(QList<TransmitConfig>, responses);
			QFETCH(QByteArray, expectedFileContent);

			MockCardConfig cardConfig(responses);
			mReader->setCard(cardConfig);

			QByteArray fileContent;
			mWorker->readFile(FileRef::efCardSecurity(), fileContent);
			QCOMPARE(fileContent, expectedFileContent);

			QCOMPARE(mWorker->transmit(CommandApdu()), ResponseApduResult({CardReturnCode::COMMAND_FAILED}));
		}


		void test_getChallenge()
		{
			QCOMPARE(mWorker->getChallenge(), ResponseApduResult{CardReturnCode::CARD_NOT_FOUND});

			mReader->setCard(MockCardConfig());
			QCOMPARE(mWorker->getChallenge(), ResponseApduResult{CardReturnCode::COMMAND_FAILED});
		}


		void test_prepareIdentification()
		{
			QCOMPARE(mWorker->prepareIdentification(QByteArray()), EstablishPaceChannelOutput(CardReturnCode::CARD_NOT_FOUND));

			mReader->setCard(MockCardConfig());
			QCOMPARE(mWorker->prepareIdentification(QByteArray()), EstablishPaceChannelOutput{CardReturnCode::COMMAND_FAILED});
		}


		void test_performTAandCA()
		{
			const QByteArray empty;
			QCOMPARE(mWorker->performTAandCA(CVCertificateChain(), empty, empty, empty, empty), TerminalAndChipAuthenticationResult{CardReturnCode::CARD_NOT_FOUND});

			mReader->setCard(MockCardConfig());
			QCOMPARE(mWorker->performTAandCA(CVCertificateChain(), empty, empty, empty, empty), TerminalAndChipAuthenticationResult{CardReturnCode::COMMAND_FAILED});
		}


};

QTEST_GUILESS_MAIN(test_CardConnectionWorker)
#include "test_CardConnectionWorker.moc"
