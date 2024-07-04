/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */


#include "StateEstablishPaceChannel.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;

StateEstablishPaceChannel::StateEstablishPaceChannel(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
	, mPasswordId(PacePasswordId::UNKNOWN)
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
	const auto& authContext = context.objectCast<AuthContext>();
	mPasswordId = context->getEstablishPaceChannelType();
	Q_ASSERT(mPasswordId != PacePasswordId::UNKNOWN);

	if (mPasswordId == PacePasswordId::PACE_PIN ||
			(mPasswordId == PacePasswordId::PACE_CAN && context->isCanAllowedMode()))
	{
		if (authContext && authContext->getDidAuthenticateEac1())
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
	}

	QByteArray password;
	switch (mPasswordId)
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
		qCDebug(statemachine) << "No card connection available.";
		context->setLastPaceResult(CardReturnCode::CARD_NOT_FOUND);
		Q_EMIT fireNoCardConnection();
		return;
	}

	if (password.isEmpty() && cardConnection->getReaderInfo().isBasicReader())
	{
		qCCritical(statemachine) << "We hit an invalid state! PACE password is empty for basic reader.";
		Q_ASSERT(false);

		updateStatus(GlobalStatus::Code::Card_Invalid_Pin);
		Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Channel_Basic_Reader_No_Pin);
		return;
	}

	//: INFO ALL_PLATFORMS First status message after the PIN was entered.
	context->setProgress(context->getProgressValue(), tr("The secure channel is opened"));

	qDebug() << "Establish connection using" << mPasswordId;
	Q_ASSERT(!password.isEmpty() || !cardConnection->getReaderInfo().isBasicReader());

	if (mPasswordId == PacePasswordId::PACE_PIN && !cardConnection->getReaderInfo().isBasicReader())
	{
		const auto pinContext = context.objectCast<ChangePinContext>();
		if (pinContext && pinContext->isRequestTransportPin())
		{
			password = QByteArray(5, 0);
		}
	}

	*this << cardConnection->callEstablishPaceChannelCommand(this,
			&StateEstablishPaceChannel::onEstablishConnectionDone,
			mPasswordId,
			password,
			effectiveChat,
			certificateDescription);
}


void StateEstablishPaceChannel::onUserCancelled()
{
	getContext()->setLastPaceResult(CardReturnCode::CANCELLATION_BY_USER);
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
	getContext()->setInitialInputErrorShown();

	auto paceCommand = pCommand.staticCast<EstablishPaceChannelCommand>();
	getContext()->setPaceOutputData(paceCommand->getPaceOutput());

	CardReturnCode returnCode = pCommand->getReturnCode();
	getContext()->setLastPaceResult(returnCode);
	handleNpaPosition(returnCode);

	switch (returnCode)
	{
		case CardReturnCode::OK:
			switch (mPasswordId)
			{
				case PacePasswordId::PACE_PIN:
					qCDebug(statemachine) << "PACE_PIN succeeded. Setting expected retry counter to:" << 3;
					getContext()->setExpectedRetryCounter(3);
					Q_EMIT fireContinue();
					break;

				case PacePasswordId::PACE_CAN:
					if (getContext()->isCanAllowedMode())
					{
						qCDebug(statemachine) << "PACE_CAN (AUTH) succeeded";
						Q_EMIT fireContinue();
						break;
					}

					qCDebug(statemachine) << "PACE_CAN (PIN) succeeded";
					getContext()->setLastPaceResult(CardReturnCode::OK_CAN);
					Q_EMIT firePaceCanEstablished();
					break;

				case PacePasswordId::PACE_PUK:
					qCDebug(statemachine) << "PACE_PUK succeeded";
					getContext()->setLastPaceResult(CardReturnCode::OK_PUK);
					Q_EMIT firePacePukEstablished();
					break;

				case PacePasswordId::PACE_MRZ:
				case PacePasswordId::UNKNOWN:
					qCritical() << "Cannot handle unknown PacePasswordId";
					updateStatus(GlobalStatus::Code::Card_Protocol_Error);
					Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Channel_Unknown_Password_Id);
					break;
			}
			return;

		case CardReturnCode::CANCELLATION_BY_USER:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Channel_User_Cancelled);
			return;

		case CardReturnCode::INVALID_PIN_3:
			Q_EMIT fireThirdPinAttemptFailed();
			return;

		case CardReturnCode::INVALID_PIN:
		case CardReturnCode::INVALID_PIN_2:
		case CardReturnCode::INVALID_CAN:
		case CardReturnCode::INVALID_PUK:
			Q_EMIT fireWrongPassword();
			return;

		default:
			if (getContext()->isNpaRepositioningRequired())
			{
				Q_EMIT fireAbortAndUnfortunateCardPosition();
				return;
			}

			Q_EMIT firePaceChannelFailed();
			return;
	}

	Q_UNREACHABLE();
}
