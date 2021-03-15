/*
 * \brief Allows the user to send a survey after a successful authentication
 * on Android.
 *
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"

#include <QSharedPointer>

namespace governikus
{
class StateSendWhitelistSurvey
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateSendWhitelistSurvey(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;
};

} // namespace governikus
