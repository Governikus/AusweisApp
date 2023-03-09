/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Handler for messages on the server side of a smartphone as card reader (SaC) scenario.
 */

#pragma once

#include "DataChannel.h"
#include "IfdDispatcherServer.h"
#include "ReaderInfo.h"
#include "ReaderManagerPlugInInfo.h"
#include "ServerMessageHandler.h"
#include "command/BaseCardCommand.h"
#include "command/CreateCardConnectionCommand.h"
#include "messages/IfdMessage.h"

#include <QMap>
#include <QPointer>
#include <QVector>


namespace governikus
{

class ServerMessageHandlerImpl
	: public ServerMessageHandler
{
	Q_OBJECT

	private:
		const QSharedPointer<IfdDispatcherServer> mDispatcher;
		QVector<ReaderManagerPlugInType> mAllowedPlugInTypes;
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
		void onMessage(IfdMessageType pMessageType, const QJsonObject& pJsonObject);
		void onReaderChanged(const ReaderInfo& pInfo);
		void onReaderRemoved(const ReaderInfo& pInfo);

	public:
		explicit ServerMessageHandlerImpl(const QSharedPointer<DataChannel>& pDataChannel,
				const QVector<ReaderManagerPlugInType>& pAllowedPlugInTypes = Enum<ReaderManagerPlugInType>::getList());

		void sendEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput& pChannelOutput) override;
		void sendModifyPinResponse(const QString& pSlotHandle, const ResponseApdu& pResponseApdu) override;
};


} // namespace governikus
