/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "states/AbstractGenericState.h"

namespace governikus
{

class StateSelectPcscReader
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateSelectPcscReader(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onReaderManagerSignal();

	Q_SIGNALS:
		void fireAbort();
};

} /* namespace governikus */
