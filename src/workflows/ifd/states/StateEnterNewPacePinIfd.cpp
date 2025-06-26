/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterNewPacePinIfd.h"

#include "ReaderManager.h"


using namespace governikus;


StateEnterNewPacePinIfd::StateEnterNewPacePinIfd(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setKeepCardConnectionAlive();
}


void StateEnterNewPacePinIfd::run()
{
	Q_EMIT fireContinue();
}


void StateEnterNewPacePinIfd::onUserError(StatusCode pStatusCode)
{
	if (getContext() && getContext()->getIfdServer() && getContext()->getIfdServer()->getMessageHandler())
	{
		getContext()->setModifyPinMessageResponseApdu(ResponseApdu(pStatusCode));
	}
	Q_EMIT fireAbort(FailureCode::Reason::Enter_New_Pace_Pin_Ifd_User_Cancelled);
}


void StateEnterNewPacePinIfd::onCardRemoved(const ReaderInfo& pInfo) const
{
	if (const auto& context = getContext(); pInfo.getName() == context->getReaderName())
	{
		qDebug() << "Card was removed while waiting for user input. Resetting card connection";
		context->resetCardConnection();
	}
}


void StateEnterNewPacePinIfd::onEntry(QEvent* pEvent)
{
	AbstractState::onEntry(pEvent);

	stopNfcScanIfNecessary();

	if (getContext() && getContext()->getIfdServer() && getContext()->getIfdServer()->getMessageHandler())
	{
		const auto& handler = getContext()->getIfdServer()->getMessageHandler();
		*this << connect(handler.data(), &ServerMessageHandler::destroyed, this, [this]{
					onUserError(StatusCode::INPUT_CANCELLED);
				});
	}

	*this << connect(getContext().data(), &IfdServiceContext::fireUserError, this, &StateEnterNewPacePinIfd::onUserError);

	const auto* readerManager = Env::getSingleton<ReaderManager>();
	*this << connect(readerManager, &ReaderManager::fireCardRemoved, this, &StateEnterNewPacePinIfd::onCardRemoved);
}
