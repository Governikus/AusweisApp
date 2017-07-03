/*!
 * CompositeStateSelectCard.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "CompositeStateProcessCvcsAndSetRights.h"

#include "context/WorkflowContext.h"
#include "states/StateBuilder.h"
#include "states/StateCertificateDescriptionCheck.h"
#include "states/StateCheckCertificates.h"
#include "states/StateEditAccessRights.h"
#include "states/StateExtractCvcsFromEac1IntputType.h"
#include "states/StatePreVerification.h"


using namespace governikus;


CompositeStateProcessCvcsAndSetRights::CompositeStateProcessCvcsAndSetRights(const QSharedPointer<WorkflowContext>& pContext)
	: QState()
	, mContext(pContext)
{
	auto sExtractCvcsFromEac1InputType = StateBuilder::createState<StateExtractCvcsFromEac1IntputType>(mContext);
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

	sExtractCvcsFromEac1InputType->addTransition(sExtractCvcsFromEac1InputType, &AbstractState::fireSuccess, sPreVerification);
	connect(sExtractCvcsFromEac1InputType, &AbstractState::fireError, this, &CompositeStateProcessCvcsAndSetRights::fireError);
	connect(sExtractCvcsFromEac1InputType, &AbstractState::fireCancel, this, &CompositeStateProcessCvcsAndSetRights::fireCancel);

	sPreVerification->addTransition(sPreVerification, &AbstractState::fireSuccess, sCertificateDescriptionCheck);
	connect(sPreVerification, &AbstractState::fireError, this, &CompositeStateProcessCvcsAndSetRights::fireError);
	connect(sPreVerification, &AbstractState::fireCancel, this, &CompositeStateProcessCvcsAndSetRights::fireCancel);

	sCertificateDescriptionCheck->addTransition(sCertificateDescriptionCheck, &AbstractState::fireSuccess, sCheckCertificates);
	connect(sCertificateDescriptionCheck, &AbstractState::fireError, this, &CompositeStateProcessCvcsAndSetRights::fireError);
	connect(sCertificateDescriptionCheck, &AbstractState::fireCancel, this, &CompositeStateProcessCvcsAndSetRights::fireCancel);

	sCheckCertificates->addTransition(sCheckCertificates, &AbstractState::fireSuccess, sEditAccessRights);
	connect(sCheckCertificates, &AbstractState::fireError, this, &CompositeStateProcessCvcsAndSetRights::fireError);
	connect(sCheckCertificates, &AbstractState::fireCancel, this, &CompositeStateProcessCvcsAndSetRights::fireCancel);

	connect(sEditAccessRights, &AbstractState::fireSuccess, this, &CompositeStateProcessCvcsAndSetRights::fireSuccess);
	connect(sEditAccessRights, &AbstractState::fireError, this, &CompositeStateProcessCvcsAndSetRights::fireError);
	connect(sEditAccessRights, &AbstractState::fireCancel, this, &CompositeStateProcessCvcsAndSetRights::fireCancel);
}


CompositeStateProcessCvcsAndSetRights::~CompositeStateProcessCvcsAndSetRights()
{
}
