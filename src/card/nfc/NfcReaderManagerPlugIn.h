/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for NFC on Android.
 */

#pragma once

#include "NfcReader.h"
#include "ReaderManagerPlugIn.h"

#include <QAtomicPointer>
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
		static QAtomicPointer<NfcReaderManagerPlugIn> instance;

		QScopedPointer<NfcReader> mNfcReader;

	private Q_SLOTS:
		void onNfcAdapterStateChanged(bool pEnabled);
		void onReaderDisconnected();

	public:
		static void setReaderMode(bool pEnabled);
		static void enqueueReaderMode(bool pEnabled);

		NfcReaderManagerPlugIn();
		~NfcReaderManagerPlugIn() override;

		[[nodiscard]] QList<Reader*> getReaders() const override;

		void init() override;
		void shutdown() override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;
};

} // namespace governikus
