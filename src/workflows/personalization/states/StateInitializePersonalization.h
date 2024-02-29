/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief State that initializes the personalization of the Smart-eID applet
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

#include <QVariant>

class test_StateInitializePersonalization;

namespace governikus
{

class StateInitializePersonalization
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateInitializePersonalization;

	private:
		explicit StateInitializePersonalization(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onCommandDone(const QVariant& pResult);
};

} // namespace governikus
