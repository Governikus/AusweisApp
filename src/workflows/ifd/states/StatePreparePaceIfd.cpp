/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StatePreparePaceIfd.h"

#include "AppSettings.h"
#include "pinpad/EstablishPaceChannel.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StatePreparePaceIfd::StatePreparePaceIfd(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StatePreparePaceIfd::run()
{
	const bool pinPadMode = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
	if (pinPadMode)
	{
		switch (getContext()->getEstablishPaceChannel().getPasswordId())
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

			case PacePasswordId::PACE_MRZ:
			case PacePasswordId::UNKNOWN:
				qCritical() << "Cannot handle unknown PacePasswordId";
				Q_EMIT fireAbort(FailureCode::Reason::Prepare_Pace_Ifd_Unknown);
				return;
		}
	}

	Q_EMIT fireContinue();
}
