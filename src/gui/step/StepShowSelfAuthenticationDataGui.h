/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/SelfAuthenticationContext.h"
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
		StepShowSelfAuthenticationDataGui(QSharedPointer<SelfAuthenticationContext> pContext, AuthenticateStepsWidget* pStepsWidget);
		virtual ~StepShowSelfAuthenticationDataGui();

		virtual void activate() override;
		virtual void deactivate() override;

	private Q_SLOTS:
		virtual void forwardStep() override;

	private:
		QSharedPointer<SelfAuthenticationContext> mContext;
		AuthenticateStepsWidget* mStepsWidget;
};

} /* namespace governikus */
