/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractGenericState.h"
#include "DeviceError.h"

namespace governikus
{

class StateSelectBluetoothReader
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateSelectBluetoothReader(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	void startScan();

	private Q_SLOTS:
		void onReaderDetected();
		void onReaderConnected();
		void onAbort();
		void onReaderDeviceError(DeviceError pDeviceError);

	protected:
		void onExit(QEvent* pEvent) override;

	public:
		void onEntry(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireAbort();
};

} /* namespace governikus */
