/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref WorkflowModel
 */

#include "WorkflowModel.h"

#include "Env.h"
#include "MockCardConnectionWorker.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "TestWorkflowContext.h"

#include <QDebug>
#include <QFile>
#include <QtTest>


Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;


class test_WorkflowModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations

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
			QSignalSpy spyReaderPlugInTypeChanged(&model, &WorkflowModel::fireReaderPlugInTypeChanged);
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

			Q_EMIT context->fireStateChanged(QString("state"));
			QCOMPARE(spyCurrentStateChanged.count(), 3);
			QCOMPARE(spyStateEntered.count(), 1);

			Q_EMIT context->fireResultChanged();
			QCOMPARE(spyResultChanged.count(), 3);

			Q_EMIT context->fireReaderPlugInTypesChanged();
			QCOMPARE(spyReaderPlugInTypeChanged.count(), 1);

			Q_EMIT context->fireCardConnectionChanged();
			QCOMPARE(spySelectedReaderChanged.count(), 1);
		}


		void test_IsBasicReader()
		{
			QThread connectionThread;
			connectionThread.start();

			WorkflowModel model;
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QVERIFY(model.isBasicReader());

			model.mContext = context;
			QVERIFY(model.isBasicReader());

			MockReader reader;
			reader.setInfoBasicReader(false);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			worker->moveToThread(&connectionThread);
			QSharedPointer<CardConnection> connection(new CardConnection(worker));
			context->setCardConnection(connection);
			model.mContext = context;
			QVERIFY(!model.isBasicReader());

			connectionThread.quit();
			connectionThread.wait();
		}


		void test_ReaderPlugInType()
		{
			WorkflowModel model;
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QCOMPARE(model.getReaderPlugInType(), ReaderManagerPlugInType::UNKNOWN);

			model.mContext = context;
			QCOMPARE(model.getReaderPlugInType(), ReaderManagerPlugInType::UNKNOWN);

			model.setReaderPlugInType(ReaderManagerPlugInType::UNKNOWN);
			QVERIFY(context->getReaderPlugInTypes().contains(ReaderManagerPlugInType::UNKNOWN));
			QCOMPARE(model.getReaderPlugInType(), ReaderManagerPlugInType::UNKNOWN);

			model.setReaderPlugInType(ReaderManagerPlugInType::NFC);
			QVERIFY(context->getReaderPlugInTypes().contains(ReaderManagerPlugInType::NFC));
			QCOMPARE(model.getReaderPlugInType(), ReaderManagerPlugInType::NFC);
		}


		void test_isCurrentSmartCardAllowed()
		{
			WorkflowModel workflowModel;
			QVERIFY2(!workflowModel.isCurrentSmartCardAllowed(), "Ensure that this will not crash when the model has no WorkflowContext.");
		}


		void test_startScanExplicitly()
		{
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			QSignalSpy spy(context.data(), &WorkflowContext::fireReaderPlugInTypesChanged);

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

			auto mockReader = MockReaderManagerPlugIn::getInstance().addReader("SomeReaderWithCard");
			auto info = mockReader->getReaderInfo();
			info.setCardInfo(CardInfo(CardType::EID_CARD, QSharedPointer<EFCardAccess>(), 3, false, false, false));
			mockReader->setReaderInfo(info);
			model.setReaderPlugInType(ReaderManagerPlugInType::MOCK);
			QCOMPARE(model.hasCard(), true);
		}


		void test_statusCodeImage_data()
		{
			QTest::addColumn<GlobalStatus::Code>("statusCode");

			QMetaEnum e = QMetaEnum::fromType<GlobalStatus::Code>();

			for (int k = 0; k < e.keyCount(); k++)
			{
				QTest::addRow("%s", e.key(k)) << GlobalStatus::Code(e.value(k));
			}
		}


		void test_statusCodeImage()
		{
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			QSignalSpy spy(context.data(), &WorkflowContext::fireReaderPlugInTypesChanged);

			QFETCH(GlobalStatus::Code, statusCode);
			context->setStatus(statusCode);

			WorkflowModel model;
			model.resetWorkflowContext(context);

			auto image = model.getStatusCodeImage();
			image = image.replace("qrc://", ":");
			if (image.isEmpty())
			{
				return;
			}

			if (image.contains("%1"))
			{
				QStringList themes;
				themes << QStringLiteral("darkmode") << QStringLiteral("lightmode") << QStringLiteral("highcontrast");

				for (const auto& theme : themes)
				{
					const auto fileName = image.arg(theme);
					QVERIFY2(QFile(fileName).exists(), qPrintable(QString("%1 not found").arg(fileName)));
				}
			}
			else
			{
				QVERIFY(QFile::exists(image));
			}

		}


};

QTEST_MAIN(test_WorkflowModel)
#include "test_WorkflowModel.moc"
