/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateEnterPacePasswordRemote
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateEnterPacePasswordRemote(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

	private Q_SLOTS:
		void onCancelEstablishPaceChannel();

	public:
		void onEntry(QEvent* pEvent) override;
};

} // namespace governikus
