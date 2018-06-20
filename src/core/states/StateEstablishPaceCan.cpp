/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */


#include "StateEstablishPaceCan.h"

#include "context/AuthContext.h"

using namespace governikus;

StateEstablishPaceCan::StateEstablishPaceCan(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateEstablishPaceCan::run()
{
	auto cardConnection = getContext()->getCardConnection();
	Q_ASSERT(cardConnection);

	QByteArray certificateDescription, effectiveChat;
	const auto& authContext = getContext().objectCast<AuthContext>();
	if (getContext()->isCanAllowedMode() && authContext)
	{
		// if PACE is performed for authentication purposes,
		// the chat and certificate description need to be send
		//
		// in other scenarios, e.g. for changing the PIN, the data
		// is not needed
		Q_ASSERT(authContext->getDidAuthenticateEac1());
		Q_ASSERT(!authContext->encodeEffectiveChat().isEmpty());
		certificateDescription = authContext->getDidAuthenticateEac1()->getCertificateDescriptionAsBinary();
		effectiveChat = authContext->encodeEffectiveChat();
	}

	qDebug() << "Establish Pace connection with CAN";
	mConnections += cardConnection->callEstablishPaceChannelCommand(
			this,
			&StateEstablishPaceCan::onEstablishConnectionDone,
			PACE_PASSWORD_ID::PACE_CAN,
			getContext()->getCan(),
			effectiveChat,
			certificateDescription);
	getContext()->setCan(QString());
}


void StateEstablishPaceCan::onUserCancelled()
{
	getContext()->setLastPaceResultAndRetryCounter(CardReturnCode::CANCELLATION_BY_USER, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
	AbstractState::onUserCancelled();
}


void StateEstablishPaceCan::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const CardReturnCode returnCode = pCommand->getReturnCode();
	if (getContext()->isCanAllowedMode())
	{
		auto paceCommand = pCommand.staticCast<EstablishPaceChannelCommand>();
		getContext()->setPaceOutputData(paceCommand->getPaceOutput());
	}

	switch (returnCode)
	{
		case CardReturnCode::OK:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireContinue();
			break;

		case CardReturnCode::CANCELLATION_BY_USER:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort();
			break;

		case CardReturnCode::INVALID_CAN:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireInvalidCan();
			break;

		default:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort();
			break;
	}
}
