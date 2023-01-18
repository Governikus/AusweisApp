/*!
 * \brief Controller for the state changing the initial Smart-eID PIN.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

class test_StateChangeSmartPin;

namespace governikus
{

class StateChangeSmartPin
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateChangeSmartPin;

	private:
		explicit StateChangeSmartPin(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void callSetEidPin();
		void onSetEidPinDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
