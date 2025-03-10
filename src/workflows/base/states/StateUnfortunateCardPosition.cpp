/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateUnfortunateCardPosition.h"

#include "AbstractState.h"
#include "GenericContextContainer.h"

#if defined(Q_OS_IOS)
	#include "Env.h"
	#include "ReaderManager.h"
#endif

using namespace governikus;


StateUnfortunateCardPosition::StateUnfortunateCardPosition(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateUnfortunateCardPosition::run()
{
	getContext()->setNpaPositionVerified();
	Q_EMIT fireContinue();
}


void StateUnfortunateCardPosition::onEntry(QEvent* pEvent)
{
#if defined(Q_OS_IOS)
	if (getContext()->getReaderPluginTypes().contains(ReaderManagerPluginType::NFC))
	{
		//: INFO The NFC signal is weak or unstable. The scan is stopped with this information in the iOS dialog.
		const auto& errorMessage = tr("Weak NFC signal");
		Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPluginType::NFC, errorMessage);
	}
#endif

	AbstractState::onEntry(pEvent);
}
