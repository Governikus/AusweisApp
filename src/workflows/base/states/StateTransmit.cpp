/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "states/StateTransmit.h"

using namespace governikus;

StateTransmit::StateTransmit(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setAbortOnCardRemoved();
}


void StateTransmit::run()
{
	const auto& transmit = getContext()->getTransmit();

	Q_ASSERT(transmit);
	Q_ASSERT(getContext()->getTransmitResponse());

	auto cardConnection = getContext()->getCardConnection();
	Q_ASSERT(cardConnection != nullptr);
	mConnections += cardConnection->callTransmitCommand(this, &StateTransmit::onCardCommandDone, transmit->getInputApduInfos());
}


void StateTransmit::onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand)
{
	auto transmitCommand = pCommand.staticCast<TransmitCommand>();
	auto returnCode = transmitCommand->getReturnCode();
	if (returnCode == CardReturnCode::OK)
	{
		getContext()->getTransmitResponse()->setOutputApdus(transmitCommand->getOutputApduAsHex());
		Q_EMIT fireContinue();
	}
	else if (returnCode == CardReturnCode::UNEXPECTED_TRANSMIT_STATUS)
	{
		getContext()->getTransmitResponse()->setOutputApdus(transmitCommand->getOutputApduAsHex());
		updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode)); // set the result to the model so it is written to the PAOS response
		Q_EMIT fireContinue();
	}
	else
	{
		updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
		Q_EMIT fireAbort({FailureCode::Reason::Transmit_Card_Command_Failed,
						  {FailureCode::Info::Card_Return_Code, Enum<CardReturnCode>::getName(returnCode)}
				});
	}
}


void StateTransmit::onEntry(QEvent* pEvent)
{
	//: INFO ALL_PLATFORMS Status message after the PIN was entered, communication between eID server and card is running.
	getContext()->setProgress(60, tr("Reading data"));
	AbstractState::onEntry(pEvent);
}
