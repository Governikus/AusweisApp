/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#include "messages/MsgHandlerAuth.h"

#include "AppController.h"
#include "AppSettings.h"
#include "LanguageLoader.h"
#include "MessageDispatcher.h"
#include "UiLoader.h"
#include "UiPluginJson.h"
#include "VolatileSettings.h"
#include "WorkflowRequest.h"
#include "states/StateGetTcToken.h"

#include "TestWorkflowContext.h"

#include <QSignalSpy>
#include <QtTest>


Q_IMPORT_PLUGIN(UiPluginJson)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(QSharedPointer<WorkflowRequest>)


class test_MsgHandlerAuth
	: public QObject
{
	Q_OBJECT

	private:
		QTemporaryDir mTranslationDir;

	private Q_SLOTS:
		void initTestCase()
		{
			qRegisterMetaType<QSharedPointer<WorkflowContext>>("QSharedPointer<WorkflowContext>");
			LanguageLoader::getInstance().setPath(mTranslationDir.path()); // avoid loaded translations

			connect(Env::getSingleton<UiLoader>(), &UiLoader::fireLoadedPlugin, this, [](UiPlugin* pUi){
						pUi->setProperty("passive", QVariant()); // fake active UI for AppController::start
					});
		}


		void cleanup()
		{
			auto* uiLoader = Env::getSingleton<UiLoader>();
			if (uiLoader->isLoaded())
			{
				QSignalSpy spyUi(uiLoader, &UiLoader::fireRemovedAllPlugins);
				uiLoader->shutdown();
				QTRY_COMPARE(spyUi.count(), 1); // clazy:exclude=qstring-allocations
			}
		}


		void undefined()
		{
			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"RUN_AUTH\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"tcTokenURL cannot be undefined\",\"msg\":\"AUTH\"}"));
		}


		void invalid()
		{
			MessageDispatcher dispatcher;
			QByteArray expected("{\"error\":\"Invalid tcTokenURL\",\"msg\":\"AUTH\"}");

			QByteArray msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": 2}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": null}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": true}";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void failed()
		{
			MessageDispatcher dispatcher;
			QByteArray expected("{\"error\":\"Validation of tcTokenURL failed\",\"msg\":\"AUTH\"}");

			QByteArray msg("{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"\"}");
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"crap\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"xyz://\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"file://\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);

			msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"https://\"}";
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void customHeader_data()
		{
			QTest::addColumn<QByteArray>("header");
			QTest::addColumn<QByteArray>("expectedError");
			QTest::addColumn<AuthContext::HeaderMap>("map");

			const AuthContext::HeaderMap emptyMap;

			// error
			const auto nonObject = "Header is not an object"_ba;
			QTest::newRow("string empty") << "\"\""_ba << nonObject << emptyMap;
			QTest::newRow("string empty whitespaces") << "\"      \""_ba << nonObject << emptyMap;
			QTest::newRow("string") << "\"dummy\""_ba << nonObject << emptyMap;
			QTest::newRow("integer") << "1"_ba << nonObject << emptyMap;
			QTest::newRow("double") << "1.2"_ba << nonObject << emptyMap;
			QTest::newRow("null") << "null"_ba << nonObject << emptyMap;
			QTest::newRow("bool") << "true"_ba << nonObject << emptyMap;
			QTest::newRow("array") << "[]"_ba << nonObject << emptyMap;

			const auto emptyString = "Key of header is empty"_ba;
			QTest::newRow("obj key empty") << "{\"\": \"dummy\"}"_ba << emptyString << emptyMap;
			QTest::newRow("obj key empty whitespaces") << "{\"        \": \"dummy\"}"_ba << emptyString << emptyMap;

			const auto nonString = "Value of header is not a string"_ba;
			QTest::newRow("obj integer") << "{\"dummy\": 1}"_ba << nonString << emptyMap;
			QTest::newRow("obj double") << "{\"dummy\": 1.2}"_ba << nonString << emptyMap;
			QTest::newRow("obj null") << "{\"dummy\": null}"_ba << nonString << emptyMap;
			QTest::newRow("obj bool") << "{\"dummy\": true}"_ba << nonString << emptyMap;
			QTest::newRow("obj array") << "{\"dummy\": []}"_ba << nonString << emptyMap;
			QTest::newRow("obj obj") << "{\"dummy\": {}}"_ba << nonString << emptyMap;
			QTest::newRow("obj str obj") << "{\"bla\": \"dummy\", \"dummy\": {}}"_ba << nonString << emptyMap;

			const auto emptyHeader = "Value of header is empty"_ba;
			QTest::newRow("obj string empty") << "{\"dummy\": \"\"}"_ba << emptyHeader << emptyMap;
			QTest::newRow("obj string empty whitespaces") << "{\"dummy\": \"    \"}"_ba << emptyHeader << emptyMap;

			// success
			const auto noError = ""_ba;
			QTest::newRow("empty obj") << "{}"_ba << noError << emptyMap;
			QTest::newRow("2 header") << "{\"bla1\":\"dummy1\",\"bla2\":\"dummy2\"}"_ba << noError << AuthContext::HeaderMap {
				{"bla1", "dummy1"},
				{"bla2", "dummy2"}
				};
		}


		void customHeader()
		{
			QFETCH(QByteArray, header);
			QFETCH(QByteArray, expectedError);
			QFETCH(AuthContext::HeaderMap, map);

			QVERIFY(Env::getSingleton<UiLoader>()->load<UiPluginJson>());
			auto jsonUi = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
			QVERIFY(jsonUi);
			QSignalSpy spy(jsonUi, &UiPlugin::fireWorkflowRequested);

			MessageDispatcher dispatcher;

			QByteArray msg(R"({"cmd": "RUN_AUTH", "tcTokenURL": "%1", "header": %2})");
			msg.replace("%1", "https://www.governikus.de").replace("%2"_ba, header);

			if (expectedError.isEmpty())
			{
				QCOMPARE(dispatcher.processCommand(msg), QByteArray());

				QCOMPARE(spy.count(), 1);
				auto param = spy.takeFirst();
				auto request = param.at(0).value<QSharedPointer<WorkflowRequest>>();
				QCOMPARE(request->getAction(), Action::AUTH);
				QVERIFY(request->getContext().objectCast<AuthContext>());
				QCOMPARE(request->getContext().objectCast<AuthContext>()->getCustomHeader(), map);
			}
			else
			{
				QByteArray expected(R"({"error":"%1","msg":"AUTH"})");
				expected.replace("%1"_ba, expectedError);

				QCOMPARE(dispatcher.processCommand(msg), expected);
				QCOMPARE(spy.count(), 0);
			}
		}


		void runAuthCmd()
		{
			QVERIFY(Env::getSingleton<UiLoader>()->load<UiPluginJson>());
			auto jsonUi = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
			QVERIFY(jsonUi);
			QSignalSpy spy(jsonUi, &UiPlugin::fireWorkflowRequested);

			MessageDispatcher dispatcher;
			QByteArray msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"https://www.governikus.de/token?session=123abc\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			auto request = param.at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getAction(), Action::AUTH);
			QVERIFY(request->getContext().objectCast<AuthContext>());
			QCOMPARE(request->getContext().objectCast<AuthContext>()->getActivationUrl(),
					QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Fwww.governikus.de%2Ftoken%3Fsession%3D123abc"_L1));
		}


		void initAuth()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), QByteArray());
			QCOMPARE(dispatcher.finish(), QByteArray());

			const QSharedPointer<AuthContext> authContext(new AuthContext(true, QUrl("http://dummy"_L1)));
			QCOMPARE(dispatcher.init(authContext), QByteArray("{\"msg\":\"AUTH\"}"));
		}


		void handleStatus_data()
		{
			QTest::addColumn<QVariant>("handleStatus");
			QTest::addColumn<int>("statusMessages");

			QTest::newRow("noStatus") << QVariant(false) << 0;
			QTest::newRow("Status") << QVariant(true) << 1; // StateGetTcToken
		}


		void handleStatus()
		{
			QVERIFY(!Env::getSingleton<UiLoader>()->isLoaded());

			QFETCH(QVariant, handleStatus);
			QFETCH(int, statusMessages);

			UiLoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();

			auto ui = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
			QVERIFY(ui);
			ui->setEnabled(true);
			ui->mMessageDispatcher.setSkipStateApprovedHook([](const QString& pState){
						return StateBuilder::isState<StateGetTcToken>(pState);
					});
			QSignalSpy spyUi(ui, &UiPlugin::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);
			static int firedStatusCount = 0;
			connect(&controller, &AppController::fireWorkflowStarted, this, [this, ui](const QSharedPointer<WorkflowRequest>& pRequest){
						const auto& context = pRequest->getContext();
						context->claim(this); // UiPluginJson is internal API and does not claim by itself
						connect(context.data(), &WorkflowContext::fireStateChanged, this, [ui](const QString& pState)
						{
							// do not CANCEL to early to get all STATUS messages and avoid flaky unit test
							if (StateBuilder::isState<StateGetTcToken>(pState))
							{
								const QByteArray msgCancel(R"({"cmd": "CANCEL"})");
								ui->doMessageProcessing(msgCancel);
							}
						});
					});
			connect(ui, &UiPluginJson::fireMessage, this, [](const QByteArray& pMsg){
						if (pMsg.contains(R"("msg":"STATUS")"))
						{
							++firedStatusCount;
						}
					});

			QByteArray msg("{"
						   "\"cmd\": \"RUN_AUTH\","
						   "\"tcTokenURL\": \"https://localhost/token?session=123abc\","
						   "\"status\": %REPLACE%"
						   "}");
			msg.replace("%REPLACE%", handleStatus.toByteArray());
			ui->doMessageProcessing(msg);

			QCOMPARE(spyUi.count(), 1);
			auto param = spyUi.takeFirst();
			auto request = param.at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getAction(), Action::AUTH);
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations
			QTRY_COMPARE(spyFinished.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(firedStatusCount, statusMessages);
		}


		void runEncoded()
		{
			QVERIFY(Env::getSingleton<UiLoader>()->load<UiPluginJson>());
			auto jsonUi = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
			QVERIFY(jsonUi);
			QSignalSpy spy(jsonUi, &UiPlugin::fireWorkflowRequested);

			MessageDispatcher dispatcher;
			QByteArray msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"https://test.governikus-eid.de/Autent-DemoApplication/api/eid/request\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			auto request = param.at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getAction(), Action::AUTH);
			QVERIFY(request->getContext().objectCast<AuthContext>());
			QCOMPARE(request->getContext().objectCast<AuthContext>()->getActivationUrl(),
					QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Ftest.governikus-eid.de%2FAutent-DemoApplication%2Fapi%2Feid%2Frequest"_L1));
		}


		void badState()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), MsgType::VOID);
			QByteArray expected("{\"error\":\"RUN_AUTH\",\"msg\":\"BAD_STATE\"}");

			QByteArray msg("{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"https://correctUrl\"}");
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


		void result()
		{
			const QSharedPointer<AuthContext> context(new AuthContext());
			context->setStatus(GlobalStatus::Code::No_Error);
			MsgHandlerAuth msg(context);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"AUTH\",\"result\":{\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\"}}"));
		}


		void resultWithUrl()
		{
			const QSharedPointer<AuthContext> context(new AuthContext());
			context->setStatus(GlobalStatus::Code::No_Error);
			context->setRefreshUrl(QUrl("http://www.governikus.de"_L1));
			MsgHandlerAuth msg(context);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"AUTH\",\"result\":{\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\"},\"url\":\"http://www.governikus.de\"}"));
		}


		void resultWithErrorUrl()
		{
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

			const QSharedPointer<AuthContext> context(new AuthContext());
			context->setStatus(GlobalStatus::Code::Workflow_Reader_Became_Inaccessible);
			context->setTcToken(token);
			MsgHandlerAuth msg(context);
			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"AUTH\","
											  "\"result\":{\"description\":\"A Communication error occurred during processing.\","
											  "\"language\":\"en\","
											  "\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\","
											  "\"message\":\"The selected card reader cannot be accessed anymore.\","
											  "\"minor\":\"http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#communicationError\"},"
											  "\"url\":\"https://service.example.de/ComError?7eb39f62\"}"));
		}


		void iosScanDialogMessages()
		{
			QVERIFY(!Env::getSingleton<UiLoader>()->isLoaded());

			bool reachedStateGetTcToken = false;

			UiLoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest>& pRequest){
						pRequest->getContext()->claim(this); // UiPluginJson is internal API and does not claim by itself
					});

			const auto& initialMessages = Env::getSingleton<VolatileSettings>()->getMessages();
			QVERIFY(initialMessages.getSessionStarted().isNull());
			QVERIFY(initialMessages.getSessionInProgress().isNull());
			QVERIFY(initialMessages.getSessionSucceeded().isNull());
			QVERIFY(initialMessages.getSessionFailed().isEmpty());
			QVERIFY(!initialMessages.getSessionFailed().isNull());

			auto ui = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
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
			QSignalSpy spyUi(ui, &UiPlugin::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);

			MessageDispatcher dispatcher;

			const QByteArray msg("{"
								 "\"cmd\": \"RUN_AUTH\","
								 "\"tcTokenURL\": \"https://localhost/token?session=123abc\","
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
			QCOMPARE(request->getAction(), Action::AUTH);
			QVERIFY(request->getContext().objectCast<AuthContext>());
			QCOMPARE(request->getContext().objectCast<AuthContext>()->getActivationUrl(),
					QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Flocalhost%2Ftoken%3Fsession%3D123abc"_L1));
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
			QVERIFY(!Env::getSingleton<UiLoader>()->isLoaded());

			QFETCH(QVariant, handleInterrupt);
			QFETCH(bool, handleInterruptExpected);
			QFETCH(char, apiLevel);

			bool reachedStateGetTcToken = false;

			UiLoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest>& pRequest){
						pRequest->getContext()->claim(this); // UiPluginJson is internal API and does not claim by itself
					});

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), false); // default

			auto ui = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
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
			QSignalSpy spyMessage(ui, &UiPluginJson::fireMessage);
			QSignalSpy spyUi(ui, &UiPlugin::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);

			QByteArray msgApiLevel = R"({"cmd": "SET_API_LEVEL", "level": *})";
			ui->doMessageProcessing(msgApiLevel.replace('*', apiLevel));
			QTRY_COMPARE(spyMessage.count(), 1); // clazy:exclude=qstring-allocations
			QByteArray msgApiLevelResponse = R"({"current":*,"msg":"API_LEVEL"})";
			QCOMPARE(spyMessage.takeFirst().at(0).toByteArray(), msgApiLevelResponse.replace('*', apiLevel));

			QByteArray msg("{"
						   "\"cmd\": \"RUN_AUTH\","
						   "\"tcTokenURL\": \"https://localhost/token?session=123abc\","
						   "\"handleInterrupt\": %REPLACE%"
						   "}");
			msg.replace("%REPLACE%", handleInterrupt.toByteArray());
			ui->doMessageProcessing(msg);

			QCOMPARE(spyUi.count(), 1);
			auto param = spyUi.takeFirst();
			auto request = param.at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getAction(), Action::AUTH);
			QVERIFY(request->getContext().objectCast<AuthContext>());
			QCOMPARE(request->getContext().objectCast<AuthContext>()->getActivationUrl(),
					QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Flocalhost%2Ftoken%3Fsession%3D123abc"_L1));
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), handleInterruptExpected);

			QTRY_VERIFY(reachedStateGetTcToken); // clazy:exclude=qstring-allocations
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
			QVERIFY(!Env::getSingleton<UiLoader>()->isLoaded());

			QFETCH(bool, handleInterruptExpected);
			QFETCH(char, apiLevel);

			bool reachedStateGetTcToken = false;

			UiLoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest>& pRequest){
						pRequest->getContext()->claim(this); // UiPluginJson is internal API and does not claim by itself
					});

			QCOMPARE(Env::getSingleton<VolatileSettings>()->handleInterrupt(), false); // default

			auto ui = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
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
			QSignalSpy spyMessage(ui, &UiPluginJson::fireMessage);
			QSignalSpy spyUi(ui, &UiPlugin::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);

			QByteArray msgApiLevel = R"({"cmd": "SET_API_LEVEL", "level": *})";
			ui->doMessageProcessing(msgApiLevel.replace('*', apiLevel));
			QTRY_COMPARE(spyMessage.count(), 1); // clazy:exclude=qstring-allocations
			QByteArray msgApiLevelResponse = R"({"current":*,"msg":"API_LEVEL"})";
			QCOMPARE(spyMessage.takeFirst().at(0).toByteArray(), msgApiLevelResponse.replace('*', apiLevel));

			const QByteArray msg("{"
								 "\"cmd\": \"RUN_AUTH\","
								 "\"tcTokenURL\": \"https://localhost/token?session=123abc\""
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
		}


		void handleDeveloperMode_data()
		{
			QTest::addColumn<QVariant>("developerMode");

			QTest::newRow("enable") << QVariant(true);
			QTest::newRow("disable") << QVariant(false);
		}


		void handleDeveloperMode()
		{
			QVERIFY(!Env::getSingleton<UiLoader>()->isLoaded());

			QFETCH(QVariant, developerMode);

			bool reachedStateGetTcToken = false;

			UiLoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest>& pRequest){
						pRequest->getContext()->claim(this); // UiPluginJson is internal API and does not claim by itself
					});

			QCOMPARE(Env::getSingleton<VolatileSettings>()->isDeveloperMode(), false); // default
			QCOMPARE(Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode(), false);

			auto ui = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
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
			QSignalSpy spyUi(ui, &UiPlugin::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);

			MessageDispatcher dispatcher;

			QByteArray msg("{"
						   "\"cmd\": \"RUN_AUTH\","
						   "\"tcTokenURL\": \"https://localhost/token?session=123abc\","
						   "\"developerMode\": %REPLACE%"
						   "}");
			msg.replace("%REPLACE%", developerMode.toByteArray());

			const QRegularExpression logRegex(QStringLiteral("Using Developer Mode on SDK: ") + developerMode.toString());
			QTest::ignoreMessage(QtDebugMsg, logRegex);
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spyUi.count(), 1);
			auto workFlowRequest = spyUi.takeFirst().at(0).value<QSharedPointer<WorkflowRequest>>();
			QVERIFY(workFlowRequest);
			QCOMPARE(workFlowRequest->getAction(), Action::AUTH);
			auto authContext = workFlowRequest->getContext().objectCast<AuthContext>();
			auto url = authContext->getActivationUrl();
			QCOMPARE(url, QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Flocalhost%2Ftoken%3Fsession%3D123abc"_L1));
			QTRY_COMPARE(spyStarted.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<VolatileSettings>()->isDeveloperMode(), developerMode.toBool());
			QCOMPARE(Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode(), developerMode.toBool());

			QTRY_VERIFY(reachedStateGetTcToken); // clazy:exclude=qstring-allocations
			const QByteArray msgCancel(R"({"cmd": "CANCEL"})");
			ui->doMessageProcessing(msgCancel);

			QTRY_COMPARE(spyFinished.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(Env::getSingleton<VolatileSettings>()->isDeveloperMode(), false); // default
			QCOMPARE(Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode(), false);
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerAuth)
#include "test_MsgHandlerAuth.moc"
