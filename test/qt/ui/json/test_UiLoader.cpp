/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiLoader.h"

#include "UiPluginJson.h"

#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_IMPORT_PLUGIN(UiPluginJson)

class test_UiLoader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isLoaded()
		{
			UiLoader loader;
			QSignalSpy spyLoaded(&loader, &UiLoader::fireLoadedPlugin);

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QVERIFY(loader.load<UiPluginJson>());
			QVERIFY(loader.isLoaded());
			QVERIFY(loader.load<UiPluginJson>()); // already loaded
			QVERIFY(loader.getLoaded<UiPluginJson>());
			QCOMPARE(spyLoaded.count(), 1);
			QVERIFY(loader.initialize());
			QVERIFY(loader.requiresReaderManager());

			const auto* ui = loader.getLoaded<UiPluginJson>();
			QCOMPARE(ui->property("default"), QVariant());
			QCOMPARE(ui->property("passive"), QVariant(true));
			QCOMPARE(ui->property("userInteractive"), QVariant());
			QCOMPARE(ui->property("readerManager"), QVariant());

			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UiLoader: QList("json"))");
		}


		void checkEmptyDefault()
		{
			// DO NOT remove the whitespace, otherwise QCommandLineParser
			// won't accept user values.
			QCOMPARE(UiLoader::getDefault(), QStringLiteral(" "));

			// not default plugins exists, see missing Q_IMPORT_PLUGIN
			UiLoader loader;
			QVERIFY(!loader.load());
			QVERIFY(!loader.isLoaded());

			QTest::ignoreMessage(QtDebugMsg, "Shutdown UiLoader: QList()");
			loader.shutdown();
		}


		void userRequestLoaded() // json can be loaded
		{
			UiLoader::setUserRequest({QStringLiteral("crap"), QStringLiteral("json")});

			UiLoader loader;
			QSignalSpy spyLoaded(&loader, &UiLoader::fireLoadedPlugin);

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QVERIFY(loader.load());
			QVERIFY(loader.isLoaded());
			QCOMPARE(spyLoaded.count(), 1);
			QVERIFY(loader.getLoaded<UiPluginJson>());

			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UiLoader: QList("json"))");
		}


		void userRequestReset() // reset to initial default
		{
			UiLoader::setUserRequest({QStringLiteral("json")});
			UiLoader::setUserRequest({});
			UiLoader loader;
			QSignalSpy spyLoaded(&loader, &UiLoader::fireLoadedPlugin);
			QVERIFY(!loader.load());
			QVERIFY(!loader.isLoaded());
			QVERIFY(loader.initialize()); // empty list is ok
			QCOMPARE(spyLoaded.count(), 0);

			QTest::ignoreMessage(QtDebugMsg, "Shutdown UiLoader: QList()");
			loader.shutdown();
		}


		void userRequestFallback() // user request has no valid plugin, fallback to initial default, too.
		{
			UiLoader::setUserRequest({QStringLiteral("crap"), QStringLiteral("only")});
			UiLoader loader;
			QSignalSpy spyLoaded(&loader, &UiLoader::fireLoadedPlugin);
			QVERIFY(!loader.load());
			QVERIFY(!loader.isLoaded());
			QCOMPARE(spyLoaded.count(), 0);

			QTest::ignoreMessage(QtDebugMsg, "Shutdown UiLoader: QList()");
			loader.shutdown();
		}


		void fireShutdownNoUi() // no ui loaded
		{
			auto loader = QSharedPointer<UiLoader>::create();
			QSignalSpy spyShutdown(loader.get(), &UiLoader::fireRemovedAllPlugins);

			QTest::ignoreMessage(QtDebugMsg, "Shutdown UiLoader: QList()");
			loader->shutdown();
			QCoreApplication::processEvents(); // cannot use QTRY_COMPARE here
			QCOMPARE(spyShutdown.count(), 0);
		}


		void fireShutdownBeforeDtor() // ui loaded, shutdown before dtor
		{
			auto loader = QSharedPointer<UiLoader>::create();
			QSignalSpy spyShutdown(loader.data(), &UiLoader::fireRemovedAllPlugins);

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QVERIFY(loader->load<UiPluginJson>());

			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UiLoader: QList("json"))");
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UI: "json")");
			loader->shutdown();
			QTRY_COMPARE(spyShutdown.count(), 1); // clazy:exclude=qstring-allocations

			loader.reset();
			QTRY_COMPARE(spyShutdown.count(), 1); // clazy:exclude=qstring-allocations
		}


		void noFireShutdownDtor() // ui loaded, but dtor won't fire
		{
			auto loader = QSharedPointer<UiLoader>::create();
			QSignalSpy spyShutdown(loader.data(), &UiLoader::fireRemovedAllPlugins);

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "json")");
			QVERIFY(loader->load<UiPluginJson>());

			QSignalSpy spyDestroyed(loader->getLoaded<UiPluginJson>(), &QObject::destroyed);
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
			QTest::addColumn<QLatin1String>("name");

			QTest::newRow("qml") << "UiPluginQml"_L1;
			QTest::newRow("json") << "governikus::UiPluginJson"_L1;
			QTest::newRow("websocket") << "WebSocket"_L1;
			QTest::newRow("localifd") << "UiPluginLocalIfd"_L1;
			QTest::newRow("aidl") << "governikus::uipluginAidl"_L1;
			QTest::newRow("functional") << "UIPluginFunctional"_L1;
		}


		void getName()
		{
			QFETCH(QLatin1String, name);
			QCOMPARE(UiLoader::unify(name), QString::fromLatin1(QTest::currentDataTag()));
		}


};

QTEST_GUILESS_MAIN(test_UiLoader)
#include "test_UiLoader.moc"
