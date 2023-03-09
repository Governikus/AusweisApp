/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref UILoader
 */

#include "UILoader.h"

#include "UIPlugInProxy.h"

#include <QSignalSpy>
#include <QtTest>

using namespace governikus;

Q_IMPORT_PLUGIN(UIPlugInProxy)

class test_UILoader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isLoaded()
		{
			UILoader loader;
			QSignalSpy spyLoaded(&loader, &UILoader::fireLoadedPlugin);

			QTest::ignoreMessage(QtDebugMsg, R"(Try to load UI plugin: "proxy")");
			QVERIFY(loader.load<UIPlugInProxy>());
			QVERIFY(loader.isLoaded());
			QVERIFY(loader.getLoaded<UIPlugInProxy>());
			QCOMPARE(spyLoaded.count(), 1);
			QVERIFY(loader.initialize());
			QVERIFY(!loader.requiresReaderManager());

			const auto* ui = loader.getLoaded<UIPlugInProxy>();
			QCOMPARE(ui->property("default"), QVariant());
			QCOMPARE(ui->property("passive"), QVariant());
			QCOMPARE(ui->property("readerManager"), QVariant(false));

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: QList("proxy"))");
#else
			QTest::ignoreMessage(QtDebugMsg, R"(Shutdown UILoader: ("proxy"))");
#endif
		}


};

QTEST_GUILESS_MAIN(test_UILoader)
#include "test_UILoader.moc"
