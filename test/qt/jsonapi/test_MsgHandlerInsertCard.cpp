/*!
 * \brief Unit tests for \ref MsgHandlerInsertCard
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "messages/MsgHandlerInsertCard.h"

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"

#include <QtPlugin>
#include <QTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

class test_MsgHandlerInsertCard
	: public QObject
{
	Q_OBJECT

	static void setContext(MessageDispatcher& pDispatcher)
	{
		QSharedPointer<WorkflowContext> context(new WorkflowContext());
		pDispatcher.init(context);
	}


	private Q_SLOTS:
		void initTestCase()
		{
			ReaderManager::getInstance().init();
			ReaderManager::getInstance().getPlugInInfos(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			ReaderManager::getInstance().shutdown();
		}


		void ctor()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			QCOMPARE(context->isStateApproved(), false);

			MsgDispatcherContext ctx;
			ctx.setWorkflowContext(context);
			MsgHandlerInsertCard msg(ctx);

			QCOMPARE(msg.toJson(), QByteArray("{\"msg\":\"INSERT_CARD\"}"));
			QCOMPARE(context->isStateApproved(), true);
		}


		void noReader()
		{
			MessageDispatcher dispatcher;
			setContext(dispatcher);

			QCOMPARE(dispatcher.processStateChange(QStringLiteral("StateSelectPcscReader")), QByteArray("{\"msg\":\"INSERT_CARD\"}"));
			QCOMPARE(dispatcher.processStateChange(QStringLiteral("StateSelectNfcReader")), QByteArray("{\"msg\":\"INSERT_CARD\"}"));
		}


		void readerWithoutCard()
		{
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 1");
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 2");
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 3");

			MessageDispatcher dispatcher;
			setContext(dispatcher);

			QCOMPARE(dispatcher.processStateChange(QStringLiteral("StateSelectPcscReader")), QByteArray("{\"msg\":\"INSERT_CARD\"}"));
			QCOMPARE(dispatcher.processStateChange(QStringLiteral("StateSelectNfcReader")), QByteArray("{\"msg\":\"INSERT_CARD\"}"));
		}


		void readerWithCard()
		{
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 1");
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader CARD");
			reader->setCard(MockCardConfig());

			MessageDispatcher dispatcher;
			setContext(dispatcher);

			QCOMPARE(dispatcher.processStateChange(QStringLiteral("StateSelectPcscReader")), QByteArray());
			QCOMPARE(dispatcher.processStateChange(QStringLiteral("StateSelectNfcReader")), QByteArray());
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerInsertCard)
#include "test_MsgHandlerInsertCard.moc"
