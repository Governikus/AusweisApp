/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateProcessRemoteMessages.h"

#include "ServerMessageHandler.h"

#include <QDebug>

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;


StateProcessRemoteMessages::StateProcessRemoteMessages(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
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
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::destroyed, this, &StateProcessRemoteMessages::onClosed, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireSecureMessagingStopped, this, &StateProcessRemoteMessages::fireSecureMessagingStopped, Qt::UniqueConnection);
}


void StateProcessRemoteMessages::onClosed()
{
	qCDebug(statemachine) << "ServerMessageHandler closed";

	for (const auto& connection : qAsConst(mMessageConnections))
	{
		disconnect(connection);
	}

	qCDebug(statemachine) << "Reseting all PACE passwords and further relevant context information.";
	getContext()->onResetMessageHandler();
}


void StateProcessRemoteMessages::onEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage, const QSharedPointer<CardConnection>& pConnection)
{
	Q_ASSERT(pMessage);

	const auto& context = getContext();
	context->setEstablishPaceChannelMessage(pMessage);
	context->setCardConnection(pConnection);
	context->setReaderName(pConnection->getReaderInfo().getName());

	Q_EMIT fireEstablishPaceChannel();
}


void StateProcessRemoteMessages::onModifyPin(const QSharedPointer<const IfdModifyPin>& pMessage, const QSharedPointer<CardConnection>& pConnection)
{
	Q_ASSERT(pMessage);

	const auto& context = getContext();
	context->setModifyPinMessage(pMessage);
	context->setCardConnection(pConnection);
	context->setReaderName(pConnection->getReaderInfo().getName());

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
