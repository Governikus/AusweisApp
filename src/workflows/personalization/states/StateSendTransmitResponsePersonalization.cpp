/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */


#include "StateSendTransmitResponsePersonalization.h"


using namespace governikus;


StateSendTransmitResponsePersonalization::StateSendTransmitResponsePersonalization(const QSharedPointer<WorkflowContext>& pContext)
	: StateGenericSendReceive(pContext,
			PaosType::TRANSMIT,
			{},
			true)
{
}


QSharedPointer<ResponseType> StateSendTransmitResponsePersonalization::getAsResponse()
{
	return getContext()->getTransmitResponse();
}


QSharedPointer<PaosCreator> StateSendTransmitResponsePersonalization::getAsCreator()
{
	return getContext()->getTransmitResponse();
}
