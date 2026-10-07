/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */


#include "StateEstablishPaceChannel.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"


Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StateEstablishPaceChannel::StateEstablishPaceChannel(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateEstablishPaceChannel::run()
{
	const auto& context = getContext();
	Q_ASSERT(context);

	if (context->getStatus().isError())
	{
		Q_ASSERT(context->getFailureCode().has_value());
		Q_EMIT firePaceChannelFailed();
		return;
	}

	QByteArray effectiveChat;
	QByteArray certificateDescription;
	const auto passwordId = context->getEstablishPaceChannelType();
	Q_ASSERT(passwordId != PacePasswordId::UNKNOWN);

	if (const auto& authContext = context.objectCast<AuthContext>();
			(
				passwordId == PacePasswordId::PACE_PIN ||
				(passwordId == PacePasswordId::PACE_CAN && context->isCanAllowedMode())
			) &&
			authContext && authContext->getDidAuthenticateEac1())
	{
		// if PACE is performed for authentication purposes,
		// the chat and certificate description need to be sent
		//
		// in other scenarios, e.g. for changing the PIN, the data
		// is not needed
		certificateDescription = authContext->getDidAuthenticateEac1()->getCertificateDescriptionAsBinary();
		effectiveChat = authContext->encodeEffectiveChat();
		Q_ASSERT(!effectiveChat.isEmpty());
	}

	QByteArray password;
	switch (passwordId)
	{
		case PacePasswordId::PACE_CAN:
			password = context->getCan().toLatin1();
			break;

		case PacePasswordId::PACE_PIN:
			password = context->getPin().toLatin1();
			break;

		case PacePasswordId::PACE_PUK:
			password = context->getPuk().toLatin1();
			break;

		case PacePasswordId::UNKNOWN:
		case PacePasswordId::PACE_MRZ:
			password = QByteArray();
			break;
	}

	auto cardConnection = context->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "No card connection available";
		context->setPaceOutput(EstablishPaceChannelOutput(passwordId, CardReturnCode::CARD_NOT_FOUND));
		Q_EMIT fireNoCardConnection();
		return;
	}

	if (password.isEmpty() && cardConnection->getReaderInfo().isBasicReader())
	{
		qCCritical(statemachine) << "We hit an invalid state! PACE password is empty for basic reader.";
		Q_ASSERT(false);

		updateStatus(GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation);
		Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Channel_Basic_Reader_No_Pin);
		return;
	}

	//: ALL_PLATFORMS First status message after the PIN was entered.
	context->setProgress(context->getProgressValue(), tr("The secure channel is opened"));

	qDebug() << "Establish connection using" << passwordId;
	Q_ASSERT(!password.isEmpty() || !cardConnection->getReaderInfo().isBasicReader());

	if (passwordId == PacePasswordId::PACE_PIN && !cardConnection->getReaderInfo().isBasicReader())
	{
		const auto pinContext = context.objectCast<ChangePinContext>();
		if (pinContext && pinContext->isRequestTransportPin())
		{
			password = QByteArray(5, 0);
		}
	}

	*this << cardConnection->callEstablishPaceChannelCommand(this,
			&StateEstablishPaceChannel::onEstablishConnectionDone,
			passwordId,
			password,
			effectiveChat,
			certificateDescription);
}


void StateEstablishPaceChannel::onUserCancelled()
{
	const auto& context = getContext();
	context->setPaceOutput(EstablishPaceChannelOutput(context->getEstablishPaceChannelType(), CardReturnCode::CANCELLATION_BY_USER));
	AbstractState::onUserCancelled();
}


void StateEstablishPaceChannel::handleNpaPosition(CardReturnCode pReturnCode) const
{
	if (pReturnCode == CardReturnCode::CARD_NOT_FOUND || pReturnCode == CardReturnCode::RESPONSE_EMPTY)
	{
		qCDebug(statemachine) << "Card vanished during PACE. Incrementing unfortunate-card-position panickiness.";
		getContext()->handleWrongNpaPosition();
		return;
	}

	qCDebug(statemachine) << "Clearing unfortunate-card-position panickiness. |" << pReturnCode;
	getContext()->setNpaPositionVerified();
	return;
}


void StateEstablishPaceChannel::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const auto& context = getContext();
	context->setInitialInputErrorShown();

	auto paceCommand = pCommand.staticCast<EstablishPaceChannelCommand>();
	context->setPaceOutput(paceCommand->getPaceOutput());

	CardReturnCode returnCode = pCommand->getReturnCode();
	handleNpaPosition(returnCode);

	switch (returnCode)
	{
		case CardReturnCode::OK:
			switch (context->getPaceOutput().getPaceResult())
			{
				case PaceResult::OK_PIN:
				case PaceResult::OK_PIN_AUTH:
					qCDebug(statemachine) << "PACE_PIN succeeded. Setting expected retry counter to:" << 3;
					context->setExpectedRetryCounter(3);
					Q_EMIT fireContinue();
					return;

				case PaceResult::OK_CAN:
					qCDebug(statemachine) << "PACE_CAN (PIN) succeeded";
					Q_EMIT firePaceCanEstablished();
					return;

				case PaceResult::OK_CAN_AUTH:
					qCDebug(statemachine) << "PACE_CAN (AUTH) succeeded";
					Q_EMIT fireContinue();
					return;

				case PaceResult::OK_PUK:
					qCDebug(statemachine) << "PACE_PUK succeeded";
					Q_EMIT firePacePukEstablished();
					return;

				case PaceResult::INVALID_PIN_3:
					Q_EMIT fireThirdPinAttemptFailed();
					return;

				case PaceResult::INVALID_PIN_1:
				case PaceResult::INVALID_PIN_2:
				case PaceResult::INVALID_CAN:
				case PaceResult::INVALID_PUK:
					Q_EMIT fireWrongPassword();
					return;

				case PaceResult::UNDEFINED:
					qCritical() << "Cannot handle unknown PacePasswordId";
					updateStatus(GlobalStatus::Code::Card_Protocol_Error);
					Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Channel_Unknown_Password_Id);
					return;
			}
			return;

		case CardReturnCode::CANCELLATION_BY_USER:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Channel_User_Cancelled);
			return;

		default:
			if (context->isNpaRepositioningRequired())
			{
				Q_EMIT fireAbortAndUnfortunateCardPosition();
				return;
			}

			Q_EMIT firePaceChannelFailed();
			return;
	}

	Q_UNREACHABLE();
}
