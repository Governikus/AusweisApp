/*!
 * \brief Implementation of ReaderManagerPlugIn for Smart-eID card.
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
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
		QScopedPointer<SmartReader> mSmartReader;
		bool initializeSmart(const QSharedPointer<SmartManager>& pSmartManager) const;
		bool isSmartAvailable(const QSharedPointer<SmartManager>& pSmartManager) const;

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
