/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
	if (getContext()->getStatus().isError())
	{
		Q_ASSERT(getContext()->getFailureCode().has_value());
		Q_EMIT firePropagateAbort();
		return;
	}

	QByteArray effectiveChat;
	QByteArray certificateDescription;
	auto authContext = getContext().objectCast<AuthContext>();
	mPasswordId = getContext()->getEstablishPaceChannelType();
	Q_ASSERT(mPasswordId != PacePasswordId::UNKNOWN);

	if (mPasswordId == PacePasswordId::PACE_PIN ||
			(mPasswordId == PacePasswordId::PACE_CAN && getContext()->isCanAllowedMode()))
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
			password = getContext()->getCan().toLatin1();
			break;

		case PacePasswordId::PACE_PIN:
			password = getContext()->getPin().toLatin1();
			if (authContext && password.size() == 5)
			{
				abortToChangePin(FailureCode::Reason::Establish_Pace_Channel_Transport_Pin);
				return;
			}
			break;

		case PacePasswordId::PACE_PUK:
			password = getContext()->getPuk().toLatin1();
			break;

		case PacePasswordId::UNKNOWN:
		case PacePasswordId::PACE_MRZ:
			password = QByteArray();
			break;
	}

	auto cardConnection = getContext()->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "No card connection available.";
		abort(FailureCode::Reason::Establish_Pace_Channel_No_Card_Connection);
		return;
	}

	if (password.isEmpty() && cardConnection->getReaderInfo().isBasicReader())
	{
		qCCritical(statemachine) << "We hit an invalid state! PACE password is empty for basic reader.";
		Q_ASSERT(false);

		qCDebug(statemachine) << "Resetting all PACE passwords.";
		getContext()->resetPacePasswords();

		abort(FailureCode::Reason::Establish_Pace_Channel_Basic_Reader_No_Pin);
		return;
	}

	qDebug() << "Establish connection using" << mPasswordId;
	Q_ASSERT(!password.isEmpty() || !cardConnection->getReaderInfo().isBasicReader());

	if (mPasswordId == PacePasswordId::PACE_PIN && !cardConnection->getReaderInfo().isBasicReader())
	{
		const auto pinContext = getContext().objectCast<ChangePinContext>();
		if (pinContext && pinContext->isRequestTransportPin())
		{
			password = QByteArray(5, 0);
		}
	}

	mConnections += cardConnection->callEstablishPaceChannelCommand(this,
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


void StateEstablishPaceChannel::abort(FailureCode::Reason pReason)
{
	getContext()->resetLastPaceResult();
	Q_EMIT fireAbort(pReason);
}


void StateEstablishPaceChannel::abortToChangePin(FailureCode::Reason pReason)
{
	if (auto authContext = getContext().objectCast<AuthContext>())
	{
		authContext->setSkipRedirect(true);
		authContext->setLastPaceResult(CardReturnCode::NO_ACTIVE_PIN_SET);
	}
	updateStatus(GlobalStatus::Code::Workflow_Cancellation_By_User);
	Q_EMIT fireAbort(pReason);
}


void StateEstablishPaceChannel::handleNpaPosition(CardReturnCode pReturnCode)
{
	if (pReturnCode == CardReturnCode::CARD_NOT_FOUND || pReturnCode == CardReturnCode::RETRY_ALLOWED)
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
	if (mPasswordId != PacePasswordId::PACE_PUK)
	{
		auto paceCommand = pCommand.staticCast<EstablishPaceChannelCommand>();
		getContext()->setPaceOutputData(paceCommand->getPaceOutput());
	}

	CardReturnCode returnCode = pCommand->getReturnCode();
	getContext()->setLastPaceResult(returnCode);

	handleNpaPosition(returnCode);

	if (mPasswordId == PacePasswordId::PACE_PIN && returnCode == CardReturnCode::OK)
	{
		qCDebug(statemachine) << "PACE_PIN succeeded. Setting expected retry counter to:" << 3;
		getContext()->setExpectedRetryCounter(3);
	}
	else if (mPasswordId == PacePasswordId::PACE_PUK && returnCode == CardReturnCode::OK)
	{
		qCDebug(statemachine) << "PACE_PUK succeeded. Resetting PACE passwords and setting expected retry counter to:" << -1;
		getContext()->resetPacePasswords();
		getContext()->setExpectedRetryCounter(-1);
	}

	switch (returnCode)
	{
		case CardReturnCode::OK:
			if (mPasswordId == PacePasswordId::PACE_PIN ||
					(mPasswordId == PacePasswordId::PACE_CAN && getContext()->isCanAllowedMode()))
			{
				Q_EMIT firePaceChannelEstablished();
				return;
			}
			else if (mPasswordId == PacePasswordId::PACE_PUK)
			{
				getContext()->setLastPaceResult(CardReturnCode::OK_PUK);

				Q_EMIT firePaceChannelInoperative();
				return;
			}

			Q_EMIT fireContinue();
			return;

		case CardReturnCode::PUK_INOPERATIVE:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Channel_Puk_Inoperative);
			return;

		case CardReturnCode::CANCELLATION_BY_USER:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Channel_User_Cancelled);
			return;

		case CardReturnCode::NO_ACTIVE_PIN_SET:
			abortToChangePin(FailureCode::Reason::Establish_Pace_Channel_No_Active_Pin);
			return;

		case CardReturnCode::INVALID_PIN:
		{
			CardReturnCode paceResult;
			switch (getContext()->getCardConnection()->getReaderInfo().getRetryCounter())
			{
				case 2:
					// Old retryCounter is 2: 2nd try failed
					paceResult = CardReturnCode::INVALID_PIN_2;
					break;

				case 1:
					// Old retryCounter is 1: 3rd try failed
					paceResult = CardReturnCode::INVALID_PIN_3;
					break;

				default:
					paceResult = CardReturnCode::INVALID_PIN;
			}
			getContext()->setLastPaceResult(paceResult);
			Q_EMIT firePaceChannelInoperative();
			return;
		}

		default:
			if (getContext()->isNpaRepositioningRequired())
			{
				Q_EMIT fireAbortAndUnfortunateCardPosition();
				return;
			}

			Q_EMIT fireAbort({FailureCode::Reason::Establish_Pace_Channel_Invalid_Card_Return_Code,
							  {FailureCode::Info::Card_Return_Code, Enum<CardReturnCode>::getName(returnCode)}
					});
			return;
	}

	Q_UNREACHABLE();
}
