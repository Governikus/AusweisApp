/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

#include "CompositeStatePrepareApplet.h"

#include "states/StateBuilder.h"
#include "states/StateCheckApplet.h"
#include "states/StateDeleteApplet.h"
#include "states/StateDeletePersonalization.h"
#include "states/StateInstallApplet.h"
#include "states/StateUpdateSupportInfo.h"


using namespace governikus;


CompositeStatePrepareApplet::CompositeStatePrepareApplet(const QSharedPointer<WorkflowContext>& pContext)
	: QState()
	, mContext(pContext)
{
	auto* sCheckApplet = StateBuilder::createState<StateCheckApplet>(mContext);
	auto* sInstallApplet = StateBuilder::createState<StateInstallApplet>(mContext);
	auto* sDeletePersonalization = StateBuilder::createState<StateDeletePersonalization>(mContext);
	auto* sDeleteApplet = StateBuilder::createState<StateDeleteApplet>(mContext);
	auto* sUpdateSupportInfo = StateBuilder::createState<StateUpdateSupportInfo>(mContext);

	sCheckApplet->setParent(this);
	sInstallApplet->setParent(this);
	sDeletePersonalization->setParent(this);
	sDeleteApplet->setParent(this);
	sUpdateSupportInfo->setParent(this);

	setInitialState(sCheckApplet);

	sCheckApplet->addTransition(sCheckApplet, &StateCheckApplet::fireInstallApplet, sInstallApplet);
	sCheckApplet->addTransition(sCheckApplet, &StateCheckApplet::fireDeletePersonalization, sDeletePersonalization);
	sCheckApplet->addTransition(sCheckApplet, &StateCheckApplet::fireDeleteApplet, sDeleteApplet);
	sCheckApplet->addTransition(sCheckApplet, &StateCheckApplet::fireContinue, sUpdateSupportInfo);
	connect(sCheckApplet, &AbstractState::fireAbort, this, &CompositeStatePrepareApplet::fireAbort);

	connect(sInstallApplet, &AbstractState::fireContinue, this, &CompositeStatePrepareApplet::fireContinue);
	connect(sInstallApplet, &AbstractState::fireAbort, this, &CompositeStatePrepareApplet::fireAbort);

	sDeletePersonalization->addTransition(sDeletePersonalization, &AbstractState::fireContinue, sCheckApplet);
	connect(sDeletePersonalization, &AbstractState::fireAbort, this, &CompositeStatePrepareApplet::fireAbort);

	sUpdateSupportInfo->addTransition(sUpdateSupportInfo, &StateUpdateSupportInfo::fireUpdateAvailable, sDeleteApplet);
	connect(sUpdateSupportInfo, &AbstractState::fireContinue, this, &CompositeStatePrepareApplet::fireContinue);
	connect(sUpdateSupportInfo, &AbstractState::fireAbort, this, &CompositeStatePrepareApplet::fireAbort);

	sDeleteApplet->addTransition(sDeleteApplet, &AbstractState::fireContinue, sInstallApplet);
	connect(sDeleteApplet, &AbstractState::fireAbort, this, &CompositeStatePrepareApplet::fireAbort);
}
