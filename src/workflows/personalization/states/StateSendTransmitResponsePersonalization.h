/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "states/StateGenericSendReceive.h"


namespace governikus
{

class StateSendTransmitResponsePersonalization
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateSendTransmitResponsePersonalization(const QSharedPointer<WorkflowContext>& pContext);

	protected:
		QSharedPointer<ResponseType> getAsResponse() override;
		QSharedPointer<PaosCreator> getAsCreator() override;
};

} // namespace governikus
