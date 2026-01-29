/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
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
		QScopedPointer<SimulatorReader> mReader;

	public:
		SimulatorReaderManagerPlugin();

		[[nodiscard]] QPointer<Reader> getReader(const QString& pReaderName) const override;

		void init() override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError) override;

		void insert(const QString& pReaderName, const QVariant& pData) override;

		void shelveAll() const override;

	private Q_SLOTS:
		void onSettingsChanged();
};

} // namespace governikus
