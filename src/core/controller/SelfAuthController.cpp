/*!
 * SelfAuthController.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "context/SelfAuthenticationContext.h"
#include "controller/SelfAuthController.h"
#include "states/CompositeStateProcessCvcsAndSetRights.h"
#include "states/CompositeStateSelectCard.h"
#include "states/FinalState.h"
#include "states/StateCheckError.h"
#include "states/StateCheckRefreshAddress.h"
#include "states/StateCleanUpReaderManager.h"
#include "states/StateDidAuthenticateEac1.h"
#include "states/StateDidAuthenticateEac2.h"
#include "states/StateDidList.h"
#include "states/StateEACAdditionalInputType.h"
#include "states/StateEstablishPaceCan.h"
#include "states/StateEstablishPacePin.h"
#include "states/StateGenericSendReceive.h"
#include "states/StateGetSelfAuthenticationData.h"
#include "states/StateGetTcToken.h"
#include "states/StateInitializeFramework.h"
#include "states/StateLoadTcTokenUrl.h"
#include "states/StateProcessCertificatesFromEac2.h"
#include "states/StateStartPaos.h"
#include "states/StateStartPaosResponse.h"
#include "states/StateTransmit.h"
#include "states/StateUpdateRetryCounter.h"
#include "states/StateWriteHistory.h"


#include <QDebug>
#include <initializer_list>


using namespace governikus;


SelfAuthController::SelfAuthController(QSharedPointer<SelfAuthenticationContext> pContext)
	: WorkflowController(pContext)
{
	auto sLoadTcTokenUrl = addAndConnectState<StateLoadTcTokenUrl>();
	mStateMachine.setInitialState(sLoadTcTokenUrl);
	auto sGetTcToken = addAndConnectState<StateGetTcToken>();
	auto sStartPaos = addAndConnectState<StateStartPaos>();
	auto sSendStartPaos = addAndConnectState<StateSendStartPaos>();
	auto sInitializeFramework = addAndConnectState<StateInitializeFramework>();
	auto sSendInitializeFrameworkResponse = addAndConnectState<StateSendInitializeFrameworkResponse>();
	auto sDidList = addAndConnectState<StateDidList>();
	auto sSendDidListResponse = addAndConnectState<StateSendDIDListResponse>();
	auto sProcessCvcsAndSetRights = new CompositeStateProcessCvcsAndSetRights(pContext);
	mStateMachine.addState(sProcessCvcsAndSetRights);
	auto sSelectCard = new CompositeStateSelectCard(pContext);
	mStateMachine.addState(sSelectCard);
	auto sUpdateRetryCounter = addAndConnectState<StateUpdateRetryCounter>();
	auto sEstablishPaceCan = addAndConnectState<StateEstablishPaceCan>();
	auto sEstablishPacePin = addAndConnectState<StateEstablishPacePin>();
	auto sDidAuthenticateEac1 = addAndConnectState<StateDidAuthenticateEac1>();
	auto sSendDidAuthenticateResponseEac1 = addAndConnectState<StateSendDIDAuthenticateResponseEAC1>();
	auto sEacAdditionalInputType = addAndConnectState<StateEACAdditionalInputType>();
	auto sSendDidAuthenticatResponseEacAdditionalInput = addAndConnectState<StateSendDIDAuthenticateResponseEACAdditionalInputType>();
	auto sProcessCertificatesFromEac2 = addAndConnectState<StateProcessCertificatesFromEac2>();
	auto sDidAuthenticateEac2 = addAndConnectState<StateDidAuthenticateEac2>();
	auto sSendDidAuthenticateResponseEac2 = addAndConnectState<StateSendDIDAuthenticateResponseEAC2>();
	auto sTransmit = addAndConnectState<StateTransmit>();
	auto sSendTransmitResponse = addAndConnectState<StateSendTransmitResponse>();
	auto sSendDisconnectResponse = addAndConnectState<StateSendDisconnectResponse>();
	auto sStartPaosResponse = addAndConnectState<StateStartPaosResponse>();
	auto sCheckErrorEpilogue = addAndConnectState<StateCheckError>();
	auto sCheckRefreshAddress = addAndConnectState<StateCheckRefreshAddress>();
	auto sWriteHistory = addAndConnectState<StateWriteHistory>();
	auto sGetSelfAuthenticationData = addAndConnectState<StateGetSelfAuthenticationData>();
	auto sCleanUpReaderManager = addAndConnectState<StateCleanUpReaderManager>();
	auto sFinal = addAndConnectState<FinalState>();

	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireSuccess, sGetTcToken);
	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireError, sCleanUpReaderManager);
	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireCancel, sCleanUpReaderManager);

	sGetTcToken->addTransition(sGetTcToken, &AbstractState::fireSuccess, sStartPaos);
	sGetTcToken->addTransition(sGetTcToken, &AbstractState::fireError, sCleanUpReaderManager);
	sGetTcToken->addTransition(sGetTcToken, &AbstractState::fireCancel, sCleanUpReaderManager);

	sStartPaos->addTransition(sStartPaos, &AbstractState::fireSuccess, sSendStartPaos);
	sStartPaos->addTransition(sStartPaos, &AbstractState::fireError, sCleanUpReaderManager);
	sStartPaos->addTransition(sStartPaos, &AbstractState::fireCancel, sCleanUpReaderManager);

	sSendStartPaos->addTransition(sSendStartPaos, &AbstractState::fireSuccess, sInitializeFramework);
	sSendStartPaos->addTransition(sSendStartPaos, &AbstractState::fireError, sCleanUpReaderManager);
	sSendStartPaos->addTransition(sSendStartPaos, &AbstractState::fireCancel, sCleanUpReaderManager);
	sSendStartPaos->addTransition(sSendStartPaos, &StateSendStartPaos::fireReceivedDidList, sDidList);
	sSendStartPaos->addTransition(sSendStartPaos, &StateSendStartPaos::fireReceivedExtractCvcsFromEac1InputType, sProcessCvcsAndSetRights);
	sSendStartPaos->addTransition(sSendStartPaos, &StateSendStartPaos::fireReceivedStartPaosResponse, sStartPaosResponse);

	sInitializeFramework->addTransition(sInitializeFramework, &AbstractState::fireSuccess, sSendInitializeFrameworkResponse);
	sInitializeFramework->addTransition(sInitializeFramework, &AbstractState::fireError, sSendInitializeFrameworkResponse);
	sInitializeFramework->addTransition(sInitializeFramework, &AbstractState::fireCancel, sSendInitializeFrameworkResponse);

	sSendInitializeFrameworkResponse->addTransition(sSendInitializeFrameworkResponse, &AbstractState::fireSuccess, sDidList);
	sSendInitializeFrameworkResponse->addTransition(sSendInitializeFrameworkResponse, &AbstractState::fireError, sCleanUpReaderManager);
	sSendInitializeFrameworkResponse->addTransition(sSendInitializeFrameworkResponse, &AbstractState::fireCancel, sCleanUpReaderManager);
	sSendInitializeFrameworkResponse->addTransition(sSendInitializeFrameworkResponse, &StateSendInitializeFrameworkResponse::fireReceivedExtractCvcsFromEac1InputType, sProcessCvcsAndSetRights);
	sSendInitializeFrameworkResponse->addTransition(sSendInitializeFrameworkResponse, &StateSendInitializeFrameworkResponse::fireReceivedStartPaosResponse, sStartPaosResponse);

	sDidList->addTransition(sDidList, &AbstractState::fireSuccess, sSendDidListResponse);
	sDidList->addTransition(sDidList, &AbstractState::fireError, sSendDidListResponse);
	sDidList->addTransition(sDidList, &AbstractState::fireCancel, sSendDidListResponse);

	sSendDidListResponse->addTransition(sSendDidListResponse, &AbstractState::fireSuccess, sProcessCvcsAndSetRights);
	sSendDidListResponse->addTransition(sSendDidListResponse, &AbstractState::fireError, sCleanUpReaderManager);
	sSendDidListResponse->addTransition(sSendDidListResponse, &AbstractState::fireCancel, sCleanUpReaderManager);
	sSendDidListResponse->addTransition(sSendDidListResponse, &StateSendDIDListResponse::fireReceivedDisconnect, sSendDisconnectResponse);
	sSendDidListResponse->addTransition(sSendDidListResponse, &StateSendDIDListResponse::fireReceivedStartPaosResponse, sStartPaosResponse);

	sProcessCvcsAndSetRights->addTransition(sProcessCvcsAndSetRights, &CompositeStateProcessCvcsAndSetRights::fireSuccess, sSelectCard);
	sProcessCvcsAndSetRights->addTransition(sProcessCvcsAndSetRights, &CompositeStateProcessCvcsAndSetRights::fireError, sSendDidAuthenticateResponseEac1);
	sProcessCvcsAndSetRights->addTransition(sProcessCvcsAndSetRights, &CompositeStateProcessCvcsAndSetRights::fireCancel, sSendDidAuthenticateResponseEac1);

	sSelectCard->addTransition(sSelectCard, &CompositeStateSelectCard::fireSuccess, sUpdateRetryCounter);
	sSelectCard->addTransition(sSelectCard, &CompositeStateSelectCard::fireError, sSendDidAuthenticateResponseEac1);
	sSelectCard->addTransition(sSelectCard, &CompositeStateSelectCard::fireCancel, sSendDidAuthenticateResponseEac1);

	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &StateUpdateRetryCounter::fireRetryCounterIsGTOne, sEstablishPacePin);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &StateUpdateRetryCounter::fireRetryCounterIsOne, sEstablishPaceCan);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &StateUpdateRetryCounter::fireRetryCounterIsZero, sSendDidAuthenticateResponseEac1);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireError, sSendDidAuthenticateResponseEac1);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireCancel, sSendDidAuthenticateResponseEac1);

	sEstablishPaceCan->addTransition(sEstablishPaceCan, &AbstractState::fireSuccess, sEstablishPacePin);
	sEstablishPaceCan->addTransition(sEstablishPaceCan, &StateEstablishPaceCan::fireInvalidCan, sUpdateRetryCounter);
	sEstablishPaceCan->addTransition(sEstablishPaceCan, &AbstractState::fireError, sSendDidAuthenticateResponseEac1);
	sEstablishPaceCan->addTransition(sEstablishPaceCan, &AbstractState::fireCancel, sSendDidAuthenticateResponseEac1);

	sEstablishPacePin->addTransition(sEstablishPacePin, &AbstractState::fireSuccess, sDidAuthenticateEac1);
	sEstablishPacePin->addTransition(sEstablishPacePin, &StateEstablishPacePin::fireInvalidPin, sUpdateRetryCounter);
	sEstablishPacePin->addTransition(sEstablishPacePin, &AbstractState::fireError, sSendDidAuthenticateResponseEac1);
	sEstablishPacePin->addTransition(sEstablishPacePin, &AbstractState::fireCancel, sSendDidAuthenticateResponseEac1);

	sDidAuthenticateEac1->addTransition(sDidAuthenticateEac1, &AbstractState::fireSuccess, sSendDidAuthenticateResponseEac1);
	sDidAuthenticateEac1->addTransition(sDidAuthenticateEac1, &AbstractState::fireError, sSendDidAuthenticateResponseEac1);
	sDidAuthenticateEac1->addTransition(sDidAuthenticateEac1, &AbstractState::fireCancel, sSendDidAuthenticateResponseEac1);

	sSendDidAuthenticateResponseEac1->addTransition(sSendDidAuthenticateResponseEac1, &AbstractState::fireSuccess, sEacAdditionalInputType);
	sSendDidAuthenticateResponseEac1->addTransition(sSendDidAuthenticateResponseEac1, &AbstractState::fireError, sCleanUpReaderManager);
	sSendDidAuthenticateResponseEac1->addTransition(sSendDidAuthenticateResponseEac1, &AbstractState::fireCancel, sCleanUpReaderManager);
	sSendDidAuthenticateResponseEac1->addTransition(sSendDidAuthenticateResponseEac1, &StateSendDIDAuthenticateResponseEAC1::fireReceivedDisconnect, sSendDisconnectResponse);
	sSendDidAuthenticateResponseEac1->addTransition(sSendDidAuthenticateResponseEac1, &StateSendDIDAuthenticateResponseEAC1::fireReceivedStartPaosResponse, sStartPaosResponse);

	sEacAdditionalInputType->addTransition(sEacAdditionalInputType, &AbstractState::fireSuccess, sProcessCertificatesFromEac2);
	sEacAdditionalInputType->addTransition(sEacAdditionalInputType, &AbstractState::fireError, sSendDidAuthenticatResponseEacAdditionalInput);
	sEacAdditionalInputType->addTransition(sEacAdditionalInputType, &AbstractState::fireCancel, sSendDidAuthenticatResponseEacAdditionalInput);
	sEacAdditionalInputType->addTransition(sEacAdditionalInputType, &StateEACAdditionalInputType::fireSendDidAuthenticatResponse, sSendDidAuthenticatResponseEacAdditionalInput);

	sSendDidAuthenticatResponseEacAdditionalInput->addTransition(sSendDidAuthenticatResponseEacAdditionalInput, &AbstractState::fireSuccess, sProcessCertificatesFromEac2);
	sSendDidAuthenticatResponseEacAdditionalInput->addTransition(sSendDidAuthenticatResponseEacAdditionalInput, &AbstractState::fireError, sCleanUpReaderManager);
	sSendDidAuthenticatResponseEacAdditionalInput->addTransition(sSendDidAuthenticatResponseEacAdditionalInput, &AbstractState::fireCancel, sCleanUpReaderManager);
	sSendDidAuthenticatResponseEacAdditionalInput->addTransition(sSendDidAuthenticatResponseEacAdditionalInput, &StateSendDIDAuthenticateResponseEACAdditionalInputType::fireReceivedStartPaosResponse, sStartPaosResponse);

	sProcessCertificatesFromEac2->addTransition(sProcessCertificatesFromEac2, &AbstractState::fireSuccess, sDidAuthenticateEac2);
	sProcessCertificatesFromEac2->addTransition(sProcessCertificatesFromEac2, &AbstractState::fireError, sSendDidAuthenticateResponseEac2);
	sProcessCertificatesFromEac2->addTransition(sProcessCertificatesFromEac2, &AbstractState::fireCancel, sSendDidAuthenticateResponseEac2);

	sDidAuthenticateEac2->addTransition(sDidAuthenticateEac2, &AbstractState::fireSuccess, sSendDidAuthenticateResponseEac2);
	sDidAuthenticateEac2->addTransition(sDidAuthenticateEac2, &AbstractState::fireError, sSendDidAuthenticateResponseEac2);
	sDidAuthenticateEac2->addTransition(sDidAuthenticateEac2, &AbstractState::fireCancel, sSendDidAuthenticateResponseEac2);

	sSendDidAuthenticateResponseEac2->addTransition(sSendDidAuthenticateResponseEac2, &AbstractState::fireSuccess, sTransmit);
	sSendDidAuthenticateResponseEac2->addTransition(sSendDidAuthenticateResponseEac2, &AbstractState::fireError, sCleanUpReaderManager);
	sSendDidAuthenticateResponseEac2->addTransition(sSendDidAuthenticateResponseEac2, &AbstractState::fireCancel, sCleanUpReaderManager);
	sSendDidAuthenticateResponseEac2->addTransition(sSendDidAuthenticateResponseEac2, &StateSendDIDAuthenticateResponseEAC2::fireReceivedDisconnect, sSendDisconnectResponse);
	sSendDidAuthenticateResponseEac2->addTransition(sSendDidAuthenticateResponseEac2, &StateSendDIDAuthenticateResponseEAC2::fireReceivedStartPaosResponse, sStartPaosResponse);

	sTransmit->addTransition(sTransmit, &AbstractState::fireSuccess, sSendTransmitResponse);
	sTransmit->addTransition(sTransmit, &AbstractState::fireError, sSendTransmitResponse);
	sTransmit->addTransition(sTransmit, &AbstractState::fireCancel, sSendTransmitResponse);

	sSendTransmitResponse->addTransition(sSendTransmitResponse, &AbstractState::fireSuccess, sTransmit);
	sSendTransmitResponse->addTransition(sSendTransmitResponse, &AbstractState::fireError, sCleanUpReaderManager);
	sSendTransmitResponse->addTransition(sSendTransmitResponse, &AbstractState::fireCancel, sCleanUpReaderManager);
	sSendTransmitResponse->addTransition(sSendTransmitResponse, &StateSendTransmitResponse::fireReceivedDisconnect, sSendDisconnectResponse);
	sSendTransmitResponse->addTransition(sSendTransmitResponse, &StateSendTransmitResponse::fireReceivedStartPaosResponse, sStartPaosResponse);

	sSendDisconnectResponse->addTransition(sSendDisconnectResponse, &AbstractState::fireSuccess, sStartPaosResponse);
	sSendDisconnectResponse->addTransition(sSendDisconnectResponse, &AbstractState::fireError, sCleanUpReaderManager);
	sSendDisconnectResponse->addTransition(sSendDisconnectResponse, &AbstractState::fireCancel, sCleanUpReaderManager);

	sStartPaosResponse->addTransition(sStartPaosResponse, &AbstractState::fireSuccess, sCleanUpReaderManager);
	sStartPaosResponse->addTransition(sStartPaosResponse, &AbstractState::fireError, sCleanUpReaderManager);
	sStartPaosResponse->addTransition(sStartPaosResponse, &AbstractState::fireCancel, sCleanUpReaderManager);

	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireSuccess, sCheckErrorEpilogue);
	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireError, sCheckErrorEpilogue);
	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireCancel, sCheckErrorEpilogue);

	sCheckErrorEpilogue->addTransition(sCheckErrorEpilogue, &AbstractState::fireSuccess, sCheckRefreshAddress);
	sCheckErrorEpilogue->addTransition(sCheckErrorEpilogue, &AbstractState::fireError, sFinal);
	sCheckErrorEpilogue->addTransition(sCheckErrorEpilogue, &AbstractState::fireCancel, sFinal);

	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireSuccess, sWriteHistory);
	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireError, sFinal);
	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireCancel, sFinal);

	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireSuccess, sGetSelfAuthenticationData);
	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireError, sGetSelfAuthenticationData);
	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireCancel, sFinal);

	sGetSelfAuthenticationData->addTransition(sGetSelfAuthenticationData, &AbstractState::fireSuccess, sFinal);
	sGetSelfAuthenticationData->addTransition(sGetSelfAuthenticationData, &AbstractState::fireError, sFinal);
	sGetSelfAuthenticationData->addTransition(sGetSelfAuthenticationData, &AbstractState::fireCancel, sFinal);
}


SelfAuthController::~SelfAuthController()
{
}
