/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/SelfAuthContext.h"
#include "StepGui.h"

#include <QLabel>

namespace governikus
{

class AuthenticateStepsWidget;

class StepShowSelfAuthenticationDataGui
	: public StepGui
{
	Q_OBJECT

	public:
		StepShowSelfAuthenticationDataGui(QSharedPointer<SelfAuthContext> pContext, AuthenticateStepsWidget* pStepsWidget);
		virtual ~StepShowSelfAuthenticationDataGui() override;

		virtual void activate() override;
		virtual void deactivate() override;

	private Q_SLOTS:
		virtual void forwardStep() override;

	private:
		QSharedPointer<SelfAuthContext> mContext;
		AuthenticateStepsWidget* mStepsWidget;
};

} // namespace governikus
