/*!
 * \brief Handler for messages on the server side of a remote reader scenario.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnection.h"
#include "DataChannel.h"
#include "messages/MessageReceiver.h"
#include "ReaderManager.h"
#include "RemoteDispatcher.h"

#include <QMap>
#include <QScopedPointer>
#include <QSharedPointer>


namespace governikus
{

class CreateCardConnectionCommand;

class ServerMessageHandler
	: public QObject
{
	Q_OBJECT

	protected:
		ServerMessageHandler() = default;

	public:
		virtual ~ServerMessageHandler();

		virtual void sendEstablishPaceChannelResponse(const QString& pSlotName, const EstablishPACEChannelOutput&) = 0;

	Q_SIGNALS:
		void fireEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage, const QSharedPointer<CardConnection>& pConnection);
		void fireClosed();
};


class ServerMessageHandlerImpl
	: public ServerMessageHandler
	, public MessageReceiver
{
	Q_OBJECT

	private:
		QPointer<ReaderManager> mReaderManager;
		const QSharedPointer<RemoteDispatcher> mRemoteDispatcher;
		QMap<QString, QSharedPointer<CardConnection> > mCardConnections;

		virtual void process(const QSharedPointer<const GetIfdStatus>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdConnect>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdDisconnect>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdTransmit>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage) override;

		virtual void unprocessed(const QSharedPointer<const RemoteMessage>& pMessage) override;
		void unexpectedMessage(const QSharedPointer<const RemoteMessage>& pMessage, bool pSendMessage = false);

	private Q_SLOTS:
		void onCreateCardConnectionCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand);
		void onTransmitCardCommandDone(QSharedPointer<BaseCardCommand> pCommand);
		void onClosed();
		void onReceived(const QSharedPointer<const RemoteMessage>& pMessage);
		void onReaderChanged(const QString& pReaderName);
		void onReaderRemoved(const QString& pReaderName);

	public:
		ServerMessageHandlerImpl(const QSharedPointer<DataChannel>& pDataChannel);

		virtual void sendEstablishPaceChannelResponse(const QString& pSlotName, const EstablishPACEChannelOutput& pChannelOutput) override;
};


} /* namespace governikus */
