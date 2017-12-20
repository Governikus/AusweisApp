/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for remote reader.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "messages/MessageReceiver.h"
#include "Reader.h"
#include "ReaderManagerPlugIn.h"

#include <QMap>
#include <QMultiMap>
#include <QSharedPointer>
#include <QTimer>

namespace governikus
{
class IfdStatus;
class RemoteClient;
class ReaderDescription;
class RemoteDeviceListEntry;

class RemoteReaderManagerPlugIn
	: public ReaderManagerPlugIn
	, public MessageReceiver
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	private:
		QTimer mScanTimer;
		QSharedPointer<RemoteClient> mRemoteClient;
		QWeakPointer<RemoteDispatcher> mRemoteToUpdate;
		QMultiMap<QSharedPointer<RemoteDispatcher>, QString> mRemoteDispatchers;
		QMap<QString, Reader*> mReaderList;

		void updateReader(const IfdStatus& pIfdStatus);
		void removeDispatcher(const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher);
		void removeAllDispatchers();
		void connectToPairedReaders();
		void unexpectedMessage(const QSharedPointer<const RemoteMessage>& pMessage, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher = QSharedPointer<RemoteDispatcher>(), bool pSendMessage = false);

	private Q_SLOTS:
		void onRemoteMessage(const QSharedPointer<const RemoteMessage>& pMessage, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher);
		void onDispatcherClosed(GlobalStatus::Code pCloseCode, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher);
		void addRemoteDispatcher(const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher);
		void checkRemoteDevices();
		void continueConnectToPairedReaders(const QVector<QSharedPointer<RemoteDeviceListEntry> >& pRemoteDevices);

	protected:
		virtual void onConnectToKnownReadersChanged() override;

	public:
		RemoteReaderManagerPlugIn();
		virtual ~RemoteReaderManagerPlugIn() override;

		virtual void setRemoteClient(const QSharedPointer<RemoteClient>& pRemoteClient) override;
		virtual QList<Reader*> getReaders() const override;

		virtual void process(const QSharedPointer<const IfdEstablishContextResponse>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdStatus>& pMessage) override;

		virtual void unprocessed(const QSharedPointer<const RemoteMessage>& pMessage) override;

		virtual void startScan() override;
		virtual void stopScan() override;

};

} /* namespace governikus */
