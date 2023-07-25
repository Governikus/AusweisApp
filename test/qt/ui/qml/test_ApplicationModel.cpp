/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ApplicationModel
 */

#include "ApplicationModel.h"

#include "MockActivationContext.h"
#include "MockIfdServer.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/IfdServiceContext.h"
#include "context/SelfAuthContext.h"
#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

#include <QtTest>

using namespace governikus;

class test_ApplicationModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_stripHtmlTags_data()
		{
			QTest::addColumn<QString>("rawText");
			QTest::addColumn<QString>("result");

			QTest::addRow("empty text") << QString() << QString();
			QTest::addRow("without tags") << QStringLiteral("Hello World!") << QStringLiteral("Hello World!");
			QTest::addRow("with tags") << QStringLiteral("<p>Hello World!</p>") << QStringLiteral("Hello World!");
			QTest::addRow("with <br> tags") << QStringLiteral("<p>Hello<br>World!</p>") << QStringLiteral("Hello World!");
			QTest::addRow("with multiple <br> tags") << QStringLiteral("<p>Hello<br>World!<br><br>How are you?</p>") << QStringLiteral("Hello World! How are you?");
			QTest::addRow("variaty of tags") << QStringLiteral("<h2><a href=\"https://www.example.org\">Hello</a> <marquee>World</marquee></h2><b>!</b>") << QStringLiteral("Hello World!");
		}


		void test_stripHtmlTags()
		{
			QFETCH(QString, rawText);
			QFETCH(QString, result);

			const auto model = Env::getSingleton<ApplicationModel>();
			QCOMPARE(model->stripHtmlTags(rawText), result);
		}


		void test_getCurrentWorkflow_data()
		{
			QTest::addColumn<QSharedPointer<WorkflowContext>>("context");
			QTest::addColumn<ApplicationModel::Workflow>("workflow");

			QTest::addRow("No Context") << QSharedPointer<WorkflowContext>() << ApplicationModel::Workflow::WORKFLOW_NONE;
			QTest::addRow("AuthContext") << QSharedPointer<WorkflowContext>(new AuthContext(QSharedPointer<MockActivationContext>::create())) << ApplicationModel::Workflow::WORKFLOW_AUTHENTICATION;
			QTest::addRow("ChangePinContext") << QSharedPointer<WorkflowContext>(new ChangePinContext()) << ApplicationModel::Workflow::WORKFLOW_CHANGE_PIN;
			QTest::addRow("IfdServiceContext") << QSharedPointer<WorkflowContext>(new IfdServiceContext(QSharedPointer<MockIfdServer>::create())) << ApplicationModel::Workflow::WORKFLOW_REMOTE_SERVICE;
			QTest::addRow("SelfAuthContext") << QSharedPointer<WorkflowContext>(new SelfAuthContext()) << ApplicationModel::Workflow::WORKFLOW_SELF_AUTHENTICATION;
#if __has_include("context/PersonalizationContext.h")
			QTest::addRow("PersonalizationContext") << QSharedPointer<WorkflowContext>(new PersonalizationContext(QString())) << ApplicationModel::Workflow::WORKFLOW_SMART;
#endif
		}


		void test_getCurrentWorkflow()
		{
			QFETCH(QSharedPointer<WorkflowContext>, context);
			QFETCH(ApplicationModel::Workflow, workflow);

			auto model = Env::getSingleton<ApplicationModel>();
			model->resetContext(context);
			QCOMPARE(model->getCurrentWorkflow(), workflow);
		}


};

QTEST_MAIN(test_ApplicationModel)
#include "test_ApplicationModel.moc"
