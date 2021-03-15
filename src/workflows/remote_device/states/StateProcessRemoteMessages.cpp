/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "StateProcessRemoteMessages.h"

#include "ServerMessageHandler.h"

#include <QDebug>

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;


StateProcessRemoteMessages::StateProcessRemoteMessages(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
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

	mConnections += connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireStatusChanged, this, &StateProcessRemoteMessages::onReaderStatusChanged);
	mConnections += connect(server.data(), &RemoteServer::fireMessageHandlerAdded, this, &StateProcessRemoteMessages::onMessageHandlerAdded);
	mConnections += connect(server.data(), &RemoteServer::fireConnectedChanged, this, &StateProcessRemoteMessages::onConnectedChanged);

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


void StateProcessRemoteMessages::onConnectedChanged(bool pConnected)
{
	if (pConnected && !getContext()->getRemoteServer()->isPairingConnection())
	{
		startScanIfNecessary();
	}
	else
	{
		stopScanIfNecessary();
	}
}


void StateProcessRemoteMessages::onClosed()
{
	qCDebug(statemachine) << "ServerMessageHandler closed";

	for (const auto& connection : qAsConst(mMessageConnections))
	{
		disconnect(connection);
	}

	qCDebug(statemachine) << "Resetting all PACE passwords and further relevant context information.";
	getContext()->onResetMessageHandler();
}


void StateProcessRemoteMessages::onReaderStatusChanged(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() != ReaderManagerPlugInType::NFC)
	{
		return;
	}

	if (Env::getSingleton<ReaderManager>()->isScanRunning(ReaderManagerPlugInType::NFC))
	{
		return;
	}

	const auto& context = getContext();
	if (context->getRemoteServer()->isConnected())
	{
		Q_EMIT fireAbort();
	}
}


void StateProcessRemoteMessages::onEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage, const QSharedPointer<CardConnection>& pConnection)
{
	Q_ASSERT(pMessage);

	const auto& context = getContext();
	context->setEstablishPaceChannel(pMessage);
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


void StateProcessRemoteMessages::onEntry(QEvent* pEvent)
{
	onConnectedChanged(getContext()->getRemoteServer()->isConnected());

	AbstractState::onEntry(pEvent);
}


void StateProcessRemoteMessages::onExit(QEvent* pEvent)
{
	for (const auto& connection : qAsConst(mMessageConnections))
	{
		disconnect(connection);
	}

	AbstractState::onExit(pEvent);
}
