/*!
 * \brief Qt UI for the authentication done step.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "StepGui.h"
#include "context/AuthContext.h"


namespace governikus
{

class StepAuthenticationDoneGui
	: public StepGui
{
	Q_OBJECT

	public:
		StepAuthenticationDoneGui(QSharedPointer<AuthContext> pContext);
		virtual ~StepAuthenticationDoneGui();

		virtual void forwardStep() override;
};

} /* namespace governikus */
