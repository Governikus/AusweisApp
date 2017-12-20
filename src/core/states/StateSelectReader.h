/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"

namespace governikus
{

class StateSelectReader
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		StateSelectReader(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

		static bool requiresCard(ReaderManagerPlugInType pPlugInType);

	private Q_SLOTS:
		void onReaderInfoChanged();
		void onAbort();
		void onReaderDeviceError(DeviceError pDeviceError);

	public:
		void onEntry(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireRetry();
};

} /* namespace governikus */
