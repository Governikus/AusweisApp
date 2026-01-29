/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerPlugin.h"

#include <QSignalSpy>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class SimpleReaderManagerPlugin
	: public ReaderManagerPlugin
{
	Q_OBJECT

	public:
		using ReaderManagerPlugin::setPluginAvailable;

		SimpleReaderManagerPlugin()
			: ReaderManagerPlugin(ReaderManagerPluginType::MOCK)
		{
		}


		[[nodiscard]] QPointer<Reader> getReader(const QString& pReaderName) const override
		{
			Q_UNUSED(pReaderName)
			return nullptr;
		}


		void shelveAll() const override
		{
		}


};


class test_ReaderManagerPlugin
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void setPluginAvailable()
		{
			SimpleReaderManagerPlugin plugin;
			QSignalSpy spy(&plugin, &ReaderManagerPlugin::fireStatusChanged);
			QCOMPARE(plugin.getInfo().isAvailable(), false);

			plugin.setPluginAvailable(false);
			QCOMPARE(plugin.getInfo().isAvailable(), false);
			QCOMPARE(spy.count(), 1);

			plugin.setPluginAvailable(true);
			QCOMPARE(plugin.getInfo().isAvailable(), true);
			QCOMPARE(spy.count(), 2);

			plugin.setPluginAvailable(true);
			QCOMPARE(plugin.getInfo().isAvailable(), true);
			QCOMPARE(spy.count(), 3);
		}


		void reset()
		{
			SimpleReaderManagerPlugin plugin;

			QTest::ignoreMessage(QtDebugMsg, "Shutdown ReaderManagerPluginType::MOCK");
			plugin.reset();
		}


		void insert()
		{
			SimpleReaderManagerPlugin plugin;

			QTest::ignoreMessage(QtDebugMsg, "insert is not supported by ReaderManagerPluginType::MOCK");
			plugin.insert("Card"_L1, QVariant());
		}


};

QTEST_GUILESS_MAIN(test_ReaderManagerPlugin)
#include "test_ReaderManagerPlugin.moc"
