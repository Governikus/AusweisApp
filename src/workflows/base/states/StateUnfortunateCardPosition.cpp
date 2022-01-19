/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "StateUnfortunateCardPosition.h"

using namespace governikus;


StateUnfortunateCardPosition::StateUnfortunateCardPosition(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
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
	stopScanIfNecessary(tr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable"));

	AbstractState::onEntry(pEvent);
}
