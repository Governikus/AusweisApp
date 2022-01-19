/*!
 * \brief Unit tests for \ref NumberModel
 *
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "NumberModel.h"

#include "context/ChangePinContext.h"
#include "context/RemoteServiceContext.h"

#include "LanguageLoader.h"
#include "MockCardConnectionWorker.h"
#include "TestWorkflowContext.h"

#include <QDebug>
#include <QtTest>

using namespace governikus;


class test_NumberModel
	: public QObject
{
	Q_OBJECT

	private:
		NumberModel* mModel = nullptr;

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
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());

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
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
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
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
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
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			const QSharedPointer<ChangePinContext> changePinContext(new ChangePinContext());
			const QSharedPointer<RemoteServiceContext> remoteServiceContext(new RemoteServiceContext());
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
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			const QString puk = QStringLiteral("111111");

			mModel->resetContext(context);
			mModel->setPuk(puk);
			QCOMPARE(context->getPuk(), puk);
			QCOMPARE(mModel->getPuk(), puk);
		}


		void test_OnReaderInfoChanged()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
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

			QSharedPointer<WorkflowContext> context(new WorkflowContext());

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
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, six-digit PIN. You have two further attempts to enter the correct PIN."));
			QVERIFY(mModel->hasError());

			context->setLastPaceResult(CardReturnCode::INVALID_PIN_2);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, six-digit PIN twice. "
												 "For a third attempt, the six-digit Card Access Number (CAN) must be entered first. "
												 "You can find your CAN in the bottom right on the front of your ID card."));
			QVERIFY(mModel->hasError());

			context->setLastPaceResult(CardReturnCode::INVALID_PIN_3);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, six-digit PIN thrice, your PIN is now blocked. "
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
												 "Please note that you may use the five-digit Transport PIN only once to change to a six-digit PIN. "
												 "If you already set a six-digit PIN, the five-digit Transport PIN is no longer valid."));
			context->setLastPaceResult(CardReturnCode::INVALID_PIN_2);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, five-digit Transport PIN twice. "
												 "For a third attempt, the six-digit Card Access Number (CAN) must be entered first. "
												 "You can find your CAN in the bottom right on the front of your ID card."
												 "<br><br>"
												 "Please note that you may use the five-digit Transport PIN only once to change to a six-digit PIN. "
												 "If you already set a six-digit PIN, the five-digit Transport PIN is no longer valid."));
			context->setLastPaceResult(CardReturnCode::INVALID_PIN_3);
			QCOMPARE(mModel->getInputError(), tr("You have entered an incorrect, five-digit Transport PIN thrice, your Transport PIN is now blocked. "
												 "To remove the block, the ten-digit PUK must be entered first."
												 "<br><br>"
												 "Please note that you may use the five-digit Transport PIN only once to change to a six-digit PIN. "
												 "If you already set a six-digit PIN, the five-digit Transport PIN is no longer valid."));

			connectionThread.quit();
			connectionThread.wait();
		}


		void test_GetRetryCounter()
		{
			QThread connectionThread;
			connectionThread.start();

			const QSharedPointer<WorkflowContext> context(new WorkflowContext());

			QCOMPARE(mModel->getRetryCounter(), -1);

			mModel->resetContext(context);
			QCOMPARE(mModel->getRetryCounter(), -1);

			const QString name = QStringLiteral("name");
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(),
					3, true, false);
			MockReader reader(name);
			reader.getReaderInfo().setCardInfo(cardInfo);
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
			QTest::addColumn<QSharedPointer<WorkflowContext> >("context");
			QTest::addColumn<bool>("contextRequestedTransportPin");

			QTest::newRow("WorkflowContext") << QSharedPointer<WorkflowContext>(new WorkflowContext()) << false;
			QTest::newRow("ChangePinContext") << QSharedPointer<WorkflowContext>(new ChangePinContext()) << false;
			QTest::newRow("ChangePinContext-false") << QSharedPointer<WorkflowContext>(new ChangePinContext(false)) << false;
			QTest::newRow("ChangePinContext-true") << QSharedPointer<WorkflowContext>(new ChangePinContext(true)) << true;
		}


		void test_RequestTransportPin()
		{
			QFETCH(QSharedPointer<WorkflowContext>, context);
			QFETCH(bool, contextRequestedTransportPin);

			QVERIFY(!mModel->isRequestTransportPin());

			mModel->resetContext(context);
			QCOMPARE(mModel->isRequestTransportPin(), contextRequestedTransportPin);

			mModel->resetContext();
			QVERIFY(!mModel->isRequestTransportPin());
		}


		void test_GetPasswordType_ContextIsNullptr()
		{
			QCOMPARE(mModel->getPasswordType(), NumberModel::QmlPasswordType::PASSWORD_PIN);
		}


		void test_GetPasswordType_RequestNewPin()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());

			mModel->resetContext(context);
			mModel->requestNewPin();

			QCOMPARE(mModel->getPasswordType(), NumberModel::QmlPasswordType::PASSWORD_NEW_PIN);
		}


		void test_GetPasswordType_data()
		{
			QTest::addColumn<PacePasswordId>("passwordId");
			QTest::addColumn<NumberModel::QmlPasswordType>("passwordType");

			QTest::newRow("unknown") << PacePasswordId::UNKNOWN << NumberModel::QmlPasswordType::PASSWORD_PIN;
			QTest::newRow("mrz") << PacePasswordId::PACE_MRZ << NumberModel::QmlPasswordType::PASSWORD_PIN;
			QTest::newRow("pin") << PacePasswordId::PACE_PIN << NumberModel::QmlPasswordType::PASSWORD_PIN;
			QTest::newRow("can") << PacePasswordId::PACE_CAN << NumberModel::QmlPasswordType::PASSWORD_CAN;
			QTest::newRow("puk") << PacePasswordId::PACE_PUK << NumberModel::QmlPasswordType::PASSWORD_PUK;
		}


		void test_GetPasswordType()
		{
			QFETCH(PacePasswordId, passwordId);
			QFETCH(NumberModel::QmlPasswordType, passwordType);

			const QSharedPointer<WorkflowContext> context(new WorkflowContext());

			mModel->resetContext(context);

			context->setEstablishPaceChannelType(passwordId);
			QCOMPARE(mModel->getPasswordType(), passwordType);
		}


		void test_OnCardConnectionChanged()
		{
			QThread connectionThread;
			connectionThread.start();

			const QSharedPointer<WorkflowContext> context(new WorkflowContext());

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


};

QTEST_GUILESS_MAIN(test_NumberModel)
#include "test_NumberModel.moc"
