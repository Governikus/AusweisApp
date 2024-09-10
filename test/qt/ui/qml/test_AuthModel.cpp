/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref AuthModel
 */

#include "AuthModel.h"

#include "context/AuthContext.h"

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
			const auto model = Env::getSingleton<AuthModel>();
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


		void test_resultHeader_data()
		{
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<QString>("resultHeader");

			QTest::addRow("Any error") << GlobalStatus::Code::Card_Communication_Error << "Authentication failed";
			QTest::addRow("No error") << GlobalStatus::Code::No_Error << "Authentication successful";
			QTest::addRow("Browser_Transmission_Error") << GlobalStatus::Code::Workflow_Browser_Transmission_Error << "Redirect failed";
		}


		void test_resultHeader()
		{
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(QString, resultHeader);

			const auto model = Env::getSingleton<AuthModel>();
			const QSharedPointer<AuthContext> context(new AuthContext());
			context->setStatus(GlobalStatus(statusCode));
			model->resetAuthContext(context);

			QCOMPARE(model->getResultHeader(), resultHeader);
		}


		void test_resultViewButtonIcon_data()
		{
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<QString>("buttonIcon");

			QTest::addRow("Any error") << GlobalStatus::Code::Card_Communication_Error << "";
			QTest::addRow("No error") << GlobalStatus::Code::No_Error << "";
			QTest::addRow("Browser_Transmission_Error") << GlobalStatus::Code::Workflow_Browser_Transmission_Error << "qrc:///images/open_website.svg";
		}


		void test_resultViewButtonIcon()
		{
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(QString, buttonIcon);

			const auto model = Env::getSingleton<AuthModel>();
			const QSharedPointer<AuthContext> context(new AuthContext());
			context->setStatus(GlobalStatus(statusCode));
			model->resetAuthContext(context);

			QCOMPARE(model->getResultViewButtonIcon(), buttonIcon);
		}


		void test_resultViewButtonText_data()
		{
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<QString>("buttonText");

			QTest::addRow("Any error") << GlobalStatus::Code::Card_Communication_Error << "Back to start page";
			QTest::addRow("No error") << GlobalStatus::Code::No_Error << "Back to start page";
			QTest::addRow("Browser_Transmission_Error") << GlobalStatus::Code::Workflow_Browser_Transmission_Error << "Back to provider";
		}


		void test_resultViewButtonText()
		{
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(QString, buttonText);

			const auto model = Env::getSingleton<AuthModel>();
			const QSharedPointer<AuthContext> context(new AuthContext());
			context->setStatus(GlobalStatus(statusCode));
			model->resetAuthContext(context);

			QCOMPARE(model->getResultViewButtonText(), buttonText);
		}


		void test_resultViewButtonLink_data()
		{
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<QUrl>("refreshUrl");
			QTest::addColumn<QUrl>("buttonLink");

			const auto& refreshUrl = QUrl("https://dummy.url"_L1);
			QTest::addRow("Any error") << GlobalStatus::Code::Card_Communication_Error << refreshUrl << QUrl();
			QTest::addRow("No error") << GlobalStatus::Code::No_Error << refreshUrl << QUrl();
			QTest::addRow("Browser_Transmission_Error") << GlobalStatus::Code::Workflow_Browser_Transmission_Error << refreshUrl << refreshUrl;
		}


		void test_resultViewButtonLink()
		{
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(QUrl, refreshUrl);
			QFETCH(QUrl, buttonLink);

			const auto model = Env::getSingleton<AuthModel>();
			const QSharedPointer<AuthContext> context(new AuthContext());
			context->setStatus(GlobalStatus(statusCode));
			context->setRefreshUrl(refreshUrl);
			model->resetAuthContext(context);

			QCOMPARE(model->getResultViewButtonLink(), buttonLink);
		}


};

QTEST_MAIN(test_AuthModel)
#include "test_AuthModel.moc"
