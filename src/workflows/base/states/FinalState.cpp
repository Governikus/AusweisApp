/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "states/FinalState.h"

#include <QFinalState>
#include <QStateMachine>

using namespace governikus;


void FinalState::run()
{
	// We need a separate QFinaleState since we do not want the controller to stop working until
	// this state has been approved. We add the QFinalState at this point since we need to know
	// the state machine and since we do not want to alter our ctor pattern.
	auto* sStopMachine = new QFinalState();
	machine()->addState(sStopMachine);
	addTransition(this, &AbstractState::fireContinue, sStopMachine);
	addTransition(this, &AbstractState::fireAbort, sStopMachine);

	// Clear plugin types and disconnect all readers when the workflow has completed.
	getContext()->setReaderPlugInTypes({});

	Q_EMIT fireContinue();
}
