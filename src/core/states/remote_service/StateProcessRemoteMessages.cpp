/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateProcessRemoteMessages.h"

#include "ServerMessageHandler.h"


using namespace governikus;


StateProcessRemoteMessages::StateProcessRemoteMessages(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
	, mMessageConnections()
{
}


StateProcessRemoteMessages::~StateProcessRemoteMessages()
{
}


void StateProcessRemoteMessages::run()
{
	const QSharedPointer<RemoteServiceContext> context = getContext();
	const QSharedPointer<RemoteServer> server = context->getRemoteServer();
	Q_ASSERT(server);

	mConnections += connect(server.data(), &RemoteServer::fireMessageHandlerAdded, this, &StateProcessRemoteMessages::onMessageHandlerAdded);

	const auto messageHandler = server->getMessageHandler();
	if (messageHandler)
	{
		onMessageHandlerAdded(server->getMessageHandler());
	}
	else
	{
		context->onResetMessageHandler();
	}
}


void StateProcessRemoteMessages::onMessageHandlerAdded(const QSharedPointer<ServerMessageHandler>& pHandler)
{
	if (!pHandler)
	{
		return;
	}

	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireEstablishPaceChannel, this, &StateProcessRemoteMessages::onEstablishPaceChannel, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireModifyPin, this, &StateProcessRemoteMessages::onModifyPin, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireClosed, this, &StateProcessRemoteMessages::onClosed, Qt::UniqueConnection);
}


void StateProcessRemoteMessages::onClosed()
{
	for (const auto& connection : qAsConst(mMessageConnections))
	{
		disconnect(connection);
	}

	getContext()->onResetMessageHandler();
}


void StateProcessRemoteMessages::onEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage, const QSharedPointer<CardConnection>& pConnection)
{
	Q_ASSERT(pMessage);

	getContext()->setEstablishPaceChannelMessage(pMessage);
	getContext()->setCardConnection(pConnection);
	Q_EMIT fireEstablishPaceChannel();
}


void StateProcessRemoteMessages::onModifyPin(const QSharedPointer<const IfdModifyPin>& pMessage, const QSharedPointer<CardConnection>& pConnection)
{
	Q_ASSERT(pMessage);

	const QSharedPointer<RemoteServiceContext> context = getContext();
	Q_ASSERT(context);

	context->setModifyPinMessage(pMessage);
	context->setCardConnection(pConnection);
	Q_EMIT fireModifyPin();
}


void StateProcessRemoteMessages::onExit(QEvent* pEvent)
{
	for (const auto& connection : qAsConst(mMessageConnections))
	{
		disconnect(connection);
	}

	AbstractState::onExit(pEvent);
}
