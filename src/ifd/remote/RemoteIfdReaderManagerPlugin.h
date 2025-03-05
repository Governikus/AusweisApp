/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdList.h"
#include "IfdReaderManagerPlugin.h"

#include <QStringList>
#include <QTimer>


class test_RemoteIfdReaderManagerPlugin;


namespace governikus
{

class RemoteIfdReaderManagerPlugin
	: public IfdReaderManagerPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugin)

	friend class ::test_RemoteIfdReaderManagerPlugin;

	private:
		QTimer mScanTimer;
		bool mConnectToPairedReaders;
		QByteArrayList mConnectionAttempts;

	private Q_SLOTS:
		void connectToPairedReaders() const;
		void continueConnectToPairedReaders(const QList<QSharedPointer<IfdListEntry>>& pRemoteDevices);
		void onDeviceVanished(const QSharedPointer<IfdListEntry>& pEntry);
		void onEstablishConnectionDone(const QSharedPointer<IfdListEntry>& pEntry, const GlobalStatus& pStatus);

	public:
		RemoteIfdReaderManagerPlugin();
		~RemoteIfdReaderManagerPlugin() override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;

	protected:
		IfdClient* getIfdClient() const override;

};

} // namespace governikus
