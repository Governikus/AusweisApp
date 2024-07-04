/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of \ref ReaderManagerPlugin for NFC on Android.
 */

#pragma once

#include "NfcReader.h"
#include "ReaderManagerPlugin.h"

#include <QAtomicPointer>
#include <QScopedPointer>


namespace governikus
{

class NfcReaderManagerPlugin
	: public ReaderManagerPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugin)

	private:
		static QAtomicPointer<NfcReaderManagerPlugin> instance;

		QScopedPointer<NfcReader> mNfcReader;

	private Q_SLOTS:
		void onNfcAdapterStateChanged(bool pEnabled);
		void onReaderDisconnected();

	public:
		static void setReaderMode(bool pEnabled);
		static void enqueueReaderMode(bool pEnabled);

		NfcReaderManagerPlugin();
		~NfcReaderManagerPlugin() override;

		[[nodiscard]] QList<Reader*> getReaders() const override;

		void init() override;
		void shutdown() override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;
};

} // namespace governikus
