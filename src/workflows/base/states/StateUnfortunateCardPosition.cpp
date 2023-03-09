/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateUnfortunateCardPosition.h"

#include "AbstractState.h"
#include "GenericContextContainer.h"

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
	//: INFO The NFC signal is weak or unstable. The scan is stopped with this information in the iOS dialog.
	stopNfcScanIfNecessary(tr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable"));

	AbstractState::onEntry(pEvent);
}
