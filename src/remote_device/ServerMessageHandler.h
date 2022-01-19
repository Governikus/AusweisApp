/*!
 * \brief Handler for messages on the server side of a smartphone as card reader (SaC) scenario.
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
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
		~ServerMessageHandler() override;

		virtual void sendEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput&) = 0;
		virtual void sendModifyPinResponse(const QString& pSlotHandle, const ResponseApdu& pResponseApdu) = 0;

	Q_SIGNALS:
		void fireCardConnectionEstablished(const QSharedPointer<CardConnection>& pConnection);
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
		QMap<QString, QSharedPointer<CardConnection>> mCardConnections;

		[[nodiscard]] QString slotHandleForReaderName(const QString& pReaderName) const;

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
		void onRemoteMessage(RemoteCardMessageType pMessageType, const QJsonObject& pJsonObject);
		void onReaderChanged(const ReaderInfo& pInfo);
		void onReaderRemoved(const ReaderInfo& pInfo);

	public:
		explicit ServerMessageHandlerImpl(const QSharedPointer<DataChannel>& pDataChannel);

		void sendEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput& pChannelOutput) override;
		void sendModifyPinResponse(const QString& pSlotHandle, const ResponseApdu& pResponseApdu) override;
};


} // namespace governikus
