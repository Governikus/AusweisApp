/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */


#include "StateEstablishPacePin.h"

#include "context/AuthContext.h"


using namespace governikus;

StateEstablishPacePin::StateEstablishPacePin(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateEstablishPacePin::run()
{
	auto cardConnection = getContext()->getCardConnection();
	Q_ASSERT(cardConnection);

	QByteArray certificateDescription, effectiveChat;
	if (auto authContext = getContext().objectCast<AuthContext>())
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

	qDebug() << "Establish connection using PIN";
	mConnections += cardConnection->callEstablishPaceChannelCommand(this,
			&StateEstablishPacePin::onEstablishConnectionDone,
			PACE_PASSWORD_ID::PACE_PIN,
			getContext()->getPin(),
			effectiveChat,
			certificateDescription);
	getContext()->setPin(QString());
}


void StateEstablishPacePin::onUserCancelled()
{
	getContext()->setLastPaceResultAndRetryCounter(CardReturnCode::CANCELLATION_BY_USER, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
	AbstractState::onUserCancelled();
}


void StateEstablishPacePin::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	CardReturnCode returnCode = pCommand->getReturnCode();

	auto paceCommand = pCommand.staticCast<EstablishPaceChannelCommand>();
	getContext()->setPaceOutputData(paceCommand->getPaceOutput());

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

		case CardReturnCode::INVALID_PIN:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireInvalidPin();
			break;

		default:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort();
			break;
	}
}
