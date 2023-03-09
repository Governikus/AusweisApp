/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref MsgHandlerChangePin
 */

#include "messages/MsgHandlerChangePin.h"

#include "Env.h"
#include "MessageDispatcher.h"
#include "UILoader.h"
#include "UIPlugInJson.h"
#include "VolatileSettings.h"
#include "controller/AppController.h"

#include "TestWorkflowContext.h"

#include <QSignalSpy>
#include <QtTest>

Q_IMPORT_PLUGIN(UIPlugInJson)

using namespace governikus;

class test_MsgHandlerChangePin
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			qRegisterMetaType<QSharedPointer<WorkflowContext>>("QSharedPointer<WorkflowContext>");

			connect(Env::getSingleton<UILoader>(), &UILoader::fireLoadedPlugin, this, [](UIPlugIn* pUi){
					pUi->setProperty("passive", QVariant()); // fake active UI for AppController::start
				});
		}


		void cleanup()
		{
			auto* uiLoader = Env::getSingleton<UILoader>();
			if (uiLoader->isLoaded())
			{
				QSignalSpy spyUi(uiLoader, &UILoader::fireRemovedAllPlugins);
				uiLoader->shutdown();
				QTRY_COMPARE(spyUi.count(), 1); // clazy:exclude=qstring-allocations
			}
		}


		void runChangePinCmd()
		{
			QVERIFY(Env::getSingleton<UILoader>()->load<UIPlugInJson>());
			auto ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(ui);
			QSignalSpy spy(ui, &UIPlugIn::fireWorkflowRequested);

			MessageDispatcher dispatcher;
			const QByteArray msg = R"({"cmd": "RUN_CHANGE_PIN"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spy.count(), 1);
		}


		void initChangePin()
		{
			const auto context = QSharedPointer<TestWorkflowContext>::create();
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), QByteArray());
			QCOMPARE(dispatcher.finish(), QByteArray());

			const auto ctx = QSharedPointer<ChangePinContext>::create();
			QCOMPARE(dispatcher.init(ctx), QByteArray(R"({"msg":"CHANGE_PIN"})"));
		}


		void allowCancel()
		{
			MessageDispatcher dispatcher;

			const auto ctx = QSharedPointer<ChangePinContext>::create();
			QCOMPARE(dispatcher.init(ctx), QByteArray(R"({"msg":"CHANGE_PIN"})"));

			QVERIFY(!ctx->isWorkflowCancelled());
			QByteArray msg = R"({"cmd": "CANCEL"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
			QVERIFY(ctx->isWorkflowCancelled());

			msg = R"({"msg":"CHANGE_PIN","success":false})";
			QCOMPARE(dispatcher.finish(), msg);
		}


		void badState()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);
			const QByteArray expected(R"({"error":"RUN_CHANGE_PIN","msg":"BAD_STATE"})");

			const QByteArray msg(R"({"cmd": "RUN_CHANGE_PIN"})");
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void cancelChangePin()
		{
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();

			auto ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(ui);
			ui->setEnabled(true);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);

			const QByteArray msg(R"({"cmd": "RUN_CHANGE_PIN"})");
			ui->doMessageProcessing(msg);
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations

			// If CANCEL is not quick enough this unit test blocks until QTRY_COMPARE timeout.
			// Even without the fix it is flaky to reproduce the problem.
			const QByteArray msgCancel(R"({"cmd": "CANCEL"})");
			ui->doMessageProcessing(msgCancel);
			QTRY_COMPARE(spyFinished.count(), 1); // clazy:exclude=qstring-allocations
		}


		void iosScanDialogMessages()
		{
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();

			const auto& initialMessages = Env::getSingleton<VolatileSettings>()->getMessages();
			QVERIFY(initialMessages.getSessionStarted().isNull());
			QVERIFY(initialMessages.getSessionInProgress().isNull());
			QVERIFY(initialMessages.getSessionSucceeded().isNull());
			QVERIFY(initialMessages.getSessionFailed().isEmpty());
			QVERIFY(!initialMessages.getSessionFailed().isNull());

			auto ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(ui);
			ui->setEnabled(true);
			QSignalSpy spyUi(ui, &UIPlugIn::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](QSharedPointer<WorkflowContext> pContext){
					pContext->claim(this); // UIPlugInJson is internal API and does not claim by itself
				});

			const QByteArray msg("{"
								 "\"cmd\": \"RUN_CHANGE_PIN\","
								 "\"messages\": {"
								 "  \"sessionStarted\": \"start\","
								 "  \"sessionFailed\": \"stop failed\","
								 "  \"sessionSucceeded\": \"stop success\","
								 "  \"sessionInProgress\": \"progress\""
								 "  }"
								 "}");

			ui->doMessageProcessing(msg);
			QCOMPARE(spyUi.count(), 1);
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations

			auto messages = Env::getSingleton<VolatileSettings>()->getMessages();
			QCOMPARE(messages.getSessionStarted(), QLatin1String("start"));
			QCOMPARE(messages.getSessionInProgress(), QLatin1String("progress"));
			QCOMPARE(messages.getSessionSucceeded(), QLatin1String("stop success"));
			QCOMPARE(messages.getSessionFailed(), QLatin1String("stop failed"));

			const QByteArray msgCancel(R"({"cmd": "CANCEL"})");
			ui->doMessageProcessing(msgCancel);
			QTRY_COMPARE(spyFinished.count(), 1); // clazy:exclude=qstring-allocations

			messages = Env::getSingleton<VolatileSettings>()->getMessages();
			QVERIFY(messages.getSessionStarted().isNull());
			QVERIFY(messages.getSessionInProgress().isNull());
			QVERIFY(messages.getSessionSucceeded().isNull());
			QVERIFY(messages.getSessionFailed().isEmpty());
			QVERIFY(!messages.getSessionFailed().isNull());
		}


		void handleInterrupt_data()
		{
			QTest::addColumn<QVariant>("handleInterrupt");
			QTest::addColumn<bool>("handleInterruptExpected");
			QTest::addColumn<char>("apiLevel");

			QTest::newRow("shouldStop_v1") << QVariant(true) << true << '1';
			QTest::newRow("shouldNotStop_v1") << QVariant(false) << false << '1';
			QTest::newRow("shouldNotStop_v2 requested") << QVariant(true) << false << '2';
			QTest::newRow("shouldNotStop_v2 not requested") << QVariant(false) << false << '2';
		}


		void handleInterrupt()
		{
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			QFETCH(QVariant, handleInterrupt);
			QFETCH(bool, handleInterruptExpected);
			QFETCH(char, apiLevel);

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), false); // default

			auto ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(ui);
			ui->setEnabled(true);
			QSignalSpy spyMessage(ui, &UIPlugInJson::fireMessage);
			QSignalSpy spyUi(ui, &UIPlugIn::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](QSharedPointer<WorkflowContext> pContext){
					pContext->claim(this); // UIPlugInJson is internal API and does not claim by itself
				});

			QByteArray msgApiLevel = R"({"cmd": "SET_API_LEVEL", "level": *})";
			ui->doMessageProcessing(msgApiLevel.replace('*', apiLevel));
			QTRY_COMPARE(spyMessage.count(), 1); // clazy:exclude=qstring-allocations
			QByteArray msgApiLevelResponse = R"({"current":*,"msg":"API_LEVEL"})";
			QCOMPARE(spyMessage.takeFirst().at(0).toByteArray(), msgApiLevelResponse.replace('*', apiLevel));

			QByteArray msg("{"
						   "\"cmd\": \"RUN_CHANGE_PIN\","
						   "\"handleInterrupt\": %REPLACE%"
						   "}");
			msg.replace("%REPLACE%", handleInterrupt.toByteArray());

			ui->doMessageProcessing(msg);
			QCOMPARE(spyUi.count(), 1);
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), handleInterruptExpected);

			const QByteArray msgCancel(R"({"cmd": "CANCEL"})");
			ui->doMessageProcessing(msgCancel);
			QTRY_COMPARE(spyFinished.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), false); // default
		}


		void handleInterruptDefault_data()
		{
			QTest::addColumn<bool>("handleInterruptExpected");
			QTest::addColumn<char>("apiLevel");

			QTest::newRow("v1") << true << '1';
			QTest::newRow("v2") << false << '2';
		}


		void handleInterruptDefault()
		{
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			QFETCH(bool, handleInterruptExpected);
			QFETCH(char, apiLevel);

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), false); // default

			auto ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(ui);
			ui->setEnabled(true);
			QSignalSpy spyMessage(ui, &UIPlugInJson::fireMessage);
			QSignalSpy spyUi(ui, &UIPlugIn::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](QSharedPointer<WorkflowContext> pContext){
					pContext->claim(this); // UIPlugInJson is internal API and does not claim by itself
				});

			QByteArray msgApiLevel = R"({"cmd": "SET_API_LEVEL", "level": *})";
			ui->doMessageProcessing(msgApiLevel.replace('*', apiLevel));
			QTRY_COMPARE(spyMessage.count(), 1); // clazy:exclude=qstring-allocations
			QByteArray msgApiLevelResponse = R"({"current":*,"msg":"API_LEVEL"})";
			QCOMPARE(spyMessage.takeFirst().at(0).toByteArray(), msgApiLevelResponse.replace('*', apiLevel));

			const QByteArray msg(R"({"cmd": "RUN_CHANGE_PIN"})");
			ui->doMessageProcessing(msg);
			QCOMPARE(spyUi.count(), 1);
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), handleInterruptExpected);

			const QByteArray msgCancel(R"({"cmd": "CANCEL"})");
			ui->doMessageProcessing(msgCancel);
			QTRY_COMPARE(spyFinished.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), false); // default
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerChangePin)
#include "test_MsgHandlerChangePin.moc"
