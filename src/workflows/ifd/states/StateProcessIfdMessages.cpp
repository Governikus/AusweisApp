/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateProcessIfdMessages.h"

#include "ServerMessageHandler.h"
#include "VolatileSettings.h"

#include <QDebug>

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;


StateProcessIfdMessages::StateProcessIfdMessages(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
	, mResetContextOnDisconnect(false)
	, mMessageConnections()
{
}


void StateProcessIfdMessages::run()
{
	const QSharedPointer<IfdServiceContext> context = getContext();
	const QSharedPointer<IfdServer> server = context->getIfdServer();
	Q_ASSERT(server);

	*this << connect(server.data(), &IfdServer::fireMessageHandlerAdded, this, &StateProcessIfdMessages::onMessageHandlerAdded);
	*this << connect(server.data(), &IfdServer::fireConnectedChanged, this, &StateProcessIfdMessages::onConnectedChanged);
	*this << connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderPropertiesUpdated, this, &StateProcessIfdMessages::onReaderPropertiesUpdated);

	const auto messageHandler = server->getMessageHandler();
	if (messageHandler)
	{
		onMessageHandlerAdded(server->getMessageHandler());
	}
	else
	{
		context->reset();
	}
}


void StateProcessIfdMessages::onMessageHandlerAdded(const QSharedPointer<ServerMessageHandler>& pHandler)
{
	if (!pHandler)
	{
		return;
	}

	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireCardConnected, this, &StateProcessIfdMessages::onCardConnected);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireDisplayTextChanged, this, &StateProcessIfdMessages::onDisplayTextChanged);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireEstablishPaceChannel, this, &StateProcessIfdMessages::onEstablishPaceChannel);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireModifyPin, this, &StateProcessIfdMessages::onModifyPin);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireCardDisconnected, this, &StateProcessIfdMessages::onCardDisconnected);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::destroyed, this, &StateProcessIfdMessages::onClosed);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireSecureMessagingStopped, this, &StateProcessIfdMessages::fireSecureMessagingStopped);
}


void StateProcessIfdMessages::onClosed()
{
	qCDebug(statemachine) << "ServerMessageHandler closed";

	for (const auto& connection : std::as_const(mMessageConnections))
	{
		disconnect(connection);
	}

	getContext()->reset();
}


void StateProcessIfdMessages::onConnectedChanged(bool pConnected) const
{
	if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		return;
	}

	if (pConnected && !getContext()->getIfdServer()->isPairingConnection())
	{
		Env::getSingleton<ReaderManager>()->startScan(ReaderManagerPluginType::SMART);
	}
	else
	{
		Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPluginType::SMART);
	}
}


void StateProcessIfdMessages::onReaderPropertiesUpdated(const ReaderInfo& pInfo) const
{
	if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		return;
	}

	if (pInfo.isInsertable() && pInfo.getCardInfo().getCardType() == CardType::NONE)
	{
		Env::getSingleton<ReaderManager>()->insert(pInfo);
	}
}


void StateProcessIfdMessages::onCardConnected()
{
	mResetContextOnDisconnect = false;
}


void StateProcessIfdMessages::onDisplayTextChanged(const QString& pDisplayText) const
{
	getContext()->setDisplayText(pDisplayText);
}


void StateProcessIfdMessages::onEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage, const QSharedPointer<CardConnection>& pConnection)
{
	Q_ASSERT(pMessage);

	const auto& context = getContext();
	context->setEstablishPaceChannel(pMessage);
	context->setCardConnection(pConnection);
	context->setReaderName(pConnection->getReaderInfo().getName());
	mResetContextOnDisconnect = true;

	Q_EMIT fireEstablishPaceChannel();
}


void StateProcessIfdMessages::onModifyPin(const QSharedPointer<const IfdModifyPin>& pMessage, const QSharedPointer<CardConnection>& pConnection)
{
	Q_ASSERT(pMessage);

	const auto& context = getContext();
	context->setModifyPinMessage(pMessage);
	context->setCardConnection(pConnection);
	context->setReaderName(pConnection->getReaderInfo().getName());

	Q_EMIT fireModifyPin();
}


void StateProcessIfdMessages::onCardDisconnected() const
{
	getContext()->setDisplayText(QString());
	if (mResetContextOnDisconnect)
	{
		getContext()->reset();
	}
}


void StateProcessIfdMessages::onExit(QEvent* pEvent)
{
	for (const auto& connection : std::as_const(mMessageConnections))
	{
		disconnect(connection);
	}

	AbstractState::onExit(pEvent);
}
