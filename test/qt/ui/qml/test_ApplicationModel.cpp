/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include "MockIfdServer.h"
#include "MockReaderManagerPlugin.h"
#include "ReaderManager.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/IfdServiceContext.h"
#include "context/SelfAuthContext.h"

#include <QtTest>

using namespace governikus;

Q_IMPORT_PLUGIN(MockReaderManagerPlugin)

class test_ApplicationModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			QThread::currentThread()->setObjectName(QStringLiteral("MainThread"));

			auto* readerManager = Env::getSingleton<ReaderManager>();
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
			MockReaderManagerPlugin::getInstance().removeAllReader();
		}


		void test_getFeedbackTimeout()
		{
			QCOMPARE(ApplicationModel::getFeedbackTimeout(), 7000);
		}


		void test_stripHtmlTags_data()
		{
			QTest::addColumn<QString>("rawText");
			QTest::addColumn<QString>("result");

			QTest::addRow("empty text") << QString() << QString();
			QTest::addRow("without tags") << QStringLiteral("Hello World!") << QStringLiteral("Hello World!");
			QTest::addRow("with tags") << QStringLiteral("<p>Hello World!</p>") << QStringLiteral("Hello World!");
			QTest::addRow("with <br> tags") << QStringLiteral("<p>Hello<br>World!</p>") << QStringLiteral("Hello World!");
			QTest::addRow("with multiple <br> tags") << QStringLiteral("<p>Hello<br>World!<br><br>How are you?</p>") << QStringLiteral("Hello World! How are you?");
			QTest::addRow("variety of tags") << QStringLiteral("<h2><a href=\"https://www.example.org\">Hello</a> <marquee>World</marquee></h2><b>!</b>") << QStringLiteral("Hello World!");
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

			QTest::addRow("No Context") << QSharedPointer<WorkflowContext>() << ApplicationModel::Workflow::NONE;
			QTest::addRow("AuthContext") << QSharedPointer<WorkflowContext>(new AuthContext()) << ApplicationModel::Workflow::AUTHENTICATION;
			QTest::addRow("ChangePinContext") << QSharedPointer<WorkflowContext>(new ChangePinContext()) << ApplicationModel::Workflow::CHANGE_PIN;
			QTest::addRow("IfdServiceContext") << QSharedPointer<WorkflowContext>(new IfdServiceContext(QSharedPointer<MockIfdServer>::create())) << ApplicationModel::Workflow::REMOTE_SERVICE;
			QTest::addRow("SelfAuthContext") << QSharedPointer<WorkflowContext>(new SelfAuthContext()) << ApplicationModel::Workflow::SELF_AUTHENTICATION;
		}


		void test_getCurrentWorkflow()
		{
			QFETCH(QSharedPointer<WorkflowContext>, context);
			QFETCH(ApplicationModel::Workflow, workflow);

			auto model = Env::getSingleton<ApplicationModel>();
			model->resetContext(context);
			QCOMPARE(model->getCurrentWorkflow(), workflow);
		}


		void test_usedPluginType_data()
		{
			QTest::addColumn<QString>("usedReader");
			QTest::addColumn<QString>("readerName");
			QTest::addColumn<ReaderManagerPluginType>("readerType");
			QTest::addColumn<ReaderManagerPluginType>("usedPluginType");

			QTest::addRow("No used reader - PCSC") << "" << "PcscReader" << ReaderManagerPluginType::PCSC << ReaderManagerPluginType::UNKNOWN;
			QTest::addRow("No used reader - Remote") << "" << "RemoteReader" << ReaderManagerPluginType::REMOTE_IFD << ReaderManagerPluginType::UNKNOWN;

			QTest::addRow("PCSC") << "PcscReader" << "PcscReader" << ReaderManagerPluginType::PCSC << ReaderManagerPluginType::PCSC;
			QTest::addRow("Remote") << "RemoteReader" << "RemoteReader" << ReaderManagerPluginType::REMOTE_IFD << ReaderManagerPluginType::REMOTE_IFD;

			QTest::addRow("Other Reader 1") << "RemoteReader" << "PcscReader" << ReaderManagerPluginType::PCSC << ReaderManagerPluginType::UNKNOWN;
			QTest::addRow("Other Reader 2") << "PcscReader" << "RemoteReader" << ReaderManagerPluginType::REMOTE_IFD << ReaderManagerPluginType::UNKNOWN;
		}


		void test_usedPluginType()
		{
			QFETCH(QString, usedReader);
			QFETCH(QString, readerName);
			QFETCH(ReaderManagerPluginType, readerType);
			QFETCH(ReaderManagerPluginType, usedPluginType);

			QSharedPointer<WorkflowContext> context(new AuthContext());
			context->setReaderName(usedReader);
			if (!readerName.isEmpty())
			{
				MockReaderManagerPlugin::getInstance().addReader(readerName, readerType);
			}

			const auto model = Env::getSingleton<ApplicationModel>();
			model->resetContext(context);

			QCOMPARE(model->getUsedPluginType(), usedPluginType);
		}


		void test_isScreenRecording()
		{
			QCOMPARE(Env::getSingleton<ApplicationModel>()->isScreenRecording(), false);
		}


};

QTEST_MAIN(test_ApplicationModel)
#include "test_ApplicationModel.moc"
