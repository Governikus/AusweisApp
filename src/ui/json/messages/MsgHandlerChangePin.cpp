/*!
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerChangePin.h"

#include "Env.h"
#include "UILoader.h"
#include "UIPlugInJson.h"
#include "VolatileSettings.h"

using namespace governikus;


MsgHandlerChangePin::MsgHandlerChangePin()
	: MsgHandler(MsgType::CHANGE_PIN)
{
}


MsgHandlerChangePin::MsgHandlerChangePin(const QJsonObject& pObj)
	: MsgHandlerChangePin()
{
	setVoid();
	initMessages(pObj[QLatin1String("messages")].toObject());
	initHandleInterrupt(pObj[QLatin1String("handleInterrupt")]);

	auto ui = qobject_cast<UIPlugInJson*>(Env::getSingleton<UILoader>()->getLoaded(UIPlugInName::UIPlugInJson));
	Q_ASSERT(ui);
	Q_EMIT ui->fireChangePinRequested(false);
}


MsgHandlerChangePin::MsgHandlerChangePin(const QSharedPointer<const ChangePinContext>& pContext)
	: MsgHandlerChangePin()
{
	Q_ASSERT(pContext);

	mJsonObject[QLatin1String("success")] = pContext->getLastPaceResult() == CardReturnCode::OK && !pContext->isWorkflowCancelled();
}


void MsgHandlerChangePin::initMessages(const QJsonObject& pUi)
{
	if (!pUi.isEmpty())
	{
		const VolatileSettings::Messages messages(pUi[QLatin1String("sessionStarted")].toString(),
				pUi[QLatin1String("sessionFailed")].toString(),
				pUi[QLatin1String("sessionSucceeded")].toString(),
				pUi[QLatin1String("sessionInProgress")].toString());

		Env::getSingleton<VolatileSettings>()->setMessages(messages);
	}
}


void MsgHandlerChangePin::initHandleInterrupt(const QJsonValue& pValue)
{
	if (pValue.isBool())
	{
		Env::getSingleton<VolatileSettings>()->setHandleInterrupt(pValue.toBool());
	}
}
