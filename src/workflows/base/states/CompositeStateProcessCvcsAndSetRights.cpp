/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "CompositeStateProcessCvcsAndSetRights.h"

#include "context/WorkflowContext.h"
#include "states/StateBuilder.h"
#include "states/StateCertificateDescriptionCheck.h"
#include "states/StateCheckCertificates.h"
#include "states/StateEditAccessRights.h"
#include "states/StateExtractCvcsFromEac1InputType.h"
#include "states/StatePreVerification.h"


using namespace governikus;


CompositeStateProcessCvcsAndSetRights::CompositeStateProcessCvcsAndSetRights(const QSharedPointer<WorkflowContext>& pContext)
	: QState()
	, mContext(pContext)
{
	auto sExtractCvcsFromEac1InputType = StateBuilder::createState<StateExtractCvcsFromEac1InputType>(mContext);
	auto sPreVerification = StateBuilder::createState<StatePreVerification>(mContext);
	auto sCertificateDescriptionCheck = StateBuilder::createState<StateCertificateDescriptionCheck>(mContext);
	auto sCheckCertificates = StateBuilder::createState<StateCheckCertificates>(mContext);
	auto sEditAccessRights = StateBuilder::createState<StateEditAccessRights>(mContext);

	sExtractCvcsFromEac1InputType->setParent(this);
	sPreVerification->setParent(this);
	sCertificateDescriptionCheck->setParent(this);
	sCheckCertificates->setParent(this);
	sEditAccessRights->setParent(this);

	setInitialState(sExtractCvcsFromEac1InputType);

	sExtractCvcsFromEac1InputType->addTransition(sExtractCvcsFromEac1InputType, &AbstractState::fireContinue, sPreVerification);
	connect(sExtractCvcsFromEac1InputType, &AbstractState::fireAbort, this, &CompositeStateProcessCvcsAndSetRights::fireAbort);

	sPreVerification->addTransition(sPreVerification, &AbstractState::fireContinue, sCertificateDescriptionCheck);
	connect(sPreVerification, &AbstractState::fireAbort, this, &CompositeStateProcessCvcsAndSetRights::fireAbort);

	sCertificateDescriptionCheck->addTransition(sCertificateDescriptionCheck, &AbstractState::fireContinue, sCheckCertificates);
	connect(sCertificateDescriptionCheck, &AbstractState::fireAbort, this, &CompositeStateProcessCvcsAndSetRights::fireAbort);

	sCheckCertificates->addTransition(sCheckCertificates, &AbstractState::fireContinue, sEditAccessRights);
	connect(sCheckCertificates, &AbstractState::fireAbort, this, &CompositeStateProcessCvcsAndSetRights::fireAbort);

	connect(sEditAccessRights, &AbstractState::fireContinue, this, &CompositeStateProcessCvcsAndSetRights::fireContinue);
	connect(sEditAccessRights, &AbstractState::fireAbort, this, &CompositeStateProcessCvcsAndSetRights::fireAbort);
}
