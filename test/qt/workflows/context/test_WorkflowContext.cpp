/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "context/WorkflowContext.h"

#include "MockCardConnection.h"
#include "MockCardConnectionWorker.h"
#include "TestWorkflowContext.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_WorkflowContext
	: public QObject
{
	Q_OBJECT
	QSharedPointer<TestWorkflowContext> mContext;

	private:
		QList<AcceptedEidType> allow_all_types()
		{
			return QList<AcceptedEidType>({AcceptedEidType::CARD_CERTIFIED, AcceptedEidType::HW_KEYSTORE, AcceptedEidType::SE_CERTIFIED, AcceptedEidType::SE_ENDORSED});
		}


		QList<AcceptedEidType> allow_all_types_but(const AcceptedEidType& pType)
		{
			auto types = allow_all_types();
			types.removeOne(pType);
			return types;
		}


		QList<AcceptedEidType> allow_only(std::initializer_list<AcceptedEidType> pTypes)
		{
			return QList<AcceptedEidType>(pTypes);
		}


		QSharedPointer<CardConnection> createCardConnection(CardType pCardType)
		{
			auto readerInfo = ReaderInfo("reader"_L1, ReaderManagerPluginType::UNKNOWN, CardInfo(pCardType));
			return QSharedPointer<CardConnection>(new MockCardConnection(readerInfo));
		}

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new TestWorkflowContext());
		}


		void cleanup()
		{
			mContext.clear();
		}


		void test_WorkflowFinished()
		{
			mContext->setWorkflowFinished(true);
			QVERIFY(mContext->isWorkflowFinished());

			mContext->setWorkflowFinished(false);
			QVERIFY(!mContext->isWorkflowFinished());
		}


		void test_Can()
		{
			const QString can1 = QStringLiteral("123256");
			const QString can2 = QStringLiteral("222222");
			const QString can3 = QStringLiteral("222222");

			QSignalSpy spy(mContext.data(), &WorkflowContext::fireCanChanged);

			mContext->setCan(can1);
			QCOMPARE(mContext->getCan(), can1);
			QCOMPARE(spy.count(), 1);

			mContext->setCan(can2);
			QCOMPARE(mContext->getCan(), can2);
			QCOMPARE(spy.count(), 2);

			mContext->setCan(can3);
			QCOMPARE(mContext->getCan(), can2);
			QCOMPARE(spy.count(), 2);
		}


		void test_Pin()
		{
			const QString pin1 = QStringLiteral("123256");
			const QString pin2 = QStringLiteral("222222");
			const QString pin3 = QStringLiteral("222222");
			QSignalSpy spy(mContext.data(), &WorkflowContext::firePinChanged);

			mContext->setPin(pin1);
			QCOMPARE(mContext->getPin(), pin1);
			QCOMPARE(spy.count(), 1);

			mContext->setPin(pin2);
			QCOMPARE(mContext->getPin(), pin2);
			QCOMPARE(spy.count(), 2);

			mContext->setPin(pin3);
			QCOMPARE(mContext->getPin(), pin2);
			QCOMPARE(spy.count(), 2);
		}


		void test_Puk()
		{
			const QString puk1 = QStringLiteral("123256789");
			const QString puk2 = QStringLiteral("222222222");
			const QString puk3 = QStringLiteral("222222222");
			QSignalSpy spy(mContext.data(), &WorkflowContext::firePukChanged);

			mContext->setPuk(puk1);
			QCOMPARE(mContext->getPuk(), puk1);
			QCOMPARE(spy.count(), 1);

			mContext->setPuk(puk2);
			QCOMPARE(mContext->getPuk(), puk2);
			QCOMPARE(spy.count(), 2);

			mContext->setPuk(puk3);
			QCOMPARE(mContext->getPuk(), puk2);
			QCOMPARE(spy.count(), 2);
		}


		void test_CurrentState()
		{
			const QString state1 = QStringLiteral("state1");
			const QString state2 = QStringLiteral("state2");
			QSignalSpy spy(mContext.data(), &WorkflowContext::fireStateChanged);

			mContext->setCurrentState(state1);
			QCOMPARE(mContext->getCurrentState(), state1);
			QCOMPARE(spy.count(), 1);
			QVERIFY(!mContext->isStateApproved());

			mContext->setCurrentState(state2);
			QCOMPARE(mContext->getCurrentState(), state2);
			QCOMPARE(spy.count(), 2);
			QVERIFY(!mContext->isStateApproved());

			mContext->killWorkflow();
			QCOMPARE(mContext->getCurrentState(), state2);
			QCOMPARE(spy.count(), 2);
			QVERIFY(mContext->isStateApproved());
		}


		void test_ReaderPluginTypes()
		{
			QList<ReaderManagerPluginType> vector1({ReaderManagerPluginType::PCSC});
			QList<ReaderManagerPluginType> vector2({ReaderManagerPluginType::REMOTE_IFD});
			QSignalSpy spy(mContext.data(), &WorkflowContext::fireReaderPluginTypesChanged);

			mContext->setReaderPluginTypes(vector1);
			QCOMPARE(mContext->getReaderPluginTypes(), vector1);
			QCOMPARE(spy.count(), 1);
			spy.clear();

			mContext->setReaderPluginTypes(vector2);
			QCOMPARE(mContext->getReaderPluginTypes(), vector2);
			QCOMPARE(spy.count(), 1);
		}


		void test_LastPaceAndResult()
		{
			QSignalSpy spy(mContext.data(), &WorkflowContext::firePaceResultUpdated);

			mContext->setLastPaceResult(CardReturnCode::COMMAND_FAILED);
			QCOMPARE(mContext->getLastPaceResult(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(spy.count(), 1);

			mContext->setLastPaceResult(CardReturnCode::OK);
			QCOMPARE(mContext->getLastPaceResult(), CardReturnCode::OK);
			QCOMPARE(spy.count(), 2);
		}


		void test_CardConnection()
		{
			QSignalSpy spy(mContext.data(), &WorkflowContext::fireCardConnectionChanged);

			QSharedPointer<CardConnection> cardConnection1(new MockCardConnection());
			mContext->setCardConnection(cardConnection1);
			QCOMPARE(mContext->getCardConnection(), cardConnection1);
			QCOMPARE(spy.count(), 1);

			QSharedPointer<CardConnection> cardConnection2(new MockCardConnection());
			mContext->setCardConnection(cardConnection2);
			QCOMPARE(mContext->getCardConnection(), cardConnection2);
			QCOMPARE(spy.count(), 2);

			mContext->setCardConnection(cardConnection2);
			QCOMPARE(mContext->getCardConnection(), cardConnection2);
			QCOMPARE(spy.count(), 2);
		}


		void test_IsPinBlocked()
		{
			QThread workerThread;
			workerThread.start();

			{
				const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
				worker->moveToThread(&workerThread);
				mContext->setCardConnection(QSharedPointer<CardConnection>::create(worker));
				QVERIFY(!mContext->isPinBlocked());

				const CardInfo cardInfo1(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 3, false, false);
				const ReaderInfo readerInfo1(QString(), ReaderManagerPluginType::UNKNOWN, cardInfo1);
				Q_EMIT worker->fireReaderInfoChanged(readerInfo1);
				QVERIFY(!mContext->isPinBlocked());

				const CardInfo cardInfo2(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 0, false, false);
				const ReaderInfo readerInfo2(QString(), ReaderManagerPluginType::UNKNOWN, cardInfo2);
				Q_EMIT worker->fireReaderInfoChanged(readerInfo2);
				QVERIFY(mContext->isPinBlocked());
			}

			workerThread.quit();
			workerThread.wait();
		}


		void test_WorkflowKilled()
		{
			QSignalSpy spy(mContext.data(), &WorkflowContext::fireCancelWorkflow);

			QVERIFY(!mContext->isWorkflowKilled());

			QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");
			mContext->killWorkflow();
			QVERIFY(mContext->isWorkflowKilled());
			QCOMPARE(mContext->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_Cancellation_By_User);
			QVERIFY(mContext->isStateApproved());
			QCOMPARE(spy.count(), 1);
		}


		void test_IsWorkflowCancelled()
		{
			QVERIFY(!mContext->isWorkflowCancelled());

			Q_EMIT mContext->fireCancelWorkflow();
			QVERIFY(mContext->isWorkflowCancelled());
		}


		void test_alreadyClaimed()
		{
			QVERIFY(!mContext->wasClaimed());

			QTest::ignoreMessage(QtDebugMsg, R"(Claim workflow by "test_WorkflowContext")");
			mContext->claim(this);
			QVERIFY(mContext->wasClaimed());

			QTest::ignoreMessage(QtDebugMsg, R"(Claim workflow by "test_WorkflowContext")");
			QTest::ignoreMessage(QtWarningMsg, R"(Workflow already claimed by "test_WorkflowContext")");
			mContext->claim(this);
			QVERIFY(mContext->wasClaimed());
		}


		void test_setFailureCode()
		{
			QVERIFY(!mContext->getFailureCode().has_value());

			mContext->setFailureCode(FailureCode::Reason::Card_Removed);

			const auto& firstFailureCode = mContext->getFailureCode();
			QVERIFY(firstFailureCode.has_value());
			QCOMPARE(firstFailureCode->getReason(), FailureCode::Reason::Card_Removed);

			QTest::ignoreMessage(QtWarningMsg, "FailureCode already set to Card_Removed - ignoring User_Cancelled");
			mContext->setFailureCode(FailureCode::Reason::User_Cancelled);
			QCOMPARE(firstFailureCode->getReason(), FailureCode::Reason::Card_Removed);
		}


		void test_eidTypeMismatch_data()
		{
			QTest::addColumn<QSharedPointer<CardConnection>>("cardConnection");
			QTest::addColumn<QList<AcceptedEidType>>("acceptedTypes");
			QTest::addColumn<bool>("result");

			QTest::addRow("No error when no cardconnection 1") << QSharedPointer<CardConnection>() << QList<AcceptedEidType>() << false;
			QTest::addRow("No error when no cardconnection 2") << QSharedPointer<CardConnection>() << allow_all_types() << false;

			QTest::addRow("No error when no card") << createCardConnection(CardType::NONE) << QList<AcceptedEidType>() << false;

			QTest::addRow("ID card allowed") << createCardConnection(CardType::EID_CARD) << allow_all_types() << false;
			QTest::addRow("ID card not allowed") << createCardConnection(CardType::EID_CARD) << allow_all_types_but(AcceptedEidType::CARD_CERTIFIED) << true;

			QTest::addRow("Smart-eID with unknown EidType not allowed") << createCardConnection(CardType::SMART_EID) << allow_only({AcceptedEidType::CARD_CERTIFIED, AcceptedEidType::SE_CERTIFIED}) << true;
		}


		void test_eidTypeMismatch()
		{
			QFETCH(QSharedPointer<CardConnection>, cardConnection);
			QFETCH(QList<AcceptedEidType>, acceptedTypes);
			QFETCH(bool, result);

			mContext->setAcceptedEidTypes(acceptedTypes);
			mContext->setCardConnection(cardConnection);

			QCOMPARE(mContext->eidTypeMismatch(), result);
		}


		void test_isMobileEidTypeAllowed_data()
		{
			QTest::addColumn<MobileEidType>("mobileEidType");
			QTest::addColumn<QList<AcceptedEidType>>("acceptedTypes");
			QTest::addColumn<bool>("result");

			QTest::addRow("UNKNOWN not allowed") << MobileEidType::UNKNOWN << allow_all_types() << false;
			QTest::addRow("All allowed 1") << MobileEidType::HW_KEYSTORE << allow_all_types() << true;
			QTest::addRow("All allowed 2") << MobileEidType::SE_CERTIFIED << allow_all_types() << true;
			QTest::addRow("All allowed 3") << MobileEidType::SE_ENDORSED << allow_all_types() << true;

			QTest::addRow("Only id card allowed 1") << MobileEidType::HW_KEYSTORE << allow_only({AcceptedEidType::CARD_CERTIFIED}) << false;
			QTest::addRow("Only id card allowed 2") << MobileEidType::SE_CERTIFIED << allow_only({AcceptedEidType::CARD_CERTIFIED}) << false;
			QTest::addRow("Only id card allowed 3") << MobileEidType::SE_ENDORSED << allow_only({AcceptedEidType::CARD_CERTIFIED}) << false;

			QTest::addRow("Specific type not allowed") << MobileEidType::SE_ENDORSED << allow_all_types_but(AcceptedEidType::SE_ENDORSED) << false;

			QTest::addRow("Specific type allowed 1") << MobileEidType::HW_KEYSTORE << allow_only({AcceptedEidType::HW_KEYSTORE}) << true;
			QTest::addRow("Specific type allowed 2") << MobileEidType::SE_CERTIFIED << allow_only({AcceptedEidType::SE_CERTIFIED}) << true;
			QTest::addRow("Specific type allowed 3") << MobileEidType::SE_ENDORSED << allow_only({AcceptedEidType::SE_ENDORSED}) << true;
		}


		void test_isMobileEidTypeAllowed()
		{
			QFETCH(MobileEidType, mobileEidType);
			QFETCH(QList<AcceptedEidType>, acceptedTypes);
			QFETCH(bool, result);

			mContext->setAcceptedEidTypes(acceptedTypes);

			QCOMPARE(mContext->isMobileEidTypeAllowed(mobileEidType), result);
		}


		void test_setInitialInputErrorShown()
		{
			QVERIFY(!mContext->isInitialInputErrorShown());
			mContext->setInitialInputErrorShown();
			QVERIFY(mContext->isInitialInputErrorShown());
		}


};

QTEST_GUILESS_MAIN(test_WorkflowContext)
#include "test_WorkflowContext.moc"
