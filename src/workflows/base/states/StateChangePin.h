/*!
 * \brief Controller for the state changing the PIN.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/ChangePinContext.h"

class test_StateChangePin;

namespace governikus
{

class StateChangePin
	: public AbstractState
	, public GenericContextContainer<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateChangePin;

	private:
		explicit StateChangePin(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onSetEidPinDone(QSharedPointer<BaseCardCommand> pCommand);

	Q_SIGNALS:
		void fireRetry();
};

} // namespace governikus
