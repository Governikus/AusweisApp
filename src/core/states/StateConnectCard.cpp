/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "ReaderManager.h"
#include "StateConnectCard.h"
#if defined(Q_OS_ANDROID)
#include "SurveyModel.h"
#endif

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;

StateConnectCard::StateConnectCard(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateConnectCard::run()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	mConnections += connect(readerManager, &ReaderManager::fireCardInserted, this, &StateConnectCard::onCardInserted);
	mConnections += connect(readerManager, &ReaderManager::fireReaderRemoved, this, &StateConnectCard::onReaderRemoved);
	onCardInserted();
}


void StateConnectCard::onCardInserted()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	ReaderInfo readerInfo = readerManager->getReaderInfo(getContext()->getReaderName());
	if (readerInfo.hasEidCard())
	{
#if defined(Q_OS_ANDROID)
		Env::getSingleton<SurveyModel>()->setMaximumNfcPacketLength(readerInfo.getMaxApduLength());
#endif
		qCDebug(statemachine) << "Card has been inserted, trying to connect";
		mConnections += readerManager->callCreateCardConnectionCommand(readerInfo.getName(), this, &StateConnectCard::onCommandDone);
	}
}


void StateConnectCard::onCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand)
{
	qCDebug(statemachine) << "Card connection command completed";
	if (pCommand->getCardConnection() == nullptr)
	{
		qCDebug(statemachine) << "Card connection failed";
		Q_EMIT fireAbort();
		return;
	}

	qCDebug(statemachine) << "Card connection was successful";
	getContext()->setCardConnection(pCommand->getCardConnection());

	const auto readerManager = Env::getSingleton<ReaderManager>();
	ReaderInfo readerInfo = readerManager->getReaderInfo(getContext()->getReaderName());
	if (readerInfo.sufficientApduLength() && (!readerInfo.isPinDeactivated() || getContext()->isCanAllowedMode()))
	{
		Q_EMIT fireContinue();
	}
	else if (readerInfo.isPinDeactivated() && !getContext()->isCanAllowedMode())
	{
		getContext()->getCardConnection()->setProgressMessage(tr("The online identification function is disabled."));
	}
}


void StateConnectCard::onReaderRemoved(const QString& pReaderName)
{
	if (pReaderName == getContext()->getReaderName())
	{
		Q_EMIT fireRetry();
	}
}


void StateConnectCard::onEntry(QEvent* pEvent)
{
	const WorkflowContext* const context = getContext().data();
	Q_ASSERT(context);

	/*
	 * Note: the plugin types to be used in this state must be already set in the workflow context before this state is entered.
	 * Changing the plugin types in the context, e.g. from {NFC} to {BLUETOOTH}, causes the state to be left with a fireRetry signal.
	 */
	mConnections += connect(context, &WorkflowContext::fireReaderPlugInTypesChanged, this, &StateConnectCard::fireRetry);

	AbstractState::onEntry(pEvent);
}
