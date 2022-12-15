/*!
 * \brief Unit tests for \ref UILoader
 *
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

#include "UILoader.h"

#include "UIPlugInJson.h"

#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>

using namespace governikus;

Q_IMPORT_PLUGIN(UIPlugInJson)

class test_UILoader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isLoaded()
		{
			UILoader loader;
			QSignalSpy spyLoaded(&loader, &UILoader::fireLoadedPlugin);

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QVERIFY(loader.load<UIPlugInJson>());
			QVERIFY(loader.isLoaded());
			QVERIFY(loader.load<UIPlugInJson>()); // already loaded
			QVERIFY(loader.getLoaded<UIPlugInJson>());
			QCOMPARE(spyLoaded.count(), 1);
			QVERIFY(loader.initialize());
			QVERIFY(loader.requiresReaderManager());

			const auto* ui = loader.getLoaded<UIPlugInJson>();
			QCOMPARE(ui->property("default"), QVariant());
			QCOMPARE(ui->property("passive"), QVariant(true));
			QCOMPARE(ui->property("userInteractive"), QVariant());
			QCOMPARE(ui->property("readerManager"), QVariant());

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: QList("json"))");
#else
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: ("json"))");
#endif
		}


		void checkEmptyDefault()
		{
			// DO NOT remove the whitespace, otherwise QCommandLineParser
			// won't accept user values.
			QCOMPARE(UILoader::getDefault(), QStringLiteral(" "));

			// not default plugins exists, see missing Q_IMPORT_PLUGIN
			UILoader loader;
			QVERIFY(!loader.load());
			QVERIFY(!loader.isLoaded());

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, "Shutdown UILoader: QList()");
#else
			QTest::ignoreMessage(QtDebugMsg, "Shutdown UILoader: ()");
#endif

			loader.shutdown();
		}


		void userRequestLoaded() // json can be loaded
		{
			UILoader::setUserRequest({QStringLiteral("crap"), QStringLiteral("json")});

			UILoader loader;
			QSignalSpy spyLoaded(&loader, &UILoader::fireLoadedPlugin);

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QVERIFY(loader.load());
			QVERIFY(loader.isLoaded());
			QCOMPARE(spyLoaded.count(), 1);
			QVERIFY(loader.getLoaded<UIPlugInJson>());

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: QList("json"))");
#else
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: ("json"))");
#endif
		}


		void userRequestReset() // reset to initial default
		{
			UILoader::setUserRequest({QStringLiteral("json")});
			UILoader::setUserRequest({});
			UILoader loader;
			QSignalSpy spyLoaded(&loader, &UILoader::fireLoadedPlugin);
			QVERIFY(!loader.load());
			QVERIFY(!loader.isLoaded());
			QVERIFY(loader.initialize()); // empty list is ok
			QCOMPARE(spyLoaded.count(), 0);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, "Shutdown UILoader: QList()");
#else
			QTest::ignoreMessage(QtDebugMsg, "Shutdown UILoader: ()");
#endif

			loader.shutdown();
		}


		void userRequestFallback() // user request has no valid plugin, fallback to initial default, too.
		{
			UILoader::setUserRequest({QStringLiteral("crap"), QStringLiteral("only")});
			UILoader loader;
			QSignalSpy spyLoaded(&loader, &UILoader::fireLoadedPlugin);
			QVERIFY(!loader.load());
			QVERIFY(!loader.isLoaded());
			QCOMPARE(spyLoaded.count(), 0);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, "Shutdown UILoader: QList()");
#else
			QTest::ignoreMessage(QtDebugMsg, "Shutdown UILoader: ()");
#endif

			loader.shutdown();
		}


		void fireShutdownNoUi() // no ui loaded
		{
			auto loader = QSharedPointer<UILoader>::create();
			QSignalSpy spyShutdown(loader.get(), &UILoader::fireRemovedAllPlugins);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, "Shutdown UILoader: QList()");
#else
			QTest::ignoreMessage(QtDebugMsg, "Shutdown UILoader: ()");
#endif
			loader->shutdown();
			QCoreApplication::processEvents(); // cannot use QTRY_COMPARE here
			QCOMPARE(spyShutdown.count(), 0);
		}


		void fireShutdownBeforeDtor() // ui loaded, shutdown before dtor
		{
			auto loader = QSharedPointer<UILoader>::create();
			QSignalSpy spyShutdown(loader.data(), &UILoader::fireRemovedAllPlugins);

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QVERIFY(loader->load<UIPlugInJson>());

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: QList("json"))");
#else
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: ("json"))");
#endif
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UI: "json")");
			loader->shutdown();
			QTRY_COMPARE(spyShutdown.count(), 1); // clazy:exclude=qstring-allocations

			loader.reset();
			QTRY_COMPARE(spyShutdown.count(), 1); // clazy:exclude=qstring-allocations
		}


		void noFireShutdownDtor() // ui loaded, but dtor won't fire
		{
			auto loader = QSharedPointer<UILoader>::create();
			QSignalSpy spyShutdown(loader.data(), &UILoader::fireRemovedAllPlugins);

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QVERIFY(loader->load<UIPlugInJson>());

			QSignalSpy spyDestroyed(loader->getLoaded<UIPlugInJson>(), &QObject::destroyed);
			loader.reset();

			QTRY_COMPARE(spyDestroyed.count(), 1); // clazy:exclude=qstring-allocations

			// We call deleteLater and add a lambda with QueuedConnection.
			// So the lambda will be disconnected if loader is destroyed abd
			// we never get a fireRemovedAllPlugins here. Even we don't have
			// blockSignals in dtor.
			QCOMPARE(spyShutdown.count(), 0);
		}


		void getName_data()
		{
			QTest::addColumn<QString>("name");

			QTest::newRow("qml") << QString("UIPlugInQml");
			QTest::newRow("json") << QString("governikus::UIPlugInJson");
			QTest::newRow("websocket") << QString("WebSocket");
			QTest::newRow("localifd") << QString("UIPlugInLocalIfd");
			QTest::newRow("aidl") << QString("governikus::uipluginAidl");
			QTest::newRow("functional") << QString("UIPluginFunctional");
		}


		void getName()
		{
			QFETCH(QString, name);
			QCOMPARE(UILoader::unify(name), QString::fromLatin1(QTest::currentDataTag()));
		}


};

QTEST_GUILESS_MAIN(test_UILoader)
#include "test_UILoader.moc"
