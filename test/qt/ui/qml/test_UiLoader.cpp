/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiLoader.h"

#include "UiPluginJson.h"
#include "UiPluginWebSocket.h"

#include <QSignalSpy>
#include <QtTest>

using namespace governikus;

Q_IMPORT_PLUGIN(UiPluginWebSocket)

class test_UiLoader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isNotLoaded()
		{
			UiLoader loader;
			QSignalSpy spyLoaded(&loader, &UiLoader::fireLoadedPlugin);

			QVERIFY(loader.initialize());
			QVERIFY(!loader.isLoaded());
			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QTest::ignoreMessage(QtCriticalMsg, R"(Cannot find UI plugin: "json")");
			QVERIFY(!loader.load<UiPluginJson>());
			QVERIFY(!loader.isLoaded());
			QVERIFY(!loader.getLoaded<UiPluginJson>());
			QCOMPARE(spyLoaded.count(), 0);

			QTest::ignoreMessage(QtDebugMsg, "Shutdown UiLoader: QList()");
			loader.shutdown();
		}


		void checkNonEmptyDefault()
		{
			QCOMPARE(UiLoader::getDefault(), QStringLiteral("websocket"));

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "websocket")");
			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QTest::ignoreMessage(QtCriticalMsg, R"(Cannot find UI plugin: "json")");
			QTest::ignoreMessage(QtWarningMsg, "Cannot start WebSocket because JSON-API is missing");

			UiLoader loader;
			QSignalSpy spyLoaded(&loader, &UiLoader::fireLoadedPlugin);
			QVERIFY(loader.load());
			QVERIFY(loader.isLoaded());
			QVERIFY(!loader.initialize());
			QVERIFY(loader.getLoaded<UiPluginWebSocket>());
			QCOMPARE(spyLoaded.count(), 1);
			QVERIFY(loader.requiresReaderManager());

			const auto* ui = loader.getLoaded<UiPluginWebSocket>();
			QCOMPARE(ui->property("default"), QVariant(true));
			QCOMPARE(ui->property("passive"), QVariant());
			QCOMPARE(ui->property("readerManager"), QVariant());

			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UiLoader: QList("websocket"))");
		}


		void userRequestReset() // reset to initial default
		{
			UiLoader::setUserRequest({QStringLiteral("json")});
			UiLoader::setUserRequest({});
			UiLoader loader;
			QSignalSpy spyLoaded(&loader, &UiLoader::fireLoadedPlugin);
			QVERIFY(loader.load());
			QVERIFY(loader.isLoaded());
			QVERIFY(!loader.initialize());
			QVERIFY(loader.getLoaded<UiPluginWebSocket>());
			QCOMPARE(spyLoaded.count(), 1);

			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UiLoader: QList("websocket"))");
		}


};

QTEST_GUILESS_MAIN(test_UiLoader)
#include "test_UiLoader.moc"
