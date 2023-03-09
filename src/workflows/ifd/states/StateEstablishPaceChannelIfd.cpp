/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateEstablishPaceChannelIfd.h"

#include "ServerMessageHandler.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StateEstablishPaceChannelIfd::StateEstablishPaceChannelIfd(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
	, mPasswordId(PacePasswordId::UNKNOWN)
{
}


void StateEstablishPaceChannelIfd::run()
{
	Q_ASSERT(getContext());
	Q_ASSERT(!getContext()->getSlotHandle().isEmpty());

	const QSharedPointer<IfdServiceContext>& context = getContext();
	auto cardConnection = context->getCardConnection();
	if (!cardConnection)
	{
		Q_EMIT fireContinue();
		return;
	}

	const EstablishPaceChannel& paceChannel = context->getEstablishPaceChannel();
	mPasswordId = paceChannel.getPasswordId();
	QByteArray pacePassword;
	switch (mPasswordId)
	{
		case PacePasswordId::PACE_CAN:
			pacePassword = context->getCan().toLatin1();
			break;

		case PacePasswordId::PACE_PIN:
			pacePassword = context->getPin().toLatin1();
			break;

		case PacePasswordId::PACE_PUK:
			pacePassword = context->getPuk().toLatin1();
			break;

		default:
			Q_EMIT fireAbort(FailureCode::Reason::Establish_Pace_Ifd_Unknown);
			return;
	}

	qDebug() << "Establish connection using" << mPasswordId;
	Q_ASSERT(!pacePassword.isEmpty() && cardConnection);

	mConnections += cardConnection->callEstablishPaceChannelCommand(this,
			&StateEstablishPaceChannelIfd::onEstablishConnectionDone,
			mPasswordId,
			pacePassword,
			paceChannel.getChat(),
			paceChannel.getCertificateDescription());
}


void StateEstablishPaceChannelIfd::onReaderInfoChanged(const ReaderInfo& pReaderInfo)
{
	if (!pReaderInfo.hasEid())
	{
		EstablishPaceChannelOutput channelOutput;
		channelOutput.setPaceReturnCode(CardReturnCode::CARD_NOT_FOUND);
		getContext()->setEstablishPaceChannelOutput(channelOutput);

		Q_EMIT fireContinue();
	}
}


void StateEstablishPaceChannelIfd::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const QSharedPointer<EstablishPaceChannelCommand> establishPaceChannelCommand = pCommand.objectCast<EstablishPaceChannelCommand>();
	if (!establishPaceChannelCommand)
	{
		Q_ASSERT(false);
		qCDebug(statemachine) << "Expected an EstablishPaceChannelCommand as response!";
	}

	getContext()->setEstablishPaceChannelOutput(establishPaceChannelCommand->getPaceOutput());

	const CardReturnCode paceReturnCode = establishPaceChannelCommand->getReturnCode();
	const bool isWrongPacePassword = CardReturnCodeUtil::equalsWrongPacePassword(paceReturnCode);

	switch (mPasswordId)
	{
		case PacePasswordId::PACE_PIN:
			if (isWrongPacePassword)
			{
				const int nextExpectedCounter = getContext()->getExpectedRetryCounter() - 1;
				qCDebug(statemachine) << "Wrong PACE password. Decreasing expected retry counter to" << nextExpectedCounter;
				getContext()->setExpectedRetryCounter(nextExpectedCounter);
			}
			else if (paceReturnCode == CardReturnCode::OK)
			{
				const int nextExpectedCounter = 3;
				qCDebug(statemachine) << "Correct PACE password. Expected retry counter is now" << nextExpectedCounter;
				getContext()->setExpectedRetryCounter(nextExpectedCounter);
			}
			break;

		case PacePasswordId::PACE_PUK:
			if (paceReturnCode == CardReturnCode::OK || isWrongPacePassword)
			{
				qCDebug(statemachine) << "Resetting PACE passwords and setting expected retry counter to -1";
				getContext()->resetPacePasswords();
				getContext()->setExpectedRetryCounter(-1);
			}
			break;

		default:
			qCDebug(statemachine) << "PACE finished with:" << paceReturnCode;
	}

	Q_EMIT fireContinue();
}
