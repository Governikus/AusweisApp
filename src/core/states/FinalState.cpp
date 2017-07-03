/*!
 * FinalError.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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
	QFinalState* sStopMachine = new QFinalState();
	machine()->addState(sStopMachine);
	addTransition(this, &AbstractState::fireSuccess, sStopMachine);
	addTransition(this, &AbstractState::fireError, sStopMachine);
	addTransition(this, &AbstractState::fireCancel, sStopMachine);

	Q_EMIT fireSuccess();
}
