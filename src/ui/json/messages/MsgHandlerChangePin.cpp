/*!
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerChangePin.h"

#include "Env.h"
#include "UILoader.h"
#include "UIPlugInJson.h"

using namespace governikus;


MsgHandlerChangePin::MsgHandlerChangePin()
	: MsgHandler(MsgType::CHANGE_PIN)
{
}


MsgHandlerChangePin::MsgHandlerChangePin(const QJsonObject& pObj)
	: MsgHandlerChangePin()
{
	Q_UNUSED(pObj)

	setVoid();
	auto ui = qobject_cast<UIPlugInJson*>(Env::getSingleton<UILoader>()->getLoaded(UIPlugInName::UIPlugInJson));
	Q_ASSERT(ui);
	Q_EMIT ui->fireChangePinRequested();
}


MsgHandlerChangePin::MsgHandlerChangePin(const QSharedPointer<const ChangePinContext>& pContext)
	: MsgHandlerChangePin()
{
	Q_ASSERT(pContext);

	mJsonObject[QLatin1String("success")] = pContext->getLastPaceResult() == CardReturnCode::OK && !pContext->isWorkflowCancelled();
}
