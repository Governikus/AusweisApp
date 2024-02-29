/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref UIPlugInAutomatic
 */

#include "UIPlugInAutomatic.h"

#include "ReaderManager.h"
#include "VolatileSettings.h"
#include "WorkflowRequest.h"
#include "states/StateEnterPacePassword.h"
#include "states/StateSelectReader.h"

#include "MockCardConnection.h"
#include "MockReaderManagerPlugIn.h"
#include "TestAuthContext.h"
#include "TestWorkflowContext.h"
#include "TestWorkflowController.h"

#include <QScopeGuard>
#include <QSignalSpy>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(QLatin1String)
Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

class DummyUI
	: public UIPlugIn
{
	Q_OBJECT

	public:
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
};

void DummyUI::doShutdown()
{
}


void DummyUI::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)
}


void DummyUI::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)
}


class test_UIPlugInAutomatic
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void cleanup()
		{
			MockReaderManagerPlugIn::getInstance().removeAllReader();
		}


		void startUpShutDown()
		{
			UIPlugInAutomatic ui;
			QTest::failOnWarning(QRegularExpression(".*"_L1));
			ui.onApplicationStarted();
			ui.doShutdown();
		}


		void isDominated()
		{
			DummyUI dummy;

			UIPlugInAutomatic ui;
			QVERIFY(!ui.isDominated());
			QVERIFY(ui.mDominator.isEmpty());

			ui.onUiDomination(&ui, QStringLiteral("info"), true);
			QVERIFY(!ui.isDominated());
			QVERIFY(ui.mDominator.isEmpty());

			ui.onUiDomination(&dummy, QStringLiteral("info"), false);
			QVERIFY(!ui.isDominated());
			QVERIFY(ui.mDominator.isEmpty());

			ui.onUiDomination(&dummy, QStringLiteral("info success"), true);
			QVERIFY(ui.isDominated());
			QCOMPARE(ui.mDominator, QLatin1String("info success"));

			ui.onUiDominationReleased();
			QVERIFY(!ui.isDominated());
			QVERIFY(ui.mDominator.isEmpty());
		}


		void workflowStartedDominated()
		{
			DummyUI dummy;

			UIPlugInAutomatic ui;
			ui.onUiDomination(&dummy, QString(), true);
			QVERIFY(ui.isDominated());

			const auto& request = TestWorkflowController::createWorkflowRequest();
			const auto& context = request->getContext();
			QVERIFY(context);

			ui.onWorkflowStarted(request);
			QVERIFY(!context->wasClaimed());

			QVERIFY(ui.mContext.isNull());
			ui.mContext = context;
			ui.onWorkflowFinished(request);
			QVERIFY(!ui.mContext.isNull()); // should be cleared if not dominated
		}


		void workflowKilled()
		{
			UIPlugInAutomatic ui;
			const auto& request = TestWorkflowController::createWorkflowRequest();
			const auto& context = request->getContext();

			QTest::ignoreMessage(QtDebugMsg, R"(Claim workflow by "governikus::UIPlugInAutomatic")");
			QTest::ignoreMessage(QtWarningMsg, "Cannot handle context... abort automatic workflow");
			QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");
			ui.onWorkflowStarted(request);
			QVERIFY(context->wasClaimed());
			QVERIFY(context->isWorkflowKilled());
		}


		void workflowStarted()
		{
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);

			UIPlugInAutomatic ui;
			const auto& request = TestWorkflowController::createWorkflowRequest<TestAuthContext>();
			const auto& context = request->getContext();

			QVERIFY(ui.mContext.isNull());
			QVERIFY(!Env::getSingleton<VolatileSettings>()->isUsedAsSDK());
			QTest::ignoreMessage(QtDebugMsg, R"(Claim workflow by "governikus::UIPlugInAutomatic")");
			QTest::ignoreMessage(QtDebugMsg, "Fallback to full automatic UI");
			ui.onWorkflowStarted(request);
			QVERIFY(!ui.mContext.isNull());
			QVERIFY(context->wasClaimed());
			QVERIFY(!context->isWorkflowKilled());
			QVERIFY(Env::getSingleton<VolatileSettings>()->isUsedAsSDK());
			QVERIFY(!ui.mPrevUsedAsSDK);

			ui.onWorkflowFinished(request);
			QVERIFY(!Env::getSingleton<VolatileSettings>()->isUsedAsSDK());
			QVERIFY(!ui.mPrevUsedAsSDK);
			QVERIFY(ui.mContext.isNull());
		}


		void stateChanged()
		{
			UIPlugInAutomatic ui;
			ui.onStateChanged(QStringLiteral("do nothing"));
			const auto& request = TestWorkflowController::createWorkflowRequest<TestAuthContext>();
			const auto& context = request->getContext();

			QVERIFY(!context->isStateApproved());
			ui.onWorkflowStarted(request);
			QVERIFY(!context->isStateApproved());

			context->setCurrentState(QStringLiteral("approve me"));
			QVERIFY(context->isStateApproved());
		}


		void insertNoCard()
		{
			UIPlugInAutomatic ui;
			const auto& request = TestWorkflowController::createWorkflowRequest<TestAuthContext>();
			const auto& context = request->getContext();

			ui.onWorkflowStarted(request);

			QTest::ignoreMessage(QtWarningMsg, "Cannot insert card... abort automatic workflow");
			QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");
			context->setCurrentState(StateBuilder::generateStateName<StateSelectReader>());
			QVERIFY(context->isWorkflowKilled());
		}


		void insertCard()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader"_L1);
			reader->setCard(MockCardConfig());
			QVERIFY(reader->getReaderInfo().hasEid());
			QVERIFY(!reader->getReaderInfo().isInsertable());

			UIPlugInAutomatic ui;
			const auto& request = TestWorkflowController::createWorkflowRequest<TestAuthContext>();
			const auto& context = request->getContext();

			ui.onWorkflowStarted(request);
			QVERIFY(!context->isStateApproved());
			QTest::ignoreMessage(QtDebugMsg, "Use existing card...");
			context->setCurrentState(StateBuilder::generateStateName<StateSelectReader>());
			QVERIFY(!context->isWorkflowKilled());
			QVERIFY(context->isStateApproved());
		}


		void insertableCard()
		{
			MockReader* unusedReader = MockReaderManagerPlugIn::getInstance().addReader("MockReader1"_L1);
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader2"_L1);
			reader->setCard(MockCardConfig());

			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderPropertiesUpdated);
			reader->shelveCard();
			QTRY_VERIFY(spy.count() > 0); // clazy:exclude=qstring-allocations

			const auto& unusedReaderInfo = Env::getSingleton<ReaderManager>()->getReaderInfo(unusedReader->getName());
			const auto& readerInfo = Env::getSingleton<ReaderManager>()->getReaderInfo(reader->getName());

			QVERIFY(!unusedReaderInfo.hasEid());
			QVERIFY(!unusedReaderInfo.isInsertable());
			QVERIFY(!unusedReaderInfo.wasShelved());
			QVERIFY(!readerInfo.hasEid());
			QVERIFY(readerInfo.isInsertable());
			QVERIFY(readerInfo.wasShelved());

			UIPlugInAutomatic ui;
			const auto& request = TestWorkflowController::createWorkflowRequest<TestAuthContext>();
			const auto& context = request->getContext();

			QSignalSpy spyCard(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardInserted);
			ui.onWorkflowStarted(request);
			QVERIFY(!context->isStateApproved());
			QTest::ignoreMessage(QtDebugMsg, R"(Automatically insert card into: "MockReader2")");
			context->setCurrentState(StateBuilder::generateStateName<StateSelectReader>());
			QVERIFY(!context->isWorkflowKilled());
			QVERIFY(context->isStateApproved());

			QTRY_VERIFY(spyCard.count() > 0); // clazy:exclude=qstring-allocations
			QVERIFY(Env::getSingleton<ReaderManager>()->getReaderInfo(reader->getName()).hasEid());
		}


		void handlePasswordFailed()
		{

			UIPlugInAutomatic ui;
			const auto& request = TestWorkflowController::createWorkflowRequest<TestAuthContext>();
			const auto& context = request->getContext();
			context->setLastPaceResult(CardReturnCode::PROTOCOL_ERROR);

			ui.onWorkflowStarted(request);
			QTest::ignoreMessage(QtWarningMsg, "Previous PACE failed... abort automatic workflow");
			QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");
			context->setCurrentState(StateBuilder::generateStateName<StateEnterPacePassword>());
			QVERIFY(context->isWorkflowKilled());
		}


		void handlePasswordSimulator_data()
		{
			QTest::addColumn<CardReturnCode>("returnCode");

			QTest::newRow("OK") << CardReturnCode::OK;
			QTest::newRow("OK_PUK") << CardReturnCode::OK_PUK;
		}


		void handlePasswordSimulator()
		{
			QFETCH(CardReturnCode, returnCode);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader"_L1, ReaderManagerPlugInType::SIMULATOR);
			reader->setCard(MockCardConfig());
			reader->setInfoBasicReader(false);

			UIPlugInAutomatic ui;
			const auto& request = TestWorkflowController::createWorkflowRequest<TestAuthContext>();
			const auto& context = request->getContext();
			context->setLastPaceResult(returnCode);
			context->setCardConnection(QSharedPointer<MockCardConnection>::create(reader->getReaderInfo()));

			ui.onWorkflowStarted(request);
			context->setCurrentState(StateBuilder::generateStateName<StateEnterPacePassword>());
			QVERIFY(!context->isWorkflowKilled());
			QVERIFY(context->isStateApproved());
		}


		void handlePasswordNonBasic()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader"_L1);
			reader->setCard(MockCardConfig());
			reader->setInfoBasicReader(false);

			UIPlugInAutomatic ui;
			const auto& request = TestWorkflowController::createWorkflowRequest<TestAuthContext>();
			const auto& context = request->getContext();
			context->setLastPaceResult(CardReturnCode::OK);
			context->setCardConnection(QSharedPointer<MockCardConnection>::create(reader->getReaderInfo()));

			ui.onWorkflowStarted(request);
			QTest::ignoreMessage(QtWarningMsg, "Cannot handle password... abort automatic workflow");
			QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");
			context->setCurrentState(StateBuilder::generateStateName<StateEnterPacePassword>());
			QVERIFY(context->isWorkflowKilled());
		}


		void handlePasswordPaceChannel_data()
		{
			QTest::addColumn<PacePasswordId>("passwordId");
			QTest::addColumn<bool>("killWorkflow");
			QTest::addColumn<QLatin1String>("envVariable");
			QTest::addColumn<QByteArray>("envValue");

			QT_WARNING_PUSH
					QT_WARNING_DISABLE_GCC("-Wconversion")
			QT_WARNING_DISABLE_CLANG("-Wconversion")
			const PacePasswordId nonExisting = static_cast<PacePasswordId>(1904);
			QT_WARNING_POP
			QTest::newRow("wrong value") << nonExisting << true << QLatin1String() << QByteArray();

			QTest::newRow("PACE_PIN undefined") << PacePasswordId::PACE_PIN << false << QLatin1String() << QByteArray();
			QTest::newRow("PACE_CAN undefined") << PacePasswordId::PACE_CAN << true << QLatin1String() << QByteArray();
			QTest::newRow("PACE_PUK undefined") << PacePasswordId::PACE_PUK << true << QLatin1String() << QByteArray();

			QTest::newRow("PACE_PIN defined OK") << PacePasswordId::PACE_PIN << false
												 << QLatin1String("AUSWEISAPP2_AUTOMATIC_PIN") << QByteArray("654321");
			QTest::newRow("PACE_CAN defined OK") << PacePasswordId::PACE_CAN << false
												 << QLatin1String("AUSWEISAPP2_AUTOMATIC_CAN") << QByteArray("123456");
			QTest::newRow("PACE_PUK defined OK") << PacePasswordId::PACE_PUK << false
												 << QLatin1String("AUSWEISAPP2_AUTOMATIC_PUK") << QByteArray("1234567890");

			QTest::newRow("PACE_PIN defined wrong size") << PacePasswordId::PACE_PIN << true
														 << QLatin1String("AUSWEISAPP2_AUTOMATIC_PIN") << QByteArray("12345");
			QTest::newRow("PACE_CAN defined wrong size") << PacePasswordId::PACE_CAN << true
														 << QLatin1String("AUSWEISAPP2_AUTOMATIC_CAN") << QByteArray("1");
			QTest::newRow("PACE_PUK defined wrong size") << PacePasswordId::PACE_PUK << true
														 << QLatin1String("AUSWEISAPP2_AUTOMATIC_PUK") << QByteArray("12345678901");

			QTest::newRow("UNKNOWN") << PacePasswordId::UNKNOWN << true << QLatin1String() << QByteArray();
			QTest::newRow("PACE_MRZ") << PacePasswordId::PACE_MRZ << true << QLatin1String() << QByteArray();
		}


		void handlePasswordPaceChannel()
		{
			QFETCH(PacePasswordId, passwordId);
			QFETCH(bool, killWorkflow);
			QFETCH(QLatin1String, envVariable);
			QFETCH(QByteArray, envValue);

			const auto guard = qScopeGuard([envVariable] {
					if (!envVariable.isEmpty())
					{
						qunsetenv(envVariable.data());
					}
				});

			if (!envVariable.isEmpty())
			{
				qputenv(envVariable.data(), envValue);
			}

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader"_L1);
			reader->setCard(MockCardConfig());
			reader->setInfoBasicReader(true);

			UIPlugInAutomatic ui;
			const auto& request = TestWorkflowController::createWorkflowRequest<TestAuthContext>();
			const auto& context = request->getContext();
			context->setLastPaceResult(CardReturnCode::OK);
			context->setEstablishPaceChannelType(passwordId);
			context->setCardConnection(QSharedPointer<MockCardConnection>::create(reader->getReaderInfo()));

			ui.onWorkflowStarted(request);
			if (killWorkflow)
			{
				QTest::ignoreMessage(QtWarningMsg, "Cannot handle password... abort automatic workflow");
				QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");
			}
			context->setCurrentState(StateBuilder::generateStateName<StateEnterPacePassword>());
			QCOMPARE(context->isWorkflowKilled(), killWorkflow);
			QVERIFY(context->isStateApproved());

			if (killWorkflow)
			{
				QVERIFY(context->getPin().isEmpty());
				QVERIFY(context->getCan().isEmpty());
				QVERIFY(context->getPuk().isEmpty());
			}
			else
			{
				const auto value = QString::fromLatin1(envValue);
				QVERIFY(context->getPin() == value
						|| context->getCan() == value
						|| context->getPuk() == value);
			}
		}


};

QTEST_GUILESS_MAIN(test_UIPlugInAutomatic)
#include "test_UIPlugInAutomatic.moc"
