/*!
 * \brief Unit tests for \ref MsgHandlerChangePin
 *
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerChangePin.h"

#include "Env.h"
#include "MessageDispatcher.h"
#include "UILoader.h"
#include "UIPlugInJson.h"

#include <QSignalSpy>
#include <QtTest>

Q_IMPORT_PLUGIN(UIPlugInJson)

using namespace governikus;

class test_MsgHandlerChangePin
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<UILoader>()->load(UIPlugInName::UIPlugInJson);
		}


		void runChangePinCmd()
		{
			auto ui = qobject_cast<UIPlugInJson*>(Env::getSingleton<UILoader>()->getLoaded(UIPlugInName::UIPlugInJson));
			QVERIFY(ui);
			QSignalSpy spy(ui, &UIPlugIn::fireChangePinRequested);

			MessageDispatcher dispatcher;
			const QByteArray msg = R"({"cmd": "RUN_CHANGE_PIN"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());

			QCOMPARE(spy.count(), 1);
		}


		void initChangePin()
		{
			const auto context = QSharedPointer<WorkflowContext>::create();
			MessageDispatcher dispatcher;
			QCOMPARE(dispatcher.init(context), QByteArray());
			QCOMPARE(dispatcher.finish(), QByteArray());

			const auto ctx = QSharedPointer<ChangePinContext>::create();
			QCOMPARE(dispatcher.init(ctx), QByteArray(R"({"msg":"CHANGE_PIN"})"));
		}


		void allowCancel()
		{
			MessageDispatcher dispatcher;

			const auto ctx = QSharedPointer<ChangePinContext>::create();
			QCOMPARE(dispatcher.init(ctx), QByteArray(R"({"msg":"CHANGE_PIN"})"));

			QVERIFY(!ctx->isWorkflowCancelled());
			QByteArray msg = R"({"cmd": "CANCEL"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray());
			QVERIFY(ctx->isWorkflowCancelled());

			msg = R"({"msg":"CHANGE_PIN","success":false})";
			QCOMPARE(dispatcher.finish(), msg);
		}


		void badState()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			MessageDispatcher dispatcher;
			dispatcher.init(context);
			const QByteArray expected(R"({"error":"RUN_CHANGE_PIN","msg":"BAD_STATE"})");

			const QByteArray msg(R"({"cmd": "RUN_CHANGE_PIN"})");
			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerChangePin)
#include "test_MsgHandlerChangePin.moc"
