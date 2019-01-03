/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "StatePreparePaceRemote.h"

#include "AppSettings.h"
#include "EstablishPaceChannelParser.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StatePreparePaceRemote::StatePreparePaceRemote(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StatePreparePaceRemote::run()
{
	const bool pinPadMode = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
	if (pinPadMode)
	{
		const QSharedPointer<const IfdEstablishPaceChannel>& establishPaceChannelMessage = getContext()->getEstablishPaceChannelMessage();
		const EstablishPaceChannelParser& parser = EstablishPaceChannelParser::fromCcid(establishPaceChannelMessage->getInputData());

		switch (parser.getPasswordId())
		{
			case PacePasswordId::PACE_CAN:
				if (getContext()->getCan().isEmpty())
				{
					Q_EMIT fireEnterPacePassword();
					return;
				}
				break;

			case PacePasswordId::PACE_PIN:
				if (getContext()->getPin().isEmpty())
				{
					Q_EMIT fireEnterPacePassword();
					return;
				}

				break;

			case PacePasswordId::PACE_PUK:
				if (getContext()->getPuk().isEmpty())
				{
					Q_EMIT fireEnterPacePassword();
					return;
				}
				break;

			default:
				Q_UNREACHABLE();
		}
	}

	Q_EMIT fireContinue();
}
