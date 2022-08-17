/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */


#include "StateSendTransmitResponsePersonalization.h"


using namespace governikus;


StateSendTransmitResponsePersonalization::StateSendTransmitResponsePersonalization(const QSharedPointer<WorkflowContext>& pContext)
	: StateGenericSendReceive(pContext,
			PaosType::TRANSMIT,
			{
				PaosType::STARTPAOS_RESPONSE
			}, true)
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


void StateSendTransmitResponsePersonalization::emitStateMachineSignal(PaosType pResponseType)
{
	if (pResponseType == PaosType::STARTPAOS_RESPONSE)
	{
		Q_EMIT fireReceivedStartPaosResponse();
	}
}
