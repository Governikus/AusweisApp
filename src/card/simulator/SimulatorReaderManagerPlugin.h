/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderManagerPlugin.h"
#include "SimulatorReader.h"

#include <QScopedPointer>


namespace governikus
{

class SimulatorReaderManagerPlugin
	: public ReaderManagerPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugin)

	private:
		QScopedPointer<SimulatorReader> mSimulatorReader;

	public:
		SimulatorReaderManagerPlugin();

		[[nodiscard]] QList<Reader*> getReaders() const override;

		void init() override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;

		void insert(const QString& pReaderName, const QVariant& pData) override;

	private Q_SLOTS:
		void onSettingsChanged();
};

} // namespace governikus
