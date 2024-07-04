/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "controller/AuthController.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_AuthController
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void createWorkflowRequest()
		{
			const QUrl url("https://www.test.de"_L1);
			const QVariant data(42);
			const AuthContext::BrowserHandler handler = [](const QSharedPointer<AuthContext>&){
						return QStringLiteral("HelloWorld");
					};
			const auto& request = AuthController::createWorkflowRequest(url, data, handler);

			const auto& context = request->getContext().objectCast<AuthContext>();
			QVERIFY(context);
			QVERIFY(context->isActivateUi());
			QCOMPARE(context->getActivationUrl(), url);
			QCOMPARE(context->getBrowserHandler()(context), QStringLiteral("HelloWorld"));
			QCOMPARE(request->getData().value<int>(), 42);
			QCOMPARE(context->isSkipMobileRedirect(), false);

			context->setWorkflowFinished(true);
			const auto handling = request->handleBusyWorkflow(request, QSharedPointer<WorkflowRequest>());
			QCOMPARE(handling, WorkflowControl::ENQUEUE);
			QCOMPARE(context->isStateApproved(), true);
			QVERIFY(context->isSkipMobileRedirect());
		}


};

QTEST_GUILESS_MAIN(test_AuthController)
#include "test_AuthController.moc"
