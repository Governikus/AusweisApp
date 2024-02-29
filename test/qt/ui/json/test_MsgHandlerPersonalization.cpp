/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref MsgHandlerPersonalization
 */

#include "messages/MsgHandlerPersonalization.h"

#include "AppSettings.h"
#include "MessageDispatcher.h"
#include "ReaderManager.h"
#include "UILoader.h"
#include "UIPlugInJson.h"
#include "VolatileSettings.h"
#include "WorkflowRequest.h"
#include "controller/AppController.h"
#include "mock/eid_applet_interface_mock.h"
#include "states/StateGetTcToken.h"

#include "TestWorkflowContext.h"

#include <QSignalSpy>
#include <QtTest>


Q_IMPORT_PLUGIN(UIPlugInJson)
Q_IMPORT_PLUGIN(SmartReaderManagerPlugIn)


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(QSharedPointer<WorkflowRequest>)


class test_MsgHandlerPersonalization
	: public QObject
{
	Q_OBJECT

	private:
		bool mSmartAvailable = false;

	private Q_SLOTS:
		void initTestCase()
		{
			setSmartEidSupportStatus(EidSupportStatus::UP_TO_DATE);
			setSmartEidStatus(EidStatus::NO_PERSONALIZATION);
			qRegisterMetaType<QSharedPointer<WorkflowContext>>("QSharedPointer<WorkflowContext>");

			const auto& readerManager = Env::getSingleton<ReaderManager>();
			connect(readerManager, &ReaderManager::firePluginAdded, this, [this](const ReaderManagerPlugInInfo& pInfo) {
					if (pInfo.getPlugInType() == ReaderManagerPlugInType::SMART && pInfo.isAvailable())
					{
						mSmartAvailable = true;
					}
				});

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


		void undefined()
		{
#if __has_include("context/PersonalizationContext.h")
			MessageDispatcher dispatcher;
			QByteArray msg(R"({"cmd": "RUN_PERSONALIZATION"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"appletServiceURL cannot be undefined","msg":"PERSONALIZATION"})"));
#endif
		}


		void invalid()
		{
#if __has_include("context/PersonalizationContext.h")
			MessageDispatcher dispatcher;
			QByteArray expected(R"({"error":"Invalid appletServiceURL","msg":"PERSONALIZATION"})");

			QByteArray msg = R"({"cmd": "RUN_PERSONALIZATION", "appletServiceURL": 2})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "RUN_PERSONALIZATION", "appletServiceURL": null})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "RUN_PERSONALIZATION", "appletServiceURL": true})";
			QCOMPARE(dispatcher.processCommand(msg), expected);
#endif
		}


		void failed()
		{
#if __has_include("context/PersonalizationContext.h")
			MessageDispatcher dispatcher;
			QByteArray expected(R"({"error":"Validation of appletServiceURL failed","msg":"PERSONALIZATION"})");

			QByteArray msg(R"({"cmd": "RUN_PERSONALIZATION", "appletServiceURL": ""})");
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "RUN_PERSONALIZATION", "appletServiceURL": "crap"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "RUN_PERSONALIZATION", "appletServiceURL": "xyz://"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "RUN_PERSONALIZATION", "appletServiceURL": "file://"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = R"({"cmd": "RUN_PERSONALIZATION", "appletServiceURL": "https://"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);
#endif
		}


		void runPersoCmd()
		{
			QVERIFY(Env::getSingleton<UILoader>()->load<UIPlugInJson>());
			auto jsonUi = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(jsonUi);
			QSignalSpy spy(jsonUi, &UIPlugIn::fireWorkflowRequested);

			MessageDispatcher dispatcher;
			QByteArray msg = R"({"cmd": "RUN_PERSONALIZATION", "appletServiceURL": "https://www.governikus.de/token/%1"})";

#if __has_include("context/PersonalizationContext.h")
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			auto request = param.at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getAction(), Action::PERSONALIZATION);
			QVERIFY(request->getContext().objectCast<PersonalizationContext>());
			QCOMPARE(request->getContext().objectCast<PersonalizationContext>()->getAppletServiceUrl(QStringLiteral("testMe")),
					QUrl("https://www.governikus.de/token/testMe"_L1));
#else

			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"error":"RUN_PERSONALIZATION","msg":"UNKNOWN_COMMAND"})"));
			QCOMPARE(spy.count(), 0);
#endif
		}


		void initPerso()
		{
#if __has_include("context/PersonalizationContext.h")
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), QByteArray());
			QCOMPARE(dispatcher.finish(), QByteArray());

			const auto& ctx = QSharedPointer<PersonalizationContext>::create(QString());
			QCOMPARE(dispatcher.init(ctx), QByteArray(R"({"msg":"PERSONALIZATION"})"));
