/**
 * Copyright (c) 2020-2026 Governikus Service GmbH, Germany
 */

#include "MsgHandlerChangePin.h"

#include "Env.h"
#include "UiLoader.h"
#include "UiPluginJson.h"
#include "controller/ChangePinController.h"


using namespace governikus;


MsgHandlerChangePin::MsgHandlerChangePin()
	: MsgHandlerWorkflows(MsgType::CHANGE_PIN)
{
}


MsgHandlerChangePin::MsgHandlerChangePin(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandlerChangePin()
{
	setVoid();
	handleWorkflowProperties(pObj, pContext);

	auto* ui = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
	Q_ASSERT(ui);
	Q_EMIT ui->fireWorkflowRequested(ChangePinController::createWorkflowRequest());
}


MsgHandlerChangePin::MsgHandlerChangePin(const QSharedPointer<const ChangePinContext>& pContext)
	: MsgHandlerChangePin()
{
	Q_ASSERT(pContext);

	const auto success = pContext->getPaceOutput().getReturnCode() == CardReturnCode::OK
			&& pContext->getPaceOutput().getPaceResult() == PaceResult::OK_PIN;
	setValue(QLatin1String("success"), success && !pContext->isWorkflowCancelled());

	const auto& failureCode = pContext->getFailureCode();
	if (failureCode.has_value())
	{
		setValue(QLatin1String("reason"), Enum<FailureCode::Reason>::getName(failureCode.value().getReason()));
	}
}
