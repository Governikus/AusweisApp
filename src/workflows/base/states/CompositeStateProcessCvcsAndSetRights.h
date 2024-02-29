/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Composite state for selecting a card.
 */

#pragma once

#include "FailureCode.h"

#include <QSharedPointer>
#include <QState>


namespace governikus
{

class WorkflowContext;

class CompositeStateProcessCvcsAndSetRights
	: public QState
{
	Q_OBJECT

	private:
		const QSharedPointer<WorkflowContext> mContext;

	public:
		explicit CompositeStateProcessCvcsAndSetRights(const QSharedPointer<WorkflowContext>& pContext);
		~CompositeStateProcessCvcsAndSetRights() override = default;

	Q_SIGNALS:
		void fireContinue();
		void fireAbort(const FailureCode& pFailure);
};

} // namespace governikus
