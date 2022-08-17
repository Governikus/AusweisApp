/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "states/StateGenericSendReceive.h"


namespace governikus
{

class StateSendStartPaosPersonalization
	: public StateGenericSendReceive
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateSendStartPaosPersonalization(const QSharedPointer<WorkflowContext>& pContext);

	protected:
		QSharedPointer<ResponseType> getAsResponse() override;
		QSharedPointer<PaosCreator> getAsCreator() override;
		void emitStateMachineSignal(PaosType pResponseType) override;

	Q_SIGNALS:
		void fireReceivedStartPaosResponse();


};

} // namespace governikus
