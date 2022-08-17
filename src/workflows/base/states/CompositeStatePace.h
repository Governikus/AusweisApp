/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSharedPointer>
#include <QState>


namespace governikus
{

class WorkflowContext;

class CompositeStatePace
	: public QState
{
	Q_OBJECT

	private:
		const QSharedPointer<WorkflowContext> mContext;

	public:
		explicit CompositeStatePace(const QSharedPointer<WorkflowContext>& pContext);
		~CompositeStatePace() override = default;

	Q_SIGNALS:
		void fireContinue();
		void fireAbort();
};

} // namespace governikus
