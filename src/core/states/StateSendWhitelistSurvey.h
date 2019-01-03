/*
 * \brief Allows the user to send a survey after a successful authentication
 * on Android.
 *
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"


namespace governikus
{
class StateSendWhitelistSurvey
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateSendWhitelistSurvey(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} // namespace governikus
