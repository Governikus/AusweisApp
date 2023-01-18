/*!
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
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


		[[nodiscard]] virtual QVector<AcceptedEidType> getAcceptedEidTypes() const override;
		void setAcceptedEidTypes(const QVector<AcceptedEidType>& pAcceptedEidTypes);

	private:
		bool mCanAllowed;
		QVector<AcceptedEidType> mAcceptedEidTypes;
};

} // namespace governikus
