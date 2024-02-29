/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateConnectCard.h"

#include "CardConnection.h"
#include "ReaderManager.h"
#include "SurveyModel.h"
#include "VolatileSettings.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;

StateConnectCard::StateConnectCard(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateConnectCard::run()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	*this << connect(readerManager, &ReaderManager::fireCardInserted, this, &StateConnectCard::onCardInserted);
	*this << connect(readerManager, &ReaderManager::fireCardRemoved, this, &StateConnectCard::onUnusableCardConnectionLost);
	*this << connect(readerManager, &ReaderManager::fireReaderRemoved, this, &StateConnectCard::onUnusableCardConnectionLost);
	onCardInserted();
}


void StateConnectCard::onCardInserted()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	ReaderInfo readerInfo = readerManager->getReaderInfo(getContext()->getReaderName());
	Env::getSingleton<SurveyModel>()->setReaderInfo(readerInfo);

	if (readerInfo.hasEid())
	{
		qCDebug(statemachine) << "Card has been inserted, trying to connect";
		*this << readerManager->callCreateCardConnectionCommand(readerInfo.getName(), this, &StateConnectCard::onCommandDone);
	}
}


void StateConnectCard::onCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand)
{
	const auto& cardConnection = pCommand->getCardConnection();

	qCDebug(statemachine) << "Card connection command completed";
	if (cardConnection == nullptr)
	{
		qCDebug(statemachine) << "Card connection failed";
		updateStatus(GlobalStatus::Code::Card_Communication_Error);
		Q_EMIT fireAbort(FailureCode::Reason::Connect_Card_Connection_Failed);
		return;
	}

	const auto& context = getContext();
	qCDebug(statemachine) << "Card connection was successful";
	context->setCardConnection(cardConnection);

	const auto& readerInfo = cardConnection->getReaderInfo();
	if (readerInfo.insufficientApduLength())
	{
		//: INFO IOS
		context->getCardConnection()->setProgressMessage(tr("The used card reader does not meet the technical requirements (Extended Length not supported)."));
		return;
	}

	if (context->eidTypeMismatch())
	{
		//: INFO IOS
		context->getCardConnection()->setProgressMessage(tr("The used ID card type is not accepted by the server."));
		return;
	}

	if (readerInfo.isPinDeactivated() && !context->isCanAllowedMode())
	{
		qCDebug(statemachine) << "The eID function of the ID card is not activated.";
		const GlobalStatus status = GlobalStatus::Code::Card_Pin_Deactivated;
		if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
		{
			context->getCardConnection()->setProgressMessage(status.toErrorDescription());
		}
		else
		{
			updateStatus(status);
			Q_EMIT fireAbort(FailureCode::Reason::Connect_Card_Eid_Inactive);
		}
		return;
	}

	Q_EMIT fireContinue();
}


void StateConnectCard::onUnusableCardConnectionLost(const ReaderInfo& pInfo)
{
	if (pInfo.getName() == getContext()->getReaderName())
	{
		getContext()->setReaderName(QString());
		getContext()->resetCardConnection();
		Q_EMIT fireRetry();
	}
}


void StateConnectCard::onEntry(QEvent* pEvent)
{
	AbstractState::onEntry(pEvent);

	const WorkflowContext* const context = getContext().data();
	Q_ASSERT(context);

	/*
	 * Note: the plugin types to be used in this state must be already set in the workflow context before this state is entered.
	 * Changing the plugin types in the context, e.g. from {NFC} to {REMOTE}, causes the state to be left with a fireRetry signal.
	 */
	*this << connect(context, &WorkflowContext::fireReaderPlugInTypesChanged, this, &StateConnectCard::fireRetry);
}
