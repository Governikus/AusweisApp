/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "states/StateTransmit.h"

using namespace governikus;

StateTransmit::StateTransmit(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateTransmit::run()
{
	const auto& transmits = getContext()->getTransmits();

	Q_ASSERT(!transmits.isEmpty());
	Q_ASSERT(transmits.size() == getContext()->getTransmitResponses().size());

	auto cardConnection = getContext()->getCardConnection();
	Q_ASSERT(cardConnection != nullptr);
	mConnections += cardConnection->callTransmitCommand(this, &StateTransmit::onCardCommandDone, transmits.last()->getInputApduInfos());
}


void StateTransmit::onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand)
{
	auto transmitCommand = pCommand.staticCast<TransmitCommand>();
	auto returnCode = transmitCommand->getReturnCode();
	if (returnCode == CardReturnCode::OK)
	{
		QSharedPointer<TransmitResponse> response(getContext()->getTransmitResponses().last());
		response->setOutputApdus(transmitCommand->getOutputApduAsHex());
		Q_EMIT fireContinue();
	}
	else if (returnCode == CardReturnCode::UNEXPECTED_TRANSMIT_STATUS)
	{
		QSharedPointer<TransmitResponse> response(getContext()->getTransmitResponses().last());
		response->setOutputApdus(transmitCommand->getOutputApduAsHex());
		updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode)); // set the result to the model so it is written to the PAOS response
		Q_EMIT fireContinue();
	}
	else
	{
		updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
		Q_EMIT fireAbort();
	}
}


void StateTransmit::onEntry(QEvent* pEvent)
{
	//: INFO ALL_PLATFORMS Status message after the PIN was entered, communication between eID server and card is running.
	getContext()->setProgress(60, tr("Reading data"));
	AbstractState::onEntry(pEvent);
}
