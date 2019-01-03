/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for remote reader.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "messages/RemoteMessage.h"
#include "Reader.h"
#include "ReaderManagerPlugIn.h"

#include <QMap>
#include <QMultiMap>
#include <QSharedPointer>
#include <QTimer>

namespace governikus
{
class RemoteDispatcherClient;
class RemoteDeviceListEntry;

class RemoteReaderManagerPlugIn
	: public ReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	private:
		QTimer mScanTimer;
		QMultiMap<QString, QString> mReadersForDispatcher;
		QMap<QString, QSharedPointer<RemoteDispatcherClient> > mDispatcherList;
		QMap<QString, Reader*> mReaderList;
		bool mConnectToPairedReaders;
		bool mConnectionCheckInProgress;

		void removeDispatcher(const QString& pId);
		void removeAllDispatchers();

		void handleIFDStatus(const QJsonObject& pJsonObject, const QString& pId);

	private Q_SLOTS:
		void onContextEstablished(const QString& pIfdName, const QString& pId);
		void onRemoteMessage(RemoteCardMessageType pMessageType, const QJsonObject& pJsonObject, const QString& pId);
		void onDispatcherClosed(GlobalStatus::Code pCloseCode, const QString& pId);
		void addRemoteDispatcher(const QSharedPointer<RemoteDispatcherClient>& pRemoteDispatcher);
		void connectToPairedReaders();
		void continueConnectToPairedReaders(const QVector<QSharedPointer<RemoteDeviceListEntry> >& pRemoteDevices);

	public:
		RemoteReaderManagerPlugIn();
		virtual ~RemoteReaderManagerPlugIn() override;

		void init() override;
		virtual QList<Reader*> getReaders() const override;

		virtual void startScan(bool pAutoConnect) override;
		virtual void stopScan() override;

};

} // namespace governikus
