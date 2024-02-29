/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */


#include "StateSendStartPaosPersonalization.h"


using namespace governikus;


StateSendStartPaosPersonalization::StateSendStartPaosPersonalization(const QSharedPointer<WorkflowContext>& pContext)
	: StateGenericSendReceive(pContext,
			PaosType::TRANSMIT,
			{},
			true)
{
}


QSharedPointer<ResponseType> StateSendStartPaosPersonalization::getAsResponse()
{
	return QSharedPointer<ResponseType>();
}


QSharedPointer<PaosCreator> StateSendStartPaosPersonalization::getAsCreator()
{
	return getContext()->getStartPaos();
}
