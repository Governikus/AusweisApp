/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"
#include "command/CreateCardConnectionCommand.h"

namespace governikus
{

class StateConnectCard
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateConnectCard(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onCardInserted();
		void onCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand);
		void onReaderRemoved(const QString& pReaderName);
		void onAbort();

	Q_SIGNALS:
		void fireRetry();
		void fireReaderRemoved();

};

} /* namespace governikus */
