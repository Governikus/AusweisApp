/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginProxy.h"

#include "Env.h"
#include "HttpServer.h"

#include <QtTest>

using namespace governikus;

class test_UiPluginProxy
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
			UiPluginProxy proxy;
			QSignalSpy logSpy(&proxy, &UiPluginProxy::fireUiDominationRequest);

			QVERIFY(proxy.initialize());
			QCOMPARE(logSpy.count(), 1);

			QVERIFY(proxy.initialize());
			QCOMPARE(logSpy.count(), 1);
		}


		void enabledDisabled()
		{
			UiPluginProxy proxy;
			QVERIFY(proxy.initialize());

			QTest::ignoreMessage(QtDebugMsg, "Reverse proxy enabled");
			proxy.onUiDomination(&proxy, QStringLiteral("dummy"), true);

			QTest::ignoreMessage(QtDebugMsg, "Reverse proxy disabled");
			proxy.onUiDominationReleased();
		}


};

QTEST_GUILESS_MAIN(test_UiPluginProxy)
#include "test_UiPluginProxy.moc"
