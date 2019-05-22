/*!
 * \brief Unit tests for \ref NumberModel
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "NumberModel.h"

#include "context/ChangePinContext.h"
#include "context/RemoteServiceContext.h"

#include "MockCardConnectionWorker.h"

#include <QDebug>
#include <QtTest>

using namespace governikus;


class test_NumberModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_ResetContext()
		{
			NumberModel model;
			QSharedPointer<WorkflowContext> context(new WorkflowContext());
			QSharedPointer<ChangePinContext> pinContext(new ChangePinContext());

			QSignalSpy spyCanChanged(&model, &NumberModel::fireCanChanged);
			QSignalSpy spyPinChanged(&model, &NumberModel::firePinChanged);
			QSignalSpy spyNewPinChanged(&model, &NumberModel::fireNewPinChanged);
			QSignalSpy spyCanAllowed(&model, &NumberModel::fireCanAllowedModeChanged);
			QSignalSpy spyPukChanged(&model, &NumberModel::firePukChanged);
			QSignalSpy spyReaderNameChanged(&model, &NumberModel::fireReaderInfoChanged);
			QSignalSpy spyLastPaceResultChanged(&model, &NumberModel::fireInputErrorChanged);

			model.resetContext();
			QCOMPARE(spyCanChanged.count(), 1);
			QCOMPARE(spyPinChanged.count(), 1);
			QCOMPARE(spyNewPinChanged.count(), 1);
			QCOMPARE(spyCanAllowed.count(), 1);
			QCOMPARE(spyPukChanged.count(), 1);
			QCOMPARE(spyReaderNameChanged.count(), 1);
			QCOMPARE(spyLastPaceResultChanged.count(), 1);

			model.resetContext(context);
			QCOMPARE(model.mContext, context);
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
			Q_EMIT context->fireLastPaceResultChanged();
			QCOMPARE(spyLastPaceResultChanged.count(), 3);

			model.resetContext(pinContext);
			QCOMPARE(model.mContext, pinContext);
			QCOMPARE(spyCanChanged.count(), 4);
			QCOMPARE(spyPinChanged.count(), 4);
			QCOMPARE(spyNewPinChanged.count(), 3);
			QCOMPARE(spyCanAllowed.count(), 4);
			QCOMPARE(spyPukChanged.count(), 3);
			QCOMPARE(spyReaderNameChanged.count(), 4);
			QCOMPARE(spyLastPaceResultChanged.count(), 4);

			Q_EMIT pinContext->fireCanChanged();
			QCOMPARE(spyCanChanged.count(), 5);
			Q_EMIT pinContext->firePinChanged();
			QCOMPARE(spyPinChanged.count(), 5);
			Q_EMIT pinContext->fireCanAllowedModeChanged();
			QCOMPARE(spyCanAllowed.count(), 5);
			Q_EMIT pinContext->fireReaderNameChanged();
			QCOMPARE(spyReaderNameChanged.count(), 5);
			Q_EMIT pinContext->fireLastPaceResultChanged();
			QCOMPARE(spyLastPaceResultChanged.count(), 5);
			Q_EMIT pinContext->fireNewPinChanged();
			QCOMPARE(spyNewPinChanged.count(), 4);
			Q_EMIT pinContext->firePukChanged();
			QCOMPARE(spyPukChanged.count(), 4);
		}


		void test_Can()
		{
			NumberModel model;
			QSharedPointer<WorkflowContext> context(new WorkflowContext());

			const QString can = QStringLiteral("111111");
			model.setCan(can);
			QCOMPARE(model.getCan(), QString());

			model.mContext = context;
			model.setCan(can);
			QCOMPARE(model.getCan(), can);
			QCOMPARE(context->getCan(), can);
		}


		void test_Pin()
		{
			NumberModel model;
			QSharedPointer<WorkflowContext> context(new WorkflowContext());

			const QString pin = QStringLiteral("111111");
			model.setPin(pin);
			QCOMPARE(model.getPin(), QString());

			model.mContext = context;
			model.setPin(pin);
			QCOMPARE(model.getPin(), pin);
			QCOMPARE(context->getPin(), pin);
		}


		void test_NewPin()
		{
			NumberModel model;
			QSharedPointer<WorkflowContext> workflowContext(new WorkflowContext());
			QSharedPointer<ChangePinContext> changePinContext(new ChangePinContext());
			QSharedPointer<RemoteServiceContext> remoteServiceContext(new RemoteServiceContext());
			const QString pin = QStringLiteral("111111");

			model.mContext = workflowContext;
			model.setNewPin(pin);
			QCOMPARE(model.getNewPin(), QString());

			model.mContext = changePinContext;
			model.setNewPin(pin);
			QCOMPARE(changePinContext->getNewPin(), pin);
			QCOMPARE(model.getNewPin(), pin);

			model.mContext = remoteServiceContext;
			model.setNewPin(pin);
			QCOMPARE(remoteServiceContext->getNewPin(), pin);
			QCOMPARE(model.getNewPin(), QString());
		}


		void test_Puk()
		{
			NumberModel model;
			QSharedPointer<WorkflowContext> workflowContext(new WorkflowContext());
			const QString puk = QStringLiteral("111111");

			model.mContext = workflowContext;
			model.setPuk(puk);
			QCOMPARE(workflowContext->getPuk(), puk);
			QCOMPARE(model.getPuk(), puk);
		}


		void test_OnReaderInfoChanged()
		{
			NumberModel model;
			QSharedPointer<WorkflowContext> context(new WorkflowContext());
			QSignalSpy spyReaderNameChanged(&model, &NumberModel::fireReaderInfoChanged);

			const QString readerName = QStringLiteral("name");
			const QString test = QStringLiteral("test");

			model.onReaderInfoChanged(readerName);
			QCOMPARE(spyReaderNameChanged.count(), 0);

			context->setReaderName(test);
			model.mContext = context;
			model.onReaderInfoChanged(readerName);
			QCOMPARE(spyReaderNameChanged.count(), 0);

			context->setReaderName(readerName);
			model.mContext = context;
			model.onReaderInfoChanged(readerName);
			QCOMPARE(spyReaderNameChanged.count(), 1);
		}


		void test_CanAllowed()
		{
			NumberModel model;
			QSharedPointer<WorkflowContext> context(new WorkflowContext());

			QVERIFY(!model.isCanAllowedMode());

			model.mContext = context;
			QVERIFY(!model.isCanAllowedMode());

			context->setCanAllowedMode(true);
			QVERIFY(model.isCanAllowedMode());
		}


		void test_PinDeactivatedFalse()
		{
			NumberModel model;
			QSharedPointer<WorkflowContext> context(new WorkflowContext());

			QVERIFY(!model.isPinDeactivated());

			model.mContext = context;
			QVERIFY(!model.isPinDeactivated());
		}


		void test_Error()
		{
			QThread connectionThread;
			connectionThread.start();

			NumberModel model;
			QSharedPointer<WorkflowContext> context(new WorkflowContext());

			QCOMPARE(model.getInputError(), QString());
			QVERIFY(!model.hasError());

			context->setLastPaceResult(CardReturnCode::OK);
			model.mContext = context;
			QCOMPARE(model.getInputError(), QString());
			QVERIFY(!model.hasError());

			context->setLastPaceResult(CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(model.getInputError(), QString());
			QVERIFY(!model.hasError());

			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&connectionThread);
			QSharedPointer<CardConnection> connection(new CardConnection(worker));
			context->setCardConnection(connection);

			context->setLastPaceResult(CardReturnCode::INVALID_PIN);
			QCOMPARE(model.getInputError(), tr("The given PIN is not correct. You have 2 tries to enter the correct PIN."));
			QVERIFY(model.hasError());

			context->setLastPaceResult(CardReturnCode::INVALID_PIN_2);
			QCOMPARE(model.getInputError(), tr("You have entered the wrong PIN twice. "
											   "Prior to a third attempt, you have to enter your six-digit card access number first. "
											   "You can find your card access number on the front of your ID card."));
			QVERIFY(model.hasError());

			context->setLastPaceResult(CardReturnCode::INVALID_PIN_3);
			QCOMPARE(model.getInputError(), tr("You have entered a wrong PIN three times. "
											   "Your PIN is now blocked. "
											   "You have to enter the PUK now for unblocking."));
			QVERIFY(model.hasError());

			context->setLastPaceResult(CardReturnCode::INVALID_CAN);
			model.mContext = context;
			QCOMPARE(model.getInputError(), tr("You have entered a wrong CAN, please try again."));

			context->setLastPaceResult(CardReturnCode::INVALID_PUK);
			QCOMPARE(model.getInputError(), tr("You have entered a wrong PUK. "
											   "Please try again."));
			QVERIFY(model.hasError());

			context->setLastPaceResult(CardReturnCode::UNKNOWN);
			QCOMPARE(model.getInputError(), tr("An unexpected error has occurred during processing."));

			context->setLastPaceResult(CardReturnCode::UNEXPECTED_TRANSMIT_STATUS);
			model.mContext = context;
			QCOMPARE(model.getInputError(), QStringLiteral("%1 <a href=\"%2\">%3</a>.").arg(
					tr("A protocol error occurred. Please make sure that your ID card is placed correctly on the card reader and try again. If the problem occurs again, please contact our support at"),
					tr("https://www.ausweisapp.bund.de/en/qa/support/"),
					tr("AusweisApp2 Support")));
			QVERIFY(model.hasError());

			connectionThread.quit();
			connectionThread.wait();
		}


		void test_GetRetryCounter()
		{
			QThread connectionThread;
			connectionThread.start();

			NumberModel model;
			QSharedPointer<WorkflowContext> context(new WorkflowContext());

			QCOMPARE(model.getRetryCounter(), -1);

			model.mContext = context;
			QCOMPARE(model.getRetryCounter(), -1);

			const QString name = QStringLiteral("name");
			CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(),
					3, true, false);
			MockReader reader(name);
			reader.getReaderInfo().setCardInfo(cardInfo);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			worker->moveToThread(&connectionThread);
			QSharedPointer<CardConnection> connection(new CardConnection(worker));
			context->setCardConnection(connection);
			QCOMPARE(model.getRetryCounter(), 3);

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

			NumberModel model;
			QVERIFY(!model.isRequestTransportPin());

			model.setRequestTransportPin(true);
			QVERIFY(model.isRequestTransportPin());
			model.setRequestTransportPin(false);
			QVERIFY(!model.isRequestTransportPin());

			model.resetContext(context);
			QCOMPARE(model.isRequestTransportPin(), contextRequestedTransportPin);

			model.setRequestTransportPin(true);
			model.resetContext(context);
			QVERIFY(model.isRequestTransportPin());

			model.setRequestTransportPin(false);
			QVERIFY(!model.isRequestTransportPin());
			model.setRequestTransportPin(true);
			QVERIFY(model.isRequestTransportPin());

			model.resetContext();
			QVERIFY(!model.isRequestTransportPin());
		}


};

QTEST_GUILESS_MAIN(test_NumberModel)
#include "test_NumberModel.moc"
