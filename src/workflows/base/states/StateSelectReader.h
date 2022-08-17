/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"

class test_StateSelectReader;

namespace governikus
{

class StateSelectReader
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateSelectReader;

	private:
		explicit StateSelectReader(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onReaderInfoChanged();
		void onReaderStatusChanged(const ReaderManagerPlugInInfo& pInfo);

	public:
		void onEntry(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireRetry();
};

} // namespace governikus
