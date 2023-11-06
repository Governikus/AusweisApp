/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "FailureCode.h"

#include <QSharedPointer>
#include <QState>


namespace governikus
{

class WorkflowContext;

class CompositeStatePrepareApplet
	: public QState
{
	Q_OBJECT

	private:
		const QSharedPointer<WorkflowContext> mContext;

	public:
		explicit CompositeStatePrepareApplet(const QSharedPointer<WorkflowContext>& pContext);
		~CompositeStatePrepareApplet() override = default;

	Q_SIGNALS:
		void fireContinue();
		void fireAbort(const FailureCode& pFailure);
};

} // namespace governikus
