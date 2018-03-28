/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for NFC on Android.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Reader.h"
#include "ReaderManagerPlugIn.h"

#include <QNearFieldManager>
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
		bool mEnabled;
		QScopedPointer<NfcReader> mNfcReader;

	private Q_SLOTS:
		void onNfcAdapterStateChanged(bool pEnabled);

	public:
		NfcReaderManagerPlugIn();
		virtual ~NfcReaderManagerPlugIn() override;

		virtual QList<Reader*> getReaders() const override;

		virtual void init() override;
		virtual void shutdown() override;
};

} /* namespace governikus */
