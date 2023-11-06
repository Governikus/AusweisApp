/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateChangeSmartPin.h"

#include "AppSettings.h"
#include "ReaderManagerPlugInInfo.h"
#include "VolatileSettings.h"


using namespace governikus;


StateChangeSmartPin::StateChangeSmartPin(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setKeepCardConnectionAlive();
}


void StateChangeSmartPin::run()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || !defined(QT_NO_DEBUG)
	if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		const auto& context = getContext();
		if (context->getNewPin().isEmpty())
		{
			*this << connect(context.data(), &PersonalizationContext::fireNewPinChanged, this, &StateChangeSmartPin::callSetEidPin);
			return;
		}
	}
#endif

	callSetEidPin();
}


void StateChangeSmartPin::callSetEidPin()
{
	auto cardConnection = getContext()->getCardConnection();

	Q_ASSERT(cardConnection);
	qDebug() << "Invoke set Eid PIN command";
	*this << cardConnection->callSetEidPinCommand(this, &StateChangeSmartPin::onSetEidPinDone, getContext()->getNewPin().toLatin1());
}


void StateChangeSmartPin::onSetEidPinDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const CardReturnCode returnCode = pCommand->getReturnCode();
	getContext()->setLastPaceResult(returnCode);
	switch (returnCode)
	{
		case CardReturnCode::OK:
		{
			const auto& context = getContext();
			context->setProgress(100, context->getProgressMessage());

			GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			settings.setLastReaderPluginType(getEnumName(ReaderManagerPlugInType::SMART));

			Q_EMIT fireContinue();
			break;
		}

		default:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort({FailureCode::Reason::Change_Smart_Pin_Failed,
							  {FailureCode::Info::Card_Return_Code, Enum<CardReturnCode>::getName(returnCode)}
					});
	}
}
