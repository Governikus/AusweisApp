/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"


namespace governikus
{

class StateSelectReader
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateSelectReader(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

		static bool requiresCard(ReaderManagerPlugInType pPlugInType);

	private Q_SLOTS:
		void onReaderInfoChanged();

	public:
		void onEntry(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireRetry();
};

} // namespace governikus