#endif
		}


		void badState()
		{
#if __has_include("context/PersonalizationContext.h")
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);
			QByteArray expected(R"({"error":"RUN_PERSONALIZATION","msg":"BAD_STATE"})");

			QByteArray msg = R"({"cmd": "RUN_PERSONALIZATION", "appletServiceURL": "https://correctUrl"})";
			QCOMPARE(dispatcher.processCommand(msg), expected);
#endif
		}


		void result()
		{
#if __has_include("context/PersonalizationContext.h")
			const auto& ctx = QSharedPointer<PersonalizationContext>::create(QString());
			ctx->setStatus(GlobalStatus::Code::No_Error);
			ctx->setRefreshUrl(QUrl("http://www.governikus.de"_L1)); // not provided in PERSONALIZATION, like in AUTH

			MsgHandlerPersonalization msg(ctx);
			QByteArray response = R"({"msg":"PERSONALIZATION","result":{"major":"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"}})";
			QCOMPARE(msg.toJson(), response);
#endif
		}


		void resultWithErrorUrl()
		{
#if __has_include("context/PersonalizationContext.h")
			const QSharedPointer<TcToken> token(new TcToken(QByteArray("<?xml version=\"1.0\"?>\n"
																	   "<TCTokenType>\n"
																	   "	<ServerAddress/>\n"
																	   "	<SessionIdentifier/>\n"
																	   "	<RefreshAddress/>\n"
																	   "	<CommunicationErrorAddress>https://service.example.de/ComError?7eb39f62</CommunicationErrorAddress>\n"
																	   "	<Binding/>\n"
																	   "	<PathSecurity-Protocol/>\n"
																	   "	<PathSecurity-Parameters/>\n"
																	   "</TCTokenType>")));

			const auto& ctx = QSharedPointer<PersonalizationContext>::create(QString());
			ctx->setStatus(GlobalStatus::Code::Workflow_Reader_Became_Inaccessible);
			ctx->setTcToken(token);
			MsgHandlerPersonalization msg(ctx);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"PERSONALIZATION\","
											  "\"result\":{\"description\":\"A Communication error occurred during processing.\","
											  "\"language\":\"en\","
											  "\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\","
											  "\"message\":\"The selected card reader cannot be accessed anymore.\","
											  "\"minor\":\"http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#communicationError\"}}"));
#endif
		}


		void iosScanDialogMessages()
		{
			bool reachedStateGetTcToken = false;

#if __has_include("context/PersonalizationContext.h")
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest>& pRequest){
					pRequest->getContext()->claim(this); // UIPlugInJson is internal API and does not claim by itself
				});

			QTRY_VERIFY(mSmartAvailable); // clazy:exclude=qstring-allocations

			const auto& initialMessages = Env::getSingleton<VolatileSettings>()->getMessages();
			QVERIFY(initialMessages.getSessionStarted().isNull());
			QVERIFY(initialMessages.getSessionInProgress().isNull());
			QVERIFY(initialMessages.getSessionSucceeded().isNull());
			QVERIFY(initialMessages.getSessionFailed().isEmpty());
			QVERIFY(!initialMessages.getSessionFailed().isNull());

			auto ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(ui);
			ui->setEnabled(true);
			ui->mMessageDispatcher.setSkipStateApprovedHook([&reachedStateGetTcToken](const QString& pState){
					if (StateBuilder::isState<StateGetTcToken>(pState))
					{
						reachedStateGetTcToken = true;
						return true;
					}

					return false;
				});
			QSignalSpy spyUi(ui, &UIPlugIn::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);

			MessageDispatcher dispatcher;

			const QByteArray msg("{"
								 "\"cmd\": \"RUN_PERSONALIZATION\","
								 "\"appletServiceURL\": \"https://localhost/%1\","
								 "\"messages\": {"
								 "  \"sessionStarted\": \"start\","
								 "  \"sessionFailed\": \"stop failed\","
								 "  \"sessionSucceeded\": \"stop success\","
								 "  \"sessionInProgress\": \"progress\""
								 "  }"
								 "}");

			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spyUi.count(), 1);
			auto param = spyUi.takeFirst();
			auto request = param.at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getAction(), Action::PERSONALIZATION);
			QVERIFY(request->getContext().objectCast<PersonalizationContext>());
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations

			auto messages = Env::getSingleton<VolatileSettings>()->getMessages();
			QCOMPARE(messages.getSessionStarted(), QLatin1String("start"));
			QCOMPARE(messages.getSessionInProgress(), QLatin1String("progress"));
			QCOMPARE(messages.getSessionSucceeded(), QLatin1String("stop success"));
			QCOMPARE(messages.getSessionFailed(), QLatin1String("stop failed"));

			QTRY_VERIFY(reachedStateGetTcToken); // clazy:exclude=qstring-allocations
			const QByteArray msgCancel(R"({"cmd": "CANCEL"})");
			ui->doMessageProcessing(msgCancel);

			QTRY_COMPARE(spyFinished.count(), 1); // clazy:exclude=qstring-allocations
			messages = Env::getSingleton<VolatileSettings>()->getMessages();
			QVERIFY(messages.getSessionStarted().isNull());
			QVERIFY(messages.getSessionInProgress().isNull());
			QVERIFY(messages.getSessionSucceeded().isNull());
			QVERIFY(messages.getSessionFailed().isEmpty());
			QVERIFY(!messages.getSessionFailed().isNull());
