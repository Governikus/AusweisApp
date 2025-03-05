/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"

namespace governikus
{

class TestWorkflowContext
	: public WorkflowContext
{
	Q_OBJECT

	public:
		TestWorkflowContext(bool pCanAllowed = false, Action pAction = Action::AUTH);
		~TestWorkflowContext() override = default;

		[[nodiscard]] bool isCanAllowedMode() const override;
		void setCanAllowedMode(bool pCanAllowed);


		[[nodiscard]] virtual QList<AcceptedEidType> getAcceptedEidTypes() const override;
		void setAcceptedEidTypes(const QList<AcceptedEidType>& pAcceptedEidTypes);

	private:
		bool mCanAllowed;
		QList<AcceptedEidType> mAcceptedEidTypes;
};

} // namespace governikus
