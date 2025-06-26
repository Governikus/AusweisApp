/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterPacePasswordIfd.h"

#include "ReaderManager.h"


using namespace governikus;


StateEnterPacePasswordIfd::StateEnterPacePasswordIfd(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setKeepCardConnectionAlive();
}


void StateEnterPacePasswordIfd::run()
{
	Q_EMIT fireContinue();
}


void StateEnterPacePasswordIfd::onUserError()
{
	if (getContext() && getContext()->getIfdServer() && getContext()->getIfdServer()->getMessageHandler())
	{
		EstablishPaceChannelOutput channelOutput;
		channelOutput.setPaceReturnCode(CardReturnCode::CANCELLATION_BY_USER);
		getContext()->setEstablishPaceChannelOutput(channelOutput);
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
