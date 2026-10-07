/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#include "StateEstablishPaceChannelIfd.h"


#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StateEstablishPaceChannelIfd::StateEstablishPaceChannelIfd(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateEstablishPaceChannelIfd::run()
{
	Q_ASSERT(getContext());
	Q_ASSERT(!getContext()->getSlotHandle().isEmpty());

	const QSharedPointer<IfdServiceContext>& context = getContext();
	const EstablishPaceChannel& paceChannel = context->getEstablishPaceChannel();
	const auto passwordId = paceChannel.getPasswordId();

	auto cardConnection = context->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "No card connection available";
		EstablishPaceChannelOutput channelOutput(passwordId, CardReturnCode::CARD_NOT_FOUND);
		getContext()->setPaceOutput(channelOutput);
		Q_EMIT fireContinue();
		return;
	}

	QByteArray pacePassword;
	switch (passwordId)
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

	qDebug() << "Establish connection using" << passwordId;
	Q_ASSERT(!pacePassword.isEmpty() || !cardConnection->getReaderInfo().isBasicReader());

	*this << cardConnection->callEstablishPaceChannelCommand(this,
			&StateEstablishPaceChannelIfd::onEstablishConnectionDone,
			passwordId,
			pacePassword,
			paceChannel.getChat(),
			paceChannel.getCertificateDescription());
}


void StateEstablishPaceChannelIfd::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const QSharedPointer<EstablishPaceChannelCommand> establishPaceChannelCommand = pCommand.objectCast<EstablishPaceChannelCommand>();
	if (!establishPaceChannelCommand)
	{
		Q_ASSERT(false);
		qCDebug(statemachine) << "Expected an EstablishPaceChannelCommand as response!";
	}

	const auto& context = getContext();
	const auto& output = establishPaceChannelCommand->getPaceOutput();
	context->setPaceOutput(output);

	const CardReturnCode paceReturnCode = output.getReturnCode();
	const bool isWrongPacePassword = output.wrongPasswordUsed();
	switch (output.getPasswordId())
	{
		case PacePasswordId::PACE_PIN:
			if (isWrongPacePassword)
			{
				const int nextExpectedCounter = context->getExpectedRetryCounter() - 1;
				qCDebug(statemachine) << "Wrong PACE password. Decreasing expected retry counter to" << nextExpectedCounter;
				context->setExpectedRetryCounter(nextExpectedCounter);
			}
			else if (paceReturnCode == CardReturnCode::OK)
			{
				const int nextExpectedCounter = 3;
				qCDebug(statemachine) << "Correct PACE password. Expected retry counter is now" << nextExpectedCounter;
				context->setExpectedRetryCounter(nextExpectedCounter);
			}
			break;

		case PacePasswordId::PACE_PUK:
			if (paceReturnCode == CardReturnCode::OK || isWrongPacePassword)
			{
				qCDebug(statemachine) << "Resetting PACE passwords and setting expected retry counter to -1";
				context->resetPacePasswords();
				context->setExpectedRetryCounter(-1);
			}
			break;

		default:
			qCDebug(statemachine) << "PACE finished with:" << paceReturnCode;
	}

	Q_EMIT fireContinue();
}
