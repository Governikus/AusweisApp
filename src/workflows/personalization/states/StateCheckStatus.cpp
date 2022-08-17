/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateCheckStatus.h"

#include "ReaderManager.h"

using namespace governikus;


StateCheckStatus::StateCheckStatus(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateCheckStatus::run()
{
	const auto pluginInfos = Env::getSingleton<ReaderManager>()->getPlugInInfos();
	for (const auto& info : pluginInfos)
	{
		if (info.getPlugInType() == ReaderManagerPlugInType::SMART && info.isAvailable())
		{
			Q_EMIT fireContinue();
			return;
		}
	}

	updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Unavailable);
	qDebug() << "Smart-eID not available";
	Q_EMIT fireAbort();
}
