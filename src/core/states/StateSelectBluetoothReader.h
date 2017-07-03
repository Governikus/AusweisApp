/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractGenericState.h"

namespace governikus
{

class StateSelectBluetoothReader
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateSelectBluetoothReader(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onReaderDetected();
		void onReaderConnected();

	Q_SIGNALS:
		void fireAbort();
};

} /* namespace governikus */
