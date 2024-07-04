/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref UiLoader
 */

#include "UiLoader.h"

#include "UiPluginProxy.h"

#include <QSignalSpy>
#include <QtTest>

using namespace governikus;

Q_IMPORT_PLUGIN(UiPluginProxy)

class test_UiLoader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isLoaded()
		{
			UiLoader loader;
			QSignalSpy spyLoaded(&loader, &UiLoader::fireLoadedPlugin);

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "proxy")");
			QVERIFY(loader.load<UiPluginProxy>());
			QVERIFY(loader.isLoaded());
			QVERIFY(loader.getLoaded<UiPluginProxy>());
			QCOMPARE(spyLoaded.count(), 1);
			QVERIFY(loader.initialize());
			QVERIFY(!loader.requiresReaderManager());

			const auto* ui = loader.getLoaded<UiPluginProxy>();
			QCOMPARE(ui->property("default"), QVariant());
			QCOMPARE(ui->property("passive"), QVariant());
			QCOMPARE(ui->property("readerManager"), QVariant(false));

			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UiLoader: QList("proxy"))");
		}


};

QTEST_GUILESS_MAIN(test_UiLoader)
#include "test_UiLoader.moc"
