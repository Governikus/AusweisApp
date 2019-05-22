/*!
 * \brief Controller for the state changing the PIN.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"
#include "context/ChangePinContext.h"

class test_StateChangePin;

namespace governikus
{

class StateChangePin
	: public AbstractGenericState<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateChangePin;

	explicit StateChangePin(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onSetEidPinDone(QSharedPointer<BaseCardCommand> pCommand);

	Q_SIGNALS:
		void fireInvalidPin();
};

} // namespace governikus
