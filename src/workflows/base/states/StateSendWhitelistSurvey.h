/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"

#include <QSharedPointer>


class test_StateSendWhitelistSurvey;


namespace governikus
{
class StateSendWhitelistSurvey
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateSendWhitelistSurvey;

	private:
		explicit StateSendWhitelistSurvey(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
