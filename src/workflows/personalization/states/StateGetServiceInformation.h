/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief State that initializes the personalization of the Smart-eID applet
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

#include <QVariant>

class test_StateGetServiceInformation;

namespace governikus
{

class StateGetServiceInformation
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateGetServiceInformation;

	private:
		explicit StateGetServiceInformation(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onCommandDone(const QVariant& pResult);
};

} // namespace governikus
