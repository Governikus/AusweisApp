/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateTransmitPersonalization.h"

#include "ReaderManager.h"
#include "SmartManager.h"


using namespace governikus;


StateTransmitPersonalization::StateTransmitPersonalization(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateTransmitPersonalization::run()
{
	const auto& transmit = getContext()->getTransmit();
	Q_ASSERT(transmit);

	const auto& inputApduInfos = transmit->getInputApduInfos();
	const auto func = [inputApduInfos] {
				return QVariant::fromValue(SmartManager::get()->performPersonalization(inputApduInfos));
			};

	*this << Env::getSingleton<ReaderManager>()->callExecuteCommand(func, this, &StateTransmitPersonalization::onCommandDone);
}


void StateTransmitPersonalization::onCommandDone(const QVariant& pResult)
{
	const auto& context = getContext();
	Q_ASSERT(context->getTransmitResponse());
	const auto& transmit = context->getTransmit();

	const auto& outputApduAsHex = pResult.value<QByteArrayList>();
	context->getTransmitResponse()->setOutputApdus(outputApduAsHex);

	if (outputApduAsHex.size() != transmit->getInputApduInfos().size())
	{
		updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Personalization_Failed);
		Q_EMIT fireAbort(FailureCode::Reason::Transmit_Personalization_Size_Mismatch);
		return;
	}

	const int newProgress = context->getProgressValue() + static_cast<int>(outputApduAsHex.size());
	context->setProgress(qMin(90, newProgress), context->getProgressMessage());
	Q_EMIT fireContinue();
}
