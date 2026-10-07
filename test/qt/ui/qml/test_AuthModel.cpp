/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#include "AuthModel.h"

#include "AppSettings.h"
#include "context/AuthContext.h"
#include "context/SelfAuthContext.h"
#include "paos/retrieve/PaosParser.h"

#include "TestParserHelper.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_AuthModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_ResetContext()
		{
			auto* const model = Env::getSingleton<AuthModel>();
			const QSharedPointer<AuthContext> context(new AuthContext());

			QSignalSpy spyWorkflowStarted(model, &WorkflowModel::fireWorkflowStarted);
			QSignalSpy spyCurrentStateChanged(model, &WorkflowModel::fireCurrentStateChanged);
			QSignalSpy spyStateEntered(model, &WorkflowModel::fireStateEntered);
			QSignalSpy spyTransactionInfoChanged(model, &AuthModel::fireTransactionInfoChanged);

			model->resetAuthContext(context);
			QCOMPARE(spyWorkflowStarted.count(), 1);
			QCOMPARE(spyCurrentStateChanged.count(), 1);
			QCOMPARE(spyStateEntered.count(), 0);
			QCOMPARE(spyTransactionInfoChanged.count(), 0);

			const auto& parser = TestParserHelper::create(":/paos/DIDAuthenticateEAC1_htmlTransactionInfo.xml"_L1);
			auto* pm = PaosParser().parse(parser).release();
			QSharedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(pm));

			context->setDidAuthenticateEac1(eac1);
			QCOMPARE(model->getTransactionInfo(), "this is a &lt;a&gt;test&lt;/a&gt; for TransactionInfo"_L1);
			model->resetAuthContext(context);
			QVERIFY(model->getTransactionInfo().isEmpty());
			Q_EMIT context->fireDidAuthenticateEac1Changed();
			QCOMPARE(model->getTransactionInfo(), "this is a &lt;a&gt;test&lt;/a&gt; for TransactionInfo"_L1);
			QCOMPARE(spyWorkflowStarted.count(), 2);
			QCOMPARE(spyCurrentStateChanged.count(), 2);
			QCOMPARE(spyStateEntered.count(), 0);
			QCOMPARE(spyTransactionInfoChanged.count(), 3);
		}


		void test_progressValue()
		{
			auto* const model = Env::getSingleton<AuthModel>();

			model->resetAuthContext(nullptr);
			QCOMPARE(model->getProgressValue(), 0);

			const QSharedPointer<AuthContext> context(new AuthContext());
			context->setProgress(50, QString());
			model->resetAuthContext(context);
			QCOMPARE(model->getProgressValue(), 50);
		}


		void test_progressMessage()
		{
			auto* const model = Env::getSingleton<AuthModel>();

			model->resetAuthContext(nullptr);
			QCOMPARE(model->getProgressMessage(), QString());

			const QSharedPointer<AuthContext> context(new AuthContext());
			context->setProgress(0, QStringLiteral("TEST"));
			model->resetAuthContext(context);
			QCOMPARE(model->getProgressMessage(), QStringLiteral("TEST"));
		}


		void test_changeTransportPin()
		{
			auto* const model = Env::getSingleton<AuthModel>();

			model->resetAuthContext(nullptr);
			QCOMPARE(model->getChangeTransportPin(), false);

			const QSharedPointer<AuthContext> context(new AuthContext());
			context->requestChangeTransportPin();
			model->resetAuthContext(context);
			QCOMPARE(model->getChangeTransportPin(), true);
		}


		void test_resultHeader_data()
		{
			QTest::addColumn<QSharedPointer<AuthContext>>("context");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<QString>("resultHeader");

			QTest::addRow("No context") << QSharedPointer<AuthContext>() << GlobalStatus::Code::No_Error << "";
			QTest::addRow("Any error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::Card_Communication_Error << "Authentication failed";
			QTest::addRow("No error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::No_Error << "Authentication successful";
			QTest::addRow("Browser_Transmission_Error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::Workflow_Browser_Transmission_Error << "Redirect failed";
		}


		void test_resultHeader()
		{
			QFETCH(QSharedPointer<AuthContext>, context);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(QString, resultHeader);

			auto* const model = Env::getSingleton<AuthModel>();
			if (context)
			{
				context->setStatus(GlobalStatus(statusCode));
			}
			model->resetAuthContext(context);

			QCOMPARE(model->getResultHeader(), resultHeader);
		}


		void test_getErrorHeader()
		{
			auto* const model = Env::getSingleton<AuthModel>();

			model->resetAuthContext(nullptr);
			QCOMPARE(model->getErrorHeader(), QString());

			const QSharedPointer<AuthContext> context(new AuthContext());
			model->resetAuthContext(context);
			QCOMPARE(model->getErrorHeader(), QString());

			context->setTcTokenUrl(QUrl("https://www.governikus.de/tcToken"_L1));
			QCOMPARE(model->getErrorHeader(), QStringLiteral("Provider: https://www.governikus.de"));

			context->setStatus(GlobalStatus::Code::Workflow_Card_Removed);
			QCOMPARE(model->getErrorHeader(), tr("Connection to ID card lost"));
		}


		void test_getErrorText_data()
		{
			QTest::addColumn<QSharedPointer<AuthContext>>("context");
			QTest::addColumn<GlobalStatus>("status");
			QTest::addColumn<QString>("result");

			const GlobalStatus::ExternalInfoMap infoMap {
				{GlobalStatus::ExternalInformation::LAST_URL, "https://www.governikus.de"_L1}
			};
			const GlobalStatus::ExternalInfoMap infoMapTags {
				{GlobalStatus::ExternalInformation::LAST_URL, "<a href=\"https://www.governikus.de\">https://www.governikus.de</a>"_L1}
			};

			QTest::addRow("No context") << QSharedPointer<AuthContext>(nullptr) << GlobalStatus(GlobalStatus::Code::No_Error) << "";
			QTest::addRow("No error") << QSharedPointer<AuthContext>::create() << GlobalStatus(GlobalStatus::Code::No_Error) << "No error occurred.";

			QTest::addRow("Any error - No info - No reason") << QSharedPointer<AuthContext>::create() << GlobalStatus(GlobalStatus::Code::Card_Communication_Error) << "An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.";
			QTest::addRow("Any error - Info - No reason") << QSharedPointer<AuthContext>::create() << GlobalStatus(GlobalStatus::Code::Card_Communication_Error, infoMap) << "An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.<br/>(https://www.governikus.de)";
			QTest::addRow("Any error - Info escaped - No reason") << QSharedPointer<AuthContext>::create() << GlobalStatus(GlobalStatus::Code::Card_Communication_Error, infoMapTags) << "An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.<br/>(&lt;a href=&quot;https://www.governikus.de&quot;&gt;https://www.governikus.de&lt;/a&gt;)";

			const auto context = QSharedPointer<AuthContext>::create();
			context->setFailureCode(FailureCode::Reason::Card_Removed);
			QTest::addRow("Any error - No info - Reason") << context << GlobalStatus(GlobalStatus::Code::Card_Communication_Error) << "An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.<br/><br/>Reason:<br/><b>Card_Removed</b>";
			QTest::addRow("Any error - Info - Reason") << context << GlobalStatus(GlobalStatus::Code::Card_Communication_Error, infoMap) << "An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.<br/>(https://www.governikus.de)<br/><br/>Reason:<br/><b>Card_Removed</b>";
			QTest::addRow("Any error - Info escaped - reason") << context << GlobalStatus(GlobalStatus::Code::Card_Communication_Error, infoMapTags) << "An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.<br/>(&lt;a href=&quot;https://www.governikus.de&quot;&gt;https://www.governikus.de&lt;/a&gt;)<br/><br/>Reason:<br/><b>Card_Removed</b>";
		}


		void test_getErrorText()
		{
			QFETCH(QSharedPointer<AuthContext>, context);
			QFETCH(GlobalStatus, status);
			QFETCH(QString, result);

			auto* const model = Env::getSingleton<AuthModel>();
			if (context)
			{
				context->setStatus(status);
			}
			model->resetAuthContext(context);

			QCOMPARE(model->getErrorText(), result);
		}


		void test_resultViewButtonIcon_data()
		{
			QTest::addColumn<QSharedPointer<AuthContext>>("context");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<QString>("buttonIcon");
			QTest::addColumn<QUrl>("refreshUrl");

			QTest::addRow("No context") << QSharedPointer<AuthContext>(nullptr) << GlobalStatus::Code::No_Error << "" << QUrl(QStringLiteral("not_empty"));
			QTest::addRow("Any error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::Card_Communication_Error << "" << QUrl();
			QTest::addRow("No error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::No_Error << "" << QUrl(QStringLiteral("not_empty"));
			QTest::addRow("Browser_Transmission_Error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::Workflow_Browser_Transmission_Error << "qrc:///images/open_website.svg" << QUrl(QStringLiteral("not_empty"));
		}


		void test_resultViewButtonIcon()
		{
			QFETCH(QSharedPointer<AuthContext>, context);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(QString, buttonIcon);
			QFETCH(QUrl, refreshUrl);

			auto* const model = Env::getSingleton<AuthModel>();
			if (context)
			{
				context->setStatus(GlobalStatus(statusCode));
				context->setRefreshUrl(refreshUrl);
			}
			model->resetAuthContext(context);

			QCOMPARE(model->getResultViewButtonIcon(), buttonIcon);
		}


		void test_resultViewButtonText_data()
		{
			QTest::addColumn<bool>("autoRedirect");
			QTest::addColumn<QSharedPointer<AuthContext>>("context");
			QTest::addColumn<QString>("buttonText");
			QTest::addColumn<QUrl>("refreshUrl");

			QTest::addRow("No context (auto)") << true << QSharedPointer<AuthContext>(nullptr) << "" << QUrl();
			QTest::addRow("No context (manual)") << false << QSharedPointer<AuthContext>(nullptr) << "" << QUrl();
			QTest::addRow("Auth no refresh Url (auto)") << true << QSharedPointer<AuthContext>(new AuthContext()) << "Back to start page" << QUrl();
			QTest::addRow("Auth no refresh Url (manual)") << false << QSharedPointer<AuthContext>(new AuthContext()) << "Back to start page" << QUrl();
			QTest::addRow("Auth with refresh Url (auto)") << true << QSharedPointer<AuthContext>(new AuthContext()) << "Back to start page" << QUrl(QStringLiteral("not_empty"));
			QTest::addRow("Auth with refresh Url (manual)") << false << QSharedPointer<AuthContext>(new AuthContext()) << "Back to provider" << QUrl(QStringLiteral("not_empty"));
			QTest::addRow("SelfAuth (auto)") << true << QSharedPointer<AuthContext>(new SelfAuthContext()) << "Back to start page" << QUrl();
			QTest::addRow("SelfAuth (manual)") << false << QSharedPointer<AuthContext>(new SelfAuthContext()) << "Back to start page" << QUrl();
		}


		void test_resultViewButtonText()
		{
			QFETCH(bool, autoRedirect);
			QFETCH(QSharedPointer<AuthContext>, context);
			QFETCH(QString, buttonText);
			QFETCH(QUrl, refreshUrl);

			Env::getSingleton<AppSettings>()->getGeneralSettings().setAutoRedirectAfterAuthentication(autoRedirect);
			if (context)
			{
				context->setRefreshUrl(refreshUrl);
			}

			auto* const model = Env::getSingleton<AuthModel>();
			model->resetAuthContext(context);

			QCOMPARE(model->getResultViewButtonText(), buttonText);
		}


		void test_resultViewButtonLink_data()
		{
			QTest::addColumn<QSharedPointer<AuthContext>>("context");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<bool>("receivedBrowserSendFail");
			QTest::addColumn<QUrl>("refreshUrl");
			QTest::addColumn<QUrl>("buttonLink");

			const auto& refreshUrl = QUrl("https://dummy.url"_L1);

			QTest::addRow("No context") << QSharedPointer<AuthContext>(nullptr) << GlobalStatus::Code::No_Error << false << refreshUrl << QUrl();
			QTest::addRow("Any error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::Card_Communication_Error << false << refreshUrl << QUrl();
			QTest::addRow("No error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::No_Error << false << refreshUrl << QUrl();
			QTest::addRow("Browser_Send_Failed") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::No_Error << true << refreshUrl << refreshUrl;
		}


		void test_resultViewButtonLink()
		{
			QFETCH(QSharedPointer<AuthContext>, context);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(bool, receivedBrowserSendFail);
			QFETCH(QUrl, refreshUrl);
			QFETCH(QUrl, buttonLink);

			auto* const model = Env::getSingleton<AuthModel>();
			if (context)
			{
				context->setReceivedBrowserSendFailed(receivedBrowserSendFail);
				context->setStatus(GlobalStatus(statusCode));
				context->setRefreshUrl(refreshUrl);
			}
			model->resetAuthContext(context);

			QCOMPARE(model->getResultViewButtonLink(), buttonLink);
		}


		void test_cancelWorkflowToQuit()
		{
			auto* const model = Env::getSingleton<AuthModel>();
			QSignalSpy spy(model, &AuthModel::fireAutoFinishBeforeQuitChanged);

			model->resetAuthContext(nullptr);
			QCOMPARE(model->getAutoFinishBeforeQuit(), false);

			model->cancelWorkflowToQuit();
			QCOMPARE(spy.count(), 0);
			QCOMPARE(model->getAutoFinishBeforeQuit(), false);

			const QSharedPointer<AuthContext> context(new AuthContext());
			model->resetAuthContext(context);
			QCOMPARE(model->getAutoFinishBeforeQuit(), false);

			model->cancelWorkflowToQuit();
			QCOMPARE(spy.count(), 1);
			QCOMPARE(model->getAutoFinishBeforeQuit(), true);
		}


		void test_getRefreshUrl_data()
		{
			QTest::addColumn<QUrl>("refreshUrl");

			QTest::addRow("Empty URL") << QUrl(""_L1);
			QTest::addRow("Non-empty URL") << QUrl("https://www.foo.bar?ResultMajor=ok"_L1);
		}


		void test_getRefreshUrl()
		{
			QFETCH(QUrl, refreshUrl);

			auto context = QSharedPointer<AuthContext>::create();
			context->setRefreshUrl(refreshUrl);

			auto* const model = Env::getSingleton<AuthModel>();
			model->resetAuthContext(context);

			QCOMPARE(model->getRefreshUrl(), refreshUrl);
		}


};

QTEST_MAIN(test_AuthModel)
#include "test_AuthModel.moc"
