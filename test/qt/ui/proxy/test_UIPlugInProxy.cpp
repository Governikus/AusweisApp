/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInProxy.h"

#include "Env.h"
#include "HttpServer.h"

#include <QtTest>

using namespace governikus;

class test_UIPlugInProxy
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			HttpServer::cPort = 0;
		}


		void initialize()
		{
			UIPlugInProxy proxy;
			QSignalSpy logSpy(&proxy, &UIPlugInProxy::fireUiDominationRequest);

			QVERIFY(proxy.initialize());
			QCOMPARE(logSpy.count(), 1);

			QVERIFY(proxy.initialize());
			QCOMPARE(logSpy.count(), 1);
		}


		void enabledDisabled()
		{
			UIPlugInProxy proxy;
			QVERIFY(proxy.initialize());

			QTest::ignoreMessage(QtDebugMsg, "Reverse proxy enabled");
			proxy.onUiDomination(&proxy, QStringLiteral("dummy"), true);

			QTest::ignoreMessage(QtDebugMsg, "Reverse proxy disabled");
			proxy.onUiDominationReleased();
		}


};

QTEST_GUILESS_MAIN(test_UIPlugInProxy)
#include "test_UIPlugInProxy.moc"
