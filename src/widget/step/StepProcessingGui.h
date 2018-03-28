/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "StepGui.h"


namespace governikus
{

class AuthenticateStepsWidget;

class StepProcessingGui
	: public StepGui
{
	Q_OBJECT

	public:
		StepProcessingGui(const QSharedPointer<WorkflowContext>& pContext, AuthenticateStepsWidget* pStepsWidget);
		virtual ~StepProcessingGui() override;

		virtual void activate() override;
		virtual void deactivate() override;

	private:
		AuthenticateStepsWidget* mStepsWidget;
};

} /* namespace governikus */
