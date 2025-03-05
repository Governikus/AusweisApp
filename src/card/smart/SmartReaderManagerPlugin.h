/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderManagerPlugin.h"
#include "SmartReader.h"

#include <QScopedPointer>


namespace governikus
{

class SmartReaderManagerPlugin
	: public ReaderManagerPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugin)

	private:
		bool mReaderAdded;
		QScopedPointer<SmartReader> mSmartReader;

		void publishReader(const ReaderInfo& pInfo);

	private Q_SLOTS:
		void onSmartAvailableChanged(bool pSmartAvailable);

	public:
		SmartReaderManagerPlugin();

		[[nodiscard]] QList<Reader*> getReaders() const override;

		void init() override;
		void shutdown() override;

		void insert(const QString& pReaderName, const QVariant& pData) override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;
};

} // namespace governikus
