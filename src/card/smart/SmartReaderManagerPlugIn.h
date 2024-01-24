/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of ReaderManagerPlugIn for Smart-eID card.
 */

#pragma once

#include "ReaderManagerPlugIn.h"
#include "SmartReader.h"

#include <QScopedPointer>


namespace governikus
{

class SmartReaderManagerPlugIn
	: public ReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	private:
		bool mReaderAdded;
		QScopedPointer<SmartReader> mSmartReader;

		void publishReader(const ReaderInfo& pInfo);

	private Q_SLOTS:
		void onSmartAvailableChanged(bool pSmartAvailable);

	public:
		SmartReaderManagerPlugIn();

		[[nodiscard]] QList<Reader*> getReaders() const override;

		void init() override;
		void shutdown() override;

		void insert(const QString& pReaderName, const QVariant& pData) override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;
};

} // namespace governikus
