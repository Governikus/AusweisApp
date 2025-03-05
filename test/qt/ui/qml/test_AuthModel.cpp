/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AuthModel.h"

#include "context/AuthContext.h"
#include "context/SelfAuthContext.h"

#include "paos/retrieve/DidAuthenticateEac1Parser.h"

#include "TestFileHelper.h"

#include <QDebug>
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

			QByteArray content = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1_htmlTransactionInfo.xml"_L1);
			QSharedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(content)));
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
			QCOMPARE(model->getErrorHeader(), QStringLiteral("https://www.governikus.de"));
		}


		void test_getErrorText_data()
		{
			QTest::addColumn<QSharedPointer<AuthContext>>("context");
			QTest::addColumn<GlobalStatus>("status");
			QTest::addColumn<QString>("result");

			const GlobalStatus::ExternalInfoMap infoMap {
				{GlobalStatus::ExternalInformation::LAST_URL, "https://www.governikus.de"_L1}
			};

			QTest::addRow("No context") << QSharedPointer<AuthContext>(nullptr) << GlobalStatus(GlobalStatus::Code::No_Error) << "";
			QTest::addRow("No error") << QSharedPointer<AuthContext>::create() << GlobalStatus(GlobalStatus::Code::No_Error) << "No error occurred.";

			QTest::addRow("Any error - No info - No reason") << QSharedPointer<AuthContext>::create() << GlobalStatus(GlobalStatus::Code::Card_Communication_Error) << "An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.";
			QTest::addRow("Any error - Info - No reason") << QSharedPointer<AuthContext>::create() << GlobalStatus(GlobalStatus::Code::Card_Communication_Error, infoMap) << "An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.<br/>(https://www.governikus.de)";

			const auto context = QSharedPointer<AuthContext>::create();
			context->setFailureCode(FailureCode::Reason::Card_Removed);
			QTest::addRow("Any error - No info - Reason") << context << GlobalStatus(GlobalStatus::Code::Card_Communication_Error) << "An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.<br/><br/>Reason:<br/><b>Card_Removed</b>";
			QTest::addRow("Any error - Info - Reason") << context << GlobalStatus(GlobalStatus::Code::Card_Communication_Error, infoMap) << "An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.<br/>(https://www.governikus.de)<br/><br/>Reason:<br/><b>Card_Removed</b>";
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


		void test_getStatusCodeString_data()
		{
			QTest::addColumn<QSharedPointer<AuthContext>>("context");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<QString>("result");

			QTest::addRow("No context") << QSharedPointer<AuthContext>(nullptr) << GlobalStatus::Code::No_Error << "Unknown_Error";
			QTest::addRow("Any error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::Card_Communication_Error << "Card_Communication_Error";
			QTest::addRow("No error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::No_Error << "No_Error";
		}


		void test_getStatusCodeString()
		{
			QFETCH(QSharedPointer<AuthContext>, context);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(QString, result);

			auto* const model = Env::getSingleton<AuthModel>();
			if (context)
			{
				context->setStatus(GlobalStatus(statusCode));
			}
			model->resetAuthContext(context);

			QCOMPARE(model->getStatusCodeString(), result);
		}


		void test_resultViewButtonIcon_data()
		{
			QTest::addColumn<QSharedPointer<AuthContext>>("context");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<QString>("buttonIcon");

			QTest::addRow("No context") << QSharedPointer<AuthContext>(nullptr) << GlobalStatus::Code::No_Error << "";
			QTest::addRow("Any error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::Card_Communication_Error << "";
			QTest::addRow("No error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::No_Error << "";
			QTest::addRow("Browser_Transmission_Error") << QSharedPointer<AuthContext>::create() << GlobalStatus::Code::Workflow_Browser_Transmission_Error << "qrc:///images/open_website.svg";
		}


		void test_resultViewButtonIcon()
		{
			QFETCH(QSharedPointer<AuthContext>, context);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(QString, buttonIcon);

			auto* const model = Env::getSingleton<AuthModel>();
			if (context)
			{
				context->setStatus(GlobalStatus(statusCode));
			}
			model->resetAuthContext(context);

			QCOMPARE(model->getResultViewButtonIcon(), buttonIcon);
		}


		void test_resultViewButtonText_data()
		{
			QTest::addColumn<QSharedPointer<AuthContext>>("context");
			QTest::addColumn<QString>("buttonText");

			QTest::addRow("No context") << QSharedPointer<AuthContext>(nullptr) << "";
			QTest::addRow("Auth") << QSharedPointer<AuthContext>(new AuthContext()) << "Return to provider";
			QTest::addRow("SelfAuth") << QSharedPointer<AuthContext>(new SelfAuthContext()) << "Back to start page";
		}


		void test_resultViewButtonText()
		{
			QFETCH(QSharedPointer<AuthContext>, context);
			QFETCH(QString, buttonText);

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


};

QTEST_MAIN(test_AuthModel)
#include "test_AuthModel.moc"
