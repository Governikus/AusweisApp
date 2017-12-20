/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "controller/SelfAuthController.h"

#include "context/SelfAuthContext.h"
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
#include "states/StateEstablishPacePuk.h"
#include "states/StateGenericSendReceive.h"
#include "states/StateGetSelfAuthenticationData.h"
#include "states/StateGetTcToken.h"
#include "states/StateHandleRetryCounter.h"
#include "states/StateInitializeFramework.h"
#include "states/StateLoadTcTokenUrl.h"
#include "states/StateProcessCertificatesFromEac2.h"
#include "states/StateStartPaos.h"
#include "states/StateStartPaosResponse.h"
#include "states/StateTransmit.h"
#include "states/StateUpdateRetryCounter.h"
#include "states/StateWriteHistory.h"


#include <initializer_list>
#include <QDebug>


using namespace governikus;


SelfAuthController::SelfAuthController(QSharedPointer<SelfAuthContext> pContext)
	: WorkflowController(pContext)
{
	auto sLoadTcTokenUrl = addState<StateLoadTcTokenUrl>();
	mStateMachine.setInitialState(sLoadTcTokenUrl);
	auto sGetTcToken = addState<StateGetTcToken>();
	auto sStartPaos = addState<StateStartPaos>();
	auto sSendStartPaos = addState<StateSendStartPaos>();
	auto sInitializeFramework = addState<StateInitializeFramework>();
	auto sSendInitializeFrameworkResponse = addState<StateSendInitializeFrameworkResponse>();
	auto sDidList = addState<StateDidList>();
	auto sSendDidListResponse = addState<StateSendDIDListResponse>();
	auto sProcessCvcsAndSetRights = new CompositeStateProcessCvcsAndSetRights(pContext);
	mStateMachine.addState(sProcessCvcsAndSetRights);
	auto sSelectCard = new CompositeStateSelectCard(pContext);
	mStateMachine.addState(sSelectCard);
	auto sUpdateRetryCounter = addState<StateUpdateRetryCounter>();
	auto sHandleRetryCounter = addState<StateHandleRetryCounter>();
	auto sEstablishPaceCan = addState<StateEstablishPaceCan>();
	auto sEstablishPacePin = addState<StateEstablishPacePin>();
	auto sEstablishPacePuk = addState<StateEstablishPacePuk>();
	auto sDidAuthenticateEac1 = addState<StateDidAuthenticateEac1>();
	auto sSendDidAuthenticateResponseEac1 = addState<StateSendDIDAuthenticateResponseEAC1>();
	auto sEacAdditionalInputType = addState<StateEACAdditionalInputType>();
	auto sSendDidAuthenticatResponseEacAdditionalInput = addState<StateSendDIDAuthenticateResponseEACAdditionalInputType>();
	auto sProcessCertificatesFromEac2 = addState<StateProcessCertificatesFromEac2>();
	auto sDidAuthenticateEac2 = addState<StateDidAuthenticateEac2>();
	auto sSendDidAuthenticateResponseEac2 = addState<StateSendDIDAuthenticateResponseEAC2>();
	auto sTransmit = addState<StateTransmit>();
	auto sSendTransmitResponse = addState<StateSendTransmitResponse>();
	auto sSendDisconnectResponse = addState<StateSendDisconnectResponse>();
	auto sStartPaosResponse = addState<StateStartPaosResponse>();
	auto sCheckErrorEpilogue = addState<StateCheckError>();
	auto sCheckRefreshAddress = addState<StateCheckRefreshAddress>();
	auto sWriteHistory = addState<StateWriteHistory>();
	auto sGetSelfAuthenticationData = addState<StateGetSelfAuthenticationData>();
	auto sUpdateRetryCounterFinal = addState<StateUpdateRetryCounter>();
	auto sCleanUpReaderManager = addState<StateCleanUpReaderManager>();
	auto sFinal = addState<FinalState>();

	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireContinue, sGetTcToken);
	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireAbort, sCleanUpReaderManager);

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

	sProcessCvcsAndSetRights->addTransition(sProcessCvcsAndSetRights, &CompositeStateProcessCvcsAndSetRights::fireContinue, sSelectCard);
	sProcessCvcsAndSetRights->addTransition(sProcessCvcsAndSetRights, &CompositeStateProcessCvcsAndSetRights::fireAbort, sSendDidAuthenticateResponseEac1);

	sSelectCard->addTransition(sSelectCard, &CompositeStateSelectCard::fireContinue, sUpdateRetryCounter);
	sSelectCard->addTransition(sSelectCard, &CompositeStateSelectCard::fireAbort, sSendDidAuthenticateResponseEac1);

	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireContinue, sHandleRetryCounter);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireAbort, sSendDidAuthenticateResponseEac1);

	sHandleRetryCounter->addTransition(sHandleRetryCounter, &StateHandleRetryCounter::fireRetryCounterIsGTOne, sEstablishPacePin);
	sHandleRetryCounter->addTransition(sHandleRetryCounter, &StateHandleRetryCounter::fireRetryCounterIsOne, sEstablishPaceCan);
	sHandleRetryCounter->addTransition(sHandleRetryCounter, &StateHandleRetryCounter::fireRetryCounterIsZero, sEstablishPacePuk);
	sHandleRetryCounter->addTransition(sHandleRetryCounter, &AbstractState::fireAbort, sSendDidAuthenticateResponseEac1);

	sEstablishPacePuk->addTransition(sEstablishPacePuk, &AbstractState::fireContinue, sEstablishPacePin);
	sEstablishPacePuk->addTransition(sEstablishPacePuk, &StateEstablishPacePuk::fireInvalidPuk, sUpdateRetryCounter);
	sEstablishPacePuk->addTransition(sEstablishPacePuk, &AbstractState::fireAbort, sSendDidAuthenticateResponseEac1);

	sEstablishPaceCan->addTransition(sEstablishPaceCan, &AbstractState::fireContinue, sEstablishPacePin);
	sEstablishPaceCan->addTransition(sEstablishPaceCan, &StateEstablishPaceCan::fireInvalidCan, sUpdateRetryCounter);
	sEstablishPaceCan->addTransition(sEstablishPaceCan, &AbstractState::fireAbort, sSendDidAuthenticateResponseEac1);

	sEstablishPacePin->addTransition(sEstablishPacePin, &AbstractState::fireContinue, sDidAuthenticateEac1);
	sEstablishPacePin->addTransition(sEstablishPacePin, &StateEstablishPacePin::fireInvalidPin, sUpdateRetryCounter);
	sEstablishPacePin->addTransition(sEstablishPacePin, &AbstractState::fireAbort, sSendDidAuthenticateResponseEac1);

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

	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireContinue, sCheckErrorEpilogue);
	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireAbort, sCheckErrorEpilogue);

	sCheckErrorEpilogue->addTransition(sCheckErrorEpilogue, &AbstractState::fireContinue, sCheckRefreshAddress);
	sCheckErrorEpilogue->addTransition(sCheckErrorEpilogue, &AbstractState::fireAbort, sFinal);

	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireContinue, sWriteHistory);
	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireAbort, sFinal);

	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireContinue, sGetSelfAuthenticationData);
	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireAbort, sGetSelfAuthenticationData);

	sGetSelfAuthenticationData->addTransition(sGetSelfAuthenticationData, &AbstractState::fireContinue, sFinal);
	sGetSelfAuthenticationData->addTransition(sGetSelfAuthenticationData, &AbstractState::fireAbort, sFinal);
}


SelfAuthController::~SelfAuthController()
{
}
