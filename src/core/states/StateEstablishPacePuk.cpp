/*!
 * StateEstablishPacePuk.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */


#include "StateEstablishPacePuk.h"

#include "ReturnCodeUtil.h"


using namespace governikus;

StateEstablishPacePuk::StateEstablishPacePuk(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateEstablishPacePuk::run()
{
	auto cardConnection = getContext()->getCardConnection();

	Q_ASSERT(cardConnection);
	qDebug() << "Invoke unblock pin command";
	mConnections += cardConnection->callUnblockPinCommand(this, &StateEstablishPacePuk::onEstablishConnectionDone, getContext()->getPuk());
	getContext()->setPuk(QString());
}


void StateEstablishPacePuk::onUserCancelled()
{
	getContext()->setLastPaceResultAndRetryCounter(ReturnCode::CANCELLATION_BY_USER, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
	AbstractState::onUserCancelled();
}


void StateEstablishPacePuk::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	ReturnCode returnCode = pCommand->getReturnCode();
	switch (returnCode)
	{
		case ReturnCode::OK:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			getContext()->setSuccessMessage(tr("Pin successfully unblocked"));
			Q_EMIT fireSuccess();
			break;

		case ReturnCode::CANCELLATION_BY_USER:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			setResult(Result::createCancelByUserError());
			Q_EMIT fireCancel();
			break;

		case ReturnCode::PUK_INVALID:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireInvalidPuk();
			break;

		case ReturnCode::PUK_INOPERATIVE:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireInoperativePuk();
			break;

		default:
			setResult(Result::createInternalError(ReturnCodeUtil::toMessage(returnCode)));
			Q_EMIT fireError();
			break;
	}
}
