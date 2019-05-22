/*!
 * \brief Handler for messages on the server side of a remote reader scenario.
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "command/CreateCardConnectionCommand.h"
#include "CardConnection.h"
#include "DataChannel.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdModifyPin.h"
#include "ReaderManager.h"
#include "RemoteDispatcherServer.h"

#include <QMap>
#include <QScopedPointer>
#include <QSharedPointer>


namespace governikus
{

class ServerMessageHandler
	: public QObject
{
	Q_OBJECT

	protected:
		ServerMessageHandler() = default;

	public:
		virtual ~ServerMessageHandler();

		virtual void sendEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput&) = 0;
		virtual void sendModifyPinResponse(const QString& pSlotHandle, const ResponseApdu& pResponseApdu) = 0;

	Q_SIGNALS:
		void fireEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage, const QSharedPointer<CardConnection>& pConnection);
		void fireModifyPin(const QSharedPointer<const IfdModifyPin>& pMessage, const QSharedPointer<CardConnection>& pConnection);
		void fireClosed();
		void fireSecureMessagingStopped();
};


class ServerMessageHandlerImpl
	: public ServerMessageHandler
{
	Q_OBJECT

	private:
		QPointer<ReaderManager> mReaderManager;
		const QSharedPointer<RemoteDispatcherServer> mRemoteDispatcher;
		QMap<QString, QSharedPointer<CardConnection> > mCardConnections;

		QString slotHandleForReaderName(const QString& pReaderName) const;
		QString convertSlotHandleBackwardsCompatibility(const QString& pReaderName);

		void handleIfdGetStatus(const QJsonObject& pJsonObject);
		void handleIfdConnect(const QJsonObject& pJsonObject);
		void handleIfdDisconnect(const QJsonObject& pJsonObject);
		void handleIfdTransmit(const QJsonObject& pJsonObject);
		void handleIfdEstablishPaceChannel(const QJsonObject& pJsonObject);
		void handleIfdModifyPIN(const QJsonObject& pJsonObject);

	private Q_SLOTS:
		void onCreateCardConnectionCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand);
		void onTransmitCardCommandDone(QSharedPointer<BaseCardCommand> pCommand);
		void onClosed();
		void onRemoteMessage(RemoteCardMessageType pMessageType, const QJsonObject pJsonObject);
		void onReaderChanged(const QString& pReaderName);
		void onReaderRemoved(const QString& pReaderName);

	public:
		ServerMessageHandlerImpl(const QSharedPointer<DataChannel>& pDataChannel);

		virtual void sendEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput& pChannelOutput) override;
		virtual void sendModifyPinResponse(const QString& pSlotHandle, const ResponseApdu& pResponseApdu) override;
};


} // namespace governikus
