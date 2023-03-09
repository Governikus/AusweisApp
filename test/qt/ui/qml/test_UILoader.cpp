/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref UILoader
 */

#include "UILoader.h"

#include "UIPlugInJson.h"
#include "UIPlugInWebSocket.h"

#include <QSignalSpy>
#include <QtTest>

using namespace governikus;

Q_IMPORT_PLUGIN(UIPlugInWebSocket)

class test_UILoader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isNotLoaded()
		{
			UILoader loader;
			QSignalSpy spyLoaded(&loader, &UILoader::fireLoadedPlugin);

			QVERIFY(loader.initialize());
			QVERIFY(!loader.isLoaded());
			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QTest::ignoreMessage(QtCriticalMsg, R"(Cannot find UI plugin: "json")");
			QVERIFY(!loader.load<UIPlugInJson>());
			QVERIFY(!loader.isLoaded());
			QVERIFY(!loader.getLoaded<UIPlugInJson>());
			QCOMPARE(spyLoaded.count(), 0);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, "Shutdown UILoader: QList()");
#else
			QTest::ignoreMessage(QtDebugMsg, "Shutdown UILoader: ()");
#endif

			loader.shutdown();
		}


		void checkNonEmptyDefault()
		{
			QCOMPARE(UILoader::getDefault(), QStringLiteral("websocket"));

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "websocket")");
			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QTest::ignoreMessage(QtCriticalMsg, R"(Cannot find UI plugin: "json")");
			QTest::ignoreMessage(QtWarningMsg, "Cannot start WebSocket because JSON-API is missing");

			UILoader loader;
			QSignalSpy spyLoaded(&loader, &UILoader::fireLoadedPlugin);
			QVERIFY(loader.load());
			QVERIFY(loader.isLoaded());
			QVERIFY(!loader.initialize());
			QVERIFY(loader.getLoaded<UIPlugInWebSocket>());
			QCOMPARE(spyLoaded.count(), 1);
			QVERIFY(loader.requiresReaderManager());

			const auto* ui = loader.getLoaded<UIPlugInWebSocket>();
			QCOMPARE(ui->property("default"), QVariant(true));
			QCOMPARE(ui->property("passive"), QVariant());
			QCOMPARE(ui->property("readerManager"), QVariant());

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: QList("websocket"))");
#else
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: ("websocket"))");
#endif
		}


		void userRequestReset() // reset to initial default
		{
			UILoader::setUserRequest({QStringLiteral("json")});
			UILoader::setUserRequest({});
			UILoader loader;
			QSignalSpy spyLoaded(&loader, &UILoader::fireLoadedPlugin);
			QVERIFY(loader.load());
			QVERIFY(loader.isLoaded());
			QVERIFY(!loader.initialize());
			QVERIFY(loader.getLoaded<UIPlugInWebSocket>());
			QCOMPARE(spyLoaded.count(), 1);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: QList("websocket"))");
#else
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: ("websocket"))");
#endif
		}


};

QTEST_GUILESS_MAIN(test_UILoader)
#include "test_UILoader.moc"
