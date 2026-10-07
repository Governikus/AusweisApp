/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#include "StateEnterPacePasswordIfd.h"

#include "ReaderManager.h"


using namespace governikus;


StateEnterPacePasswordIfd::StateEnterPacePasswordIfd(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateEnterPacePasswordIfd::run()
{
	Q_EMIT fireContinue();
}


void StateEnterPacePasswordIfd::onUserError()
{
	const auto& context = getContext();
	if (context && context->getIfdServer() && context->getIfdServer()->getMessageHandler())
	{
		EstablishPaceChannelOutput channelOutput(context->getEstablishPaceChannelType(), CardReturnCode::CANCELLATION_BY_USER);
		context->setPaceOutput(channelOutput);
	}

	Q_EMIT fireAbort(FailureCode::Reason::Enter_Pace_Password_Ifd_User_Cancelled);
}


void StateEnterPacePasswordIfd::onCardRemoved(const ReaderInfo& pInfo) const
{
	if (const auto& context = getContext(); pInfo.getName() == context->getReaderName())
	{
		qDebug() << "Card was removed while waiting for user input. Resetting card connection";
		context->resetCardConnection();
	}
}


void StateEnterPacePasswordIfd::onEntry(QEvent* pEvent)
{
	AbstractState::onEntry(pEvent);

	stopNfcScanIfNecessary();

	if (getContext() && getContext()->getIfdServer() && getContext()->getIfdServer()->getMessageHandler())
	{
		const auto& handler = getContext()->getIfdServer()->getMessageHandler();
		*this << connect(handler.data(), &ServerMessageHandler::destroyed, this, &StateEnterPacePasswordIfd::onUserError);
	}

	*this << connect(getContext().data(), &IfdServiceContext::fireUserError, this, &StateEnterPacePasswordIfd::onUserError);

	const auto* readerManager = Env::getSingleton<ReaderManager>();
	*this << connect(readerManager, &ReaderManager::fireCardRemoved, this, &StateEnterPacePasswordIfd::onCardRemoved);
}
