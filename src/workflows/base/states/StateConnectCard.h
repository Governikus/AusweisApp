/*!
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "command/CreateCardConnectionCommand.h"

class test_StateConnectCard;

namespace governikus
{

class StateConnectCard
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateConnectCard;

	private:
		explicit StateConnectCard(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onCardInserted();
		void onCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand);
		void onReaderRemoved(const ReaderInfo& pInfo);
		void onReaderStatusChanged(const ReaderManagerPlugInInfo& pInfo);

	public:
		void onEntry(QEvent* pEvent) override;

	Q_SIGNALS:
		void fireRetry();

};

} // namespace governikus
