/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderManagerPlugin.h"
#include "SmartReader.h"

#include <QScopedPointer>


class test_SmartReaderManagerPlugin;


namespace governikus
{

class SmartReaderManagerPlugin
	: public ReaderManagerPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugin)

	friend class ::test_SmartReaderManagerPlugin;

	private:
		bool mReaderAdded;
		QScopedPointer<SmartReader> mReader;

		void publishReader(const ReaderInfo& pInfo);

	private Q_SLOTS:
		void onSmartAvailableChanged(bool pSmartAvailable);

	public:
		SmartReaderManagerPlugin();

		[[nodiscard]] QPointer<Reader> getReader(const QString& pReaderName) const override;

		void init() override;
		void shutdown() override;

		void insert(const QString& pReaderName, const QVariant& pData) override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError) override;

		void shelveAll() const override;
};

} // namespace governikus
