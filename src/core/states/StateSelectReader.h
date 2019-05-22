/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"

class test_StateSelectReader;

namespace governikus
{

class StateSelectReader
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateSelectReader;

	private:
		explicit StateSelectReader(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

		static bool requiresCard(ReaderManagerPlugInType pPlugInType);

	private Q_SLOTS:
		void onReaderInfoChanged();
		void onAbort();
		void onReaderDeviceError(const GlobalStatus pError);

	public:
		void onEntry(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireRetry();
};

} // namespace governikus
