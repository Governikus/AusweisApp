/*!
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerChangePin.h"

#include "Env.h"
#include "UILoader.h"
#include "UIPlugInJson.h"
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

	auto* ui = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
	Q_ASSERT(ui);
	Q_EMIT ui->fireWorkflowRequested(ChangePinController::createWorkflowRequest());
}


MsgHandlerChangePin::MsgHandlerChangePin(const QSharedPointer<const ChangePinContext>& pContext)
	: MsgHandlerChangePin()
{
	Q_ASSERT(pContext);

	mJsonObject[QLatin1String("success")] = pContext->getLastPaceResult() == CardReturnCode::OK && !pContext->isWorkflowCancelled();
}
