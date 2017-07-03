/*!
 * StateEstablishPacePin.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */


#include "ReturnCodeUtil.h"
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
	if (auto authContext = getContext().dynamicCast<AuthContext>())
	{
		// if PACE is performed for authentication purposes,
		// the chat and certificate description need to be send
		//
		// in other scenarios, e.g. for changing the PIN, the data
		// is not needed
		Q_ASSERT(authContext->getDidAuthenticateEac1());
		Q_ASSERT(authContext->getEffectiveChat());
		certificateDescription = authContext->getDidAuthenticateEac1()->getCertificateDescriptionAsBinary();
		effectiveChat = authContext->getEffectiveChat()->encode();
	}

	qDebug() << "Establish connection using Pin";
	mConnections += cardConnection->callEstablishPaceChannelCommand(this,
			&StateEstablishPacePin::onEstablishConnectionDone,
			PACE_PIN_ID::PACE_PIN,
			getContext()->getPin(),
			effectiveChat,
			certificateDescription);
	getContext()->setPin(QString());
}


void StateEstablishPacePin::onUserCancelled()
{
	getContext()->setLastPaceResultAndRetryCounter(ReturnCode::CANCELLATION_BY_USER, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
	AbstractState::onUserCancelled();
}


void StateEstablishPacePin::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	ReturnCode returnCode = pCommand->getReturnCode();

	auto paceCommand = pCommand.staticCast<EstablishPaceChannelCommand>();
	getContext()->setPaceOutputData(paceCommand->getPaceOutput());

	switch (returnCode)
	{
		case ReturnCode::OK:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireSuccess();
			break;

		case ReturnCode::CANCELLATION_BY_USER:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			setResult(Result::createCancelByUserError());
			Q_EMIT fireCancel();
			break;

		case ReturnCode::PIN_INVALID:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireInvalidPin();
			break;

		default:
			setResult(Result::createInternalError(ReturnCodeUtil::toMessage(returnCode)));
			Q_EMIT fireError();
			break;
	}
}
