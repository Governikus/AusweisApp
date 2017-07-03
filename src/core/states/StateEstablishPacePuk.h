/*!
 * \brief Controller for the step that tries to establish a PACE
 *        connection using the card's Puk.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractGenericState.h"

#include "context/ChangePinContext.h"

namespace governikus
{

class StateEstablishPacePuk
	: public AbstractGenericState<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateEstablishPacePuk(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
	virtual void onUserCancelled() override;

	private Q_SLOTS:
		void onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand);

	Q_SIGNALS:
		void fireInvalidPuk();
		void fireInoperativePuk();
};

} /* namespace governikus */
