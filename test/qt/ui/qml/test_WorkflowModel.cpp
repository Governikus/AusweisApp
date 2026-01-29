/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowModel.h"

#include "AppSettings.h"
#include "ApplicationModel.h"
#include "ProviderConfiguration.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "SmartModel.h"

#include "MockCardConnectionWorker.h"
#include "MockReaderManagerPlugin.h"
#include "TestHookThread.h"
#include "TestWorkflowContext.h"

#include <QDebug>
#include <QFile>
#include <QPointer>
#include <QtTest>


Q_IMPORT_PLUGIN(MockReaderManagerPlugin)


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_WorkflowModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations

			Env::getSingleton<AppSettings>(); // just init in MainThread because of QObject
			Env::getSingleton<ApplicationModel>(); // just init in MainThread because of QObject
			Env::getSingleton<SmartModel>(); // just init in MainThread because of QObject

			ResourceLoader::getInstance().init();
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void test_ResetContext()
		{
			WorkflowModel model;
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QSignalSpy spyWorkflowStarted(&model, &WorkflowModel::fireWorkflowStarted);
			QSignalSpy spyCurrentStateChanged(&model, &WorkflowModel::fireCurrentStateChanged);
			QSignalSpy spyStateEntered(&model, &WorkflowModel::fireStateEntered);
			QSignalSpy spyResultChanged(&model, &WorkflowModel::fireResultChanged);
			QSignalSpy spyReaderPluginTypeChanged(&model, &WorkflowModel::fireReaderPluginTypeChanged);
			QSignalSpy spySelectedReaderChanged(&model, &WorkflowModel::fireSelectedReaderChanged);
			QSignalSpy spyWorkflowFinished(&model, &WorkflowModel::fireWorkflowFinished);

			model.resetWorkflowContext();
			QCOMPARE(spyWorkflowStarted.count(), 0);
			QCOMPARE(spyCurrentStateChanged.count(), 1);
			QCOMPARE(spyStateEntered.count(), 0);
			QCOMPARE(spyResultChanged.count(), 1);
			QCOMPARE(spyWorkflowFinished.count(), 1);

			model.resetWorkflowContext(context);
			QCOMPARE(spyWorkflowStarted.count(), 1);
			QCOMPARE(spyCurrentStateChanged.count(), 2);
			QCOMPARE(spyStateEntered.count(), 0);
			QCOMPARE(spyResultChanged.count(), 2);
			QCOMPARE(spyWorkflowFinished.count(), 1);

			Q_EMIT context->fireStateChanged("state"_L1);
			QCOMPARE(spyCurrentStateChanged.count(), 3);
			QCOMPARE(spyStateEntered.count(), 1);

			Q_EMIT context->fireResultChanged();
			QCOMPARE(spyResultChanged.count(), 3);

			Q_EMIT context->fireReaderPluginTypesChanged();
			QCOMPARE(spyReaderPluginTypeChanged.count(), 1);

			Q_EMIT context->fireCardConnectionChanged();
			QCOMPARE(spySelectedReaderChanged.count(), 1);
		}


		void test_IsBasicReader()
		{
			TestHookThread connectionThread;

			{
				WorkflowModel model;
				QVERIFY(model.isBasicReader());

				QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
				model.resetWorkflowContext(context);
				QVERIFY(model.isBasicReader());

				QPointer<MockReader> reader = new MockReader();
				reader->setInfoBasicReader(false);
				const auto& worker = MockCardConnectionWorker::create(&connectionThread, reader);
				context->setCardConnection(QSharedPointer<CardConnection>::create(worker));
				QVERIFY(!model.isBasicReader());

				model.resetWorkflowContext();
				context.reset();
			}
		}


		void test_ReaderPluginType()
		{
			WorkflowModel model;
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QCOMPARE(model.getReaderPluginType(), ReaderManagerPluginType::UNKNOWN);

			model.mContext = context;
			QCOMPARE(model.getReaderPluginType(), ReaderManagerPluginType::UNKNOWN);

			model.setReaderPluginType(ReaderManagerPluginType::UNKNOWN);
			QVERIFY(context->getReaderPluginTypes().contains(ReaderManagerPluginType::UNKNOWN));
			QCOMPARE(model.getReaderPluginType(), ReaderManagerPluginType::UNKNOWN);

			model.setReaderPluginType(ReaderManagerPluginType::NFC);
			QVERIFY(context->getReaderPluginTypes().contains(ReaderManagerPluginType::NFC));
			QCOMPARE(model.getReaderPluginType(), ReaderManagerPluginType::NFC);
		}


		void test_isCurrentSmartCardAllowed()
		{
			WorkflowModel workflowModel;
			QVERIFY2(!workflowModel.isCurrentSmartCardAllowed(), "Ensure that this will not crash when the model has no WorkflowContext.");
		}


		void test_startScanExplicitly()
		{
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			QSignalSpy spy(context.data(), &WorkflowContext::fireReaderPluginTypesChanged);

			WorkflowModel model;
			model.resetWorkflowContext(context);

			model.startScanExplicitly();
			QTRY_COMPARE(spy.count(), 1);
			QCOMPARE(spy.takeFirst().at(0).toBool(), true);
		}


		void test_hasCard()
		{
			WorkflowModel model;
			QCOMPARE(model.hasCard(), false);

			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			model.resetWorkflowContext(context);
			QCOMPARE(model.hasCard(), false);

			auto mockReader = MockReaderManagerPlugin::getInstance().addReader("SomeReaderWithCard"_L1);
			auto info = mockReader->getReaderInfo();
			info.setCardInfo(CardInfo(CardType::EID_CARD, FileRef(), QSharedPointer<EFCardAccess>(), 3, false, false, false));
			mockReader->setReaderInfo(info);
			model.setReaderPluginType(ReaderManagerPluginType::MOCK);
			QCOMPARE(model.hasCard(), true);
		}


		void test_isPukInoperative()
		{
			WorkflowModel model;
			QVERIFY(!model.isPukInoperative());

			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			model.resetWorkflowContext(context);
			QVERIFY(!model.isPukInoperative());

			context->setStatus(GlobalStatus::Code::Card_Invalid_Pin);
			QVERIFY(!model.isPukInoperative());

			context->setStatus(GlobalStatus::Code::Card_Puk_Blocked);
			QVERIFY(model.isPukInoperative());
		}


		void test_getLastReturnCode()
		{
			WorkflowModel model;
			QCOMPARE(model.getLastReturnCode(), CardReturnCode::UNDEFINED);

			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			model.resetWorkflowContext(context);
			QCOMPARE(model.getLastReturnCode(), CardReturnCode::OK);

			context->setLastPaceResult(CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(model.getLastReturnCode(), CardReturnCode::CANCELLATION_BY_USER);
		}


		void test_getStatusCodeDisplayString_data()
		{
			QTest::addColumn<QSharedPointer<WorkflowContext>>("context");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<QString>("result");

			QTest::addRow("No context") << QSharedPointer<WorkflowContext>(nullptr) << GlobalStatus::Code::No_Error << "Error code: Unknown_Error";
			QTest::addRow("Any error") << QSharedPointer<WorkflowContext>(new TestWorkflowContext()) << GlobalStatus::Code::Card_Communication_Error << "Error code: Card_Communication_Error";
			QTest::addRow("No error") << QSharedPointer<WorkflowContext>(new TestWorkflowContext()) << GlobalStatus::Code::No_Error << "Error code: No_Error";
		}


		void test_getStatusCodeDisplayString()
		{
			QFETCH(QSharedPointer<WorkflowContext>, context);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(QString, result);

			auto* const model = Env::getSingleton<WorkflowModel>();
			if (context)
			{
				context->setStatus(GlobalStatus(statusCode));
			}
			model->resetWorkflowContext(context);

			QCOMPARE(model->getStatusCodeDisplayString(), result);
		}


		void test_getStatusHint_data()
		{
			QTest::addColumn<bool>("prs");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<QString>("box");
			QTest::addColumn<QString>("title");
			QTest::addColumn<QString>("text");
			QTest::addColumn<QString>("action");

			QTest::addRow("no_prs_No_error") << false << GlobalStatus::Code::No_Error << QString() << QString() << QString() << QString();
			QTest::addRow("no_prs_Card_Puk_Blocked") << false << GlobalStatus::Code::Card_Puk_Blocked
													 << "My PUK is used up. How do I set a new PIN?"
													 << "At your competent authority" << "You may turn to the competent authority and reset the ID card PIN there."
													 << "Find competent authority";
			QTest::addRow("no_prs_Card_Pin_Deactivated") << false << GlobalStatus::Code::Card_Pin_Deactivated
														 << "How do I activate the eID function?"
														 << "At your competent authority"
														 << "You may turn to your competent authority to activate the eID function."
														 << "Find competent authority";
			QTest::addRow("no_prs_Card_ValidityVerificationFailed") << false << GlobalStatus::Code::Card_ValidityVerificationFailed
																	<< QString()
																	<< QString()
																	<< "Contact your local citizens' office (Bürgeramt) to apply for a new ID card or to unblock the ID card."
																	<< QString();

			QTest::addRow("prs_No_error") << true << GlobalStatus::Code::No_Error << QString() << QString() << QString() << QString();
			QTest::addRow("prs_Card_Puk_Blocked") << true << GlobalStatus::Code::Card_Puk_Blocked
												  << "My PUK is used up. How do I set a new PIN?"
												  << "Online via PIN Reset Service"
												  << "You may request a PIN Reset Letter with a new PIN and it's according activation code on the following website."
												  << "Request PIN Reset Letter";
			QTest::addRow("prs_Card_Pin_Deactivated") << true << GlobalStatus::Code::Card_Pin_Deactivated
													  << "How do I activate the eID function?"
													  << "Online via PIN Reset Service"
													  << "You may request a PIN Reset Letter with a new PIN and it's according activation code on the following website to activate the eID function."
													  << "Request PIN Reset Letter";
			QTest::addRow("prs_Card_ValidityVerificationFailed") << true << GlobalStatus::Code::Card_ValidityVerificationFailed
																 << QString()
																 << QString()
																 << "Contact your local citizens' office (Bürgeramt) to apply for a new ID card or to unblock the ID card."
																 << QString();
		}


		void test_getStatusHint()
		{
			QFETCH(bool, prs);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(QString, box);
			QFETCH(QString, title);
			QFETCH(QString, text);
			QFETCH(QString, action);

			WorkflowModel model;
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			model.resetWorkflowContext(context);
			QCOMPARE(model.getStatusHintTitle(), QString());

			auto* config = Env::getSingleton<ProviderConfiguration>();
			if (prs)
			{
				config->parseProviderConfiguration(":/updatable-files/supported-providers_prs.json"_L1);
			}
			else
			{
				config->parseProviderConfiguration(":/updatable-files/supported-providers_no-prs.json"_L1);
			}
			context->setStatus(statusCode);

			QCOMPARE(model.getStatusHintBoxesTitle(), box);
			QCOMPARE(model.getStatusHintTitle(), title);
			QCOMPARE(model.getStatusHintText(), text);
			QCOMPARE(model.getStatusHintActionText(), action);
		}


};

QTEST_MAIN(test_WorkflowModel)
#include "test_WorkflowModel.moc"
