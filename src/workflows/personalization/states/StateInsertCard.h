/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderManagerPlugInInfo.h"
#include "context/PersonalizationContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

#include <QSharedPointer>


namespace governikus
{

class StateInsertCard
	: public AbstractState
	, public GenericContextContainer<PersonalizationContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateInsertCard(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onCardInfoChanged(const ReaderInfo& pInfo);
		void onStatusChanged(const ReaderManagerPlugInInfo& pInfo);

	Q_SIGNALS:
		void fireSkipPinChange();
};

} // namespace governikus
