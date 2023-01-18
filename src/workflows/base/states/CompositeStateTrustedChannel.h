/*!
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSharedPointer>
#include <QState>


namespace governikus
{

class WorkflowContext;

class CompositeStateTrustedChannel
	: public QState
{
	Q_OBJECT

	private:
		const QSharedPointer<WorkflowContext> mContext;

	public:
		explicit CompositeStateTrustedChannel(const QSharedPointer<WorkflowContext>& pContext);
		~CompositeStateTrustedChannel() override = default;

	Q_SIGNALS:
		void fireContinue();
		void fireAbort();
};

} // namespace governikus
