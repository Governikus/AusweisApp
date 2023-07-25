/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
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

	mConnections += connect(server.data(), &IfdServer::fireMessageHandlerAdded, this, &StateProcessIfdMessages::onMessageHandlerAdded);

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

	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireCardConnected, this, &StateProcessIfdMessages::onCardConnected, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireDisplayTextChanged, this, &StateProcessIfdMessages::onDisplayTextChanged, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireEstablishPaceChannel, this, &StateProcessIfdMessages::onEstablishPaceChannel, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireModifyPin, this, &StateProcessIfdMessages::onModifyPin, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireCardDisconnected, this, &StateProcessIfdMessages::onCardDisconnected, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::destroyed, this, &StateProcessIfdMessages::onClosed, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireSecureMessagingStopped, this, &StateProcessIfdMessages::fireSecureMessagingStopped, Qt::UniqueConnection);
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


void StateProcessIfdMessages::onCardDisconnected()
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
