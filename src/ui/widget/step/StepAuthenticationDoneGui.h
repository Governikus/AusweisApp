/*!
 * \brief Qt UI for the authentication done step.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "StepGui.h"


namespace governikus
{

class StepAuthenticationDoneGui
	: public StepGui
{
	Q_OBJECT

	public:
		StepAuthenticationDoneGui(QSharedPointer<AuthContext> pContext);
		virtual ~StepAuthenticationDoneGui() override;

		virtual void forwardStep() override;
};

} // namespace governikus
