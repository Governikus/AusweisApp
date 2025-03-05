/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
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
