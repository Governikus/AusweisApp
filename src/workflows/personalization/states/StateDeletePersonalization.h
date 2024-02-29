/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Helper state that deletes the Smart-eID personalization
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

#include <QVariant>


class test_StateDeletePersonalization;


namespace governikus
{

class StateDeletePersonalization
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateDeletePersonalization;

	private:
		explicit StateDeletePersonalization(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onCommandDone(const QVariant& pResult);

	public Q_SLOTS:
		void onUserCancelled() override;

};

} // namespace governikus
