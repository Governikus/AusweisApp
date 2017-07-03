/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StepAuthenticationDoneGui.h"

using namespace governikus;


StepAuthenticationDoneGui::StepAuthenticationDoneGui(QSharedPointer<AuthContext> pContext)
	: StepGui(pContext)
{
}


StepAuthenticationDoneGui::~StepAuthenticationDoneGui()
{
}


void StepAuthenticationDoneGui::forwardStep()
{
	Q_EMIT fireCancelled();
}
