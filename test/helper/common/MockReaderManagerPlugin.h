/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MockReader.h"
#include "ReaderManagerPlugin.h"

#include <QMap>
#include <QSharedPointer>
#include <QString>


namespace governikus
{

class MockReaderManagerPlugin
	: public ReaderManagerPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugin" FILE "MockReaderManagerPlugin.metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugin)

	private:
		static MockReaderManagerPlugin* mInstance;
		QMap<QString, QSharedPointer<MockReader>> mReaders;

	public:
		MockReaderManagerPlugin();
		~MockReaderManagerPlugin() override = default;

		static MockReaderManagerPlugin& getInstance();

		void insert(const QString& pReaderName, const QVariant& pData) override;
		[[nodiscard]] QPointer<Reader> getReader(const QString& pReaderName) const override;
		void startScan(bool pAutoConnect) override;
		QPointer<MockReader> addReader(const QString& pReaderName = QStringLiteral("MockReader"), ReaderManagerPluginType pType = MockReader::cMOCKED_READERMANAGER_TYPE);
		void removeReader(const QString& pReaderName);
		void removeAllReader();

		void shelveAll() const override;

		using ReaderManagerPlugin::setPluginAvailable;
};


} // namespace governikus