#endif
		}


		void handleInterrupt_data()
		{
#if __has_include("context/PersonalizationContext.h")
			QTest::addColumn<QVariant>("handleInterrupt");
			QTest::addColumn<bool>("handleInterruptExpected");
			QTest::addColumn<char>("apiLevel");

			QTest::newRow("shouldStop_v1") << QVariant(true) << true << '1';
			QTest::newRow("shouldNotStop_v1") << QVariant(false) << false << '1';
			QTest::newRow("shouldNotStop_v2 requested") << QVariant(true) << false << '2';
			QTest::newRow("shouldNotStop_v2 not requested") << QVariant(false) << false << '2';
#endif
		}


		void handleInterrupt()
		{
#if __has_include("context/PersonalizationContext.h")
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			QFETCH(QVariant, handleInterrupt);
			QFETCH(bool, handleInterruptExpected);
			QFETCH(char, apiLevel);

			bool reachedStateGetTcToken = false;

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest>& pRequest){
					pRequest->getContext()->claim(this); // UIPlugInJson is internal API and does not claim by itself
				});

			QTRY_VERIFY(mSmartAvailable); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), false); // default

			auto ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(ui);
			ui->setEnabled(true);
			ui->mMessageDispatcher.setSkipStateApprovedHook([&reachedStateGetTcToken](const QString& pState){
					if (StateBuilder::isState<StateGetTcToken>(pState))
					{
						reachedStateGetTcToken = true;
						return true;
					}

					return false;
				});
			QSignalSpy spyMessage(ui, &UIPlugInJson::fireMessage);
			QSignalSpy spyUi(ui, &UIPlugIn::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);

			QByteArray msgApiLevel = R"({"cmd": "SET_API_LEVEL", "level": *})";
			ui->doMessageProcessing(msgApiLevel.replace('*', apiLevel));
			QTRY_COMPARE(spyMessage.count(), 1); // clazy:exclude=qstring-allocations
			QByteArray msgApiLevelResponse = R"({"current":*,"msg":"API_LEVEL"})";
			QCOMPARE(spyMessage.takeFirst().at(0).toByteArray(), msgApiLevelResponse.replace('*', apiLevel));

			QByteArray msg("{"
						   "\"cmd\": \"RUN_PERSONALIZATION\","
						   "\"appletServiceURL\": \"https://localhost/%1\","
						   "\"handleInterrupt\": %REPLACE%"
						   "}");
			msg.replace("%REPLACE%", handleInterrupt.toByteArray());
			ui->doMessageProcessing(msg);

			QCOMPARE(spyUi.count(), 1);
			auto param = spyUi.takeFirst();
			auto request = param.at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getAction(), Action::PERSONALIZATION);
			QVERIFY(request->getContext().objectCast<PersonalizationContext>());
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), handleInterruptExpected);

			QTRY_VERIFY(reachedStateGetTcToken); // clazy:exclude=qstring-allocations
			const QByteArray msgCancel(R"({"cmd": "CANCEL"})");
			ui->doMessageProcessing(msgCancel);

			QTRY_COMPARE(spyFinished.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), false); // default
