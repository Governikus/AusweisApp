/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractGenericState.h"

namespace governikus
{

class StateSelectReaderType
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateSelectReaderType(const QSharedPointer<WorkflowContext>& pContext);

	virtual void run() override;

	protected:
		void onExit(QEvent* pEvent) override;

	private Q_SLOTS:
		void onReaderTypeChanged();

	Q_SIGNALS:
		void fireSelectBluetoothReader();
		void fireSelectNfcReader();
		void fireSelectPcscReader();
};

} /* namespace governikus */
