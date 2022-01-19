/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for NFC on iOS.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Reader.h"
#include "ReaderManagerPlugIn.h"

#include <QScopedPointer>


namespace governikus
{

class IosReaderManagerPlugIn
	: public ReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	private:
		QScopedPointer<IosReader> mIosReader;

	private Q_SLOTS:
		void onReaderDisconnected();

	public:
		IosReaderManagerPlugIn();
		virtual ~IosReaderManagerPlugIn() override;

		virtual QList<Reader*> getReaders() const override;

		virtual void init() override;
		virtual void shutdown() override;

		virtual void startScan(bool pAutoConnect) override;
		virtual void stopScan(const QString& pError = QString()) override;
};

} // namespace governikus
