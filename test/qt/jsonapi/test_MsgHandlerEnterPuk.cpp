/*!
 * \brief Unit tests for \ref MsgHandlerEnterPuk
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "messages/MsgHandlerEnterPuk.h"

#include "MessageDispatcher.h"

#include <QTest>
#include <QtPlugin>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

class test_MsgHandlerEnterPuk
	: public QObject
{
	Q_OBJECT

	static void setValidState(MessageDispatcher& pDispatcher, const QString& pState = QStringLiteral("StateEstablishPacePuk"))
	{
		QSharedPointer<WorkflowContext> context(new WorkflowContext());
		pDispatcher.init(context);

		QByteArray expected;
		if (pState == QLatin1String("StateEstablishPacePuk"))
		{
			expected = "{\"msg\":\"ENTER_PUK\"}";
		}

		QCOMPARE(pDispatcher.processStateChange(pState), expected);
	}


	private Q_SLOTS:
		void stateMsg()
		{
			MessageDispatcher dispatcher;
			setValidState(dispatcher);
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerEnterPuk)
#include "test_MsgHandlerEnterPuk.moc"
