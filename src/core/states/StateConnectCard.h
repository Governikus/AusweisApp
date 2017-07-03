/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
	void error(const QString& pText);
	virtual void run() override;

	private Q_SLOTS:
		void onCardInserted();
		void onCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand);
		void onReaderRemoved(const QString& pReaderName);

	Q_SIGNALS:
		void fireAbort();
		void fireReaderRemoved();

};

} /* namespace governikus */
