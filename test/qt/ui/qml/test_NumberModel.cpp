/*!
 * \brief Unit tests for \ref NumberModel
 *
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "NumberModel.h"

#include "context/ChangePinContext.h"
#include "context/IfdServiceContext.h"
#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

#include "LanguageLoader.h"
#include "MockCardConnectionWorker.h"
#include "ReaderManager.h"
#include "RemoteIfdServer.h"
#include "TestWorkflowContext.h"

#include <QDebug>
#include <QtTest>

using namespace governikus;


Q_DECLARE_METATYPE(EstablishPaceChannel)


class test_NumberModel
	: public QObject
{
	Q_OBJECT

	private:
		NumberModel* mModel = nullptr;

		static EstablishPaceChannel createDataToParse(const PacePasswordId& pinId)
		{
			const QByteArray chat = QByteArray::fromHex("7F4C12060904007F00070301020253050000000F0F");
			const QByteArray certDescription = QByteArray::fromHex("30 8202A4"
																   "        06 0A 04007F00070301030103"
																   "        A1 0E 0C0C442D547275737420476D6248"
																   "        A3 3A 0C38476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E"
																   "        A5 820248"
																   "            04 820244 4E616D652C20416E7363687269667420756E6420452D4D61696C2D4164726573736520646573204469656E737465616E626965746572733A0D0A476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E0D0A57696C68656C6D73747261C39F652034332F3433670D0A3130313137204265726C696E0D0A6265726C696E406764762E64650D0A0D0A4765736368C3A46674737A7765636B3A0D0A2D52656769737472696572756E6720756E64204C6F67696E20616D204744562D4D616B6C6572706F7274616C2D0D0A0D0A48696E7765697320617566206469652066C3BC722064656E204469656E737465616E626965746572207A757374C3A46E646967656E205374656C6C656E2C20646965206469652045696E68616C74756E672064657220566F7273636872696674656E207A756D20446174656E73636875747A206B6F6E74726F6C6C696572656E3A0D0A4265726C696E6572204265617566747261677465722066C3BC7220446174656E73636875747A20756E6420496E666F726D6174696F6E7366726569686569740D0A416E20646572205572616E696120342D31300D0A3130373837204265726C696E0D0A3033302F3133382038392D300D0A6D61696C626F7840646174656E73636875747A2D6265726C696E2E64650D0A687474703A2F2F7777772E646174656E73636875747A2D6265726C696E2E64650D0A416E737072656368706172746E65723A2044722E20416C6578616E64657220446978");

			return EstablishPaceChannel(pinId, chat, certDescription);
		}

	private Q_SLOTS:
		void init()
		{
			mModel = Env::getSingleton<NumberModel>();
		}


		void cleanup()
		{
			mModel->resetContext();
		}


		void cleanupTestCase()
		{
			mModel->deleteLater();
		}


		void test_ResetContext()
		{
			const QSharedPointer<ChangePinContext> pinContext(new ChangePinContext());
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QSignalSpy spyCanChanged(mModel, &NumberModel::fireCanChanged);
			QSignalSpy spyPinChanged(mModel, &NumberModel::firePinChanged);
			QSignalSpy spyNewPinChanged(mModel, &NumberModel::fireNewPinChanged);
			QSignalSpy spyCanAllowed(mModel, &NumberModel::fireCanAllowedModeChanged);
			QSignalSpy spyPukChanged(mModel, &NumberModel::firePukChanged);
			QSignalSpy spyReaderNameChanged(mModel, &NumberModel::fireReaderInfoChanged);
			QSignalSpy spyLastPaceResultChanged(mModel, &NumberModel::fireInputErrorChanged);

			mModel->resetContext();
			QCOMPARE(spyCanChanged.count(), 1);
			QCOMPARE(spyPinChanged.count(), 1);
			QCOMPARE(spyNewPinChanged.count(), 1);
			QCOMPARE(spyCanAllowed.count(), 1);
			QCOMPARE(spyPukChanged.count(), 1);
			QCOMPARE(spyReaderNameChanged.count(), 1);
			QCOMPARE(spyLastPaceResultChanged.count(), 1);

			mModel->resetContext(context);
			QCOMPARE(spyCanChanged.count(), 2);
			QCOMPARE(spyPinChanged.count(), 2);
			QCOMPARE(spyNewPinChanged.count(), 2);
			QCOMPARE(spyCanAllowed.count(), 2);
			QCOMPARE(spyPukChanged.count(), 2);
			QCOMPARE(spyReaderNameChanged.count(), 2);
			QCOMPARE(spyLastPaceResultChanged.count(), 2);
			Q_EMIT context->fireCanChanged();
			QCOMPARE(spyCanChanged.count(), 3);
			Q_EMIT context->firePinChanged();
			QCOMPARE(spyPinChanged.count(), 3);
			Q_EMIT context->fireCanAllowedModeChanged();
			QCOMPARE(spyCanAllowed.count(), 3);
			Q_EMIT context->fireReaderNameChanged();
			QCOMPARE(spyReaderNameChanged.count(), 3);
			Q_EMIT context->firePaceResultUpdated();
			QCOMPARE(spyLastPaceResultChanged.count(), 4);

			mModel->resetContext(pinContext);
			QCOMPARE(spyCanChanged.count(), 4);
			QCOMPARE(spyPinChanged.count(), 4);
			QCOMPARE(spyNewPinChanged.count(), 3);
			QCOMPARE(spyCanAllowed.count(), 4);
			QCOMPARE(spyPukChanged.count(), 3);
			QCOMPARE(spyReaderNameChanged.count(), 4);
			QCOMPARE(spyLastPaceResultChanged.count(), 5);

			Q_EMIT pinContext->fireCanChanged();
			QCOMPARE(spyCanChanged.count(), 5);
			Q_EMIT pinContext->firePinChanged();
			QCOMPARE(spyPinChanged.count(), 5);
			Q_EMIT pinContext->fireCanAllowedModeChanged();
			QCOMPARE(spyCanAllowed.count(), 5);
			Q_EMIT pinContext->fireReaderNameChanged();
			QCOMPARE(spyReaderNameChanged.count(), 5);
			Q_EMIT pinContext->firePaceResultUpdated();
			QCOMPARE(spyLastPaceResultChanged.count(), 7);
			Q_EMIT pinContext->fireNewPinChanged();
			QCOMPARE(spyNewPinChanged.count(), 4);
			Q_EMIT pinContext->firePukChanged();
			QCOMPARE(spyPukChanged.count(), 4);
		}


		void test_Can()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			const QString can = QStringLiteral("111111");

			mModel->setCan(can);
			QCOMPARE(mModel->getCan(), QString());

			mModel->resetContext(context);
			mModel->setCan(can);
			QCOMPARE(mModel->getCan(), can);
			QCOMPARE(context->getCan(), can);
		}


		void test_Pin()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			const QString pin = QStringLiteral("111111");
			mModel->setPin(pin);
			QCOMPARE(mModel->getPin(), QString());

			mModel->resetContext(context);
			mModel->setPin(pin);
			QCOMPARE(mModel->getPin(), pin);
			QCOMPARE(context->getPin(), pin);
		}


		void test_NewPin()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			const QSharedPointer<ChangePinContext> changePinContext(new ChangePinContext());
			const QSharedPointer<IfdServiceContext> remoteServiceContext(new IfdServiceContext(QSharedPointer<IfdServer>(new RemoteIfdServer())));
			const QString pin = QStringLiteral("111111");

			mModel->resetContext(context);
			mModel->setNewPin(pin);
			QCOMPARE(mModel->getNewPin(), QString());

			mModel->resetContext(changePinContext);
			mModel->setNewPin(pin);
			QCOMPARE(changePinContext->getNewPin(), pin);
			QCOMPARE(mModel->getNewPin(), pin);

			mModel->resetContext(remoteServiceContext);
			mModel->setNewPin(pin);
			QCOMPARE(remoteServiceContext->getNewPin(), pin);
			QCOMPARE(mModel->getNewPin(), QString());
		}


		void test_Puk()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			const QString puk = QStringLiteral("111111");

			mModel->resetContext(context);
			mModel->setPuk(puk);
			QCOMPARE(context->getPuk(), puk);
			QCOMPARE(mModel->getPuk(), puk);
		}


		void test_OnReaderInfoChanged()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			QSignalSpy spyReaderNameChanged(mModel, &NumberModel::fireReaderInfoChanged);

			const QString readerName = QStringLiteral("name");
			const QString test = QStringLiteral("test");

			const auto readerManager = Env::getSingleton<ReaderManager>();
			Q_EMIT readerManager->fireReaderPropertiesUpdated(ReaderInfo());
			QCOMPARE(spyReaderNameChanged.count(), 0);

			context->setReaderName(test);
			mModel->resetContext(context);
			Q_EMIT readerManager->fireReaderPropertiesUpdated(ReaderInfo());
			QCOMPARE(spyReaderNameChanged.count(), 1);

			context->setReaderName(readerName);
			mModel->resetContext(context);
			Q_EMIT readerManager->fireReaderPropertiesUpdated(ReaderInfo());
			QCOMPARE(spyReaderNameChanged.count(), 3);
		}


		void test_CanAllowed()
		{
			QSharedPointer<TestWorkflowContext> context(new TestWorkflowContext());
			QVERIFY(!mModel->isCanAllowedMode());

			mModel->resetContext(context);
			QVERIFY(!mModel->isCanAllowedMode());

			context->setCanAllowedMode(true);
			QVERIFY(mModel->isCanAllowedMode());
		}


		void test_Error()
		{
			QThread connectionThread;
			connectionThread.start();

			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QCOMPARE(mModel->getInputError(), QString());
			QVERIFY(!mModel->hasError());

			context->setLastPaceResult(CardReturnCode::OK);
			mModel->resetContext(context);
			QCOMPARE(mModel->getInputError(), QString());
			QVERIFY(!mModel->hasError());

			context->setLastPaceResult(CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(mModel->getInputError(), QString());
			QVERIFY(!mModel->hasError());

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&connectionThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			context->setCardConnection(connection);

			context->setLastPaceResult(CardReturnCode::INVALID_PIN);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, six-digit ID card PIN. You have two further attempts to enter the correct ID card PIN."));
			QVERIFY(mModel->hasError());

			context->setLastPaceResult(CardReturnCode::INVALID_PIN_2);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, six-digit ID card PIN twice. "
												 "For a third attempt, the six-digit Card Access Number (CAN) must be entered first. "
												 "You can find your CAN in the bottom right on the front of your ID card."));
			QVERIFY(mModel->hasError());

			context->setLastPaceResult(CardReturnCode::INVALID_PIN_3);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, six-digit ID card PIN thrice, your ID card PIN is now blocked. "
												 "To remove the block, the ten-digit PUK must be entered first."));
			QVERIFY(mModel->hasError());

			context->setLastPaceResult(CardReturnCode::INVALID_CAN);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, six-digit Card Access Number (CAN). Please try again."));

			context->setLastPaceResult(CardReturnCode::INVALID_PUK);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, ten-digit PUK. Please try again."));
			QVERIFY(mModel->hasError());

			context->setLastPaceResult(CardReturnCode::UNKNOWN);
			QCOMPARE(mModel->getInputError(), tr("An unexpected error has occurred during processing."));

			context->setLastPaceResult(CardReturnCode::UNEXPECTED_TRANSMIT_STATUS);
			QCOMPARE(mModel->getInputError(), QStringLiteral("%1 <a href=\"https://www.ausweisapp.bund.de/%2/aa2/support\">%3</a>.").arg(
					tr("A protocol error occurred. Please make sure that your ID card is placed correctly on the card reader and try again. If the problem occurs again, please contact our support at"),
					LanguageLoader::getLocaleCode(),
					tr("AusweisApp2 Support")));
			QVERIFY(mModel->hasError());

			context.reset(new ChangePinContext(true));
			mModel->resetContext(context);
			context->setCardConnection(connection);
			context->setLastPaceResult(CardReturnCode::INVALID_PIN);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, five-digit Transport PIN. "
												 "You have two further attempts to enter the correct Transport PIN."
												 "<br><br>"
												 "Please note that you may use the five-digit Transport PIN only once to change to a six-digit ID card PIN. "
												 "If you already set a six-digit ID card PIN, the five-digit Transport PIN is no longer valid."));
			context->setLastPaceResult(CardReturnCode::INVALID_PIN_2);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, five-digit Transport PIN twice. "
												 "For a third attempt, the six-digit Card Access Number (CAN) must be entered first. "
												 "You can find your CAN in the bottom right on the front of your ID card."
												 "<br><br>"
												 "Please note that you may use the five-digit Transport PIN only once to change to a six-digit ID card PIN. "
												 "If you already set a six-digit ID card PIN, the five-digit Transport PIN is no longer valid."));
			context->setLastPaceResult(CardReturnCode::INVALID_PIN_3);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, five-digit Transport PIN thrice, your Transport PIN is now blocked. "
												 "To remove the block, the ten-digit PUK must be entered first."
												 "<br><br>"
												 "Please note that you may use the five-digit Transport PIN only once to change to a six-digit ID card PIN. "
												 "If you already set a six-digit ID card PIN, the five-digit Transport PIN is no longer valid."));

			connectionThread.quit();
			connectionThread.wait();
		}


		void test_GetRetryCounter()
		{
			QThread connectionThread;
			connectionThread.start();

			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QCOMPARE(mModel->getRetryCounter(), -1);

			mModel->resetContext(context);
			QCOMPARE(mModel->getRetryCounter(), -1);

			const QString name = QStringLiteral("name");
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(),
					3, true, false);
			MockReader reader(name);
			reader.setInfoCardInfo(cardInfo);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			worker->moveToThread(&connectionThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			context->setCardConnection(connection);
			QCOMPARE(mModel->getRetryCounter(), 3);

			connectionThread.quit();
			connectionThread.wait();
		}


		void test_RequestTransportPin_data()
		{
			QTest::addColumn<QSharedPointer<WorkflowContext>>("context");
			QTest::addColumn<PasswordType>("passwordType");

			QTest::newRow("WorkflowContext") << QSharedPointer<WorkflowContext>(new TestWorkflowContext()) << PasswordType::PIN;
			QTest::newRow("ChangePinContext") << QSharedPointer<WorkflowContext>(new ChangePinContext()) << PasswordType::PIN;
			QTest::newRow("ChangePinContext-false") << QSharedPointer<WorkflowContext>(new ChangePinContext(false)) << PasswordType::PIN;
			QTest::newRow("ChangePinContext-true") << QSharedPointer<WorkflowContext>(new ChangePinContext(true)) << PasswordType::TRANSPORT_PIN;
		}


		void test_RequestTransportPin()
		{
			QFETCH(QSharedPointer<WorkflowContext>, context);
			QFETCH(PasswordType, passwordType);

			QCOMPARE(mModel->getPasswordType(), PasswordType::PIN);

			mModel->resetContext(context);
			QCOMPARE(mModel->getPasswordType(), passwordType);

			mModel->resetContext();
			QCOMPARE(mModel->getPasswordType(), PasswordType::PIN);
		}


		void test_GetPasswordType_ContextIsNullptr()
		{
			QCOMPARE(mModel->getPasswordType(), PasswordType::PIN);
		}


		void test_GetPasswordType_data()
		{
			QTest::addColumn<PacePasswordId>("passwordId");
			QTest::addColumn<PasswordType>("passwordType");

			QTest::newRow("unknown") << PacePasswordId::UNKNOWN << PasswordType::PIN;
			QTest::newRow("mrz") << PacePasswordId::PACE_MRZ << PasswordType::PIN;
			QTest::newRow("pin") << PacePasswordId::PACE_PIN << PasswordType::PIN;
			QTest::newRow("can") << PacePasswordId::PACE_CAN << PasswordType::CAN;
			QTest::newRow("puk") << PacePasswordId::PACE_PUK << PasswordType::PUK;
		}


		void test_GetPasswordType()
		{
			QFETCH(PacePasswordId, passwordId);
			QFETCH(PasswordType, passwordType);

			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			mModel->resetContext(context);

			context->setEstablishPaceChannelType(passwordId);
			QCOMPARE(mModel->getPasswordType(), passwordType);
		}


		void test_GetPasswordTypeNewPin()
		{
			const QSharedPointer<WorkflowContext> context(new ChangePinContext());
			mModel->resetContext(context);

			context->setEstablishPaceChannelType(PacePasswordId::PACE_PIN);
			QCOMPARE(mModel->getPasswordType(), PasswordType::PIN);

			context->setPin(QStringLiteral("123456"));
			QCOMPARE(mModel->getPasswordType(), PasswordType::NEW_PIN);
		}


		void test_GetPasswordTypeNewSmartPin()
		{
#if __has_include("context/PersonalizationContext.h")
			const QSharedPointer<WorkflowContext> context(new PersonalizationContext(QString()));
			mModel->resetContext(context);
			QCOMPARE(mModel->getPasswordType(), PasswordType::PIN);

			auto personalizationContext = context.objectCast<PersonalizationContext>();
			QVERIFY(personalizationContext);
			personalizationContext->setSessionIdentifier(QUuid::createUuid());
			QCOMPARE(mModel->getPasswordType(), PasswordType::NEW_SMART_PIN);
#endif
		}


		void test_OnCardConnectionChanged()
		{
			QThread connectionThread;
			connectionThread.start();

			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			mModel->resetContext(context);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&connectionThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			context->setCardConnection(connection);
			QSignalSpy spy(mModel, &NumberModel::fireReaderInfoChanged);

			Q_EMIT context->fireCardConnectionChanged();
			Q_EMIT connection->fireReaderInfoChanged(ReaderInfo());
			QCOMPARE(spy.count(), 3);

			connectionThread.quit();
			connectionThread.wait();
		}


		void test_passwordTypeRemote_data()
		{
			QTest::addColumn<EstablishPaceChannel>("inputData");
			QTest::addColumn<int>("pinLength");
			QTest::addColumn<PasswordType>("passwordType");

			QTest::newRow("default - 0") << EstablishPaceChannel() << 0 << PasswordType::PIN;
			QTest::newRow("default - 5") << EstablishPaceChannel() << 5 << PasswordType::TRANSPORT_PIN;
			QTest::newRow("default - 6") << EstablishPaceChannel() << 6 << PasswordType::PIN;

			QTest::newRow("pin - no cert - 0") << EstablishPaceChannel(PacePasswordId::PACE_PIN) << 0 << PasswordType::PIN;
			QTest::newRow("pin - no cert - 5") << EstablishPaceChannel(PacePasswordId::PACE_PIN) << 5 << PasswordType::TRANSPORT_PIN;
			QTest::newRow("pin - no cert - 6") << EstablishPaceChannel(PacePasswordId::PACE_PIN) << 6 << PasswordType::PIN;

			QTest::newRow("can - no cert - 0") << EstablishPaceChannel(PacePasswordId::PACE_CAN) << 0 << PasswordType::CAN;
			QTest::newRow("can - no cert - 5") << EstablishPaceChannel(PacePasswordId::PACE_CAN) << 5 << PasswordType::CAN;
			QTest::newRow("can - no cert - 6") << EstablishPaceChannel(PacePasswordId::PACE_CAN) << 6 << PasswordType::CAN;

			QTest::newRow("puk - no cert - 0") << EstablishPaceChannel(PacePasswordId::PACE_PUK) << 0 << PasswordType::PUK;
			QTest::newRow("puk - no cert - 5") << EstablishPaceChannel(PacePasswordId::PACE_PUK) << 5 << PasswordType::PUK;
			QTest::newRow("puk - no cert - 6") << EstablishPaceChannel(PacePasswordId::PACE_PUK) << 6 << PasswordType::PUK;

			QTest::newRow("unknown - no cert - 0") << EstablishPaceChannel(PacePasswordId::UNKNOWN) << 0 << PasswordType::PIN;
			QTest::newRow("unknown - no cert - 5") << EstablishPaceChannel(PacePasswordId::UNKNOWN) << 5 << PasswordType::TRANSPORT_PIN;
			QTest::newRow("unknown - no cert - 6") << EstablishPaceChannel(PacePasswordId::UNKNOWN) << 6 << PasswordType::PIN;

			QTest::newRow("pin - cert - 0") << createDataToParse(PacePasswordId::PACE_PIN) << 0 << PasswordType::PIN;
			QTest::newRow("pin - cert - 5") << createDataToParse(PacePasswordId::PACE_PIN) << 5 << PasswordType::TRANSPORT_PIN;
			QTest::newRow("pin - cert - 6") << createDataToParse(PacePasswordId::PACE_PIN) << 6 << PasswordType::PIN;

			QTest::newRow("can - cert - 0") << createDataToParse(PacePasswordId::PACE_CAN) << 0 << PasswordType::CAN;
			QTest::newRow("can - cert - 5") << createDataToParse(PacePasswordId::PACE_CAN) << 5 << PasswordType::CAN;
			QTest::newRow("can - cert - 6") << createDataToParse(PacePasswordId::PACE_CAN) << 6 << PasswordType::CAN;

			QTest::newRow("puk - cert - 0") << createDataToParse(PacePasswordId::PACE_PUK) << 0 << PasswordType::PUK;
			QTest::newRow("puk - cert - 5") << createDataToParse(PacePasswordId::PACE_PUK) << 5 << PasswordType::PUK;
			QTest::newRow("puk - cert - 6") << createDataToParse(PacePasswordId::PACE_PUK) << 6 << PasswordType::PUK;

			QTest::newRow("unknown - cert - 0") << createDataToParse(PacePasswordId::UNKNOWN) << 0 << PasswordType::PIN;
			QTest::newRow("unknown - cert - 5") << createDataToParse(PacePasswordId::UNKNOWN) << 5 << PasswordType::TRANSPORT_PIN;
			QTest::newRow("unknown - cert - 6") << createDataToParse(PacePasswordId::UNKNOWN) << 6 << PasswordType::PIN;
		}


		void test_passwordTypeRemote()
		{
			QFETCH(EstablishPaceChannel, inputData);
			QFETCH(int, pinLength);
			QFETCH(PasswordType, passwordType);

			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(QSharedPointer<IfdServer>(new RemoteIfdServer())));
			mModel->resetContext(context);

			QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel(QString(), inputData, pinLength));
			context->setEstablishPaceChannel(message);

			QCOMPARE(mModel->getPasswordType(), passwordType);

			context->changePinLength();
			PasswordType otherType = passwordType;
			if (context->isPinChangeWorkflow())
			{
				if (passwordType == PasswordType::PIN)
				{
					otherType = PasswordType::TRANSPORT_PIN;
				}
				else if (passwordType == PasswordType::TRANSPORT_PIN)
				{
					otherType = PasswordType::PIN;
				}
			}
			QCOMPARE(mModel->getPasswordType(), otherType);

			context->changePinLength();
			QCOMPARE(mModel->getPasswordType(), passwordType);
		}


};

QTEST_MAIN(test_NumberModel)
#include "test_NumberModel.moc"
