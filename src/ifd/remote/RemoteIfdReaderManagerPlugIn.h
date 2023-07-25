/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for smartphone as card reader (SaC).
 */

#pragma once

#include "IfdList.h"
#include "IfdReaderManagerPlugIn.h"

#include <QTimer>

namespace governikus
{

class RemoteIfdReaderManagerPlugIn
	: public IfdReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	private:
		QTimer mScanTimer;
		bool mConnectToPairedReaders;
		bool mConnectionCheckInProgress;

	private Q_SLOTS:
		void connectToPairedReaders();
		void continueConnectToPairedReaders(const QVector<QSharedPointer<IfdListEntry>>& pRemoteDevices);

	public:
		RemoteIfdReaderManagerPlugIn();
		~RemoteIfdReaderManagerPlugIn() override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;

	protected:
		IfdClient* getIfdClient() override;

};

} // namespace governikus