#endif
		}


		void handleInterruptDefault_data()
		{
#if __has_include("context/PersonalizationContext.h")
			QTest::addColumn<bool>("handleInterruptExpected");
			QTest::addColumn<char>("apiLevel");

			QTest::newRow("v1") << true << '1';
			QTest::newRow("v2") << false << '2';
#endif
		}


		void handleInterruptDefault()
		{
#if __has_include("context/PersonalizationContext.h")
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			QFETCH(bool, handleInterruptExpected);
			QFETCH(char, apiLevel);

			bool reachedStateGetTcToken = false;

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest>& pRequest){
					pRequest->getContext()->claim(this); // UIPlugInJson is internal API and does not claim by itself
				});

			QTRY_VERIFY(mSmartAvailable); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), false); // default

			auto ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(ui);
			ui->setEnabled(true);
			ui->mMessageDispatcher.setSkipStateApprovedHook([&reachedStateGetTcToken](const QString& pState){
					if (StateBuilder::isState<StateGetTcToken>(pState))
					{
						reachedStateGetTcToken = true;
						return true;
					}

					return false;
				});
			QSignalSpy spyMessage(ui, &UIPlugInJson::fireMessage);
			QSignalSpy spyUi(ui, &UIPlugIn::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);

			QByteArray msgApiLevel = R"({"cmd": "SET_API_LEVEL", "level": *})";
			ui->doMessageProcessing(msgApiLevel.replace('*', apiLevel));
			QTRY_COMPARE(spyMessage.count(), 1); // clazy:exclude=qstring-allocations
			QByteArray msgApiLevelResponse = R"({"current":*,"msg":"API_LEVEL"})";
			QCOMPARE(spyMessage.takeFirst().at(0).toByteArray(), msgApiLevelResponse.replace('*', apiLevel));

			const QByteArray msg("{"
								 "\"cmd\": \"RUN_PERSONALIZATION\","
								 "\"appletServiceURL\": \"https://localhost/%1\""
								 "}");
			ui->doMessageProcessing(msg);
			QCOMPARE(spyUi.count(), 1);
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), handleInterruptExpected);

			QTRY_VERIFY(reachedStateGetTcToken); // clazy:exclude=qstring-allocations
			const QByteArray msgCancel(R"({"cmd": "CANCEL"})");
			ui->doMessageProcessing(msgCancel);

			QTRY_COMPARE(spyFinished.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), false); // default
#endif
		}


		void handleDeveloperMode_data()
		{
#if __has_include("context/PersonalizationContext.h")
			QTest::addColumn<QVariant>("developerMode");

			QTest::newRow("enable") << QVariant(true);
			QTest::newRow("disable") << QVariant(false);
#endif
		}


		void handleDeveloperMode()
		{
#if __has_include("context/PersonalizationContext.h")
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			QFETCH(QVariant, developerMode);

			bool reachedStateGetTcToken = false;

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest>& pRequest){
					pRequest->getContext()->claim(this); // UIPlugInJson is internal API and does not claim by itself
				});

			QTRY_VERIFY(mSmartAvailable); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->isDeveloperMode(), false); // default
			QCOMPARE(Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode(), false);

			auto ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(ui);
			ui->setEnabled(true);
			ui->mMessageDispatcher.setSkipStateApprovedHook([&reachedStateGetTcToken](const QString& pState){
					if (StateBuilder::isState<StateGetTcToken>(pState))
					{
						reachedStateGetTcToken = true;
						return true;
					}

					return false;
				});
			QSignalSpy spyUi(ui, &UIPlugIn::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);

			MessageDispatcher dispatcher;

			QByteArray msg("{"
						   "\"cmd\": \"RUN_PERSONALIZATION\","
						   "\"appletServiceURL\": \"https://localhost/%1\","
						   "\"developerMode\": %REPLACE%"
						   "}");
			msg.replace("%REPLACE%", developerMode.toByteArray());

			const QRegularExpression logRegex(QStringLiteral("Using Developer Mode on SDK: ") + developerMode.toString());
			QTest::ignoreMessage(QtDebugMsg, logRegex);
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spyUi.count(), 1);
			auto workFlowRequest = spyUi.takeFirst().at(0).value<QSharedPointer<WorkflowRequest>>();
			QVERIFY(workFlowRequest);
			QCOMPARE(workFlowRequest->getAction(), Action::PERSONALIZATION);
			QVERIFY(workFlowRequest->getContext().objectCast<PersonalizationContext>());
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->isDeveloperMode(), developerMode.toBool());
			QCOMPARE(Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode(), developerMode.toBool());

			QTRY_VERIFY(reachedStateGetTcToken); // clazy:exclude=qstring-allocations
			const QByteArray msgCancel(R"({"cmd": "CANCEL"})");
			ui->doMessageProcessing(msgCancel);

			QTRY_COMPARE(spyFinished.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(Env::getSingleton<VolatileSettings>()->isDeveloperMode(), false); // default
			QCOMPARE(Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode(), false);
#endif
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerPersonalization)
#include "test_MsgHandlerPersonalization.moc"
