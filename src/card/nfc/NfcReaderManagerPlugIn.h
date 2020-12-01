/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for NFC on Android.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "NfcReader.h"
#include "ReaderManagerPlugIn.h"

#include <QScopedPointer>


namespace governikus
{

class NfcReaderManagerPlugIn
	: public ReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	private:
		QScopedPointer<NfcReader> mNfcReader;

	private Q_SLOTS:
		void onNfcAdapterStateChanged(bool pEnabled);
		void onReaderDisconnected();

	public:
		NfcReaderManagerPlugIn();
		~NfcReaderManagerPlugIn() override;

		QList<Reader*> getReaders() const override;

		void init() override;
		void shutdown() override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;
};

} // namespace governikus
