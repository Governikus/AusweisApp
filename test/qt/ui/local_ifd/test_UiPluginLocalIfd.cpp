/**
 * Copyright (c) 2026 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginLocalIfd.h"

#include "ResourceLoader.h"
#include "WorkflowRequest.h"

#include <QSignalSpy>
#include <QtTest>


using namespace governikus;


class test_UiPluginLocalIfd
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void shortPsk()
		{
			UiPluginLocalIfd plugin;
			QSignalSpy spy(&plugin, &UiPluginLocalIfd::fireWorkflowRequested);

			QTest::ignoreMessage(QtCriticalMsg, "PSK must be at least 256 bit");
			QVERIFY(!plugin.onStartWorkflowRequested(QString(31, QChar(0))));
			QCOMPARE(spy.size(), 0);
		}


		void workflow()
		{
			UiPluginLocalIfd plugin;
			QSignalSpy spy(&plugin, &UiPluginLocalIfd::fireWorkflowRequested);

			QTest::ignoreMessage(QtDebugMsg, "Generate IFD Service token");
			QVERIFY(plugin.onStartWorkflowRequested(QString(32, QChar(0))));
			QCOMPARE(spy.size(), 1);

			const auto& request = spy.at(0).at(0).value<QSharedPointer<WorkflowRequest>>();
			const auto& context = request->getContext();
			QVERIFY(!context->wasClaimed());

			QTest::ignoreMessage(QtDebugMsg, "Claim workflow by \"governikus::UiPluginLocalIfd\"");
			plugin.onWorkflowStarted(request);
			QVERIFY(context->wasClaimed());

			QVERIFY(!context->isStateApproved());
			Q_EMIT context->fireStateChanged(QStringLiteral("StateTest"));
			QVERIFY(context->isStateApproved());

			QVERIFY(!context->isWorkflowKilled());
			QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");
			QTest::ignoreMessage(QtDebugMsg, "Clear IFD Service token");
			plugin.onAbortWorkflowRequested();
			QVERIFY(context->isWorkflowKilled());

			context->setStateApproved(false);
			plugin.onWorkflowFinished(request);
			Q_EMIT context->fireStateChanged(QStringLiteral("StateTest"));
			QVERIFY(!context->isStateApproved());
		}


};

QTEST_GUILESS_MAIN(test_UiPluginLocalIfd)
#include "test_UiPluginLocalIfd.moc"
