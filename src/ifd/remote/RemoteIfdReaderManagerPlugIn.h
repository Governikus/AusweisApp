/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for smartphone as card reader (SaC).
 */

#pragma once

#include "IfdList.h"
#include "IfdReaderManagerPlugIn.h"

#include <QStringList>
#include <QTimer>

class test_RemoteIfdReaderManagerPlugIn;

namespace governikus
{

class RemoteIfdReaderManagerPlugIn
	: public IfdReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	friend class ::test_RemoteIfdReaderManagerPlugIn;

	private:
		QTimer mScanTimer;
		bool mConnectToPairedReaders;
		QStringList mConnectionAttempts;

	private Q_SLOTS:
		void connectToPairedReaders();
		void continueConnectToPairedReaders(const QVector<QSharedPointer<IfdListEntry>>& pRemoteDevices);
		void onDeviceVanished(const QSharedPointer<IfdListEntry>& pEntry);
		void onEstablishConnectionDone(const QSharedPointer<IfdListEntry>& pEntry, const GlobalStatus& pStatus);

	public:
		RemoteIfdReaderManagerPlugIn();
		~RemoteIfdReaderManagerPlugIn() override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;

	protected:
		IfdClient* getIfdClient() override;

};

} // namespace governikus
