/*
 * StatePrepareChat.h
 *
 *
 * \brief Determines the optional and required chat either from elements OptionalCHAT and RequiredCHAT in EAC1InputType or from CVC.
 * Initially sets the effective chat to the required chat.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"


namespace governikus
{

class StatePrepareChat
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StatePrepareChat(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} /* namespace governikus */
