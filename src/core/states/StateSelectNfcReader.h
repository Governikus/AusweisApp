/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractGenericState.h"

namespace governikus
{

class StateSelectNfcReader
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateSelectNfcReader(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onReaderDetected();

	Q_SIGNALS:
		void fireAbort();
};

} /* namespace governikus */
