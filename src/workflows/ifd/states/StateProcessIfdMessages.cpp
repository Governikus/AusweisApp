/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
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

	mConnections += connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireStatusChanged, this, &StateProcessIfdMessages::onReaderStatusChanged);
	mConnections += connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderPropertiesUpdated, this, &StateProcessIfdMessages::onReaderPropertiesUpdated);
	mConnections += connect(server.data(), &IfdServer::fireMessageHandlerAdded, this, &StateProcessIfdMessages::onMessageHandlerAdded);
	mConnections += connect(server.data(), &IfdServer::fireConnectedChanged, this, &StateProcessIfdMessages::onConnectedChanged);

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
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireEstablishPaceChannel, this, &StateProcessIfdMessages::onEstablishPaceChannel, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireModifyPin, this, &StateProcessIfdMessages::onModifyPin, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireCardDisconnected, this, &StateProcessIfdMessages::onCardDisconnected, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::destroyed, this, &StateProcessIfdMessages::onClosed, Qt::UniqueConnection);
	mMessageConnections += connect(pHandler.data(), &ServerMessageHandler::fireSecureMessagingStopped, this, &StateProcessIfdMessages::fireSecureMessagingStopped, Qt::UniqueConnection);
}


void StateProcessIfdMessages::onConnectedChanged(bool pConnected)
{
	if (pConnected && !getContext()->getIfdServer()->isPairingConnection())
	{
		startNfcScanIfNecessary();
		Env::getSingleton<ReaderManager>()->startScan(ReaderManagerPlugInType::SMART);
	}
	else
	{
		stopNfcScanIfNecessary();
		Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPlugInType::SMART);
	}
}


void StateProcessIfdMessages::onClosed()
{
	qCDebug(statemachine) << "ServerMessageHandler closed";

	for (const auto& connection : qAsConst(mMessageConnections))
	{
		disconnect(connection);
	}

	getContext()->reset();
}


void StateProcessIfdMessages::onReaderStatusChanged(const ReaderManagerPlugInInfo& pInfo)
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
	if (context->getIfdServer()->isConnected())
	{
		Q_EMIT fireAbort();
	}
}


void StateProcessIfdMessages::onReaderPropertiesUpdated(const ReaderInfo& pInfo)
{
	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		if (pInfo.isInsertable() && pInfo.getCardInfo().getCardType() == CardType::NONE)
		{
			Env::getSingleton<ReaderManager>()->insert(pInfo);
		}
	}
}


void StateProcessIfdMessages::onCardConnected()
{
	mResetContextOnDisconnect = false;
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
	if (mResetContextOnDisconnect)
	{
		getContext()->reset();
	}
}


void StateProcessIfdMessages::onEntry(QEvent* pEvent)
{
	onConnectedChanged(getContext()->getIfdServer()->isConnected());

	AbstractState::onEntry(pEvent);
}


void StateProcessIfdMessages::onExit(QEvent* pEvent)
{
	for (const auto& connection : qAsConst(mMessageConnections))
	{
		disconnect(connection);
	}

	AbstractState::onExit(pEvent);
}
