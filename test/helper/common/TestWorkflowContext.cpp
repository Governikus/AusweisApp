/*!
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

#include "TestWorkflowContext.h"


using namespace governikus;


TestWorkflowContext::TestWorkflowContext(bool pCanAllowed, Action pAction)
	: WorkflowContext(pAction)
	, mCanAllowed(pCanAllowed)
	, mAcceptedEidTypes({AcceptedEidType::CARD_CERTIFIED, AcceptedEidType::SE_CERTIFIED, AcceptedEidType::SE_ENDORSED})
{
}


bool TestWorkflowContext::isCanAllowedMode() const
{
	return mCanAllowed;
}


void TestWorkflowContext::setCanAllowedMode(bool pCanAllowed)
{
	mCanAllowed = pCanAllowed;
	Q_EMIT fireCanAllowedModeChanged();
}


QVector<AcceptedEidType> TestWorkflowContext::getAcceptedEidTypes() const
{
	return mAcceptedEidTypes;
}


void TestWorkflowContext::setAcceptedEidTypes(const QVector<AcceptedEidType>& pAcceptedEidTypes)
{
	mAcceptedEidTypes = pAcceptedEidTypes;
}
