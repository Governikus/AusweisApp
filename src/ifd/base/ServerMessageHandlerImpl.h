/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DataChannel.h"
#include "IfdDispatcherServer.h"
#include "ReaderInfo.h"
#include "ReaderManagerPluginInfo.h"
#include "ServerMessageHandler.h"
#include "command/BaseCardCommand.h"
#include "command/CreateCardConnectionCommand.h"
#include "messages/IfdMessage.h"

#include <QList>
#include <QMap>
#include <QPointer>


namespace governikus
{

class ServerMessageHandlerImpl
	: public ServerMessageHandler
{
	Q_OBJECT

	private:
		const QSharedPointer<IfdDispatcherServer> mDispatcher;
		QList<ReaderManagerPluginType> mAllowedPluginTypes;
		QList<ReaderManagerPluginType> mAllowedCardTypes;
		QMap<QString, QSharedPointer<CardConnection>> mCardConnections;

		[[nodiscard]] QString slotHandleForReaderName(const QString& pReaderName) const;
		[[nodiscard]] bool isAllowed(const QSharedPointer<CardConnection>& pCardConnection, QStringView pCommand) const;

		void handleIfdGetStatus(const QJsonObject& pJsonObject);
		void handleIfdConnect(const QJsonObject& pJsonObject);
		void handleIfdDisconnect(const QJsonObject& pJsonObject);
		void handleIfdTransmit(const QJsonObject& pJsonObject);
		void handleIfdEstablishPaceChannel(const QJsonObject& pJsonObject);
		void handleIfdDestroyPaceChannel(const QJsonObject& pJsonObject);
		void handleIfdModifyPIN(const QJsonObject& pJsonObject);
		void sendIfdStatus(const ReaderInfo& pReaderInfo);

	private Q_SLOTS:
		void onCreateCardConnectionCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand);
		void onTransmitCardCommandDone(QSharedPointer<BaseCardCommand> pCommand);
		void onDestroyPaceChannelCommandDone(QSharedPointer<BaseCardCommand> pCommand);
		void onClosed();
		void onMessage(IfdMessageType pMessageType, const QJsonObject& pJsonObject);
		void onReaderChanged(const ReaderInfo& pInfo);
		void onReaderRemoved(const ReaderInfo& pInfo);

	public:
		explicit ServerMessageHandlerImpl(const QSharedPointer<DataChannel>& pDataChannel,
				const QList<ReaderManagerPluginType>& pAllowedTypes = Enum<ReaderManagerPluginType>::getList());

		void sendEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput& pChannelOutput) override;
		void sendModifyPinResponse(const QString& pSlotHandle, const ResponseApdu& pResponseApdu) override;
		void setAllowedCardTypes(const QList<ReaderManagerPluginType>& pAllowedCardTypes) override;
};


} // namespace governikus
