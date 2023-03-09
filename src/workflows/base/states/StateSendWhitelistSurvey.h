/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Allows the user to send a survey after a successful authentication
 * on Android.
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
