/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref MsgHandlerAuth
 */

#include "messages/MsgHandlerAuth.h"

#include "AppSettings.h"
#include "LanguageLoader.h"
#include "MessageDispatcher.h"
#include "UILoader.h"
#include "UIPlugInJson.h"
#include "VolatileSettings.h"
#include "WorkflowRequest.h"
#include "controller/AppController.h"
#include "states/StateGetTcToken.h"

#include "TestWorkflowContext.h"

#include <QSignalSpy>
#include <QtTest>


Q_IMPORT_PLUGIN(UIPlugInJson)


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


		void runAuthCmd()
		{
			QVERIFY(Env::getSingleton<UILoader>()->load<UIPlugInJson>());
			auto jsonUi = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(jsonUi);
			QSignalSpy spy(jsonUi, &UIPlugIn::fireWorkflowRequested);

			MessageDispatcher dispatcher;
			QByteArray msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"https://www.governikus.de/token?session=123abc\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			auto request = param.at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getAction(), Action::AUTH);
			QVERIFY(request->getContext().objectCast<AuthContext>());
			QCOMPARE(request->getContext().objectCast<AuthContext>()->getActivationUrl(),
					QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Fwww.governikus.de%2Ftoken%3Fsession%3D123abc"));
		}


		void initAuth()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), QByteArray());
			QCOMPARE(dispatcher.finish(), QByteArray());

			const QSharedPointer<AuthContext> authContext(new AuthContext(true, QUrl("http://dummy")));
			QCOMPARE(dispatcher.init(authContext), QByteArray("{\"msg\":\"AUTH\"}"));
		}


		void handleStatus_data()
		{
			QTest::addColumn<QVariant>("handleStatus");
			QTest::addColumn<int>("statusMessages");

			QTest::newRow("noStatus") << QVariant(false) << 0;
			QTest::newRow("Status") << QVariant(true) << 2; // StateGetTcToken, StateCheckRefreshAddress
		}


		void handleStatus()
		{
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			QFETCH(QVariant, handleStatus);
			QFETCH(int, statusMessages);

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();

			auto ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(ui);
			ui->setEnabled(true);
			ui->mMessageDispatcher.setSkipStateApprovedHook([](const QString& pState){
					return StateBuilder::isState<StateGetTcToken>(pState);
				});
			QSignalSpy spyUi(ui, &UIPlugIn::fireWorkflowRequested);
			QSignalSpy spyStarted(&controller, &AppController::fireWorkflowStarted);
			QSignalSpy spyFinished(&controller, &AppController::fireWorkflowFinished);
			static int firedStatusCount = 0;
			connect(&controller, &AppController::fireWorkflowStarted, this, [this, ui](const QSharedPointer<WorkflowRequest>& pRequest){
					const auto& context = pRequest->getContext();
					context->claim(this); // UIPlugInJson is internal API and does not claim by itself
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
			connect(ui, &UIPlugInJson::fireMessage, this, [](const QByteArray& pMsg){
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
			QVERIFY(Env::getSingleton<UILoader>()->load<UIPlugInJson>());
			auto jsonUi = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
			QVERIFY(jsonUi);
			QSignalSpy spy(jsonUi, &UIPlugIn::fireWorkflowRequested);

			MessageDispatcher dispatcher;
			QByteArray msg = "{\"cmd\": \"RUN_AUTH\", \"tcTokenURL\": \"https://test.governikus-eid.de/gov_autent/async?SAMLRequest=nVbZsppIGH4Vi7m0lEURsI4nBbIcV1TA7SbF0myy0wj49NMeTXIySWYyueTvf%2FuWrublUxNHnSsoyiBNJhjZJ7AOSOzUCRJvghm63GOxT68vpRlHVDbmK%2BgnO5BXoIQdVJiU48fJBKuKZJyaZVCOEzMG5RjaY41fLcdUnxhnRQpTO42wDl%2BWoIBo1DRNyioGhQaKa2ADY7ecYD6EWTnG8bqu3X50jfsOwJPMvA%2FFgZ3Gn9HHPYAXoMxQPcA6IlokSMx7x2%2F1EAX7XopAJcGlKnsgcO6tUOSzWUGQQNws28TGOjNxgjG2NaBphu3RnEX1hiQDepwzGvVIa8S6nM3ZBHBRZllWYJaU0EzgBKMIctQj2B7J6iQ1HpBjiukTLHPGOpsivQYOKNaIg58Cwjr7L1wjZrAns%2BP3%2FsVHSv%2BdUfMLj9jrT1h7wd9VeXb9qp7UIPB3ncvXF8TJWErsos0gcD7K%2BjXpuQxKfABBwlhl0Leq5J1NMBNxtH6DzPKtkWhC81l3P%2FhaiBTt14N%2BWng4RRAkTgxx5B6U8RfW0dvsSdVD%2Bl8kShGIkXT%2FGInArAD0U6fDR15aBNCPf2OoCUqSYnu2hTzwcPHvbou%2FvjjleAHaWeKmz1qn%2FMVIAie4O06nDLy%2FfuAKNfmfw79n%2B8%2BgF6XZIz%2FTGALyLt00yHxQ3HX7LrA3owq8nqchd%2FChs84FcV%2F7K2XeNSNam7c1zXHmaE01PIgXcC8dVEkwOF9cgKS6Gjw7z%2Fcm301Gy4G2U3ldYZNjokPNXknswARRE1X1eWSDZvgGGUNwzhS6g1yrGsui1cBFSMJwF8Do1rihPLyZPH%2B7haovT4eaFrDbAcB3ah6G24xiVGFr5%2BUlOxlvuORmrFaQuzIHyvE257rT0Qpe9xqlVeAYWIGhioZiWWcoRnVIadR5YbORNmUAawRruznL%2FsUN8GCjjEA7mpmLnAqrZRU5i22Sd91An2blxp2R7IY6VLcisRwhJslgyKjEleEbMO9yF4m4bQapCs8jnWblM6WpFru02fDCevopWnteylL1ZPKCf%2BD%2BQfV3oYccj6SvlxT5BcW%2Bue8HAf%2FMTQ8DPJYQZXuh7SMTD8zuydusrZJf86WxyozjgobSkWfDkMn5S3d7mFJTXKWEkXuKdWKGFnXOQrbYJwodtcXONjyt686N5T4ZCOtpO4PeJiLo0PDieXdIzt11lzLndd4KjLqkY3kqH287U2svdeN6852480LvYm4vTEUXbN0FOKMUtroeXtdRGHOncHqMtZTJcvEircycDWNFy89VumWnqsTBvT7jo60ypUYbLTkt97nbGOvGJm1wahNDk5q3Q93qNVgufWV6s66GlTnbzDVxejiq7ThcNUqs3FyfVmVCYbeOElnXzU0JL7wtIefm1Wwv4%2FQZF%2Bp0mCa6tiVO8qI2Z7kx8KKteIipPN7dToNqFzCU7s9xRStX25wkV3SVpjK1sXTeL%2FRmPuAovBL5dsaTDCFJSnwaDfNhxXXdZq%2BLZ1fGU8GxBHxJweVVONRvvnySDkyOV0P1EOf8fHAEx5vPnzPLSvnB4dJGAhzGbJAJFJnvDFGo9gBuzduaetsf4PHmOJc6APybfPkzDz6N%2Bd%2BPB3Lq85fh46PzJfbxvXn9Gw%3D%3D&RelayState=fc4b5fad-76fd-49d2-841b-b5cd39568029&SigAlg=http%3A%2F%2Fwww.w3.org%2F2001%2F04%2Fxmldsig-more%23rsa-sha256&Signature=jEVAVmY1XMMuNt2GJ%2BoaN6FaGj6ACZokOfme1OmkBiCPJyr%2B4rsg%2BrajIyMtSoUbtOoOaH63g1JP%0AqT7Wc%2Bucd%2FLcEYf7qU%2FJKN2uTMvssQblsSKuPHOWLJQUKPZHiSPA67%2F9JZhov8FXYM9uBYSs1Far%0AMRBpM49o1m1pRzwOKx3%2FcwOqc%2BT5mOWM78tNOj37mFFzwDTGuoB9iE2cZCUu1wfBbTPIvRZ2ND8Y%0AVYEO5FdR8W0sLR9LNOVjm64D%2Fl9u%2FjFJXntW4CPRPXiOiCr7TLlnFWVxXOTwywF5LTyLYQR5OjwB%0AK3cWmRPkN6OAejN%2F3Su3nWHDlGttrZuZBYzfag%3D%3D\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			auto request = param.at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getAction(), Action::AUTH);
			QVERIFY(request->getContext().objectCast<AuthContext>());
			QCOMPARE(request->getContext().objectCast<AuthContext>()->getActivationUrl(),
					QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Ftest.governikus-eid.de%2Fgov_autent%2Fasync%3FSAMLRequest%3DnVbZsppIGH4Vi7m0lEURsI4nBbIcV1TA7SbF0myy0wj49NMeTXIySWYyueTvf%252FuWrublUxNHnSsoyiBNJhjZJ7AOSOzUCRJvghm63GOxT68vpRlHVDbmK%252BgnO5BXoIQdVJiU48fJBKuKZJyaZVCOEzMG5RjaY41fLcdUnxhnRQpTO42wDl%252BWoIBo1DRNyioGhQaKa2ADY7ecYD6EWTnG8bqu3X50jfsOwJPMvA%252FFgZ3Gn9HHPYAXoMxQPcA6IlokSMx7x2%252F1EAX7XopAJcGlKnsgcO6tUOSzWUGQQNws28TGOjNxgjG2NaBphu3RnEX1hiQDepwzGvVIa8S6nM3ZBHBRZllWYJaU0EzgBKMIctQj2B7J6iQ1HpBjiukTLHPGOpsivQYOKNaIg58Cwjr7L1wjZrAns%252BP3%252FsVHSv%252BdUfMLj9jrT1h7wd9VeXb9qp7UIPB3ncvXF8TJWErsos0gcD7K%252BjXpuQxKfABBwlhl0Leq5J1NMBNxtH6DzPKtkWhC81l3P%252FhaiBTt14N%252BWng4RRAkTgxx5B6U8RfW0dvsSdVD%252Bl8kShGIkXT%252FGInArAD0U6fDR15aBNCPf2OoCUqSYnu2hTzwcPHvbou%252FvjjleAHaWeKmz1qn%252FMVIAie4O06nDLy%252FfuAKNfmfw79n%252B8%252BgF6XZIz%252FTGALyLt00yHxQ3HX7LrA3owq8nqchd%252FChs84FcV%252F7K2XeNSNam7c1zXHmaE01PIgXcC8dVEkwOF9cgKS6Gjw7z%252Fcm301Gy4G2U3ldYZNjokPNXknswARRE1X1eWSDZvgGGUNwzhS6g1yrGsui1cBFSMJwF8Do1rihPLyZPH%252B7haovT4eaFrDbAcB3ah6G24xiVGFr5%252BUlOxlvuORmrFaQuzIHyvE257rT0Qpe9xqlVeAYWIGhioZiWWcoRnVIadR5YbORNmUAawRruznL%252FsUN8GCjjEA7mpmLnAqrZRU5i22Sd91An2blxp2R7IY6VLcisRwhJslgyKjEleEbMO9yF4m4bQapCs8jnWblM6WpFru02fDCevopWnteylL1ZPKCf%252BD%252BQfV3oYccj6SvlxT5BcW%252Bue8HAf%252FMTQ8DPJYQZXuh7SMTD8zuydusrZJf86WxyozjgobSkWfDkMn5S3d7mFJTXKWEkXuKdWKGFnXOQrbYJwodtcXONjyt686N5T4ZCOtpO4PeJiLo0PDieXdIzt11lzLndd4KjLqkY3kqH287U2svdeN6852480LvYm4vTEUXbN0FOKMUtroeXtdRGHOncHqMtZTJcvEircycDWNFy89VumWnqsTBvT7jo60ypUYbLTkt97nbGOvGJm1wahNDk5q3Q93qNVgufWV6s66GlTnbzDVxejiq7ThcNUqs3FyfVmVCYbeOElnXzU0JL7wtIefm1Wwv4%252FQZF%252Bp0mCa6tiVO8qI2Z7kx8KKteIipPN7dToNqFzCU7s9xRStX25wkV3SVpjK1sXTeL%252FRmPuAovBL5dsaTDCFJSnwaDfNhxXXdZq%252BLZ1fGU8GxBHxJweVVONRvvnySDkyOV0P1EOf8fHAEx5vPnzPLSvnB4dJGAhzGbJAJFJnvDFGo9gBuzduaetsf4PHmOJc6APybfPkzDz6N%252Bd%252BPB3Lq85fh46PzJfbxvXn9Gw%253D%253D%26RelayState%3Dfc4b5fad-76fd-49d2-841b-b5cd39568029%26SigAlg%3Dhttp%253A%252F%252Fwww.w3.org%252F2001%252F04%252Fxmldsig-more%2523rsa-sha256%26Signature%3DjEVAVmY1XMMuNt2GJ%252BoaN6FaGj6ACZokOfme1OmkBiCPJyr%252B4rsg%252BrajIyMtSoUbtOoOaH63g1JP%250AqT7Wc%252Bucd%252FLcEYf7qU%252FJKN2uTMvssQblsSKuPHOWLJQUKPZHiSPA67%252F9JZhov8FXYM9uBYSs1Far%250AMRBpM49o1m1pRzwOKx3%252FcwOqc%252BT5mOWM78tNOj37mFFzwDTGuoB9iE2cZCUu1wfBbTPIvRZ2ND8Y%250AVYEO5FdR8W0sLR9LNOVjm64D%252Fl9u%252FjFJXntW4CPRPXiOiCr7TLlnFWVxXOTwywF5LTyLYQR5OjwB%250AK3cWmRPkN6OAejN%252F3Su3nWHDlGttrZuZBYzfag%253D%253D"));
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
			context->setRefreshUrl(QUrl("http://www.governikus.de"));
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
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			bool reachedStateGetTcToken = false;

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest>& pRequest){
					pRequest->getContext()->claim(this); // UIPlugInJson is internal API and does not claim by itself
				});

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
					QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Flocalhost%2Ftoken%3Fsession%3D123abc"));
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
					QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Flocalhost%2Ftoken%3Fsession%3D123abc"));
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
			QVERIFY(!Env::getSingleton<UILoader>()->isLoaded());

			QFETCH(QVariant, developerMode);

			bool reachedStateGetTcToken = false;

			UILoader::setUserRequest({QStringLiteral("json")});
			AppController controller;
			controller.start();
			connect(&controller, &AppController::fireWorkflowStarted, this, [this](const QSharedPointer<WorkflowRequest>& pRequest){
					pRequest->getContext()->claim(this); // UIPlugInJson is internal API and does not claim by itself
				});

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
			QCOMPARE(url, QUrl("http://localhost/?tcTokenURL=https%3A%2F%2Flocalhost%2Ftoken%3Fsession%3D123abc"));
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
