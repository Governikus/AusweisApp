/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CompositeStateTrustedChannel.h"

#include "context/WorkflowContext.h"
#include "states/CompositeStatePace.h"
#include "states/CompositeStateProcessCvcsAndSetRights.h"
#include "states/StateBuilder.h"
#include "states/StateCleanUpReaderManager.h"
#include "states/StateClearPacePasswords.h"
#include "states/StateDidAuthenticateEac1.h"
#include "states/StateDidAuthenticateEac2.h"
#include "states/StateDidList.h"
#include "states/StateEACAdditionalInputType.h"
#include "states/StateGenericSendReceive.h"
#include "states/StateGetTcToken.h"
#include "states/StateInitializeFramework.h"
#include "states/StateProcessCertificatesFromEac2.h"
#include "states/StateStartPaos.h"
#include "states/StateStartPaosResponse.h"
#include "states/StateTransmit.h"
#include "states/StateUpdateRetryCounter.h"

using namespace governikus;


CompositeStateTrustedChannel::CompositeStateTrustedChannel(const QSharedPointer<WorkflowContext>& pContext)
	: QState()
	, mContext(pContext)
{
	auto sGetTcToken = StateBuilder::createState<StateGetTcToken>(mContext);
	auto sStartPaos = StateBuilder::createState<StateStartPaos>(mContext);
	auto sSendStartPaos = StateBuilder::createState<StateSendStartPaos>(mContext);
	auto sInitializeFramework = StateBuilder::createState<StateInitializeFramework>(mContext);
	auto sSendInitializeFrameworkResponse = StateBuilder::createState<StateSendInitializeFrameworkResponse>(mContext);
	auto sDidList = StateBuilder::createState<StateDidList>(mContext);
	auto sSendDidListResponse = StateBuilder::createState<StateSendDIDListResponse>(mContext);
	auto sProcessCvcsAndSetRights = new CompositeStateProcessCvcsAndSetRights(mContext);
	auto sStatePace = new CompositeStatePace(mContext);
	auto sClearPacePasswords = StateBuilder::createState<StateClearPacePasswords>(mContext);
	auto sDidAuthenticateEac1 = StateBuilder::createState<StateDidAuthenticateEac1>(mContext);
	auto sSendDidAuthenticateResponseEac1 = StateBuilder::createState<StateSendDIDAuthenticateResponseEAC1>(mContext);
	auto sEacAdditionalInputType = StateBuilder::createState<StateEACAdditionalInputType>(mContext);
	auto sSendDidAuthenticatResponseEacAdditionalInput = StateBuilder::createState<StateSendDIDAuthenticateResponseEACAdditionalInputType>(mContext);
	auto sProcessCertificatesFromEac2 = StateBuilder::createState<StateProcessCertificatesFromEac2>(mContext);
	auto sDidAuthenticateEac2 = StateBuilder::createState<StateDidAuthenticateEac2>(mContext);
	auto sSendDidAuthenticateResponseEac2 = StateBuilder::createState<StateSendDIDAuthenticateResponseEAC2>(mContext);
	auto sTransmit = StateBuilder::createState<StateTransmit>(mContext);
	auto sSendTransmitResponse = StateBuilder::createState<StateSendTransmitResponse>(mContext);
	auto sSendDisconnectResponse = StateBuilder::createState<StateSendDisconnectResponse>(mContext);
	auto sStartPaosResponse = StateBuilder::createState<StateStartPaosResponse>(mContext);
	auto sUpdateRetryCounterFinal = StateBuilder::createState<StateUpdateRetryCounter>(mContext);
	auto sCleanUpReaderManager = StateBuilder::createState<StateCleanUpReaderManager>(mContext);

	sGetTcToken->setParent(this);
	sStartPaos->setParent(this);
	sSendStartPaos->setParent(this);
	sInitializeFramework->setParent(this);
	sSendInitializeFrameworkResponse->setParent(this);
	sDidList->setParent(this);
	sSendDidListResponse->setParent(this);
	sProcessCvcsAndSetRights->setParent(this);
	sStatePace->setParent(this);
	sClearPacePasswords->setParent(this);
	sDidAuthenticateEac1->setParent(this);
	sSendDidAuthenticateResponseEac1->setParent(this);
	sEacAdditionalInputType->setParent(this);
	sSendDidAuthenticatResponseEacAdditionalInput->setParent(this);
	sProcessCertificatesFromEac2->setParent(this);
	sDidAuthenticateEac2->setParent(this);
	sSendDidAuthenticateResponseEac2->setParent(this);
	sTransmit->setParent(this);
	sSendTransmitResponse->setParent(this);
	sSendDisconnectResponse->setParent(this);
	sStartPaosResponse->setParent(this);
	sUpdateRetryCounterFinal->setParent(this);
	sCleanUpReaderManager->setParent(this);

	setInitialState(sGetTcToken);

	sGetTcToken->addTransition(sGetTcToken, &AbstractState::fireContinue, sStartPaos);
	sGetTcToken->addTransition(sGetTcToken, &AbstractState::fireAbort, sCleanUpReaderManager);

	sStartPaos->addTransition(sStartPaos, &AbstractState::fireContinue, sSendStartPaos);
	sStartPaos->addTransition(sStartPaos, &AbstractState::fireAbort, sCleanUpReaderManager);

	sSendStartPaos->addTransition(sSendStartPaos, &AbstractState::fireContinue, sInitializeFramework);
	sSendStartPaos->addTransition(sSendStartPaos, &AbstractState::fireAbort, sCleanUpReaderManager);
	sSendStartPaos->addTransition(sSendStartPaos, &StateSendStartPaos::fireReceivedDidList, sDidList);
	sSendStartPaos->addTransition(sSendStartPaos, &StateSendStartPaos::fireReceivedExtractCvcsFromEac1InputType, sProcessCvcsAndSetRights);
	sSendStartPaos->addTransition(sSendStartPaos, &StateSendStartPaos::fireReceivedStartPaosResponse, sStartPaosResponse);

	sInitializeFramework->addTransition(sInitializeFramework, &AbstractState::fireContinue, sSendInitializeFrameworkResponse);
	sInitializeFramework->addTransition(sInitializeFramework, &AbstractState::fireAbort, sSendInitializeFrameworkResponse);

	sSendInitializeFrameworkResponse->addTransition(sSendInitializeFrameworkResponse, &AbstractState::fireContinue, sDidList);
	sSendInitializeFrameworkResponse->addTransition(sSendInitializeFrameworkResponse, &AbstractState::fireAbort, sCleanUpReaderManager);
	sSendInitializeFrameworkResponse->addTransition(sSendInitializeFrameworkResponse, &StateSendInitializeFrameworkResponse::fireReceivedExtractCvcsFromEac1InputType, sProcessCvcsAndSetRights);
	sSendInitializeFrameworkResponse->addTransition(sSendInitializeFrameworkResponse, &StateSendInitializeFrameworkResponse::fireReceivedStartPaosResponse, sStartPaosResponse);

	sDidList->addTransition(sDidList, &AbstractState::fireContinue, sSendDidListResponse);
	sDidList->addTransition(sDidList, &AbstractState::fireAbort, sSendDidListResponse);

	sSendDidListResponse->addTransition(sSendDidListResponse, &AbstractState::fireContinue, sProcessCvcsAndSetRights);
	sSendDidListResponse->addTransition(sSendDidListResponse, &AbstractState::fireAbort, sCleanUpReaderManager);
	sSendDidListResponse->addTransition(sSendDidListResponse, &StateSendDIDListResponse::fireReceivedDisconnect, sSendDisconnectResponse);
	sSendDidListResponse->addTransition(sSendDidListResponse, &StateSendDIDListResponse::fireReceivedStartPaosResponse, sStartPaosResponse);

	sProcessCvcsAndSetRights->addTransition(sProcessCvcsAndSetRights, &CompositeStateProcessCvcsAndSetRights::fireContinue, sStatePace);
	sProcessCvcsAndSetRights->addTransition(sProcessCvcsAndSetRights, &CompositeStateProcessCvcsAndSetRights::fireAbort, sSendDidAuthenticateResponseEac1);

	sStatePace->addTransition(sStatePace, &CompositeStatePace::firePaceChannelEstablished, sClearPacePasswords);
	sStatePace->addTransition(sStatePace, &CompositeStatePace::fireAbort, sSendDidAuthenticateResponseEac1);

	sClearPacePasswords->addTransition(sClearPacePasswords, &AbstractState::fireContinue, sDidAuthenticateEac1);
	sClearPacePasswords->addTransition(sClearPacePasswords, &AbstractState::fireAbort, sDidAuthenticateEac1);

	sDidAuthenticateEac1->addTransition(sDidAuthenticateEac1, &AbstractState::fireContinue, sSendDidAuthenticateResponseEac1);
	sDidAuthenticateEac1->addTransition(sDidAuthenticateEac1, &AbstractState::fireAbort, sSendDidAuthenticateResponseEac1);

	sSendDidAuthenticateResponseEac1->addTransition(sSendDidAuthenticateResponseEac1, &AbstractState::fireContinue, sEacAdditionalInputType);
	sSendDidAuthenticateResponseEac1->addTransition(sSendDidAuthenticateResponseEac1, &AbstractState::fireAbort, sUpdateRetryCounterFinal);
	sSendDidAuthenticateResponseEac1->addTransition(sSendDidAuthenticateResponseEac1, &StateSendDIDAuthenticateResponseEAC1::fireReceivedDisconnect, sSendDisconnectResponse);
	sSendDidAuthenticateResponseEac1->addTransition(sSendDidAuthenticateResponseEac1, &StateSendDIDAuthenticateResponseEAC1::fireReceivedStartPaosResponse, sStartPaosResponse);

	sEacAdditionalInputType->addTransition(sEacAdditionalInputType, &AbstractState::fireContinue, sProcessCertificatesFromEac2);
	sEacAdditionalInputType->addTransition(sEacAdditionalInputType, &AbstractState::fireAbort, sSendDidAuthenticatResponseEacAdditionalInput);
	sEacAdditionalInputType->addTransition(sEacAdditionalInputType, &StateEACAdditionalInputType::fireSendDidAuthenticatResponse, sSendDidAuthenticatResponseEacAdditionalInput);

	sSendDidAuthenticatResponseEacAdditionalInput->addTransition(sSendDidAuthenticatResponseEacAdditionalInput, &AbstractState::fireContinue, sProcessCertificatesFromEac2);
	sSendDidAuthenticatResponseEacAdditionalInput->addTransition(sSendDidAuthenticatResponseEacAdditionalInput, &AbstractState::fireAbort, sUpdateRetryCounterFinal);
	sSendDidAuthenticatResponseEacAdditionalInput->addTransition(sSendDidAuthenticatResponseEacAdditionalInput, &StateSendDIDAuthenticateResponseEACAdditionalInputType::fireReceivedStartPaosResponse, sStartPaosResponse);

	sProcessCertificatesFromEac2->addTransition(sProcessCertificatesFromEac2, &AbstractState::fireContinue, sDidAuthenticateEac2);
	sProcessCertificatesFromEac2->addTransition(sProcessCertificatesFromEac2, &AbstractState::fireAbort, sSendDidAuthenticateResponseEac2);

	sDidAuthenticateEac2->addTransition(sDidAuthenticateEac2, &AbstractState::fireContinue, sSendDidAuthenticateResponseEac2);
	sDidAuthenticateEac2->addTransition(sDidAuthenticateEac2, &AbstractState::fireAbort, sSendDidAuthenticateResponseEac2);

	sSendDidAuthenticateResponseEac2->addTransition(sSendDidAuthenticateResponseEac2, &AbstractState::fireContinue, sTransmit);
	sSendDidAuthenticateResponseEac2->addTransition(sSendDidAuthenticateResponseEac2, &AbstractState::fireAbort, sUpdateRetryCounterFinal);
	sSendDidAuthenticateResponseEac2->addTransition(sSendDidAuthenticateResponseEac2, &StateSendDIDAuthenticateResponseEAC2::fireReceivedDisconnect, sSendDisconnectResponse);
	sSendDidAuthenticateResponseEac2->addTransition(sSendDidAuthenticateResponseEac2, &StateSendDIDAuthenticateResponseEAC2::fireReceivedStartPaosResponse, sStartPaosResponse);

	sTransmit->addTransition(sTransmit, &AbstractState::fireContinue, sSendTransmitResponse);
	sTransmit->addTransition(sTransmit, &AbstractState::fireAbort, sSendTransmitResponse);

	sSendTransmitResponse->addTransition(sSendTransmitResponse, &AbstractState::fireContinue, sTransmit);
	sSendTransmitResponse->addTransition(sSendTransmitResponse, &AbstractState::fireAbort, sUpdateRetryCounterFinal);
	sSendTransmitResponse->addTransition(sSendTransmitResponse, &StateSendTransmitResponse::fireReceivedDisconnect, sSendDisconnectResponse);
	sSendTransmitResponse->addTransition(sSendTransmitResponse, &StateSendTransmitResponse::fireReceivedStartPaosResponse, sStartPaosResponse);

	sSendDisconnectResponse->addTransition(sSendDisconnectResponse, &AbstractState::fireContinue, sStartPaosResponse);
	sSendDisconnectResponse->addTransition(sSendDisconnectResponse, &AbstractState::fireAbort, sUpdateRetryCounterFinal);

	sStartPaosResponse->addTransition(sStartPaosResponse, &AbstractState::fireContinue, sUpdateRetryCounterFinal);
	sStartPaosResponse->addTransition(sStartPaosResponse, &AbstractState::fireAbort, sUpdateRetryCounterFinal);

	sUpdateRetryCounterFinal->addTransition(sUpdateRetryCounterFinal, &AbstractState::fireContinue, sCleanUpReaderManager);
	sUpdateRetryCounterFinal->addTransition(sUpdateRetryCounterFinal, &AbstractState::fireAbort, sCleanUpReaderManager);

	connect(sCleanUpReaderManager, &AbstractState::fireContinue, this, &CompositeStateTrustedChannel::fireContinue);
	connect(sCleanUpReaderManager, &AbstractState::fireAbort, this, &CompositeStateTrustedChannel::fireAbort);
}
