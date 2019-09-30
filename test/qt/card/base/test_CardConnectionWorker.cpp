/*!
 * \brief Tests for the class CardConnectionWorker.
 *
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnectionWorker.h"

#include "asn1/KnownOIDs.h"

#include "MockReader.h"

#include <QtTest>

using namespace governikus;

class test_CardConnectionWorker
	: public QObject
{
	Q_OBJECT
	QSharedPointer<CardConnectionWorker> mWorker;
	QScopedPointer<MockReader> mReader;

	void setCard()
	{
		QVector<TransmitConfig> transmitConfigs;
		transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6982")));
		transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("9000")));
		MockCardConfig cardConfig(transmitConfigs);
		mReader->setCard(cardConfig);
	}


	private Q_SLOTS:
		void init()
		{
			mReader.reset(new MockReader());
			mWorker = CardConnectionWorker::create(mReader.data());
		}


		void cleanup()
		{
			mWorker.clear();
			mReader.reset();
		}


		void test_Transmit()
		{
			CommandApdu emptyCommandApdu(QByteArray(""));
			ResponseApdu emptyApdu;

			//no card
			QCOMPARE(mWorker->transmit(emptyCommandApdu, emptyApdu), CardReturnCode::CARD_NOT_FOUND);

			setCard();

			//no secure messaging
			QCOMPARE(mWorker->transmit(emptyCommandApdu, emptyApdu), CardReturnCode::OK);
		}


		void test_EstablishPaceChannel()
		{
			const QString password("111111");
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

			//basic reader
			QTest::ignoreMessage(QtInfoMsg, "Starting PACE for PACE_CAN");
			QTest::ignoreMessage(QtInfoMsg, "Finished PACE for PACE_CAN with result PROTOCOL_ERROR");
			QCOMPARE(mWorker->establishPaceChannel(PacePasswordId::PACE_CAN, password, chat, certDescription).getPaceReturnCode(), CardReturnCode::PROTOCOL_ERROR);

			//comfort reader
			mReader->getReaderInfo().setBasicReader(false);
			QTest::ignoreMessage(QtInfoMsg, "Starting PACE for PACE_PIN");
			QTest::ignoreMessage(QtInfoMsg, "Finished PACE for PACE_PIN with result COMMAND_FAILED");
			QTest::ignoreMessage(QtWarningMsg, "Establishment of PACE channel not supported");
			QCOMPARE(mWorker->establishPaceChannel(PacePasswordId::PACE_PIN, QString(), chat, certDescription).getPaceReturnCode(), CardReturnCode::COMMAND_FAILED);
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
			mReader->getReaderInfo().setBasicReader(false);
			QTest::ignoreMessage(QtInfoMsg, "Destroying PACE channel");
			QTest::ignoreMessage(QtWarningMsg, "Destruction of PACE channel not supported");
			QCOMPARE(mWorker->destroyPaceChannel(), CardReturnCode::COMMAND_FAILED);
		}


		void test_SetEidPin()
		{
			const QString newPin("111111");
			ResponseApdu emptyApdu;
			ResponseApdu apduSuccess(StatusCode::SUCCESS);

			//no card
			QCOMPARE(mWorker->setEidPin(newPin, 5, emptyApdu), CardReturnCode::CARD_NOT_FOUND);

			setCard();

			//basic reader
			QTest::ignoreMessage(QtWarningMsg, "Modify PIN failed");
			QCOMPARE(mWorker->setEidPin(newPin, 5, emptyApdu), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(mWorker->setEidPin(newPin, 5, apduSuccess), CardReturnCode::OK);

			//comfort reader
			mReader->getReaderInfo().setBasicReader(false);
			QTest::ignoreMessage(QtWarningMsg, "Setting eID PIN is not supported");
			QCOMPARE(mWorker->setEidPin(QString(), 5, apduSuccess), CardReturnCode::COMMAND_FAILED);
		}


		void test_UpdateRetryCounter()
		{
			//no card
			QCOMPARE(mWorker->updateRetryCounter(), CardReturnCode::CARD_NOT_FOUND);
		}


};

QTEST_GUILESS_MAIN(test_CardConnectionWorker)
#include "test_CardConnectionWorker.moc"
